#include <stdio.h>
#include <crypt.h>
#include <unistd.h>
#include <string.h>

int main()
{
	char* argv[3];
	pid_t pid;
	char spid[16];

	argv[0] = strdup("/challenge/cryptanalyse/ch21/ch21");
	snprintf(spid, sizeof(spid), "%i", getpid());
	argv[1] = crypt(spid, "$1$awesome");
	argv[2] = NULL;
	execve("/challenge/cryptanalyse/ch21/ch21", argv, NULL);

	return 0;
}
