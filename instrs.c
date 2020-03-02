/*--------------------------------------------------------------------*/
/*--- instrgrind                                                   ---*/
/*---                                                     instrs.c ---*/
/*--------------------------------------------------------------------*/

/*
   This file is part of instrgrind, a dynamic instruction tracker tool.

   Copyright (C) 2019, Andrei Rimsa (andrei@cefetmg.br)

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
   02111-1307, USA.

   The GNU General Public License is contained in the file COPYING.
*/

#include "global.h"

#define DEFAULT_POOL_SIZE 262144 // 256k instructions

struct {
	enum {
		TKN_COLON,
		TKN_ADDR,
		TKN_NUMBER
	} type;

	union {
		Addr addr;
		ULong number;
	} data;

	HChar text[1024];
} token;

SmartHash* instrs_pool = 0;

static
void delete_instr(UniqueInstr* instr) {
	IGD_ASSERT(instr != 0);
	IGD_DATA_FREE(instr, sizeof(UniqueInstr));
}

void IGD_(init_instrs_pool)() {
	IGD_ASSERT(instrs_pool == 0);

	instrs_pool = IGD_(new_smart_hash)(DEFAULT_POOL_SIZE);

	// set the growth rate to half the size.
	IGD_(smart_hash_set_growth_rate)(instrs_pool, 1.5f);
}

void IGD_(destroy_instrs_pool)() {
	IGD_ASSERT(instrs_pool != 0);

	IGD_(smart_hash_clear)(instrs_pool, (void (*)(void*)) delete_instr);
	IGD_(delete_smart_hash)(instrs_pool);
	instrs_pool = 0;
}

UniqueInstr* IGD_(get_instr)(Addr addr, Int size) {
	UniqueInstr* instr = IGD_(find_instr)(addr);
	if (instr) {
		IGD_ASSERT(instr->addr == addr);
		if (size != 0) {
			if (instr->size == 0) {
				instr->size = size;
			} else {
				IGD_ASSERT(instr->size == size);
			}
		}
	} else {
		instr = (UniqueInstr*) IGD_MALLOC("igd.instrs.gi.1", sizeof(UniqueInstr));
		VG_(memset)(instr, 0, sizeof(UniqueInstr));
		instr->addr = addr;
		instr->size = size;

		IGD_(smart_hash_put)(instrs_pool, instr, (HWord (*)(void*)) IGD_(instr_addr));
	}

	return instr;
}

UniqueInstr* IGD_(find_instr)(Addr addr) {
	return (UniqueInstr*) IGD_(smart_hash_get)(instrs_pool, addr, (HWord (*)(void*)) IGD_(instr_addr));
}

Addr IGD_(instr_addr)(UniqueInstr* instr) {
	IGD_ASSERT(instr != 0);
	return instr->addr;
}

Int IGD_(instr_size)(UniqueInstr* instr) {
	IGD_ASSERT(instr != 0);
	return instr->size;
}

Bool IGD_(instrs_cmp)(UniqueInstr* i1, UniqueInstr* i2) {
	return i1 && i2 && i1->addr == i2->addr && i1->size == i2->size;
}

void IGD_(print_instr)(UniqueInstr* instr) {
	IGD_ASSERT(instr != 0);
	VG_(printf)("0x%lx [%d]", instr->addr, instr->size);
}

void IGD_(fprint_instr)(VgFile* fp, UniqueInstr* instr) {
	IGD_ASSERT(fp != 0);
	IGD_ASSERT(instr != 0);

	VG_(fprintf)(fp, "0x%lx [%d]", instr->addr, instr->size);
}

