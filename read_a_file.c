#include <sys/stat.h>
#include <sys/types.h>

#include <fcntl.h>

#include <unistd.h>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* read_file(char const* pathname)
{
  if (pathname == NULL)
    return NULL;

  char* bfr = NULL;

  const int fd = open(pathname, O_RDONLY);

  if (fd == -1) {
    fprintf(stderr, "error from open of %s: %s\n", pathname, strerror(errno));
    goto close_fd_return_bfr;
  }

  struct stat sb;
  if (fstat(fd, &sb) == -1) {
    fprintf(stderr, "error from fstat of %s: %s\n", pathname, strerror(errno));
    goto close_fd_return_bfr;
  }

  if (!S_ISREG(sb.st_mode)) {
    fprintf(stderr, "not a regular file %s\n", pathname);
    goto close_fd_return_bfr;
  }

  size_t sz = sb.st_size;

  bfr = malloc(sz + 1);
  if (bfr == NULL) /* may never happen on Linux */
    goto close_fd_return_bfr;
  bfr[sz] = '\0';

  char* bp = bfr;

  while (sz) {
    const ssize_t bytes_read = read(fd, bp, sz);
    if (bytes_read == -1) {
      if (errno == EINTR)
        continue;
      fprintf(stderr, "error from read from %s, size=%ld: %s\n", pathname, sz,
              strerror(errno));
      free(bfr); /* no partial data returned */
      bfr = NULL;
      goto close_fd_return_bfr;
    }

    bp += bytes_read;
    sz -= bytes_read;
  }

close_fd_return_bfr:

  if (fd != -1)
    close(fd);

  return bfr;
}

void test_it(char const* pathname)
{
  char* bfr = read_file(pathname);
  if (bfr) {
    printf("%s is %ld bytes\n", pathname, strlen(bfr));
    free(bfr);
  }
}

int main()
{
  test_it("/etc/shadow");
  test_it("/dev/null");
  test_it("/dev/random");
  test_it("./read_a_file.c");

  return 0;
}
