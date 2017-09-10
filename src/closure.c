#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "closure.h"

static int	get_closure_elements(rules_t *rules,
				     closure_element_t **closure_elems, int counter, int offset);
static int	verify_use_ofrule(closure_element_t *closure_elems,
				  size_t size, rules_t *rule);

static int		state_nb = 1;
static goto_states_t	*g_cursor;

static int	verify_use_of_rule2(closure_element_t *closure_elems,
				    size_t size, rules_t *rule, int parsing_counter){
  int	i = 0;

  while (i < size){
    if (closure_elems[i].rule == rule && closure_elems[i]._parsing_counter == parsing_counter){
      return 1;
    }
    ++i;
  }
  return 0;  
}

static int	find_existing_state_edge(goto_states_t **states, int size, char c){
  int		i = 0;

  while (i < size){
    if (states[i]->_character == c)
      return i;
    ++i;
  }
  return -1;
}


static int	get_closure_elements2(rules_t *rules,
				     closure_element_t **closure_elems, int counter, int offset){
  rules_t	*tmp2,*tmp;
  int		parsing_counter = (*closure_elems)[offset]._parsing_counter;

  tmp = rules;
  tmp2 = (*closure_elems)[offset].rule;
  while (tmp != NULL){
    if (strncmp(tmp2->_rule+parsing_counter, tmp->_pre_rule, strlen(tmp->_pre_rule)) == 0)
      {
	if (!verify_use_of_rule2(*closure_elems, counter, tmp, 0))
	  {  
	    (*closure_elems) = realloc(*closure_elems,
				       (counter + 1) * sizeof(closure_element_t));
	    (*closure_elems)[counter].rule = tmp;
	    (*closure_elems)[counter++]._parsing_counter = 0;
	    counter = get_closure_elements2(tmp, closure_elems, counter, counter-1);
	  }
      }
    tmp = tmp->_next;
  }
  return counter;
}

static goto_states_t	*find_existing_state(goto_states_t *state){
  goto_states_t		*tmp = g_cursor;
  int			i = 0;
  int			same = 0;

  while(tmp != NULL) {
    same = 0;
    if (state != tmp && tmp->kernel && state->kernel->size == tmp->kernel->size){
      for (i = 0; i < tmp->kernel->size;++i){
	if (state->kernel->elements[i]._parsing_counter == tmp->kernel->elements[i]._parsing_counter
	    && state->kernel->elements[i].rule == tmp->kernel->elements[i].rule){
	  same = 1;
	}
	else {
	  same = 0;
	  break;
	}
      }
      if (same == 1){
	return tmp;
      }
    }
    tmp = tmp->_prev;
  }
  return NULL;
}

static goto_states_t	*add_new_state(closure_element_t elem, goto_states_t *states_top,
				      char c, rules_t *rules){
  goto_states_t		*state;

  state = calloc(1, sizeof(goto_states_t));
  state->closure = calloc(1, sizeof(closure_t));
  state->kernel = calloc(1, sizeof(closure_t));
  state->kernel->elements = realloc(state->closure->elements,
				     ++state->kernel->size * sizeof(closure_element_t));
  state->kernel->elements[0] = elem;
  state->kernel->elements[0]._parsing_counter++;
  state->closure->elements = realloc(state->closure->elements,
				     ++state->closure->size * sizeof(closure_element_t));
  state->closure->elements[0] = state->kernel->elements[0];
  state->edge = states_top;
  if (c != 0)
    state->closure->size = get_closure_elements2(rules, &state->closure->elements,
						 state->closure->size, 0);
  state->_character = c;
  states_top->states_follow_up = realloc(states_top->states_follow_up,
					 (states_top->size + 1) * sizeof(goto_states_t *));
  states_top->states_follow_up[states_top->size++] = state;
  g_cursor->_next = state;
  state->_prev = g_cursor;
  g_cursor = state;
  return state;
}

