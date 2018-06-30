#include <stdarg.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include "irc_server.h"

int		send_response(t_user *user, int code,
			      const char *fmt, ...)
{
  char          buff[0x1000];
  va_list       va;
  size_t        to_send;
  int           ret;

  memset(buff, 0, sizeof(buff));
  va_start(va, fmt);
  to_send = sprintf(buff, ":%s %03d %s ",
		    g_server.hostname, code, user->name);
  to_send += vsnprintf(buff + to_send,
		       sizeof(buff) - to_send
		       , fmt, va);
  va_end(va);
  ret = send(user->user_fd, buff, to_send, 0);
  if (ret < 0)
    {
      fprintf(stderr, "SEND_ERR %d %s\n",
	      errno, strerror(errno));
      return (-1);
    }
  return (to_send);
}

int		pm_user(t_user *sender,
			char *name,
			char *msg)
{
  t_user	*recipient;
  char		buffer[0x800];
  int		ret;

  ret = 0;
  recipient = get_user_by_name(name);
  if (recipient == NULL)
    return (411);
  memset(buffer, 0, sizeof(buffer));
  ret = sprintf(buffer,
		":%s!%s@%s %s\r\n",
		sender->name, sender->ident,
		sender->saddr,
		msg);
  send(recipient->user_fd, buffer, ret, 0);
  return (ret);
}

int		pm_chan_user(t_user *sender,
			     t_user *dst,
			     char *msg)
{
  char		buffer[0x800];
  int		ret;

  memset(buffer, 0, sizeof(buffer));
  ret = sprintf(buffer,
		":%s!%s@%s %s\r\n",
		sender->name,
		sender->ident,
		sender->saddr,
		msg);
  ret = send(dst->user_fd, buffer, ret, 0);
  return (ret);
}

int		pm_chan(t_user *sender,
			char *chan,
			char *msg,
			int self)
{
  t_channel	*dst;
  int		n;

  dst = get_chan_by_name(chan);
  if (dst == NULL)
    return (411);
  n = 0;
  while (n < MAX_USERS)
    {
      if (dst->users[n])
	{
	  if (!(!self && dst->users[n] == sender))
	    pm_chan_user(sender,
			 dst->users[n],
			 msg);
	}
      ++n;
    }
  return (0);
}

void		fmt_pm(t_user *src,
		       char *dst,
		       char *msg)
{
  char          buffer[0x200];

  memset(buffer, 0, sizeof(buffer));
  sprintf(buffer,
	  "PRIVMSG %s :%s",
	  dst,
	  msg);
  if (*dst != '#')
    pm_user(src, dst, buffer);
  else
    pm_chan(src, dst, buffer, 0);
}
