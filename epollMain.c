//https://banu.com/blog/2/how-to-use-epoll-a-complete-example-in-c/epoll-example.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <errno.h>

#define MAXEVENTS 64

static int
make_socket_non_blocking (int sfd) {
  int flags, s;

  flags = fcntl (sfd, F_GETFL, 0);
  if (flags == -1) {
      perror ("fcntl");
      return -1;
    }

  flags |= O_NONBLOCK;
  s = fcntl (sfd, F_SETFL, flags);
  if (s == -1) {
      perror ("fcntl");
      return -1;
    }

  return 0;
}

static int create_and_bind (char *port) {
  struct addrinfo hints;
  struct addrinfo *result, *rp;
  int s, sfd;

  memset (&hints, 0, sizeof (struct addrinfo));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_DGRAM;
  hints.ai_flags = AI_PASSIVE;

  s = getaddrinfo (NULL, port, &hints, &result);
  if (s != 0) {
    fprintf (stderr, "getaddrinfo: %s\n", gai_strerror (s));
    return -1;
  }

  for (rp = result; rp != NULL; rp = rp->ai_next) {
    sfd = socket (rp->ai_family, rp->ai_socktype, rp->ai_protocol);
    if (sfd == -1)
    continue;

    s = bind (sfd, rp->ai_addr, rp->ai_addrlen);
    if (s == 0) {
      break;
    }

    close (sfd);
  }

  if (rp == NULL) {
      fprintf (stderr, "Could not bind\n");
      return -1;
  }

  freeaddrinfo (result);

  return sfd;
}

int main (int argc, char *argv[]) {
  int sfd, s;
  int efd;
  struct epoll_event event;
  struct epoll_event *events;

  sfd = create_and_bind ("5683");
  if (sfd == -1){
    abort ();
  }

  s = make_socket_non_blocking (sfd);
  if (s == -1){
    abort ();
  }

  efd = epoll_create1 (0);
  if (efd == -1){
      perror ("epoll_create");
      abort ();
  }

  event.data.fd = sfd;
  event.events = EPOLLIN | EPOLLET;
  s = epoll_ctl (efd, EPOLL_CTL_ADD, sfd, &event);
  if (s == -1){
      perror ("epoll_ctl");
      abort ();
    }
  events = calloc (MAXEVENTS, sizeof event);

  while (1){
    int n, i;

    n = epoll_wait (efd, events, MAXEVENTS, 500);
    for (i = 0; i < n; i++){
	    if ((events[i].events & EPOLLERR) || (events[i].events & EPOLLHUP) || (!(events[i].events & EPOLLIN))){
        /* An error has occured on this fd, or the socket is not
        ready for reading (why were we notified then?) */
	      fprintf (stderr, "epoll error\n");
	      close (events[i].data.fd);
	      continue;
	    } else {
        /* We have data on the fd waiting to be read. Read and
        display it. We must read whatever data is available
        completely, as we are running in edge-triggered mode
        and won't get a notification again for the same
        data. */
        int done = 0;
        while (1){
          ssize_t count;
          char buf[512];
          struct sockaddr_storage peer_addr;
          socklen_t peer_addr_len = sizeof(peer_addr);
          count = recvfrom (events[i].data.fd, buf, sizeof buf, 0, (struct sockaddr *) &peer_addr, &peer_addr_len);
          if (count == -1) {
            /* If errno == EAGAIN, that means we have read all
            data. So go back to the main loop. */
            if (errno != EAGAIN) {
              perror ("read");
              done = 1;
            }
            break;
          } else if (count == 0){
            printf("COUNT 0");
            break;
          }

          /* Write the buffer to standard output */
          s = write (1, buf, count);
          if (s == -1) {
            perror ("write");
            abort ();
          }
        }
      }
    }
    printf("START\n");
  }

  free (events);

  close (sfd);

  return EXIT_SUCCESS;
}