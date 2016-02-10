#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define NUMBER_MAPPERS 4
#define NUMBER_REDUCERS 26

#define READ_END 0
#define WRITE_END 1

int mapper_pipes[NUMBER_MAPPERS][2];
int reducer_pipes[NUMBER_REDUCERS][2];

pid_t mapper_pids[NUMBER_MAPPERS];
pid_t reducer_pids[NUMBER_REDUCERS];


void create_mappers(pid_t parent_id);
void handle_children(int i);
void handle_child();
void handle_parent();

int main(void)
{
  FILE *input_file = fopen("input.txt", "r");

  if(pipe(mapper_pipes[0]))
  {
    perror("pipe");
    exit(1);
  }

  mapper_pids[0] = fork();
  if(mapper_pids[0] < 0)
  {
    perror("fork");
    exit(1);
  }
  else if( mapper_pids[0] == 0)
  {
    close(mapper_pipes[0][WRITE_END]);
    handle_child();
  }
  else
  {
    close(mapper_pipes[0][READ_END]);
    handle_parent();
    int val;
    wait(&val);
    printf("child process terminated\n");
  }

  return 0;
}

void handle_child()
{
  char buffer[512];

  //int i;
  //for(i = 0; i < 512 && read(mapper_pipes[0][READ_END], &buffer[i], 1) > 0; i++);
  while(read(mapper_pipes[0][READ_END], &buffer[0], 9) > 0)
  {
    printf("%s\n", buffer);
    fflush(stdout);
  }
  /*if(i < 512)
  {
    buffer[i] = '\0';
  }
  else
  {
    buffer[511] = '\0';
  }*/

  /*int child_id = getpid();
  printf("%d: and coming through pipe: %s...\n", child_id, buffer);
  fflush(stdout);
  // cant close bc closes file desc


  char buf[256];
  if(read(mapper_pipes[0][READ_END], buf, 5) < 0)
  {
    perror("read");
    _exit(1);
  }
  printf("%d: and coming through pipe: %s...\n", child_id, buf);
  fflush(stdout);*/
  close(mapper_pipes[0][READ_END]);

  _exit(0);
}

void handle_parent()
{
  sleep(3);
  char *str = "Falcon";
  printf("%d: passing %s through pipe\n", getpid(), str);
  fflush(stdout);
  write(mapper_pipes[0][WRITE_END], str, 7);
  // cant close bc closes file desc

  char *s = "Hawk";
  printf("%d: passing %s through pipe\n", getpid(), s);
  fflush(stdout);
  if(write(mapper_pipes[0][WRITE_END], s, 5) < 0)
  {
     perror("write");
    _exit(1);
  }
  close(mapper_pipes[0][WRITE_END]);
}







void create_mappers(pid_t parent_id)
{
  int i;
  for(i = 0; i <= NUMBER_MAPPERS; i++)
  {
    if(getpid() == parent_id) // only passed by main process
    {
      fork();
    }
    else
    {
      handle_children(i);
    }
  }
}

void handle_children(int i)
{
  sleep(i);
  printf("child %d: PID = %d\n", i, getpid());
  fflush(stdout);
  _exit(0);
}
