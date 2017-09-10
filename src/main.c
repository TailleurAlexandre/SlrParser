#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "slr.h"
#include "parsing_table.h"
#include "parsing_statements.h"
#include "closure.h"

void            prompt_read(){
    size_t      size = 0, nb;
    char        *line = NULL;

    write(1, ">>",2);
    while ((nb = getline(&line, &size, stdin)) != -1){
        nb--;
        if (!strncmp(line, "FIRST", nb)) {
            print_firsts();
        } 
        else if (!strncmp(line, "FOLLOW", nb)){
            print_follows();
        }
        else if (!strncmp(line, "TABLE", nb)){
            print_table();
        }
        else if (!strncmp(line, "GOTO", nb)) {
            print_goto_table();
        }
        else if (!strncmp(line, "ACTION", nb)){
            print_action_table();
        }
        else if (!strncmp(line, "I", 1)){
            print_state(atoi(line+1));
        }
        else if (!strncmp(line, "exit", nb)){
            break;
        } 
        else {
            if (parse_string(line))
                printf("ACCEPT\n");
            else
                printf("ERROR\n");
        }
        write(1,">>", 2);
    }
}

int		main(int ac, char **av)
{
    if (ac < 2){
        return EXIT_FAILURE;
    }
    init(av[1]);
    prompt_read();
    return EXIT_SUCCESS;
}
