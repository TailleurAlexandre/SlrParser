#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "slr.h"
#include "parsing_statements.h"

static follow_t	g_follows = { NULL, NULL, 0 };

static void	save_follow(char *follow, char *rule_name);

static char *get_follow_single(char *follow, rules_t *rule, char *rule_name){
  int	size = strlen(rule->_rule);
  char *res;
  
  if ((res = strstr(rule->_rule, rule_name))){
    int	dif = res - rule->_rule;
    if (dif + 1 < size){
      if (follow == NULL)
	follow = calloc(2,1);
      else
	follow = realloc(follow, strlen(follow) + 1);
      sprintf(follow, "%s%c", follow, rule->_rule[dif]);
    }
  }
  return follow;
}

static char	*get_saved_follow(char *rule_name){
  int		i = 0;
  
  for (i = 0; i < g_follows.size;++i){
    if (!strcmp(g_follows.rule_names[i], rule_name))
      break;
  }
  if (g_follows.size == i)
    return NULL;
  return g_follows.follows[i];
}

static char	*merge_follows(char *f1, char *f2){
  int		i, o, c = 0;
  int		size1 = strlen(f1), size2 = strlen(f2);
  char		*res = malloc(size1 + size2);

  for(i = 0; i < size1; ++i){
    for (o = 0; o < size2; ++o){
      if (f1[i] == f2[o])
	break;
    }
    if (o == size2) {
      res[c++] = f1[i];
    }
  }
  for(i = 0; i < size2; ++i){
    for (o = 0; o < size1; ++o){
      if (f2[i] == f1[o])
	break;
    }
    if (o == size1) {
      res[c++] = f2[i];
    }
  }
  return res;
}

static char	*process_follows(char *rule_name, rules_t *rules){
  rules_t	*tmp = rules;
  char		*res;
  char		*follow;
  
  while (tmp){
    if ((res = strstr(tmp->_rule, rule_name)) && strcmp(rule_name, tmp->_pre_rule)){
      follow = get_saved_follow(rule_name);
      char *f2 = get_saved_follow(tmp->_pre_rule);
      if (follow && f2){
	follow = merge_follows(follow, f2);
	save_follow(follow, rule_name);
      }
    }
    tmp = tmp->_next;
  }
  return follow;
}

static void	save_follow(char *follow, char *rule_name){
  int		i = 0;

  for (i = 0; i < g_follows.size;++i){
    if (!strcmp(g_follows.rule_names[i], rule_name))
      break;
  }
  if (i == g_follows.size){
    g_follows.follows = realloc(g_follows.follows, sizeof(char *) * (g_follows.size + 1));
    g_follows.rule_names = realloc(g_follows.rule_names, sizeof(char *) * (g_follows.size + 1));
    g_follows.size++;
  }
  g_follows.follows[i] = follow;
  g_follows.rule_names[i] = rule_name;
}

char	*get_follow(rules_t *rules, char *rule_name){
  char *follow;
  rules_t *tmp = rules;
  
  while (tmp){
    if (strcmp(tmp->_pre_rule, rule_name)){
      follow = get_follow_single(follow, tmp, rule_name);
    }
    tmp = tmp->_next;
  }
  save_follow(follow, rule_name);
  return process_follows(rule_name, rules);
}

static char *get_first_rec(char *first, rules_t *rules){
  rules_t	*tmp = rules->_next;

  while (tmp){
    if (tmp->_rule[0] == rules->_pre_rule[0])
      first = get_first_rec(first, tmp);
    else if (islower(tmp->_rule[0]))
      {
	if (first)
	  first = realloc(first, strlen(first) + 2);
	else{
	  first = calloc(2, 1);
	}
	sprintf(first, "%s%c" ,first, tmp->_rule[0]);
      }
    tmp = tmp->_next;
  }
  return first;
}

char	*get_first(rules_t *rules, char *rule_name){
  char *first;
  rules_t *tmp = rules;

  while (tmp){
    if (strcmp(tmp->_pre_rule, rule_name)){
      first = get_first_rec(first, rules);
    }
    tmp = tmp->_next;
  }
  return first;
}
