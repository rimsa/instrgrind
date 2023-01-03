/*--------------------------------------------------------------------*/
/*--- instrgrind                                                   ---*/
/*---                                                     groups.c ---*/
/*--------------------------------------------------------------------*/

/*
   This file is part of instrgrind, a dynamic instruction tracker tool.

   Copyright (C) 2023, Andrei Rimsa (andrei@cefetmg.br)

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

#define DEFAULT_POOL_SIZE 131072 // 128k groups

SmartList* groups_pool = 0;

static
void delete_group(InstrGroup* group) {
	IGD_ASSERT(group != 0);

	IGD_(flush_group)(group);

	IGD_(smart_list_clear)(group->instrs, 0);
	IGD_(delete_smart_list)(group->instrs);

	IGD_DATA_FREE(group, sizeof(InstrGroup*));
}

void IGD_(init_groups_pool)() {
	IGD_ASSERT(groups_pool == 0);

	groups_pool = IGD_(new_smart_list)(DEFAULT_POOL_SIZE);
}

void IGD_(destroy_groups_pool)() {
	IGD_ASSERT(groups_pool != 0);

	IGD_(smart_list_clear)(groups_pool, (void (*)(void*)) delete_group);
	IGD_(delete_smart_list)(groups_pool);
	groups_pool = 0;
}

InstrGroup* IGD_(new_group)() {
	InstrGroup* group;

	group = (InstrGroup*) IGD_MALLOC("igd.groups.ng.1", sizeof(InstrGroup));
	group->exec_count = 0;
	group->instrs = IGD_(new_smart_list)(10);

	IGD_(smart_list_add)(groups_pool, group);

	return group;
}

void IGD_(flush_group)(InstrGroup* group) {
	Int i, size;

	IGD_ASSERT(group != 0);

	size = IGD_(smart_list_count)(group->instrs);
	for (i = 0; i < size; i++) {
		UniqueInstr* instr;

		instr = IGD_(smart_list_at)(group->instrs, i);
		IGD_ASSERT(instr != 0);

		instr->exec_count += group->exec_count;
	}

	group->exec_count = 0;
}
