#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "closure.h"
#include "parsing_table.h"
#include "parsing_statements.h"

action_table_t  g_a_table;
extern rules_t  *g_rules;
goto_table_t    g_go_table;

static rules_t *find_rule_by_number(int nb)
{
    rules_t *tmp = g_rules;
    while (tmp){
        if (atoi(tmp->_name+1) == nb)
            return tmp;
        tmp = tmp->_next;
    }
    return NULL;
}

static int  pop_stack(char *stack, int cursor){
    int i = cursor;
    while (i > 0){
        if (isupper(stack[cursor])){
            i--;
            break;
        }
        stack[i]=-1;
        i--;
    }
    return i;
}

static int	find_rule_name(char **rule_names, char *rule, int size){
    int		i;

    for(i = 0; i < size; ++i){
        if (!strcmp(rule_names[i], rule))
            return i;
    }

    return -1;
}

/*
 * Function to use the parsing table to parse a string
 */
int             parse_string(char *str){
    int         i = 0;
    char        c = 0;
    int         cur_state = 1; 
    int         cur_state_number = 0;
    char        stack[512] = {0};
    int         stack_cursor = 1;

    while (i < strlen(str)){
        if (cur_state == SHIFT){
            c = str[i];
            int u;
            for (u = 0; u < strlen(g_a_table.terminals); ++u){
                if (g_a_table.terminals[u] == c)
                    break;
            }
            if (u < strlen(g_a_table.terminals) 
                    && u < g_a_table.elems[cur_state_number].size)
            {
                cur_state_number = 
                    g_a_table.elems[cur_state_number].elems[u].state_number;
                stack[stack_cursor++] = c;
                stack[stack_cursor++] = cur_state_number;
                for (u = 0; u < strlen(g_a_table.terminals); ++u){
                    if (g_a_table.terminals[u] == str[i+1])
                        break;
                }
                if (u < strlen(g_a_table.terminals) 
                        && u < g_a_table.elems[cur_state_number].size)
                {    
                    cur_state = g_a_table.elems[cur_state_number].elems[u].action;
                }
                else
                {
                    return 0;
                }
            }
            else{
                return 0;
            }
            i++;
        }
        else if (cur_state == REDUCE){
            c = str[i];
            int u;
            for (u = 0; u < strlen(g_a_table.terminals); ++u){
                if (g_a_table.terminals[u] == c)
                    break;
            }
            if (u < strlen(g_a_table.terminals) 
                    && u < g_a_table.elems[cur_state_number].size)
            {
                int nb = g_a_table.elems[cur_state_number].elems[u].state_number;
                rules_t *r = find_rule_by_number(nb);
                nb = find_rule_name(g_go_table.rule_names, r->_pre_rule, g_go_table.size);
                stack[--stack_cursor] = -1;
                stack[--stack_cursor] = -1;
                stack_cursor = pop_stack(stack, stack_cursor);
                int tmp_state = stack[stack_cursor-1];
                cur_state_number = g_go_table.state_goto[nb][tmp_state];
                if (g_go_table.state_goto[nb][tmp_state] == 0)
                {
                    return 0;
                }
                stack[stack_cursor++] = g_go_table.rule_names[nb][0];
                stack[stack_cursor++] = cur_state_number;
                for (u = 0; u < strlen(g_a_table.terminals); ++u){
                    if (g_a_table.terminals[u] == str[i])
                        break;
                }
                if (u < strlen(g_a_table.terminals) 
                        && u < g_a_table.elems[cur_state_number].size)
                {    
                    cur_state = g_a_table.elems[cur_state_number].elems[u].action;
                }
                else{
                    return 0;
                }
            }
            else{
                return 0;
            }
        }
        else if (cur_state == ACCEPT)
            break;
        else
            return 0;
   }
    return 1;
}


static goto_table_t construct_goto_table(rules_t *rules,
        goto_states_t *states, int state_number){
    goto_table_t		go_table;
    goto_states_t		*tmp = states;

    go_table.state_goto = NULL;
    go_table.rule_names = NULL;
    go_table.rules_number = 0;
    go_table.size = state_number;
    while (tmp){
        if (!tmp->copy_of){
            int	i;
            for (i = 0; i < tmp->size; ++i){
                if (isupper(tmp->states_follow_up[i]->_character))
                {
                    char t[] = {tmp->states_follow_up[i]->_character, 0};
                    int	nb = find_rule_name(go_table.rule_names,
                            t, go_table.rules_number);
                    if (nb == -1){
                        go_table.rule_names = realloc(go_table.rule_names,
                                (go_table.rules_number + 1) * sizeof(char *));
                        go_table.rule_names[go_table.rules_number] = malloc(2);
                        strcpy(go_table.rule_names[go_table.rules_number], t);
                        go_table.state_goto = realloc(go_table.state_goto,
                                (go_table.rules_number + 1) * sizeof(int **));
                        go_table.state_goto[go_table.rules_number] = calloc(state_number,
                                sizeof(int *));
                        get_follow(rules, go_table.rule_names[go_table.rules_number]);
                        get_first(rules, go_table.rule_names[go_table.rules_number]);
                        nb = go_table.rules_number++;
                    }
                    go_table.state_goto[nb][tmp->_number] = 
                        tmp->states_follow_up[i]->_number;
                }
            }
        }
        tmp = tmp->_next;
    }
    return go_table;
}

