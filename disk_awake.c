#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <time.h>

int main(int argc, char* argv[])
{
  FILE *fp=NULL;
  pid_t process_id=0;
  pid_t sid=0;
  int secs=300;
  char log_file[512];
  time_t raw_time;
  struct tm *time_info;
  int ticks=0;
  unsigned long step=1;
  
  if (argc<2||argc>3)
    {
      printf("Usage: %s target_directory [seconds]\n",argv[0]);
      exit(1);
    }
  if (strlen(argv[1])>500)
    {
      printf("String length of target_directory is HUGE!\n");
      exit(1);
    }
  if (chdir(argv[1])<0)
    {
      printf("Directory %s does not exist\n",argv[1]);
      exit(1);
    }
  strcpy(log_file,argv[1]);
  strcat(log_file,"/awake.log");
  if (!(fp=fopen(log_file,"w+")))
    {
      printf("Could not open log file %s\n",log_file);
      exit(1);
    }
  if (!(argv[2]))
    {
      printf("Delay argument not specified. Defaulting to 300 seconds\n");
      secs=300;
    }
  if (argv[2]&&(secs=atoi(argv[2]))<=0)
    {
      printf("Could not parse delay option. Defaulting to 300 seconds\n");
      secs=300;
    }
  printf("Delay interval %d seconds\n",secs);
  process_id=fork();
  if (process_id<0)
    {
      printf("Could not fork()\n");
      exit(1);
    }
  if (process_id>0)
    {
      printf("Started with pid %d\n", process_id);
      exit(0);
    }
  umask(0);
  sid=setsid();
  if(sid<0)
    {
      printf("Could not setsid()\n");
      exit(1);
    }
  close(STDIN_FILENO);
  close(STDOUT_FILENO);
  close(STDERR_FILENO);
  while (1)
    {
      if (ticks==10)
	{
	  fclose(fp);
	  if (!(fp=fopen(log_file,"w+"))) exit(1);
	  ticks=0;step++;
	}
      time(&raw_time);
      time_info=localtime(&raw_time);
      fprintf(fp,"%s %lu : %s","Step",step,asctime(time_info));
      fflush(fp);
      ticks++;
      sleep(secs);
    }
  fclose(fp);
  return(0);
}
