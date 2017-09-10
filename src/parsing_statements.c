#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "slr.h"
#include "parsing_statements.h"

follow_t	g_follows = { NULL, NULL, 0 };

follow_t	g_firsts = { NULL, NULL, 0 };

static void	save_follow(char *follow, char *rule_name);

void        print_follows(){
    int     i;

    for (i = 0; i < g_follows.size;++i){
        printf("%s:", g_follows.rule_names[i]);
        int o;
        for (o = 0; o < strlen(g_follows.follows[i]); ++i){
            if (g_follows.follows[i][o] == 10){
                printf("$");
            }
            else
                printf("%c", g_follows.follows[i][o]);
        }
    }
}

void        print_firsts(){
    int     i;

    for (i = 0; i < g_firsts.size;++i){
        printf("%s:", g_firsts.rule_names[i]);
        int o;
        for (o = 0; o < strlen(g_firsts.follows[i]); ++i){
            if (g_firsts.follows[i][o] == 10){
                printf("$");
            }
            else
                printf("%c", g_firsts.follows[i][o]);
        }
    }

}

/*
 * recursive function to get follow of rules
 */
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
			sprintf(follow, "%s%c", follow, rule->_rule[dif+1]);
		}
	}
	return follow;
}

/*
 *  get follow saved
 */

char	*get_saved_follow(char *rule_name){
	int		i = 0;

	for (i = 0; i < g_follows.size;++i){
		if (!strcmp(g_follows.rule_names[i], rule_name))
			break;
	}
	if (g_follows.size == i)
		return NULL;
	return g_follows.follows[i];
}

char	*merge_strings(char *f1, char *f2){
	int		i, o, c = 0;
	int		size1 = strlen(f1), size2 = strlen(f2);
	char		*res = calloc(size1 + size2, 1);

	for (i = 0; i < size1; ++i){
		for (o = 0; res && o < strlen(res); ++o){
			if (res[o] == f1[i])
				break;
		}
		if (res && o == strlen(res))
			res[c++] = f1[i];
	}
	for (i = 0; i < size2; ++i){
		for (o = 0; res && o < strlen(res); ++o){
			if (res[o] == f2[i])
				break;
		}
		if (res && o == strlen(res))
			res[c++] = f2[i];
	}
	return res;
}

/*
 * processing follow according to the algorithm
 */
static char	*process_follows(char *rule_name, rules_t *rules){
	rules_t	*tmp = rules;
	char		*res;
	char		*follow = NULL;

	while (tmp){
		if ((res = strstr(tmp->_rule, rule_name)) && strcmp(rule_name, tmp->_pre_rule)){
			follow = get_saved_follow(rule_name);
			char *f2 = get_saved_follow(tmp->_pre_rule);
			if (follow && f2){
				follow = merge_strings(follow, f2);
			}
			else if (f2){
				follow = malloc(strlen(f2)+1);
				follow = strcpy(follow, f2);
			}
		}
		tmp = tmp->_next;
	}
    if (follow)
        save_follow(follow, rule_name);
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
	char *follow = NULL;
	rules_t *tmp = rules;
	if (strstr(rule_name, rules->_rule)){
		follow = calloc(2,1);
		follow[0] = '\n';
	}
	while (tmp){
		follow = get_follow_single(follow, tmp, rule_name);
		tmp = tmp->_next;
	}
	save_follow(follow, rule_name);
	return process_follows(rule_name, rules);
}

/*
 * recursive function to get firsts
 */
static char *get_first_rec(char *first, rules_t *rules, char *rule_name){
	rules_t	*tmp = rules;
    if (tmp){
        if (!strcmp(rule_name, tmp->_pre_rule) && !isupper(tmp->_rule[0])){
            if (!first || !strchr(first, tmp->_rule[0])){
                if (first)
                    first = realloc(first, strlen(first) + 1);
                else
                    first = calloc(2, 1);
                sprintf(first, "%s%c", first, tmp->_rule[0]);
            }
        }
        else if (!strcmp(rule_name, tmp->_pre_rule) && isupper(tmp->_rule[0])) 
        {
            char buf[2] = {tmp->_rule[0], 0};
            tmp = tmp->_next;
            while (tmp){
                first = get_first_rec(first, tmp, buf);
                tmp = tmp->_next;
            }
        }
        else {
            first = get_first_rec(first, tmp->_next, rule_name);
        }
    }
	return first;
}

static void    save_first(char *first, char *name) {
    int		i = 0;

	for (i = 0; i < g_firsts.size;++i){
		if (!strcmp(g_firsts.rule_names[i], name))
			break;
	}
	if (i == g_firsts.size){
		g_firsts.follows = realloc(g_firsts.follows, sizeof(char *) 
                * (g_firsts.size + 1));
		g_firsts.rule_names = realloc(g_firsts.rule_names, sizeof(char *) 
                * (g_firsts.size + 1));
		g_firsts.size++;
	}
	g_firsts.follows[i] = first;
	g_firsts.rule_names[i] = name;
}

char    *get_saved_first(char *rule_name){
	int		i = 0;

	for (i = 0; i < g_firsts.size;++i){
		if (!strcmp(g_firsts.rule_names[i], rule_name))
			break;
	}
	if (g_firsts.size == i)
		return NULL;
	return g_firsts.follows[i];

}

char	*get_first(rules_t *rules, char *rule_name){
	char *first = NULL;
    first = get_first_rec(first, rules, rule_name);
    save_first(first, rule_name);
	return first;
}
