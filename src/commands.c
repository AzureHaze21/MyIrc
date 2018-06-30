#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <time.h>
#include "irc_server.h"
#include "commands.h"
#include "color.h"

static t_command	g_irc_commands[] =
  {
    SET_CMD(TIME),
    SET_CMD(NICK),
    SET_CMD(USER),
    SET_CMD(QUIT),
    SET_CMD(USERS),
    SET_CMD(JOIN),
    SET_CMD(PART),
    SET_CMD(PRIVMSG),
    SET_CMD(LIST),
    SET_CMD(NAMES),
    SET_CMD(TOPIC),
    SET_CMD(ISON),
    SET_CMD(PING),
//    SET_CMD(VERSION),
//    SET_CMD(ADMIN)
  };

static int	ncmd = sizeof(g_irc_commands) / sizeof(t_command);

void		transfer_exec(t_user *user,
						  char *cmd)
{
  char	*p;
  char	*p_args;
  int	i;

  p = cmd;
  while (*p && isspace(*p))
    ++p;
  cmd = p;
  while (*p && !isspace(*p))
    ++p;
  *p++ = 0;
  while (*p && isspace(*p))
    ++p;
  p_args = p;
  i = 0;
  while (i < ncmd)
    {
      if (!strcasecmp(cmd, g_irc_commands[i].name))
	{
	  g_irc_commands[i].command(user, p_args);
	  return ;
	}
      ++i;
    }
  send_response(user, 421, "%s :Unknown command\r\n", cmd);
}

int		exec_cmd(t_user *user,
			 char *cmd)
{
  int		i;
  int		j;
  char		buffer[0x1000];

  i = 0;
  j = 0;
  memset(buffer, 0, sizeof(buffer));
  while (cmd[i])
    {
      if (cmd[i] == '\n')
	{
	  memset(buffer, 0, sizeof(buffer));
	  strncpy(buffer, cmd + j, i - j);
	  transfer_exec(user, buffer);
	  j = i + 1;
	}
      ++i;
    }
  return (0);
}

void		send_welcome_msg(t_user *user)
{
  char		buffer[0x10000];
  int		ret;

  memset(buffer, 0, sizeof(buffer));
  ret = fmt_buffer(user, buffer, 1,
		   ":Welcome to myIRC server\r\n");
  ret += fmt_buffer(user, buffer + ret, 2,
		   ":Your host is pc-paul "		\
		   "running version myIRC1.0\r\n");
  ret += fmt_buffer(user, buffer + ret, 3,
		   ":This server was created on %s\r\n",
		   get_time(g_server.tm_online));
  ret += fmt_buffer(user, buffer + ret, 4,
		    "%s myIRCv1.0 o o\r\n",
		    g_server.name);
  ret += fmt_buffer(user, buffer + ret, 251,
		    ":There are %d users online"	\
		    " and %d channels formed\r\n",
		    g_server.user_count,
		    g_server.channel_count);
  ret += fmt_buffer(user, buffer + ret, 422,
		    ":MOTD File is missing\r\n");
  send(user->user_fd, buffer, ret, 0);
}

CMD		(USER)
{
  char		*p;

  if (strlen(args) < 1)
    {
      send_response(user, 461,
		    ":Not enough parameters\r\n");
      return ;
    }
  p = args;
  while (*p && !isspace(*p))
    ++p;
  *p = 0;
  if (strlen(args) < sizeof(user->ident))
    {
      strncpy(user->ident, args,
	      sizeof(user->ident) - 1);
      send_welcome_msg(user);
    }
  else
    send_response(user, 461, ":Bad parameters\r\n");
}

CMD		(TIME)
{
  char	*tmstr;

  tmstr = get_time(time(NULL));
  if (tmstr)
    {
      send_response(user, 391, ":%s\r\n", tmstr);
      free(tmstr);
    }
  else
    send_response(user, 391, ":internal error\r\n");
}
