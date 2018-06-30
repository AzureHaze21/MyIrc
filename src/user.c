#include <malloc.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/time.h>
#include "irc_server.h"
#include "commands.h"
#include "color.h"

static int		id_counter = 1;

void			read_user_cmd(t_user *user)
{
  char			buffer[0x1000];
  int			ret;

  memset(buffer, 0, sizeof(buffer));
  ret = recv(user->user_fd, buffer, sizeof(buffer), 0);
  if (ret < 0)
    {
      fprintf(stderr, "READ_CMD_ERR %d %s\n",
	      errno, strerror(errno));
      close_fd(user);
      return ;
    }
  else if (ret == 0)
    close_fd(user);
  else
    exec_cmd(user, buffer);
}

void			destroy_user(t_user *user)
{
  print_time(time(NULL));
  printf(FG_YELLOW "%s left the server\n"
	 FG_RESET, user->name);
  close_fd(user);
  if (user->next != NULL)
    user->next->prev = user->prev;
  if (user == g_server.users)
    {
      g_server.users = user->next;
    }
  else
    {
      user->prev->next = user->next;
      if (user == g_server.users->prev)
	g_server.users->prev = user->prev;
    }
  if (!user->has_quit)
    notify_quit(user, "EOT");
  free(user);
  g_server.user_count -= 1;
}

void			push_user(t_user *new_user)
{
  if (g_server.users == NULL)
    {
      g_server.users = new_user;
      new_user->prev = new_user;
    }
  else
    {
      g_server.users->prev->next = new_user;
      new_user->prev = g_server.users->prev;
      g_server.users->prev = new_user;
    }
  print_time(time(NULL));
  printf(FG_BLUE "%s joined the server\n"
	 FG_RESET, new_user->name);
}

void			create_user(int fd,
				    struct sockaddr_in *addr)
{
  t_user		*new_user;

  new_user = (t_user*)calloc(1, sizeof(t_user));
  if (new_user == NULL)
    {
      fprintf(stderr, "ALLOC_USER_FAIL\n");
      close_fd(new_user);
      return ;
    }
  strcpy(new_user->saddr, inet_ntoa(addr->sin_addr));
  new_user->user_fd = fd;
  new_user->in_lobby = 1;
  new_user->access = GUEST;
  new_user->join_count = 0;
  new_user->tm_connect = time(NULL);
  sprintf(new_user->name, "guest%d", id_counter);
  strcpy(new_user->ident, "anon");
  g_server.user_count += 1;
  g_server.user_count_max += 1;
  id_counter++;
  push_user(new_user);
}

void			add_user()
{
  int			new_fd;
  struct sockaddr_in	addr;
  socklen_t		addlen;

  addlen = sizeof(addr);
  new_fd = accept(g_server.listenfd,
		  (struct sockaddr*)&addr, &addlen);
  if (new_fd < 0)
    {
      fprintf(stderr, "ACCEPT_ERR %d %s\n",
	      errno, strerror(errno));
      return ;
    }
  print_time(time(NULL));
  printf(FG_GREEN "host %s:%u connected\n" FG_RESET,
	 inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
  create_user(new_fd, &addr);
}
