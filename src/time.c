#include <string.h>
#include <malloc.h>
#include <time.h>
#include <stdio.h>

void		print_time(time_t tm)
{
  char		buffer[64];
  struct tm	*tminfo;

  tminfo = localtime(&tm);
  memset(buffer, 0, sizeof(buffer));
  strftime(buffer, sizeof(buffer),
	   "[%H:%M:%S]", tminfo);
  printf("%s ", buffer);
}

char		*get_time(time_t tm)
{
  char		buffer[64];
  struct tm	*tminfo;

  tminfo = localtime(&tm);
  memset(buffer, 0, sizeof(buffer));
  strftime(buffer, sizeof(buffer),
	   "%H:%M:%S ", tminfo);
  return (strdup(buffer));
}