static action_table_t     add_action(action_t action, int number, 
        char c, action_table_t a_table, int number_action) {
    char                *res;

    if ((res = strchr(a_table.terminals, c))) {
        int dif = res - a_table.terminals;
        if (a_table.elems[number].size < strlen(a_table.terminals)) {
            a_table.elems[number].size = strlen(a_table.terminals);
            a_table.elems[number].elems = realloc(a_table.elems[number].elems, 
                    (strlen(a_table.terminals) + 1) * sizeof(action_elem_t));
        }
        a_table.elems[number].elems[dif].terminal = c;
        a_table.elems[number].elems[dif].state_number = number_action;
        a_table.elems[number].elems[dif].action = action;
    } 
    else {
        char buf[2] = {c, 0};
        a_table.terminals = merge_strings(a_table.terminals, buf); 
        a_table.elems[number].size = strlen(a_table.terminals);
        a_table.elems[number].elems = realloc(a_table.elems[number].elems, 
                (strlen(a_table.terminals) + 1) * sizeof(action_elem_t));
        a_table = add_action(action, number, c, a_table, number_action);
    }
    return a_table;
}

/*
 * function to add end of parsing table ACCEPT
 */
static action_table_t   add_end_state(action_table_t a_table, rules_t *rule, 
        goto_states_t *states){
    goto_states_t *tmp = states;
    int i;

    while (tmp){
        for (i = 0; i < tmp->size; ++i){
            if (tmp->states_follow_up[i]->_character == 0 
                    && tmp->states_follow_up[i]->closure->elements[0].rule == rule){
                break;
            }
        }
        if (i != tmp->size)
            break;
        tmp = tmp->_next;
    }
    for (i = 0; i < strlen(a_table.terminals);++i){
        if (a_table.terminals[i] == '\n')
            break;
    }
    a_table = add_action(ACCEPT, tmp->_number, '\n', a_table, 0);
    return a_table;
}

/*
 * function to construct action table
 */
static action_table_t	construct_action_table(rules_t *rules, 
        goto_states_t *states, int state_number){
    action_table_t  a_table;
    goto_states_t   *tmp = states;
    char            *first;

    while (tmp && tmp->_prev){
        tmp = tmp->_prev;
    }
    first = get_first(rules, tmp->closure->elements[0].rule->_rule);
    a_table.terminals = first;
    a_table.elems = calloc(state_number, sizeof(action_vector_t));
    a_table.state_nb = state_number;
    while (tmp){
        int i;
        for (i = 0; i < tmp->size;++i){
            if (!tmp->copy_of){
                if (!isupper(tmp->states_follow_up[i]->_character) && tmp->states_follow_up[i]->_character != 0){ 
                    a_table =  add_action(SHIFT, tmp->_number, 
                            tmp->states_follow_up[i]->_character, a_table,
                            tmp->states_follow_up[i]->_number);
                }
            }
        }
        if (tmp->size == 0  && !tmp->copy_of)
        {
            char *follow = get_saved_follow(tmp->closure->elements[0].rule->_pre_rule);
            int o;
            if (follow != NULL){
                a_table.terminals = merge_strings(a_table.terminals, follow);
                for (o = 0; o < strlen(a_table.terminals); ++o){
                    if (strchr(follow, a_table.terminals[o]))
                        a_table = add_action(REDUCE, tmp->edge->_number,
                                a_table.terminals[o], 
                                a_table, atoi(tmp->closure->elements[0].rule->_name+1));
                }
            }
        }
        tmp = tmp->_next;
    } 
    a_table = add_end_state(a_table, rules, states);
    return a_table;
}

void    print_action_table(){
    int i;

    for(i = 0; i < g_a_table.state_nb; ++i){
        int o;
        printf ("%d: ", i);
        for (o = 0; o < g_a_table.elems[i].size; ++o){
            action_t action = g_a_table.elems[i].elems[o].action;
            char c = g_a_table.terminals[o] == '\n' ? '$' : g_a_table.terminals[o];
            if (g_a_table.elems[i].elems[o].state_number != 0) 
                printf("%c[%c%d] ", c, 
                        (action == SHIFT) ? 's': 
                        (action == REDUCE) ? 'r':' ', 
                        g_a_table.elems[i].elems[o].state_number);
            else if (g_a_table.elems[i].elems[o].action == ACCEPT)
                printf("%c[ACCEPT]", c);
        }
        printf("\n");
    }

}

void    print_goto_table(){
    int i;

    for(i = 0; i < g_a_table.state_nb; ++i){
        int o;
        printf ("%d: ", i);
        for (o = 0; o < g_go_table.rules_number; o++){
            if (g_go_table.state_goto[o][i])
                printf("%s [%d] ", g_go_table.rule_names[o], g_go_table.state_goto[o][i]);
        }
        printf("\n");
    }
}

void    print_table(){
    int i;

    for(i = 0; i < g_a_table.state_nb; ++i){
        int o;
        printf ("%d: ", i);
        for (o = 0; o < g_a_table.elems[i].size; ++o){
            action_t action = g_a_table.elems[i].elems[o].action;
            char c = g_a_table.terminals[o] == '\n' ? '$' : g_a_table.terminals[o];
            if (g_a_table.elems[i].elems[o].state_number != 0) 
                printf("%c[%c%d] ", c, 
                        (action == SHIFT) ? 's': 
                        (action == REDUCE) ? 'r':' ', 
                        g_a_table.elems[i].elems[o].state_number);
            else if (g_a_table.elems[i].elems[o].action == ACCEPT)
                printf("%c[ACCEPT]", c);
        }
        printf("\t\t\t");
        for (o = 0; o < g_go_table.rules_number; o++){
            if (g_go_table.state_goto[o][i])
                printf("%s [%d] ", g_go_table.rule_names[o], g_go_table.state_goto[o][i]);
        }
        printf("\n");
    }
}

void	construct_parsing_table(rules_t *rules, goto_states_t *states, int state_number)
{
    g_go_table = construct_goto_table(rules, states, state_number);
    g_a_table = construct_action_table(rules, states, state_number);
}
