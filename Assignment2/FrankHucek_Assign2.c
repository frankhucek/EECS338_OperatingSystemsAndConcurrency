#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define NUMBER_MAPPERS 4
#define NUMBER_REDUCERS 26

#define READ_END 0
#define WRITE_END 1

#define PARENT_IDENTIFIER 0
#define MAPPER_IDENTIFIER 1
#define REDUCER_IDENTIFIER 2

void create_pipes();
void create_mapper_pipes();
void create_reducer_pipes();
void create_mappers(pid_t parent_id);
void create_reducers(pid_t parent_id);

int mapper_pipes[NUMBER_MAPPERS][2];
int reducer_pipes[NUMBER_REDUCERS][2];

pid_t mapper_pids[NUMBER_MAPPERS];
pid_t reducer_pids[NUMBER_REDUCERS];

int *identifier;

int main(void)
{
  identifier = (int *)calloc(1, sizeof(int));
  *identifier = PARENT_IDENTIFIER;

  /* Open file for reading */
  FILE *input_file = fopen("input.txt", "r");

  /* Create necessary pipes */
  create_pipes();

  /* Fork children processes */
  pid_t main_id = getpid();
  create_mappers(main_id);
  create_reducers(main_id);



  return 0;
}

void create_pipes()
{
  create_mapper_pipes();
  create_reducer_pipes();
}

/* Create pipes that are used to send info from parent to mappers */
void create_mapper_pipes()
{
  int i;
  for(i = 0; i < NUMBER_MAPPERS; i++)
  {
    if(pipe(mapper_pipes[i]) < 0)
    {
      perror("pipe");
      _exit(1);
    }
  }
}

/* Create pipes that are used to send info from mappers to reducers */
void create_reducer_pipes()
{
  int i;
  for(i = 0; i < NUMBER_REDUCERS; i++)
  {
    if(pipe(reducer_pipes[i]) < 0)
    {
      perror("pipe");
      _exit(1);
    }
  }
}

void create_mappers(pid_t parent_id)
{
  int i;
  for(i = 0; i < NUMBER_MAPPERS; i++)
  {
    if(getpid() == parent_id) // check getpid for errors
    {
      if((mapper_pids[i] = fork()) < 0)
      {
        perror("Failed fork mappers");
        _exit(1);
      }

      if(getpid() != parent_id)
      {
        *identifier = MAPPER_IDENTIFIER;
      }
    }
  }
}

void create_reducers(pid_t parent_id)
{
  int i;
  for(i = 0; i < NUMBER_REDUCERS; i++)
  {
    if(getpid() == parent_id)
    {
      if((reducer_pids[i] = fork()) < 0)
      {
        perror("Failed fork reducers");
        _exit(1);
      }

      if(getpid() != parent_id)
      {
        *identifier = REDUCER_IDENTIFIER;
      }
    }
  }
}