static
Bool next_token(Int fd) {
	Int idx, state;
	static Int last = -1;

	idx = 0;
	VG_(memset)(&token, 0, sizeof(token));

	state = 1;
	while (state != 6) {
		Int c;

		IGD_ASSERT(idx >= 0 && idx < ((sizeof(token.text) / sizeof(HChar))-1));
		if (last == -1) {
			Int s;
			HChar tmp;

			s = VG_(read)(fd, &tmp, 1);
			c = s < 1 ? -1 : tmp;
		} else {
			c = last;
			last = -1;
		}

		switch (state) {
			case 1:
				if (c == -1) {
					return False;
				} else if (c == ' ' || c == '\t' || c == '\r' || c == '\n') {
					state = 1;
				} else if (c == '0') {
					token.text[idx++] = c;
					token.type = TKN_NUMBER;
					token.data.number = 0;
					state = 2;
				} else if (c >= '1' && c <= '9') {
					token.text[idx++] = c;
					token.type = TKN_NUMBER;
					state = 4;
				} else if (c == '#') {
					state = 5;
				} else if (c == ':') {
					token.text[idx++] = c;
					token.type = TKN_COLON;
					state = 6;
				} else {
					tl_assert(0);
				}

				break;
			case 2:
				if (VG_(tolower)(c) == 'x') {
					token.text[idx++] = VG_(tolower)(c);
					token.type = TKN_ADDR;
					state = 3;
				} else {
					if (c != -1)
						last = c;

					state = 6;
				}

				break;
			case 3:
				if ((c >= '0' && c <= '9') ||
						(VG_(tolower)(c) >= 'a' && VG_(tolower)(c) <= 'f')) {
					token.text[idx++] = VG_(tolower)(c);
					state = 3;
				} else {
					token.data.addr = VG_(strtoull16)(token.text, 0);

					if (c != -1)
						last = c;

					state = 6;
				}

				break;
			case 4:
				if (c >= '0' && c <= '9') {
					token.text[idx++] = c;
					state = 4;
				} else {
					token.data.number = VG_(strtoull10)(token.text, 0);

					if (c != -1)
						last = c;

					state = 6;
				}

				break;
			case 5:
				if (c == -1 || c == '\n')
					state = 1;
				else
					state = 5;

				break;
			default:
				tl_assert(0);
		}
	}

	return True;
}


void IGD_(read_instrs)(Int fd) {
	Addr addr;
	Int size;
	Bool has;
	UniqueInstr* instr;

	while (next_token(fd)) {
		IGD_ASSERT(token.type == TKN_ADDR);
		addr = token.data.addr;

		has = next_token(fd);
		IGD_ASSERT(has && token.type == TKN_COLON);

		has = next_token(fd);
		IGD_ASSERT(has && token.type == TKN_NUMBER);
		size = (Int) token.data.number;
		IGD_ASSERT(size > 0);

		instr = IGD_(get_instr)(addr, size);
		IGD_ASSERT(instr != 0);

		has = next_token(fd);
		IGD_ASSERT(has && token.type == TKN_COLON);

		has = next_token(fd);
		IGD_ASSERT(has && token.type == TKN_NUMBER);
		instr->exec_count += token.data.number;
	}
}

static
Bool dump_instr(UniqueInstr* instr, VgFile* outfile) {
	IGD_ASSERT(instr != 0);
	IGD_ASSERT(outfile != 0);

	VG_(fprintf)(outfile, "0x%lx:%d:%llu\n", instr->addr, instr->size, instr->exec_count);

	return False;
}

void IGD_(dump_instrs)(const HChar* filename) {
	VgFile* outfile;

	outfile = VG_(fopen)(filename, VKI_O_WRONLY|VKI_O_TRUNC, 0);
	if (outfile == 0) {
		outfile = VG_(fopen)(filename, VKI_O_CREAT|VKI_O_WRONLY,
										VKI_S_IRUSR|VKI_S_IWUSR);
	}
	IGD_ASSERT(outfile != 0);

	IGD_(smart_hash_forall)(instrs_pool, (Bool (*)(void*, void*)) dump_instr, outfile);

	VG_(fclose)(outfile);
}
