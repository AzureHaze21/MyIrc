#include <string.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <poll.h>
#include "irc_server.h"

static inline void	poll_user(t_user *user)
{
  struct pollfd		pfd;
  int			ret;

  pfd.fd = user->user_fd;
  pfd.events = POLLIN;
  pfd.revents = 0;
  ret = poll(&pfd, 1, 0);
  if (ret < 0)
    {
      fprintf(stderr, "POLL_ERR %d %s\n",
	      errno, strerror(errno));
      close_fd(user);
    }
  else if (ret > 0)
    {
      if (pfd.revents & (POLLIN|POLLPRI))
	read_user_cmd(user);
    }
  if (user->user_fd < 0)
    destroy_user(user);
}

static void		loop_users()
{
  t_user		*it;
  t_user		*next;

  it = g_server.users;
  while (it)
    {
      next = it->next;
      poll_user(it);
      it = next;
    }
}

t_irc_status		irc_loop()
{
  struct pollfd		pfd;
  int			ret;

  pfd.fd = g_server.listenfd;
  pfd.events = POLLIN;
  pfd.revents = 0;
  ret = poll(&pfd, 1, 50);
  if (ret < 0)
    {
      fprintf(stderr, "poll : %d %s\n",
	      errno, strerror(errno));
      return (EXIT);
    }
  else if (ret > 0)
    {
      if (pfd.revents & POLLIN)
	add_user();
    }
  loop_users();
  return (CONTINUE);
}
