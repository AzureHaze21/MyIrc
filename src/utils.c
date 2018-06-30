#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "irc_server.h"

int		fmt_buffer(t_user *user,
			   char *buffer,
			   int code,
			   const char *fmt, ...)
{
  int		ret;
  va_list	va;

  va_start(va, fmt);
  ret = sprintf(buffer, ":%s %03d %s ",
		g_server.hostname, code, user->name);
  ret += vsnprintf(buffer + ret, 512 - ret, fmt, va);
  va_end(va);
  return (ret);
}

t_channel	*get_chan_by_name(char *name)
{
  t_channel	*chan;

  chan = g_server.channels;
  while (chan)
    {
      if (!strcmp(chan->name, name))
	return (chan);
      chan = chan->next;
    }
  return (NULL);
}

t_user		*get_user_by_name(char *name)
{
  t_user	*user;

  user = g_server.users;
  while (user)
    {
      if (!strcmp(user->name, name))
	return (user);
      user = user->next;
    }
  return (NULL);
}

t_user		*get_chan_user(t_channel *chan,
			       char *name)
{
  int		n;

  n = 0;
  while (n < MAX_USERS)
    {
      if (chan->users[n] != NULL)
	{
	  if (!strcmp(chan->users[n]->name,
		      name))
	    return (chan->users[n]);
	}
      ++n;
    }
  return (NULL);
}
