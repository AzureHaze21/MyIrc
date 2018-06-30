#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "irc_server.h"
#include "commands.h"

int		try_join(t_user *user,
			 char *name)
{
  t_channel     *it;
  int           i;

  i = -1;
  it = get_chan_by_name(name);
  if (it)
    {
      if (get_chan_user(it, user->name) != NULL)
	return (1);
      while (i < MAX_USERS)
	{
	  if (!it->users[++i])
	    {
	      it->users[i] = user;
	      it->user_count += 1;
	      user->join_count += 1;
	      notify_join(it, user);
	      return (1);
	    }
	}
    }
  return (0);
}

CMD		(JOIN)
{
  char          name[MAX_CHAN_NAME];
  int           i;

  i = -1;
  memset(name, 0, sizeof(name));
  if (strlen(args) == 0)
    {
      send_response(user, 461,
		    "JOIN :Not enough parameters\r\n");
      return ;
    }
  if (*args != '#' || strlen(args) >= MAX_CHAN_NAME)
    {
      send_response(user, 403,
		    "args :No such channel\r\n");
      return ;
    }
  while (args[++i] && !isspace(args[i]))
    name[i] = args[i];
  if (user->join_count >= MAX_JOIN)
    send_response(user, 405, ":You have joined"\
		  "too many channels\r\n");
  else if (!try_join(user, name))
    create_channel(user, name);
}
