#include <sys/stat.h>
#include <sys/types.h>

#include <fcntl.h>

#include <unistd.h>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* read_fd(int fd, char const* pathname)
{
  if (pathname == NULL)
    pathname = "<unknown>";

  struct stat sb;
  if (fstat(fd, &sb) == -1) {
    char err_bfr[256];
    strerror_r(errno, err_bfr, sizeof(err_bfr));
    fprintf(stderr, "error from fstat of %s: %s\n", pathname, err_bfr);
    return NULL;
  }

  if (!S_ISREG(sb.st_mode)) {
    fprintf(stderr, "not a regular file %s\n", pathname);
    return NULL;
  }

  size_t sz = sb.st_size;

  char* bfr = malloc(sz + 1);
  if (bfr == NULL) /* may never happen on Linux */
    return NULL;
  bfr[sz] = '\0';

  char* bp = bfr;

  while (sz) {
    const ssize_t bytes_read = read(fd, bp, sz);
    if (bytes_read == -1) {
      if (errno == EINTR)
        continue;
      char err_bfr[256];
      strerror_r(errno, err_bfr, sizeof(err_bfr));
      fprintf(stderr, "error from read from %s, size=%ld: %s\n",
              pathname, sz, err_bfr);
      free(bfr); /* no partial data returned */
      return NULL;
    }

    bp += bytes_read;
    sz -= bytes_read;
  }

  return bfr;
}

char* read_file(char const* pathname)
{
  if (pathname == NULL)
    return NULL;

  const int fd = open(pathname, O_RDONLY);

  if (fd == -1) {
    char err_bfr[256];
    strerror_r(errno, err_bfr, sizeof(err_bfr));
    fprintf(stderr, "error from open of %s: %s\n", pathname, err_bfr);
    return NULL;
  }

  char* bfr = read_fd(fd, pathname);

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
  test_it("/some-non-file");
  test_it("/etc/shadow");
  test_it("/dev/null");
  test_it("/dev/random");
  test_it("./read_a_file.c");

  return 0;
}
