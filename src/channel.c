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

int		remove_user2(t_channel *channel,
			     int n,
			     t_user *user)
{
  notify_part(channel, user);
  channel->users[n] = NULL;
  channel->user_count -= 1;
  print_time(time(NULL));
  printf("%s left channel %s\n", user->name,
	 channel->name);
  if (channel->user_count < 1)
    destroy_channel(channel);
  return (1);
}

int		remove_user(t_user *user,
			    char *chan)
{
  t_channel	*channel;
  int		n;

  channel = get_chan_by_name(chan);
  if (channel == NULL || *chan != '#')
    {
      send_response(user, 403,
		    "%s :No such channel\r\n", chan);
      return (0);
    }
  n = -1;
  while (++n < MAX_USERS)
    {
      if (channel->users[n] != NULL)
	{
	  if (!strcmp(user->name,
		      channel->users[n]->name))
	    return (remove_user2(channel, n, user));
	}
    }
  send_response(user, 442,
		"%s :You're not on the channel\r\n", chan);
  return (0);
}

void		destroy_channel(t_channel *channel)
{
  print_time(time(NULL));
  printf(FG_YELLOW "channel %s destroyed\n"
	 FG_RESET, channel->name);
  if (channel->next != NULL)
    channel->next->prev = channel->prev;
  if (channel == g_server.channels)
    g_server.channels = channel->next;
  else
    {
      channel->prev->next = channel->next;
      if (channel == g_server.channels->prev)
	g_server.channels->prev = channel->prev;
    }
  free(channel);
  g_server.channel_count -= 1;
}

void		push_channel(t_channel *new_channel,
			     t_user *user)
{
  if (g_server.channels == NULL)
    {
      g_server.channels = new_channel;
      new_channel->prev = new_channel;
    }
  else
    {
      g_server.channels->prev->next = new_channel;
      new_channel->prev = g_server.channels->prev;
      g_server.channels->prev = new_channel;
    }
  print_time(time(NULL));
  printf(FG_BLUE "channel %s created by %s\n"
	 FG_RESET, new_channel->name,
	 user->name);
}

void		create_channel(t_user *user, char *name)
{
  t_channel	*new_channel;

  new_channel = (t_channel*)calloc(1, sizeof(t_channel));
  if (new_channel == NULL)
    {
      fprintf(stderr, "ALLOC_CHANNEL_FAIL\n");
      send_response(user, 437, ":Tempory unavailable\r\n");
      return ;
    }
  new_channel->tm_create = time(NULL);
  new_channel->users[0] = user;
  strcpy(new_channel->name, name);
  g_server.channel_count += 1;
  new_channel->user_count = 1;
  push_channel(new_channel, user);
  notify_join(new_channel, user);
}
