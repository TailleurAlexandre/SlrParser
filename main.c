#include <stdlib.h>
#include "slr.h"

int		main(int ac, char **av)
{
  if (ac < 2){
    return EXIT_FAILURE;
  }
  init(av[1]);
  return EXIT_SUCCESS;
}
