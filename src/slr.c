#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "slr.h"
#include "closure.h"

rules_t		*g_rules = NULL;

/*
 * Augment of first rule
 */

static void		augment_rule(){
  rules_t		*augment;
  if (g_rules == NULL)
    return ;
  augment = calloc(1, sizeof(rules_t));
  g_rules->_prev = augment;
  augment->_next = g_rules;
  augment->_name = "R0";
  augment->_pre_rule = "S";
  augment->_rule = g_rules->_line;
  augment->_line = malloc(strlen(g_rules->_line) + 3); 
  sprintf(augment->_line, "%s>%s", "S", g_rules->_line);  
  g_rules = augment;
}

/*
** Function to parse file given in argv[1]
*/

static void		parse_rules(FILE *file_ptr){
  char *line = NULL;
  size_t n = 0;
  rules_t *head = g_rules,*tail = NULL;
  char *save_ptr;
  ssize_t lim = 0;

  while ((lim = getline(&line, &n, file_ptr)) != -1){
    tail = calloc(1, sizeof(rules_t));
    line[lim] = 0;
    tail->_name = line;
    // getline of the line below the rule name
    // _pre_rule is letter before >
    // _rule is letters after >
    line = NULL;
    n = 0;
    lim = getline(&line, &n, file_ptr);
    line[lim] = 0;
    tail->_pre_rule = strtok_r(line, ">", &save_ptr);
    tail->_rule = strtok_r(NULL, "\n", &save_ptr);
    tail->_line = line;
    printf("%s", tail->_name);
    if (head != NULL){
      head->_next = tail;
      tail->_prev = head;
    }
    head = tail;
    line = NULL;
    n = 0;
  }
  while (head && head->_prev){
    head = head->_prev;
  }
  g_rules = head;
  augment_rule();
}

int		init(const char *file_name){
  FILE *file_ptr = fopen(file_name, "r+");
  if (file_ptr == NULL)
    return 1;
  parse_rules(file_ptr);
  create_closure(g_rules);
  return 0;
}
