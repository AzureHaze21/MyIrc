#ifndef _IRC_SERVER_H_
#define _IRC_SERVER_H_

#include <stdarg.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <time.h>

#define USAGE	"USAGE: ./server port"

#define CHANNEL_BUFFSIZE	0x1000
#define MAX_USERS	        32
#define CMD_BUFFSIZE		0x1000
#define MAX_NICKLEN		0x9
#define MAX_CHAN_NAME		200
#define MAX_JOIN		10
#define MAX_TOPIC_NAME		128

typedef enum
  {
    RESET,
    CONTINUE,
    EXIT
  }			t_irc_status;

typedef enum
  {
    GUEST,
    CHAN_OPERATOR,
    IRC_OPERATOR
  }			t_access;

typedef struct s_channel	t_channel;
typedef struct s_user		t_user;

struct			s_user
{
  //int			authenticated;
  int			has_quit;
  int			user_fd;
  int			in_lobby;
  time_t		tm_connect;
  t_access		access;
  int			join_count;
  char			name[MAX_NICKLEN + 1];
  char			ident[0x32];
  char			saddr[20];
  t_user		*prev;
  t_user		*next;
};

struct			s_channel
{
  char			name[MAX_CHAN_NAME + 1];
  char			topic[MAX_TOPIC_NAME + 1];
  int			id;
  char			buffer[CHANNEL_BUFFSIZE];
  t_user		*users[MAX_USERS];
  int			user_count;
  time_t		tm_create;
  t_channel		*prev;
  t_channel		*next;
};

typedef struct		s_irc_server
{
  int			listenfd;
  char			name[128];
  char			hostname[128];
  struct sockaddr_in	addr;
  t_channel		*channels;
  t_user		*users;
  int			user_count;
  int			user_count_max;
  int			channel_count;
  time_t		tm_online;
}			t_irc_server;

void		get_topic(t_channel*, t_user*);
void		set_topic(t_channel*, t_user*, char*);

void		notify_quit(t_user*, char *msg);
void		notify_nick_change(t_user*,char*);
void		notify_join(t_channel*, t_user*);
void		notify_part(t_channel*, t_user*);
void		notify_topic_change(t_channel*,t_user*);

int	        fmt_buffer(t_user*, char *,
			   int, const char*, ...);
void		send_chan_names(t_user*,char*);
void		send_all_names(t_user*);
void		fmt_pm(t_user*, char*, char*);
int		pm_chan_user(t_user*,
			     t_user*,
			     char*);
int		pm_chan(t_user*, char*, char*,int);
int		pm_user(t_user*,char*,char*);

t_channel	*get_chan_by_name(char*);
t_user		*get_chan_user(t_channel*,char*);
t_user		*get_user_by_name(char *name);

int		check_nick_collision(t_user*, char*);
int		chan_exists(char *name);
void		send_welcome_msg(t_user *user);
int		exec_cmd(t_user *u, char *cmd);
void		print_time(time_t tm);
char		*get_time(time_t tm);
int		send_response(t_user *,
			      int code,
			      const char *fmt, ...);
void		read_user_cmd(t_user *user);
void		add_user();

void		destroy_user(t_user *user);
void		destroy_channel(t_channel *channel);
void		create_channel(t_user *user, char *name);
int		try_join(t_user *user, char *name);
void		destroy_channels();
void		destroy_users();

int		irc_init(in_port_t port);
void		irc_exit(void);
t_irc_status	irc_loop();
void		close_fd(t_user *user);
int		remove_user(t_user *, char*);

extern	t_irc_server	g_server;

#endif
