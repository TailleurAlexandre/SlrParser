#ifndef PARSING_STATEMENTS_H_
#define PARSING_STATEMENTS_H_

#include "slr.h"

typedef struct follow_s {
  char	**rule_names;
  char	**follows;
  int	size;
} follow_t;

char	*get_follow(rules_t *, char *);
char	*get_first(rules_t *, char *);

#endif
