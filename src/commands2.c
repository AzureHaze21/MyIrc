#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <time.h>
#include "irc_server.h"
#include "commands.h"
#include "color.h"

CMD		(USERS)
{
  char		buffer[0x1000];
  t_user	*it;
  t_user	*next;
  int		ret;
  int		len;
  
  memset(buffer, 0, sizeof(buffer));
  len = sizeof(buffer) - 1;
  it = g_server.users;
  ret = 0;
  while (it)
    {
      if (it)
	{
	  ret += snprintf(buffer + ret, len - ret, "%s", it->name);
	  next = it->next;
	}
       it = next;
       if (it)
	 ret += snprintf(buffer + ret, len - ret, " ");
    }
  send_response(user, 392, ":%d users online\r\n",
		g_server.user_count);
  send_response(user, 393, ":%s\r\n", buffer);
  send_response(user, 394, ":End of USERS list\r\n");
}

CMD		(NAMES)
{
  char		*p;

  if (strlen(args) < 1)
    send_all_names(user);
  else
    {
      if (*args != '#')
	send_response(user, 403, ":No such channel\r\n");
      else
	{
	  p = args;
	  while (*p && !isspace(*p))
	    ++p;
	  *p = 0;
	  send_chan_names(user, args);
	}
    }
}

CMD		(ISON)
{
  t_user	*it;
  char		*p;

  p = args;
  while (*p && !isspace(*p))
    ++p;
  p = 0;
  it = get_user_by_name(args);
  if (it)
    send_response(user, 303, ":%s\r\n", args);
  else
    send_response(user, 303, ":\r\n");
}

CMD		(NICK)
{
  char		*p;

  if (strlen(args) < 1)
    return ;
  p = args;
  while (*p && !isspace(*p))
    ++p;
  *p = 0;
  if (strlen(args) > MAX_NICKLEN)
    {
      send_response(user, 432, ":Erroneous nickname\r\n");
      return ;
    }
  if (check_nick_collision(user, args))
    {
      send_response(user, 436, ":Nick already in use\r\n");
      return ;
    }
  print_time(time(NULL));
  notify_nick_change(user, args);
  printf("%s is now known as ", user->name);
  memset(user->name, 0, sizeof(user->name));
  strcpy(user->name, args);
  printf("%s\r\n", user->name);
}

CMD		(PART)
{
  char		*p;
  char		*comma;

  p = args;
  comma = p;
  while (*comma)
    {
      ++comma;
      if (*comma == ' ' && *(comma+1) != ',')
	send_response(user, 403,
		      ":No such channel\r\n");
      while (isspace(*comma))
	++comma;
      if (*comma == ',' || *comma == 0)
	{
	  *comma = 0;
	  remove_user(user, p);
	  ++comma;
	  p = comma;
	  while (isspace(*p))
	    ++p;
	}
    }
}
