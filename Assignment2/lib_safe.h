#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>

/*
  This library contains system calls with error checking already done.
*/

/* Safe call to calloc */
void *calloc_safe(size_t nmemb, size_t size);

/* Safe call to malloc */
void *malloc_safe(size_t size);

/* Safe call to realloc */
void *realloc_safe(void *ptr, size_t size);

/* Safe call to fork() */
pid_t fork_safe(void);

/* Safe call to wait(int) */
pid_t wait_safe(int *status);

/* Safe call to fopen */
FILE *fopen_safe(const char *path, const char *mode);

/* Safe call to pipe */
int pipe_safe(int pipefd[2]);

/* Safe call to close */
int close_safe(int fd);

/* Safe call to fflush */
int fflush_safe(FILE *stream);

/* Safe call to write */
ssize_t write_safe(int fd, const void *buf, size_t count);

/* Safe call to read */
ssize_t read_safe(int fd, void *buf, size_t count);
