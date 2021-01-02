#include "global.h"

void sigsegv_handle(int n)
{
	printf("\n####### CRASHED ##########\n");
	creat("reboot.txt", 0666);
	exit(0);
}
int main(int argc, char **argv)
{
	int fd;
	struct sigaction sig;
	
	memset(&sig, 0, sizeof(struct sigaction));
	sig.sa_handler = sigsegv_handle;
	sigaction(SIGSEGV, &sig, NULL);
	
	
	init_handlers();
	fd = connect_to_server(SERVER, PORT);
	write_msg(fd, "NICK %s\r\n", NICK);
	write_msg(fd, "USER %s %s %s :%s\r\n", USER, USER, USER, USER);
	
	recv_loop(fd);
	close(fd);
	return 0;
}
