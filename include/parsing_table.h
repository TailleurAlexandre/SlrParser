#ifndef _PARSING_TABLE_H_
#define _PARSING_TABLE_H_

#include "slr.h"
#include "closure.h"

typedef enum action_e{
  SHIFT = 1,
  REDUCE = 2,
  ERROR = 3,
  ACCEPT = 4
} action_t;

typedef struct	action_elem_s {
  action_t	action;
  int		state_number;
  char		*rule_name;
} action_elem_t;

typedef struct	action_table_s {
  char		*terminals;
  action_elem_t	**elems;
} action_table_t;

typedef struct	goto_table_s {
  char		**rule_names;
  int		**state_goto;
  int		rules_number;
} goto_table_t;

#endif
