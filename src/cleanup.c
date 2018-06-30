#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "irc_server.h"

void		close_fd(t_user *user)
{
  if (user->user_fd != -1)
    {
      if (close(user->user_fd) != 0)
	{
	  fprintf(stderr, "CLOSE_ERR: %d %s\n",
		  errno, strerror(errno));
	}
    }
  user->user_fd = -1;
}

void		destroy_users()
{
  t_user	*curr;
  t_user	*next;

  curr = g_server.users;
  while (curr)
    {
      next = curr->next;
      destroy_user(curr);
      curr = next;
    }
}

void		destroy_channels()
{
  t_channel	*curr;
  t_channel	*next;

  curr = g_server.channels;
  while (curr)
    {
      next = curr->next;
      destroy_channel(curr);
      curr = next;
    }

}
