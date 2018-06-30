#include <string.h>
#include <stdio.h>
#include "irc_server.h"
#include "commands.h"

void notify_topic_change(t_channel *chan,
			 t_user *user)
{
	char buffer[0x300];

	memset(buffer, 0, sizeof(buffer));
	sprintf(buffer, "TOPIC %s :%s", chan->name, chan->topic);
	pm_chan(user, chan->name, buffer, 1);
}

void notify_nick_change(t_user *user,
			char *new)
{
	char buffer[0x300];
	t_channel *it;

	memset(buffer, 0, sizeof(buffer));
	sprintf(buffer, "NICK %s", new);
	it = g_server.channels;
	while (it)
	{
		if (get_chan_user(it, user->name))
			pm_chan(user, it->name, buffer, 0);	
		it = it->next;
	}
	pm_user(user, user->name, buffer);
}

void notify_join(t_channel *chan,
		 t_user *user)
{
	char buffer[0x300];

	memset(buffer, 0, sizeof(buffer));
	sprintf(buffer, "JOIN %s", chan->name);
	pm_chan(user, chan->name, buffer, 1);
	TOPIC(user, chan->name);
	NAMES(user, chan->name);
}

void notify_part(t_channel *chan,
		 t_user *user)
{
	char buffer[0x300];

	memset(buffer, 0, sizeof(buffer));
	sprintf(buffer, "PART %s :%s", chan->name,
		user->name);
	pm_chan(user, chan->name, buffer, 1);
}

void notify_quit(t_user *user,
		 char *msg)
{
	t_channel *it;
	char buffer[0x300];

	it = g_server.channels;
	if (it)
	{
		memset(buffer, 0, sizeof(buffer));
		sprintf(buffer, "QUIT %s :%s",
			user->name, msg);
		if (get_chan_user(it, user->name))
			pm_chan(user, it->name, buffer, 0);
		it = it->next;
	}
}
