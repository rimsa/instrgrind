#ifndef IGD_GLOBAL
#define IGD_GLOBAL

#include "pub_tool_basics.h"
#include "pub_tool_vki.h"
#include "pub_tool_debuginfo.h"
#include "pub_tool_libcbase.h"
#include "pub_tool_libcassert.h"
#include "pub_tool_libcfile.h"
#include "pub_tool_libcprint.h"
#include "pub_tool_libcproc.h"
#include "pub_tool_machine.h"
#include "pub_tool_mallocfree.h"
#include "pub_tool_options.h"
#include "pub_tool_tooliface.h"
#include "pub_tool_clientstate.h"

// Chain Smart List: 1
// Realloc Smart List: 2
#define SMART_LIST_MODE 2

// Calling an external function: 1
// Expression built in IR: 2
#define INSTR_INC_MODE 2

#define IGD_(str) VGAPPEND(vginstrgrind_,str)

#define IGD_DEBUGIF(x) if (0)
#define IGD_DEBUG(x...) {}
#define IGD_ASSERT(cond) tl_assert(cond);

#define IGD_MALLOC(_cc,x)		VG_(malloc)((_cc),x)
#define IGD_FREE(p)				VG_(free)(p)
#define IGD_REALLOC(_cc,p,x)	VG_(realloc)((_cc),p,x)
#define IGD_STRDUP(_cc,s)		VG_(strdup)((_cc),s)

#define IGD_UNUSED(arg)			(void)arg;
#define IGD_DATA_FREE(p,x)		\
	do { 						\
		VG_(memset)(p, 0x41, x);	\
		IGD_FREE(p); 			\
	} while (0)

typedef struct _SmartHash		SmartHash;
typedef struct _SmartList		SmartList;
typedef struct _SmartValue		SmartValue;
typedef struct _SmartSeek		SmartSeek;
typedef struct _InstrGroup		InstrGroup;
typedef struct _UniqueInstr 	UniqueInstr;

struct _SmartValue {
	Int index;
	void* value;
	SmartValue* next;
};

struct _InstrGroup {
	ULong exec_count;  // The number of times this group was executed.
	SmartList* instrs; // The list of instructions of this group.
};

struct _UniqueInstr {
	Addr addr;
	Int size;
	ULong exec_count; // The number of times this instruction was executed.
};

/* from groups.c */
void IGD_(init_groups_pool)(void);
void IGD_(destroy_groups_pool)(void);
InstrGroup* IGD_(new_group)(void);
void IGD_(flush_group)(InstrGroup* group);

/* from instrs.c */
void IGD_(init_instrs_pool)(void);
void IGD_(destroy_instrs_pool)(void);
UniqueInstr* IGD_(get_instr)(Addr addr, Int size);
UniqueInstr* IGD_(find_instr)(Addr addr);
Addr IGD_(instr_addr)(UniqueInstr* instr);
Int IGD_(instr_size)(UniqueInstr* instr);
Bool IGD_(instrs_cmp)(UniqueInstr* i1, UniqueInstr* i2);
void IGD_(print_instr)(UniqueInstr* instr);
void IGD_(fprint_instr)(VgFile* fp, UniqueInstr* instr);
void IGD_(read_instrs)(Int fd);
void IGD_(dump_instrs)(const HChar* filename);

/* from smarthash.c */
SmartHash* IGD_(new_smart_hash)(Int size);
SmartHash* IGD_(new_fixed_smart_hash)(Int size);
void IGD_(delete_smart_hash)(SmartHash* shash);
void IGD_(smart_hash_clear)(SmartHash* shash, void (*remove_value)(void*));
Int IGD_(smart_hash_count)(SmartHash* shash);
Int IGD_(smart_hash_size)(SmartHash* shash);
Bool IGD_(smart_hash_is_empty)(SmartHash* shash);
Float IGD_(smart_hash_growth_rate)(SmartHash* shash);
void IGD_(smart_hash_set_growth_rate)(SmartHash* shash, Float rate);
void* IGD_(smart_hash_get)(SmartHash* shash, HWord key, HWord (*hash_key)(void*));
void* IGD_(smart_hash_put)(SmartHash* shash, void* value, HWord (*hash_key)(void*));
void* IGD_(smart_hash_remove)(SmartHash* shash, HWord key, HWord (*hash_key)(void*));
Bool IGD_(smart_hash_contains)(SmartHash* shash, HWord key, HWord (*hash_key)(void*));
void IGD_(smart_hash_forall)(SmartHash* shash, Bool (*func)(void*, void*), void* arg);
void IGD_(smart_hash_merge)(SmartHash* dst, SmartHash* src, HWord (*hash_key)(void*));

/* from smartlist.c */
SmartList* IGD_(new_smart_list)(Int size);
SmartList* IGD_(new_fixed_smart_list)(Int size);
SmartList* IGD_(clone_smart_list)(SmartList* slist);
void IGD_(delete_smart_list)(SmartList* slist);
void IGD_(smart_list_clear)(SmartList* slist, void (*remove_element)(void*));
Int IGD_(smart_list_size)(SmartList* slist);
Int IGD_(smart_list_count)(SmartList* slist);
Bool IGD_(smart_list_is_empty)(SmartList* slist);
void* IGD_(smart_list_at)(SmartList* slist, Int index);
void* IGD_(smart_list_head)(SmartList* slist);
void* IGD_(smart_list_tail)(SmartList* slist);
void IGD_(smart_list_set)(SmartList* slist, Int index, void* value);
void IGD_(smart_list_del)(SmartList* slist, Int index, Bool remove_contents);
void IGD_(smart_list_add)(SmartList* slist, void* value);
void IGD_(smart_list_copy)(SmartList* dst, SmartList* src);
void IGD_(smart_list_forall)(SmartList* slist, Bool (*func)(void*, void*), void* arg);
Bool IGD_(smart_list_contains)(SmartList* slist, void* value, Bool (*cmp)(void*, void*));
Float IGD_(smart_list_growth_rate)(SmartList* slist);
void IGD_(smart_list_set_growth_rate)(SmartList* slist, Float rate);
SmartValue* IGD_(smart_list_find)(SmartList* slist, Bool (*cmp)(void*, void*), void* arg);
void IGD_(smart_list_delete_value)(SmartValue* sv);
SmartSeek* IGD_(smart_list_seek)(SmartList* slist);
void IGD_(smart_list_delete_seek)(SmartSeek* ss);
void IGD_(smart_list_rewind)(SmartSeek* ss);
Int IGD_(smart_list_get_index)(SmartSeek* ss);
void IGD_(smart_list_set_index)(SmartSeek* ss, Int index);
Bool IGD_(smart_list_has_next)(SmartSeek* ss);
void IGD_(smart_list_next)(SmartSeek* ss);
void* IGD_(smart_list_get_value)(SmartSeek* ss);
void IGD_(smart_list_set_value)(SmartSeek* ss, void* value);

#endif