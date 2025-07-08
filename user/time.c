#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h" 

static int
uitoa(int x, char *buf)
{
  char tmp[16];
  int i = 0;
  if(x == 0){
    buf[0] = '0';
    buf[1] = 0;
    return 1;
  }
  while(x){
    tmp[i++] = '0' + (x % 10);
    x /= 10;
  }
  int j = 0;
  while(i > 0)
    buf[j++] = tmp[--i];
  buf[j] = 0;
  return j;
}

int
main(int argc, char *argv[])
{
  if(argc < 2){
    fprintf(2, "Usage: time <command>\n");
    exit(1);
  }

  int start = uptime();

  if(fork() == 0){
    exec(argv[1], argv + 1);
    fprintf(2, "exec %s failed\n", argv[1]);
    exit(1);
  }

  wait(0);
  int diff = uptime() - start;

  int fd = open("time.txt", O_CREATE | O_WRONLY);
  if(fd >= 0){
    char buf[64] = "time: ";
    int n = uitoa(diff, buf + strlen(buf));  
    buf[6 + n] = '\n';
    buf[7 + n] = 0;
    write(fd, buf, 7 + n);
    close(fd);
  } else {
    fprintf(2, "time: cannot create time.txt\n");
  }

  exit(0);
}
