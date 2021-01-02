#include "global.h"

void write_msg(int fd, const char *format, ...)
{
	char data[MAX_PAQUET_LEN+1];
	int len;
	va_list ap;
	va_start(ap, format);
	len = vsnprintf(data, MAX_PAQUET_LEN, format, ap);
	va_end(ap);
	
	printf("==> %s", data);
	write(fd, data, len);
}

int connect_to_server (char *server, char *port)
{
   struct addrinfo sin, *res;
   int sock;

   memset (&sin, 0x00, sizeof (struct addrinfo));

   sin.ai_flags = AI_CANONNAME;
   sin.ai_family = AF_INET;
   sin.ai_socktype = SOCK_STREAM;

   if (getaddrinfo (server, port, &sin, &res) == -1)
   {
   	perror("getaddrinfo ");
      exit(0);
	}

   if ((sock = socket (res->ai_family, res->ai_socktype, 6)) == -1)
   {
      perror("socket ");
      exit(0);
   }
   if (connect (sock, res->ai_addr, res->ai_addrlen) == -1)
   {
     perror("connect ");
     exit(0);
	}
   freeaddrinfo (res);
   return sock;
}

void recv_loop(int fd)
{
	char data[MAX_PAQUET_LEN+1];
	int rd_len;
	
	while((rd_len = read(fd, data, MAX_PAQUET_LEN)) > 0)
	{
		dump_data(data, rd_len);
		handle_data(fd, data, rd_len);
	}
}
