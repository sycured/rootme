#include "global.h"

#define DUMP_FILE "./dump.txt"

#define BYTES_PER_LINE 32

void writef(int fd, const char *format, ...)
{
   char data[MAX_PAQUET_LEN+1];
   int len;
   va_list ap;
   va_start(ap, format);
   len = vsnprintf(data, MAX_PAQUET_LEN, format, ap);
   va_end(ap);
   
   write(fd, data, len);
}

void log_data(int fd, unsigned char *data, int len)
{
   int i, j;
   
   for(i = 0; i < len; i += BYTES_PER_LINE)
   {
      for(j = i; j < i + BYTES_PER_LINE && j < len; j++)
      {
         writef(fd, "%.2x ", data[j]);
      }
      
      for(j = 0; j < ((i + BYTES_PER_LINE >= len) ? ((i+BYTES_PER_LINE) - len)*3 + 3 : 3); j++)
      {
         write(fd, " ", 1);
      }
      
      for(j = i; j < i + BYTES_PER_LINE && j < len; j++)
      {
         writef(fd, "%c", (isalnum(data[j])) ? data[j] : '.');
      }
      write(fd, "\n", 1);
   }
}
void dump_data(char *data, int len)
{
#ifdef __DUMP__
   int fd;
   
   if((fd = open(DUMP_FILE, O_WRONLY | O_APPEND | O_CREAT, 0666)) != -1)
   {
      writef(fd, "\n\nNew paquet (%d bytes) : \n", len);
      log_data(fd, (unsigned char*)data, len);
      close(fd);
   }
   else
   {
      perror("open ");
      exit(0);
   }
#endif //__DUMP__
}
