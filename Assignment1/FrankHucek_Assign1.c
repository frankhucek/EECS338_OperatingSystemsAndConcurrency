/*
  Frank Hucek
  fjh32
  EECS 338: Intro to OSs and Concurrency
  Assignment 1
  2/1/2016
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

void adjust_whale(char *process, int start_val);
void print_pid(char *calling_process);
void print_userhost();
void print_time();
void print_cwd(char *process);
void handle_processes();
void handle_parent();
void handle_child1();
void handle_child2();

char env_var[8] = "WHALE=7";

int main(void)
{
  /* print PIDs */
  print_pid("P0");

  /* User/Hostname */
  print_userhost();

  /* Time */
  print_time();

  /* CWD */
  print_cwd("P0");

  /* Create ENV Var */
  putenv(env_var); // to change env variable, change actual character pointer

  handle_processes();

  return 0;
}

/*
  Properly adjust WHALE env variable.
  Adjustments are made to accommodate for proper output format.
 */
void adjust_whale(char *process, int start_val)
{
  int val;
  char *wh = getenv("WHALE"); // returns NULL if nothing
  if(wh == NULL)
  {
    perror("getenv failed.");
    _exit(1);
  }

  *wh = start_val + '0';

  do
  {
    wh = getenv("WHALE");
    if(wh == NULL)
    {
      perror("getenv failed.");
      _exit(1);
    }

    val = atoi(wh);
    printf("%s:\t%d shrimp (WHALE environment variable value is now %d)\n", process, val, val);
    if(fflush(stdout) < 0)
    {
      perror("Failed to fflush");
      _exit(1);
    }
    val = val - 3;

    *wh = val +'0';
    sleep(3);
  } while (val > 0);
}

/*
  Prints this process ID followed by its parent ID
 */
void print_pid(char *calling_process)
{
  /* PIDs */
  pid_t main_pid = getpid();
  pid_t parent_pid = getppid();
  printf("%s:\tmain PID: %d\tparent PID: %d\n", calling_process, main_pid, parent_pid);
  if(fflush(stdout) < 0)
  {
    perror("Failed to fflush");
    _exit(1);
  }
}

/*
  Prints formatted date/time
 */
void print_time()
{
  time_t *t = (time_t *) calloc(50, sizeof(time_t));
  if(t == NULL)
  {
    perror("Couldn't allocate memory");
    _exit(1);
  }

  int success = time(t); // ignore return value
  if(success < 0)
  {
    perror("call to time failed");
    _exit(1);
  }

  char *tme = ctime(t);
  if(tme == NULL)
  {
    perror("Failed to format time with ctime");
    _exit(1);
  }

  printf("P0:\t%s", tme);
  if(fflush(stdout) < 0)
  {
    perror("Failed to fflush");
    _exit(1);
  }
}

/*
  Prints the username and the hostname
 */
void print_userhost()
{
  char *hostname = (char *) calloc(50, sizeof(char));
  char *user = (char *) calloc(50, sizeof(char));
  if((hostname == NULL) || (user == NULL))
  {
    perror("Failed to allocate memory");
    _exit(1);
  }

  int success = gethostname(hostname, 50 * sizeof(char)); // ignore return value
  //cuserid(user, 50 * sizeof(char)); // ignore return value
  user = getlogin();

  if((success < 0) || (user == NULL))
  {
    perror("Failed to get user/hostname");
    _exit(1);
  }

  printf("P0:\tuser: %s\thostname: %s\n", user, hostname);
  if(fflush(stdout) < 0)
  {
    perror("Failed to fflush");
    _exit(1);
  }
}

/*
  Print the current working directory
 */
void print_cwd(char* process)
{
  char *cwd = (char *)calloc(100, sizeof(char));
  if(cwd == NULL)
  {
    perror("Failed to allocate memory");
    _exit(1);
  }

  char *success = getcwd(cwd, 100 * sizeof(char)); // ignore return value
  if(success == NULL)
  {
    perror("Failed to get cwd");
    _exit(1);
  }

  printf("%s:\t%s\n", process, cwd);
  if(fflush(stdout) < 0)
  {
    perror("Failed to fflush");
    _exit(1);
  }
}

/*
  Handle all processes
 */
void handle_processes()
{
  int c1 = fork();
  int c2;

  // means this is parent process
  if(c1 != 0)
  {
    c2 = fork();
  }

  // make sure no failures with fork
  if(c1 < 0 || c2 < 0)
  {
    perror("Something failed with fork...\n");
    _exit(1);
  }

  /* Handle Each process */
  /* Handle Child 1 */
  if(c1 == 0)
  {
    handle_child1();
  }
  /* Handle Child 2 */
  else if(c2 == 0)
  {
    handle_child2();
  }
  /* Handle Parent Process */
  else // if c1 && c2 != 0
  {
    handle_parent(c1, c2);
  }
}

/*
  Handle child 1
 */
void handle_child1()
{
  print_pid("C1");
  sleep(3);
  adjust_whale("C1", 6);

  sleep(3);

  printf("C1:\t");
  if(fflush(stdout) < 0)
  {
    perror("Failed to fflush");
    _exit(1);
  }
  chdir("/");
  execlp("ls", "ls", "-la", (char *) NULL);

  // should never reach this. if we do, exec fails
  _exit(1);
}

/*
  Handles child 2
 */
void handle_child2()
{
  sleep(1);
  print_pid("C2");
  sleep(3);
  adjust_whale("C2", 5);

  sleep(3);
  print_cwd("C2");

  _exit(0);
}

/*
  Handles the parent
 */
void handle_parent(int c1, int c2)
{
  sleep(2);
  adjust_whale("P0", 7);

  int status1, status2;
  int success1 = waitpid(c1, &status1, 0);
  int success2 = waitpid(c2, &status2, 0);
  if((success1 < 0) || (success2 < 0))
  {
    perror("waiting for children failed");
    _exit(1);
  }

  char *wh = getenv("WHALE");
  if(wh == NULL)
  {
    perror("getenv failed");
    _exit(1);
  }

  int val = atoi(wh);
  printf("P0:\t%d shrimp (WHALE environment variable value now is now %d)\n", val, val);
  if(fflush(stdout) < 0)
  {
    perror("Failed to fflush");
    _exit(1);
  }
  val = val - 1;

  *wh = val +'0';

  if(status1 == 0 && status2 == 0)
    printf("All terminated successfully.\n");
}
