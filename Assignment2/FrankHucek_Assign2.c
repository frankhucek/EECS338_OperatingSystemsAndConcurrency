/*
  Frank Hucek
  fjh32
  EECS 338
  Assignment 2
  2/12/2016
*/

#include <string.h>

/* Pre-error checked versions of system calls */
#include "lib_safe.h"

#define NUMBER_MAPPERS 4
#define NUMBER_REDUCERS 26

#define READ_END 0
#define WRITE_END 1

#define PARENT_IDENTIFIER 0
#define MAPPER_IDENTIFIER 1
#define REDUCER_IDENTIFIER 2

#define DEFAULT_BUFFER 512 // default buffer size for lines

void create_pipes();
void create_mapper_pipes();
void create_reducer_pipes();
void create_mappers(pid_t parent_id);
void create_reducers(pid_t parent_id);
void close_pipe_ends();
void parse_file(FILE *input);
void handle_parent(FILE *input);
void handle_mapper();
void handle_reducer();

int mapper_pipes[NUMBER_MAPPERS][2];
int reducer_pipes[NUMBER_REDUCERS][2];

pid_t mapper_pids[NUMBER_MAPPERS];
pid_t reducer_pids[NUMBER_REDUCERS];

/*
   Used for identifying children processes
   - identifier tells whether process is main, mapper, or reducer
   - process_number tells which of either a mapper or reducer this process is

   These help to determine what type of worker a given process is
   which helps with reading from correct pipe
 */
int *identifier;
int process_number;

int main(void)
{
  /* Setup */
  identifier = (int *)calloc_safe(1, sizeof(int));
  *identifier = PARENT_IDENTIFIER;
  process_number = -1;

  /* Open file for reading */
  FILE *input_file = fopen_safe("input.txt", "r");

  /* Create necessary pipes */
  create_pipes();

  /* Fork children processes */
  pid_t main_id = getpid(); // main_id contains parent process ID

  create_mappers(main_id);
  if (getpid() == main_id) // more efficient to include this statement
    create_reducers(main_id);

  /* Close proper pipe ends */
  close_pipe_ends();

  /* Parse the input file. Processes handle properly. */
  parse_file(input_file);

  // free heap ptrs

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
    pipe_safe(mapper_pipes[i]);
  }
}

/* Create pipes that are used to send info from mappers to reducers */
void create_reducer_pipes()
{
  int i;
  for(i = 0; i < NUMBER_REDUCERS; i++)
  {
    pipe_safe(reducer_pipes[i]);
  }
}

void create_mappers(pid_t parent_id)
{
  int i;
  for(i = 0; i < NUMBER_MAPPERS; i++)
  {
    // have to be parent to get into this statement
    if(getpid() == parent_id)
    {
      mapper_pids[i] = fork_safe();
      // only the child that was just forked gets into this statement
      if(getpid() != parent_id)
      {
        *identifier = MAPPER_IDENTIFIER;
        process_number = i;
      }
    }
  }
}

void create_reducers(pid_t parent_id)
{
  int i;
  for(i = 0; i < NUMBER_REDUCERS; i++)
  {
    // have to be parent to get into this statement
    if(getpid() == parent_id)
    {
      reducer_pids[i] = fork_safe();
      // only the child that was just forked gets into this statement
      if(getpid() != parent_id)
      {
        *identifier = REDUCER_IDENTIFIER;
        process_number = i;
      }
    }
  }
}

void close_pipe_ends()
{
  int i;
  switch (*identifier)
  {
  /* Parent process maintains mapper write and reducer write */
  case PARENT_IDENTIFIER:
    // close mapper read ends and reducer read ends
    for(i = 0; i < NUMBER_MAPPERS; i++)
      close_safe(mapper_pipes[i][READ_END]);
    for(i = 0; i < NUMBER_REDUCERS; i++)
    {
      close_safe(reducer_pipes[i][READ_END]);
      //close(reducer_pipes[i][WRITE_END]);
    }
    break;
  /* Mappers maintain mapper read and reducer write */
  case MAPPER_IDENTIFIER:
    // close mapper write ends and reducer read ends
    for(i = 0; i < NUMBER_MAPPERS; i++)
      close_safe(mapper_pipes[i][WRITE_END]);
    for(i = 0; i < NUMBER_REDUCERS; i++)
      close_safe(reducer_pipes[i][READ_END]);
    break;
  /* Reducers maintain reducer read ends */
  case REDUCER_IDENTIFIER:
    // close mapper read/write ends and reducer write ends
    for(i = 0; i < NUMBER_MAPPERS; i++)
    {
      close_safe(mapper_pipes[i][READ_END]);
      close_safe(mapper_pipes[i][WRITE_END]);
    }
    for(i = 0; i < NUMBER_REDUCERS; i++)
      close_safe(reducer_pipes[i][WRITE_END]);
    break;

  default:
    exit(1);
    break;
  }
}

/* File handling starts here and tasks are assigned to each process */
void parse_file(FILE *input)
{
  switch (*identifier)
  {
  case PARENT_IDENTIFIER:
    handle_parent(input);
    break;
  case MAPPER_IDENTIFIER:
    handle_mapper();
    break;
  case REDUCER_IDENTIFIER:
    handle_reducer();
    break;
  default:
    exit(1);
    break;
  }
}

void handle_parent(FILE *input)
{
  char buffer[DEFAULT_BUFFER];
  int i;
  for(i = 0; (fgets(buffer, DEFAULT_BUFFER, input) != NULL); i++)
  {
    write_safe(mapper_pipes[i % NUMBER_MAPPERS][WRITE_END], buffer, DEFAULT_BUFFER);
  }

  // close mapper pipes then wait for mappers to exit
  for(i = 0; i < NUMBER_MAPPERS; i++)
  {
    close_safe(mapper_pipes[i][WRITE_END]);
  }
  int status;
  for(i = 0; i < NUMBER_MAPPERS; i++)
  {
    wait_safe(&status);
  }

  // close reducer pipes then wait for reducers to exit
  for(i = 0; i < NUMBER_REDUCERS; i++)
  {
    close_safe(reducer_pipes[i][WRITE_END]);
  }
  for(i = 0; i < NUMBER_REDUCERS; i++)
  {
    wait_safe(&status);
  }
}

void handle_mapper()
{
  char buffer[DEFAULT_BUFFER];
  int i;

  while(read_safe(mapper_pipes[process_number][READ_END], buffer, DEFAULT_BUFFER) > 0)
  {
    for(i = 0; i < strlen(buffer) && buffer[i] != '\n'; i++)
    {
      char letter = buffer[i];
      if((letter >= (int)'a') && (letter <= (int) 'z'))
      {
        int index = (int)letter - (int)'a';
        write_safe(reducer_pipes[index][WRITE_END], &letter, 1);
      }
    }
  }

  for(i = 0; i < NUMBER_MAPPERS; i++)
  {
    close_safe(mapper_pipes[i][READ_END]);
  }
  for(i = 0; i < NUMBER_REDUCERS; i++)
  {
    close_safe(reducer_pipes[i][WRITE_END]);
  }

  exit(0);
}

void handle_reducer()
{
  char my_letter = (char)((int)'a' + process_number);
  int count = 0;
  char c;

  while(read_safe(reducer_pipes[process_number][READ_END], &c, 1) > 0)
  {
    if(c == my_letter)
    {
      count++;
    }
  }

  printf("count %c:\t%d\n", my_letter, count);
  fflush_safe(stdout);

  int i;
  for(i = 0; i < NUMBER_REDUCERS; i++)
  {
    close_safe(reducer_pipes[i][READ_END]);
  }

  exit(0);
}
