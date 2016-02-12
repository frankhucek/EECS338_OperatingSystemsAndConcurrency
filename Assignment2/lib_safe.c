#include "lib_safe.h"

void *calloc_safe(size_t nmemb, size_t size)
{
  void *ptr = calloc(nmemb, size);

  if(ptr == NULL)
  {
    perror("Failed to allocate memory");
    exit(1);
  }
  else
    return ptr;
}

void *malloc_safe(size_t size)
{
  void *ptr = malloc(size);

  if(ptr == NULL)
  {
    perror("Failed to allocate memory");
    exit(1);
  }
  else
    return ptr;
}

void *realloc_safe(void *ptr, size_t size)
{
  void *p = realloc(ptr, size);

  if(p == NULL)
  {
    perror("Failed to allocate memory");
    exit(1);
  }
  else
    return p;
}

pid_t fork_safe(void)
{
  pid_t c = fork();
  if(c < 0)
  {
    perror("Fork failed");
    exit(1);
  }
  else
    return c;
}

pid_t wait_safe(int *status)
{
  pid_t id = wait(status);

  if(id < 0)
  {
    perror("Wait failed");
    exit(1);
  }
  else
    return id;
}

FILE *fopen_safe(const char *path, const char *mode)
{
  FILE* file = fopen(path, mode);

  if(file == NULL)
  {
    perror("Failed to open file");
    exit(1);
  }
  else
    return file;
}

int pipe_safe(int pipefd[2])
{
  int i = pipe(pipefd);
  if(i < 0)
  {
    perror("pipe");
    exit(1);
  }
  return i;
}

int close_safe(int fd)
{
  int i = close(fd);

  if(i < 0)
  {
    perror("Failed to close file descriptor");
  }
  else
    return i;
}

int fflush_safe(FILE *stream)
{
  int i = fflush(stream);

  if(i == EOF)
  {
    perror("Failed to fflush");
  }
  else
    return i;
}

ssize_t write_safe(int fd, const void *buf, size_t count)
{
  ssize_t bytes_written = write(fd, buf, count);

  if(bytes_written < 0)
  {
    perror("Failed to write");
  }

  else return bytes_written;
}

ssize_t read_safe(int fd, void *buf, size_t count)
{
  ssize_t bytes_read = read(fd, buf, count);

  if(bytes_read < 0)
    perror("Failed to read");
  else
    return bytes_read;
}