static void	get_parsing_states(rules_t *rules, goto_states_t *states_top){
  closure_element_t	*closure = states_top->closure->elements;
  int			size = states_top->closure->size;
  int			o = 0,i = 0;
  goto_states_t		*state;
  char			c = 0;
  int			nb;

  for (o = 0; o < size; ++o){
    c = closure[o].rule->_rule[closure[o]._parsing_counter];
    if (c == 0){
      state = add_new_state(closure[o], states_top, c, rules);
      continue;
    }
    if  ((nb = find_existing_state_edge(states_top->states_follow_up, states_top->size, c)) == -1){
      state = add_new_state(closure[o], states_top, c, rules);
    }
    else {
      closure_t *closure_tmp = states_top->states_follow_up[nb]->closure,
	*closure_tmp2 = states_top->states_follow_up[nb]->kernel;
      closure_tmp2->elements = realloc(closure_tmp2->elements,
				       sizeof(closure_element_t) * ++closure_tmp2->size);
      closure_tmp2->elements[closure_tmp2->size - 1] = closure[o];
      closure_tmp2->elements[closure_tmp2->size - 1]._parsing_counter++; 
      closure_tmp->elements = realloc(closure_tmp->elements, sizeof(closure_element_t) * ++closure_tmp->size);
      closure_tmp->elements[closure_tmp->size - 1] = closure_tmp2->elements[state->closure->size - 1];
      closure_tmp->size = get_closure_elements2(rules, &closure_tmp->elements,
						closure_tmp->size, closure_tmp->size-1);
      states_top->states_follow_up[nb]->closure = closure_tmp;
      states_top->states_follow_up[nb]->kernel = closure_tmp2;
    }
  }

  goto_states_t *verif = NULL; 
  for (i = 0; i < states_top->size; ++i){
    if ((verif = find_existing_state(states_top->states_follow_up[i]))){
      states_top->states_follow_up[i]->copy_of = verif;
      states_top->states_follow_up[i]->_number = verif->_number;
    }
    else{
      if (states_top->states_follow_up[i]->_character){
	states_top->states_follow_up[i]->_number = state_nb++;
      }
    }
  }
  for (i = 0; i < states_top->size; ++i){
    if (states_top->states_follow_up[i]->copy_of == NULL && states_top->states_follow_up[i]->_character != 0)
      {
	get_parsing_states(rules, states_top->states_follow_up[i]);
      }
  }
  
  
}

static int	verify_use_ofrule(closure_element_t *closure_elems,
				   size_t size, rules_t *rule) {
  int	i = 0;

  while (i < size){
    if (closure_elems[i].rule == rule){
      return 1;
    }
    ++i;
  }
  return 0;
}



static int	get_closure_elements(rules_t *rules,
				     closure_element_t **closure_elems, int counter, int offset){
  rules_t	*tmp2,*tmp;
  int		parsing_counter = (*closure_elems)[offset]._parsing_counter;

  tmp = rules->_next;
  tmp2 = rules;
  while (tmp != NULL){
    if (strncmp(tmp2->_rule+parsing_counter, tmp->_pre_rule, strlen(tmp->_pre_rule)) == 0)
      {
	if (!verify_use_ofrule(*closure_elems, counter, tmp))
	  {  
	    (*closure_elems) = realloc(*closure_elems,
				       (counter + 1) * sizeof(closure_element_t));
	    (*closure_elems)[counter].rule = tmp;
	    (*closure_elems)[counter++]._parsing_counter = 0;
	    counter = get_closure_elements(tmp, closure_elems, counter, offset);
	  }
      }
    tmp = tmp->_next;
  }
  return counter;
}

static closure_t	*closure_creation_init(rules_t *rules,
					       rules_t *cursor){
  closure_t		*tmp = NULL;
  goto_states_t		*states = calloc(1, sizeof(goto_states_t));

  g_cursor = states;
  tmp = calloc(1, sizeof(closure_t));
  (tmp)->elements = calloc(1, sizeof(closure_element_t));
  (tmp)->elements[0].rule = cursor;
  (tmp)->size = get_closure_elements(rules, &(tmp)->elements, 1, 0);
  states->closure = tmp;
  get_parsing_states(rules, states);
  construct_parsing_table(rules, states, state_nb);
  return tmp;
}

void	create_closure(rules_t *rules){
  closure_creation_init(rules, rules);
}
