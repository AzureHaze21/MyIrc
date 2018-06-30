#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "irc_server.h"
#include "color.h"

void	get_topic(t_channel *chan,
		  t_user *user)
{
  if (chan != NULL)
    {
      if (strlen(chan->topic) < 1)
	send_response(user, 331, "%s :No topic is set\r\n",
		      chan->name);
      else
	{

	  send_response(user, 331, "%s :%s\r\n",
			chan->name, chan->topic);
	}
    }
}

void	set_topic(t_channel *chan,
		  t_user *user,
		  char *topic)
{
  if (chan)
    {
      memset(chan->topic, 0, sizeof(chan->topic));
      strcpy(chan->topic, topic);
      notify_topic_change(chan, user);
      print_time(time(NULL));
      printf(FG_BLUE "%s: topic changed to: %s\n"
	     FG_RESET,
	     chan->name,
	     chan->topic);
    }
}
