#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <time.h>
#include "irc_server.h"
#include "commands.h"
#include "color.h"

CMD	(PRIVMSG)
{
	char *p;
	char *msg;

	if (strlen(args) < 1) {
		send_response(user, 411, ":No recipient given\r\n");
		return ;
	}
	p = args;
	while (*p && !isspace(*p))
		++p;
	*p = 0;
	msg = ++p;
	while (*msg && isspace(*msg))
		++msg;
	if (*msg == ':')
		fmt_pm(user, args, ++msg);
	else {
		p = msg;
		while (*p && !isspace(*p))
			++p;
		*p++ = 0;
		fmt_pm(user, args, msg);
	}
}

CMD	(LIST)
{
	char buffer[MAX_TOPIC_NAME + MAX_CHAN_NAME + 64];
	t_channel *it = g_server.channels;

	while (it)
	{
		memset(buffer, 0, 256);
		snprintf(buffer, sizeof(buffer), "%s 1 :%s\r\n",
			 it->name,
			 it->topic);
		send_response(user, 322,
			      buffer);
		it = it->next;
	}
	send_response(user, 323, ":End of list\r\n");
}

CMD     (TOPIC)
{
	t_channel *chan;
	char *topic = args;

	while (*topic && !isspace(*topic))
		++topic;
	*topic = 0;
	++topic;
	while (*topic && isspace(*topic))
		++topic;
	if (*topic == ':')
		++topic;
	chan = get_chan_by_name(args);
	if (strlen(args) < 1)
		send_response(user, 461, "%s :Not enough parameters\r\n", args);
	else if (chan == NULL)
		send_response(user, 442, " :Not on that channel\r\n");
	else
	{
		if (strlen(topic) < 1)
			get_topic(chan, user);
		else
			set_topic(chan, user, topic);
	}
}

CMD	(QUIT)
{
	char *p;

	close_fd(user);
	user->has_quit = 1;
	if (*args == ':')
		notify_quit(user, ++args);
	else
	{
		p = args;
		while (*p && !isspace(*p))
			++p;
		*p = 0;
		notify_quit(user, args);
	}
}

CMD	(PING)
{
	char buffer[512];
	char host[128];
	char dst[128];
	socklen_t len;
	struct sockaddr_in addr;
	
	memset(buffer, 0, sizeof(buffer));
	if (!getsockname(user->user_fd, (struct sockaddr*)&addr, &len)) {
		if (!getnameinfo((struct sockaddr*)&addr,
				 len, dst, sizeof(dst),
				 NULL, 0, NI_NAMEREQD)) {
			if (*args == ':')
				++args;
			if (!gethostname(host, sizeof(host))) {
				snprintf(buffer, sizeof(buffer),
					 ":%s PONG %s :%s\r\n",
					 host, dst, args);
				send(user->user_fd, buffer, strlen(buffer), 0);
			}
		}
	}
}
