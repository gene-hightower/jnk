#include <bsd/libutil.h>
#include <err.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>

int main()
{
  struct pidfh* pfh;
  pid_t         otherpid, childpid;

  pfh = pidfile_open("/var/run/daemon.pid", 0600, &otherpid);
  if (pfh == NULL) {
    if (errno == EEXIST) {
      errx(EXIT_FAILURE, "Daemon already running, pid: %jd.",
           (intmax_t)otherpid);
    }
    /* If we cannot create pidfile from other reasons, only warn. */
    warn("Cannot open or create pidfile");
    /*
     * Even though pfh is NULL we can continue, as the other pidfile_*
     * function can handle such situation by doing nothing except setting
     * errno to EINVAL.
     */
  }

  if (daemon(0, 0) == -1) {
    warn("Cannot daemonize");
    pidfile_remove(pfh);
    exit(EXIT_FAILURE);
  }

  pidfile_write(pfh);

  for (;;) {
    /* Do work. */
    childpid = fork();
    switch (childpid) {
    case -1: syslog(LOG_ERR, "Cannot fork(): %s.", strerror(errno)); break;
    case 0:
      pidfile_close(pfh);
      /* Do child work. */
      break;
    default: syslog(LOG_INFO, "Child %jd started.", (intmax_t)childpid); break;
    }
  }

  pidfile_remove(pfh);
  exit(EXIT_SUCCESS);
}
