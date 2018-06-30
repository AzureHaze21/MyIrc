#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "irc_server.h"
#include "color.h"

t_irc_server	g_server;

int		irc_init_socket()
{
  g_server.listenfd = socket(AF_INET, SOCK_STREAM, 0);
  if (g_server.listenfd < 0)
    {
      fprintf(stderr, "SOCK_ERR: %d %s\n",
	      errno, strerror(errno));
      return (-1);
    }
  if (setsockopt(g_server.listenfd,
		 SOL_SOCKET, SO_REUSEADDR,
		 &(int){ 1 }, sizeof(int)) != 0)
    {
      fprintf(stderr, "SETSOCKOPT_ERR: %d %s\n",
	      errno, strerror(errno));
      return (-1);
    }
  return (0);
}

int		irc_bind(in_port_t port)
{
  int   ret;

  g_server.addr.sin_family = AF_INET;
  g_server.addr.sin_port = htons(port);
  g_server.addr.sin_addr.s_addr = INADDR_ANY;
  ret = bind(g_server.listenfd,
	     (struct sockaddr*)&g_server.addr,
	     sizeof(g_server.addr));
  if (ret < 0)
    {
      printf("BIND_ERR: %d %s\n", errno, strerror(errno));
      return (-1);
    }
  return (0);
}

int		irc_listen()
{
  int   ret;

  ret = listen(g_server.listenfd, 10);
  if (ret < 0)
    {
      printf("listen error : %d %s\n", errno, strerror(errno));
      return (-1);
    }
  return (0);
}

int		irc_init(in_port_t port)
{
  if (irc_init_socket() != 0)
    return (84);
  if (irc_bind(port) != 0)
    return (84);
  if (irc_listen() != 0)
    return (84);
  strcpy(g_server.name, "irc_server1");
  g_server.users = NULL;
  g_server.channels = NULL;
  g_server.user_count = 0;
  g_server.user_count_max = 0;
  g_server.tm_online = time(NULL);
  gethostname(g_server.hostname, sizeof(g_server.hostname));
  print_time(time(NULL));
  printf(FG_BLUE "server started on port %d\n"
	 FG_RESET,
	 (int)port);
  return (0);
}

void		irc_exit()
{
  print_time(time(NULL));
  printf(FG_YELLOW "shutting down server\n"
	 FG_RESET);
  close(g_server.listenfd);
  destroy_users();
  destroy_channels();
  exit(0);
}
