#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "irc_server.h"

static int	check_args(int argc,
			   char **argv)
{
  if (argc != 2)
    {
      printf("%s\n", USAGE);
      return (84);
    }
  if (!strcmp(argv[1], "--help"))
    {
      printf("%s\n", USAGE);
      return (84);
    }
  return (0);
}

int		main(int argc, char **argv)
{
  int		port;
  int		ret;

  if (check_args(argc, argv) == 84)
    return (84);
  else
    {
      ret = CONTINUE;
      port = strtoul(argv[1], NULL, 0);
      if (port != 0)
	{
	  if (!irc_init(port))
	    while (ret == CONTINUE)
	      ret = irc_loop();
	}
      else
	{
	  fprintf(stderr, "port out of range\n");
	  return (84);
	}
    }
  return (0);
}
