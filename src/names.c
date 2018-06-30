#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include "irc_server.h"

void		send_all_names(t_user *user)
{
  t_channel	*it;
  char		names[1024];
  int		n;

  it = g_server.channels;
  memset(names, 0, sizeof(names));
  while (it)
    {
      n = 0;
      sprintf(names, "= %s :", it->name);
      if (it->user_count != 0)
	{
	  while (it->users[n] != 0)
	    {
	      strcat(names, it->users[n]->name);
	      strcat(names, " ");
	      ++n;
	    }
	  strcat(names, "\r\n");
	  send_response(user, 353, "%s", names);
	}
      it = it->next;
    }
  send_response(user, 366,
		":End of NAMES list\r\n");
}

void		send_chan_names(t_user *user, char *chan)
{
  t_channel	*it;
  char		names[1024];
  int		n;

  it = get_chan_by_name(chan);
  if (it)
    {
      memset(names, 0, sizeof(names));
      n = 0;
      sprintf(names, "= %s :", it->name);
      if (it->user_count != 0)
	{
	  while (it->users[n] != 0)
	    {
	      strcat(names, it->users[n]->name);
	      strcat(names, " ");
	      ++n;
	    }
	  strcat(names, "\r\n");
	  send_response(user, 353, "%s", names);
	}
    }
  send_response(user, 366,
		"%s :End of NAMES list\r\n", chan);
}

int		check_nick_collision(t_user *user, char *name)
{
  t_user	*it;

  it = g_server.users;
  while (it)
    {
      if (!strcmp(it->name, name) &&
	  strcmp(name, user->name) != 0)
	return (1);
      it = it->next;
    }
  return (0);
}
