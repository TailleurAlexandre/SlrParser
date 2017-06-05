#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "closure.h"
#include "parsing_table.h"
#include "parsing_statements.h"

static int	find_rule_name(char **rule_names, char *rule, int size){
  int		i;

  for(i = 0; i < size; ++i){
    if (!strcmp(rule_names[i], rule))
      return i;
  }
  return -1;
}

static goto_table_t construct_goto_table(rules_t *rules, goto_states_t *states, int state_number){
  goto_table_t		go_table;
  goto_states_t		*tmp = states;

  go_table.state_goto = NULL;
  go_table.rule_names = NULL;
  go_table.rules_number = 0;
  while (tmp){
    if (!tmp->edge){
      int	i;
      for (i = 0; i < tmp->size; ++i){
	if (isupper(tmp->states_follow_up[i]->_character))
	  {
	    char t[] = {tmp->states_follow_up[i]->_character, 0};
	    int	nb = find_rule_name(go_table.rule_names, t, go_table.rules_number);
	    if (nb == -1){
	      go_table.rule_names = realloc(go_table.rule_names, (go_table.rules_number + 1) * sizeof(char *));
	      go_table.rule_names[go_table.rules_number] = malloc(2);
	      strcpy(go_table.rule_names[go_table.rules_number], t);
	      go_table.state_goto = realloc(go_table.state_goto, (go_table.rules_number + 1) * sizeof(int *));
	      go_table.state_goto[go_table.rules_number] = calloc(state_number, sizeof(int *));
	      nb = go_table.rules_number++;
	    }
	    go_table.state_goto[nb][tmp->_number] = tmp->states_follow_up[i]->_number;
	  }
      }
    }
    tmp = tmp->_prev;
  }
  return go_table;
}

static action_table_t	construct_action_table(rules_t *rules, goto_states_t *states, int state_number){
  action_table_t	a_table;

  a_table.elems = calloc(state_number, sizeof(action_elem_t *));
  return a_table;
}

void	construct_parsing_table(rules_t *rules, goto_states_t *states, int state_number)
{
  goto_table_t		go_table = construct_goto_table(rules, states, state_number);
  action_table_t	a_table = construct_action_table(rules, states, state_number);  
}
