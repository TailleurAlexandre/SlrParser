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

typedef struct  action_elem_s {
    action_t    action;
    char        terminal;
    int         state_number;
    char        *rule_name;
} action_elem_t;

typedef struct      action_vector_s {
    int             size;
    action_elem_t   *elems;   
} action_vector_t;

typedef struct	    action_table_s {
    char		    *terminals;
    int             state_nb;
    action_vector_t	*elems;
} action_table_t;

typedef struct	goto_table_s {
    char		**rule_names;
    int		**state_goto;
    int     *sizes;
    int		rules_number;
    int     size;
} goto_table_t;

void        print_table();
int         parse_string(char*);
void        print_goto_table();
void        print_action_table();
void	    construct_parsing_table(rules_t *rules, 
        goto_states_t *states, 
        int state_number);

#endif
