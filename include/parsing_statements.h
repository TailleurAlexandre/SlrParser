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
char	*get_saved_follow(char *rule_name);
char    *merge_strings(char *, char *);
char    *get_saved_first(char *);
void    print_follows();
void    print_firsts();

#endif
