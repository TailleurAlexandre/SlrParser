#ifndef _CLOSURE_H_
#define _CLOSURE_H_

#include "slr.h"

struct closure_element_s {
  rules_t	*rule;
  int		_parsing_counter;
};

typedef struct closure_element_s closure_element_t;

struct closure_s {
  closure_element_t	*elements;
  int			size;
  struct closure_s	*_next;
  struct closure_s	*_prev;
};

typedef struct closure_s closure_t;

struct goto_states_s {
  int			_number;
  char			_character;
  closure_t		*kernel;
  closure_t		*closure;
  int			size;
  struct goto_states_s	*edge;
  struct goto_states_s	*copy_of;
  struct goto_states_s	**states_follow_up;
  struct goto_states_s	*_prev;
  struct goto_states_s	*_next;
};

typedef struct goto_states_s goto_states_t;

void	create_closure(rules_t *rules);

#endif
