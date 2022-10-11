/*
 * TCPLoadEchoClient
 *
 * A Linux C program for a setting up a large number of concurrent TCP sessions, and periodically sending data over them.
 * It uses the linux Epoll library to scale with many thousands of sessions.
 *
 * Copyright Arnout Diels 2013
 */

#include <stdio.h>
#include <sys/epoll.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/time.h>
#include <math.h>

#include "main.h"
#include "limits.h"
#include "userio.h"
#include "time_functions.h"

#define STR_EXPAND(tok) #tok
#define STR(tok) STR_EXPAND(tok)
#define VERSION 0.1.1

int main(int argc, char ** argv) {
	int epollfd, eventstriggered, n,currfd,currindex,flags;
	struct epoll_event events[MAXEVENTS];
	struct epoll_event ev;
	struct fdlist fdlist[MAXCONNS] = {{0}};
	struct timeval tv_start;
	struct timeval tv_current;
	struct timeval tv_diff;
	double time_difference;
	double temp;
	int writecounter = 0;

	int startedsessions = 0;
	int activesessions = 0;
	int errorsessions = 0;

	/* Parse input parameters */
	parse_input_parameter(argc, argv);

	/* Check and change if needed the per-user limit of open files */
	if(input_pars.skipopenfile != 1) {
		check_user_limits(MAXCONNS+1); // TODO: do a more refined check here
	} else {
		printf("WARNING: Not checking open file limit on system (this could limit performance).\n");
	}

	/* Set up our epoll queue, and record start time */
	if(-1 == (epollfd = epoll_create(MAXEVENTS))) 											die("Could not create epoll fd.");


	/*
	 * Start first loop, starting all our connections
	 */
	printf("Starting set up of %d connections at a rate of %d connections per second\n",input_pars.total_sessions,input_pars.conns_per_second);
	gettimeofday(&tv_start, NULL);
	for (;;) {
    	eventstriggered = epoll_wait(epollfd, events, MAXEVENTS, 1); // Trigger at least after 1 ms
        if (eventstriggered == -1) 													die("epoll wait returned -1");
        /* Handle triggered events */
        for (n = 0; n < eventstriggered; ++n) {
        	// If error message, show this clearly in output that the session failed.
        	currindex = events[n].data.fd;
        	if(currindex < 0 || currindex >= MAXCONNS)								dies("Error: Epoll fd index out of bound");
        	currfd = fdlist[currindex].fd;
        	flags = fdlist[currindex].flags;
        	// Check if the socket went in err/hangup (triggered by wait by default)
			if((events[n].events & EPOLLERR) || (events[n].events & EPOLLHUP)) {
				// Socket will automatically be removed from epoll list
				errorsessions++;
				fdlist[currindex].fd = -1;
				fdlist[currindex].flags =  2;
				if((flags & 1) > 0) {
					activesessions--;
				}
				printf("WARNING: Closed connection! (fd=%d) (currently %d connections active)\n",currfd, activesessions);
				close(currfd);
				continue;
			}
			if (events[n].events & EPOLLIN)  {
				// Read-in data.
				if(datasocket_handler(epollfd, currfd)) { // Returns 1 if connection was closed
					fdlist[currindex].fd = -1;
					fdlist[currindex].flags =  2;
					errorsessions++;
					printf("WARNING: Closed connection! (fd=%d) (currently %d connections active)\n",currfd, activesessions);
					if((flags & 1) > 0) {
						activesessions--;
					} else {
						dies("Error: Read event from socket, but socket not recorded as established.");
					}
				}
			}
			if (events[n].events & EPOLLOUT)  {
				// The socket became writable.
				// Mark this socket as really active, and remove this event from the list
				activesessions++;
				fdlist[currindex].flags |=  1;
				/* Remove the socket from the queue */
				if (-1 == epoll_ctl(epollfd, EPOLL_CTL_DEL, currfd, NULL)) 	die("epoll_ctl removal failure on fd %d",currfd);
				/* Readd the socket to the queue */
				ev.events = EPOLLIN ; // available for read and non edge_triggered
				ev.data.fd = currindex;
				if (-1 == epoll_ctl(epollfd, EPOLL_CTL_ADD, currfd, &ev)) 	die("epoll_ctl failure on readdition of fd %d",currfd);
			}
        }

        /* Check if we need to start new connections */
        gettimeofday(&tv_current, NULL);
		timeval_subtract(&tv_diff,&tv_current,&tv_start);
		if (tv_diff.tv_sec < 0 || tv_diff.tv_usec < 0)  							dies("ERROR: Timestamps went into the past!\n");
		time_difference = ((double) tv_diff.tv_sec) * 1000000 + (double) tv_diff.tv_usec;
		while (((time_difference * (double) input_pars.conns_per_second > (double)startedsessions*1000000)) && (startedsessions < input_pars.total_sessions)) {
			// We have to make at least one new session to keep up.
			if(open_conn_add2epoll(fdlist, epollfd,startedsessions) == 0) {
				// Executed connect call, but session is only open when it is writable. (will be check next loop)
				fdlist[startedsessions].flags = 0;
			} else {
				// Session not opened successfully!
				fdlist[startedsessions].flags |= 2;
				fdlist[startedsessions].fd = -1;
				errorsessions++;
				printf("WARNING: Connection attempt failed! (currently %d connections active)\n", activesessions);
			}
			startedsessions++;
		}

        /* Check if we can stop the loop */
		if ((activesessions + errorsessions) == input_pars.total_sessions) break;
    }

    printf("Successfully opened %d sessions, %d failed (out of target of %d)!\n",activesessions, errorsessions, input_pars.total_sessions);
    sleep(1);


	/*
	 * Main loop
	 * Poll for data to read, and discard it
	 * When the time comes, send the tcp ping to the appropriate socket.
	 */
    printf("Starting TCP ping loop\n");
    gettimeofday(&tv_start, NULL);
    for (;;) {
		eventstriggered = epoll_wait(epollfd, events, MAXEVENTS, 1); // Trigger at least after 1 ms
		if (eventstriggered == -1) 													die("epoll wait returned -1");
		/* Handle triggered events */
		for (n = 0; n < eventstriggered; ++n) {
			// If error message, show this clearly in output that the session failed.
			currindex = events[n].data.fd;
			if(currindex < 0 || currindex >= MAXCONNS)								dies("Error: Epoll fd index out of bound");
			currfd = fdlist[currindex].fd;
			flags = fdlist[currindex].flags;
			// Check if the socket went in err/hangup (triggered by wait by default)
			if((events[n].events & EPOLLERR) || (events[n].events & EPOLLHUP)) {
				// Socket will automatically be removed from epoll list
				errorsessions++;
				fdlist[currindex].fd = -1;
				fdlist[currindex].flags =  2;
				if((flags & 1) > 0) {
					activesessions--;
				}
				printf("WARNING: Closed connection! (fd=%d) (currently %d connections active)\n",currfd, activesessions);
				close(currfd);
				continue;
			}
			if (events[n].events & EPOLLIN)  {
				// Read-in data.
				if(datasocket_handler(epollfd, currfd)) { // Returns 1 if connection was closed
					if((flags & 1) > 0) {
						activesessions--;
						errorsessions++;
						fdlist[currindex].fd = -1;
						fdlist[currindex].flags =  2;
						printf("WARNING: Closed connection! (fd=%d) (currently %d connections active)\n",currfd, activesessions);
					} else {
						dies("Error: Read event from socket, but socket not recorded as established.");
					}
				}
			}
		}

		/* Check if we need to write something */
		gettimeofday(&tv_current, NULL);
		timeval_subtract(&tv_diff,&tv_current,&tv_start);
		// Timestamps are allowed to be negative here
		time_difference = ((double) tv_diff.tv_sec) * 1000000 + (double) tv_diff.tv_usec;
		while (((time_difference * ((double) input_pars.pushes_per_second) > ((double)writecounter)*1000000))) {
			// We can write to our session at index writecounter
			if((fdlist[writecounter].flags & 2) < 1 && (fdlist[writecounter].fd > 0)) {
				// The socket can be written to.
				// Do so, if it is writable
				if(-1 == (write(fdlist[writecounter].fd, "TCP Ping Push", 13))) {
					// Something went wrong. Either we are blocking (buffer full), or there was a real error
					if((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
						// We did not sent all the bytes we wanted to send.
						// We don't care, and just ignore this
					} else {
						printf("Could not write to socket (fd %d) due to unexpected reason (hence closing it). Reason: %s\n",fdlist[writecounter].fd, strerror(errno));
						close(fdlist[writecounter].fd);
						if((fdlist[writecounter].flags & 1) > 0) {
							activesessions--;
							errorsessions++;
							printf("WARNING: Closed connection! (fd=%d) (currently %d connections active)\n",fdlist[writecounter].fd, activesessions);
							fdlist[writecounter].fd = -1;
							fdlist[writecounter].flags =  2;
						} else {
							dies("Error: Write event from socket, but socket not recorded as established.");
						}
					}
				}
			} else {
				// Skipping this one, since it is down
			}
			writecounter++;
			if(writecounter == input_pars.total_sessions) {
				// Wraparound
				// We also have to lower the timedifference now
				writecounter = 0;
				temp = ((1000000*((double) input_pars.total_sessions))/input_pars.pushes_per_second);
				tv_start.tv_sec = tv_start.tv_sec + (__time_t) floor((temp / 1000000));
				tv_start.tv_usec = tv_start.tv_usec + (__suseconds_t) (((int) temp) % 1000000);
				break; // Force goaround
			}
		}

		/* Check if we can stop the loop */
		if ((errorsessions) == input_pars.total_sessions) break;
	}

	printf("No more sessions active! Quitting! \n");

	return 0;
}

/*
 * Print usage
 */
void usage(int argc, char ** argv) {
	printf("TCPLoadEchoClient %s -- By Arnout Diels 2013\n",STR(VERSION));
	printf("\n");
	printf("Usage: %s -i <IP> [-i <IP> ..] -D <IP> -p <port> -n <conns> -r <pushrate> -c <connrate> [-s]\n",argv[0]);
	printf("\n");
	printf("\n");
	printf(" -r <pushrate>	: Pushrate equals the number of ping \"pushes\" per second. ");
	printf("This is equal to the number \"data pushes\" to the TCP stack per second over all connections\n");
	printf(" -c <connrate>	: Connrate equals the number of new connections that are made each second. (by default = pushrate)\n");
	printf(" -s		: Disables the check on the number of open files limit.\n");
	printf("\n");
	printf("\n");
	exit(1);
}
/*
 * Parse input parameters and set the parameter struct correctly
 */
void parse_input_parameter(int argc, char ** argv) {
	int opt;
	struct sockaddr_in skaddr4;

	int sin_addr_flag = 0;
	int din_addr_flag = 0;
	int port_flag = 0;
	int push_flag = 0;
	int conn_flag = 0;
	int sesscount_flag = 0;

	while ((opt = getopt(argc, argv, "i:D:p:n:r:c:hs")) != -1) {
		switch (opt) {
		case 'i':
			// Determine if this is either a valid IPv4 or IPv6 address.
			// If IPv4, convert it into IPv4-IPv6 mapped.
			if(sin_addr_flag < 256) {
				if (inet_pton(AF_INET,optarg,&skaddr4.sin_addr) > 0) {
					// This was a valid IPv4 address. Convert it to IPv6
					input_pars.local_saddrin[sin_addr_flag].sin6_addr.__in6_u.__u6_addr32[0] = 0;
					input_pars.local_saddrin[sin_addr_flag].sin6_addr.__in6_u.__u6_addr32[1] = 0;
					input_pars.local_saddrin[sin_addr_flag].sin6_addr.__in6_u.__u6_addr16[4] = 0;
					input_pars.local_saddrin[sin_addr_flag].sin6_addr.__in6_u.__u6_addr16[5] = 0xFFFF;
					input_pars.local_saddrin[sin_addr_flag].sin6_addr.__in6_u.__u6_addr32[3] = skaddr4.sin_addr.s_addr;
				} else {
					// Not a valid IPv4 address. Check if it is a valid IPv6 address
					if (inet_pton(AF_INET6,optarg,&input_pars.local_saddrin[sin_addr_flag].sin6_addr) < 1) {
						// Not valid either. Print help.
						usage(argc,argv);
					}
				}
				// We should now have a valid IPv6 socket address
				// Make sure we use the right family
				input_pars.local_saddrin[sin_addr_flag].sin6_family = AF_INET6;
				sin_addr_flag++;
			}
			break;
		case 'D':
			// Determine if this is either a valid IPv4 or IPv6 address.
			// If IPv4, convert it into IPv4-IPv6 mapped.
			if (inet_pton(AF_INET,optarg,&skaddr4.sin_addr) > 0) {
				// This was a valid IPv4 address. Convert it to IPv6
				input_pars.remote_saddrin.sin6_addr.__in6_u.__u6_addr32[0] = 0;
				input_pars.remote_saddrin.sin6_addr.__in6_u.__u6_addr32[1] = 0;
				input_pars.remote_saddrin.sin6_addr.__in6_u.__u6_addr16[4] = 0;
				input_pars.remote_saddrin.sin6_addr.__in6_u.__u6_addr16[5] = 0xFFFF;
				input_pars.remote_saddrin.sin6_addr.__in6_u.__u6_addr32[3] = skaddr4.sin_addr.s_addr;
			} else {
				// Not a valid IPv4 address. Check if it is a valid IPv6 address
				if (inet_pton(AF_INET6,optarg,&input_pars.remote_saddrin.sin6_addr) < 1) {
					// Not valid either. Print help.
					usage(argc,argv);
				}
			}
			// We should now have a valid IPv6 socket address
			din_addr_flag = 1;

			break;
		case 'p':
			input_pars.remote_saddrin.sin6_port = htons(atoi(optarg));
			port_flag = 1;
			break;
		case 'n':
			input_pars.total_sessions = atoi(optarg);
			if(input_pars.total_sessions < 1 || input_pars.total_sessions > MAXCONNS) usage(argc,argv);
			sesscount_flag = 1;
			break;
		case 'r':
			input_pars.pushes_per_second = atoi(optarg);
			if(input_pars.pushes_per_second < 1 || input_pars.pushes_per_second >= 10000000) usage(argc,argv);
			push_flag = 1;
			break;
		case 'c':
			input_pars.conns_per_second = atoi(optarg);
			if(input_pars.conns_per_second < 1 || input_pars.conns_per_second >= 10000000) usage(argc,argv);
			conn_flag = 1;
			break;
		case 's':
			input_pars.skipopenfile = 1;
			break;
		default:
			usage(argc,argv);
			break;
		}
	}
	// We have successfully parsed our options.
	// Check if nothing was filled in.
	if(!port_flag) usage(argc,argv);
	if(!push_flag) usage(argc,argv);
	if(!conn_flag) input_pars.conns_per_second = input_pars.pushes_per_second;
	if(!din_addr_flag) usage(argc,argv);
	if(!sesscount_flag) usage(argc,argv);
	if(sin_addr_flag < 1) {
		input_pars.local_saddrin[0].sin6_addr = in6addr_any;
		input_pars.local_saddrin[0].sin6_family = AF_INET6;
		input_pars.local_addresses = 1;
	} else {
		input_pars.local_addresses = sin_addr_flag;
	}
	// Make sure we use IPv6 family
	input_pars.remote_saddrin.sin6_family = AF_INET6;
	// Return
	return;
}

/*
 * Read data that arrived on our interface, and for now, simply discard it
 */
int datasocket_handler(int epollfd,int currfd) {
	int bytesread;
	char readbuffer[READBUFF];
	// Note that because we potentially have a lot of data, we might need to do this in a few loops
	while (1) {
		if(-1 == (bytesread = read(currfd, readbuffer, sizeof readbuffer))) {
			// This is a special condition, because we are in the loop.
			// If the error was WOULDBLOCK, this means we might just have read exactly the amount of data we had to last time.
			// If it is another error, we need to stop.
			if((errno == EAGAIN) || (errno == EWOULDBLOCK)) break;
			printf("Could not write to socket (fd %d) due to unexpected reason (hence closing it). Reason: %s\n",currfd, strerror(errno));
			close(currfd);
			return 1;
		}
		// We successfully read from the socket. Check if there is data
		if(bytesread == 0) {
			// The socket sent EOF.
			// Closing the descriptor will make epoll remove it from the set of descriptors which are monitored.
			close (currfd);
			return 1;
		}
		// We have successfully read our data. Don't do anything with it. (will go out of scope of the function)
	}
	// Done
	return 0;
}

/*
 * Attempt to open a new connection to the destination. Then add socket to the epoll queue
 * Return 0 on success, 1 on lost connection.
 */
int open_conn_add2epoll(struct fdlist * fdlist, int epollfd, int fdindex) {
	int newsocket;
	struct epoll_event ev;

	/* Start server */
	if(-1 == (newsocket = socket(AF_INET6,SOCK_STREAM | SOCK_NONBLOCK,0))) 											die("Could not allocate listening socket.");
	/* Set reuse socket option */
	// This allows tw-sockets to be recycled, UNLESS you try to connect to the same dest & port
	// (http://hea-www.harvard.edu/~fine/Tech/addrinuse.html)
	// DISABLED FOR NOW (does more bad than good, since we mostly connect to the same dest)
	//int one = 1;
	//if(-1 == setsockopt(newsocket, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one)))									die("Could not change socket option.");

	if(-1 == bind(newsocket, (struct sockaddr*) &input_pars.local_saddrin[fdindex%input_pars.local_addresses],sizeof(input_pars.local_saddrin[fdindex%input_pars.local_addresses]))) {
		// Check output. If the reason is that the address is in use, we print more info
		if(errno == EADDRINUSE) {
			dies("Could not bind to address due to the fact that there was no binding socket free. \n\n"
					" This can happen if you try to start more connections than there are free source ports,\n"
					" or if there are still sockets in TIME_WAIT,.. Try \"netstat -tn\" for more info.\n"
					" Normally with no other applications using ephemeral ports in netstat, you should be able to do\n"
					"   </proc/sys/net/ipv4/ip_local_port_range>*#IPs simultaneous connections.\n"
					" (Note: kernels < 3.5 are known to have a bug which limits the number of connections to that of roughly #IP=1..)\n");
		} else {
			if(input_pars.local_addresses > 1) {
				die("Could not bind to address (IP nr %d).", (fdindex%input_pars.local_addresses));
			} else {
				die("Could not bind to address");
			}
		}
	}
	if (-1 == connect(newsocket,(struct sockaddr*) &input_pars.remote_saddrin, sizeof(input_pars.remote_saddrin))) {
		// Our connect did not succeed immediately. This is quite normal, since the socket was configured non-blocking.
		// Check if the error is due to this
		if(errno != (EINPROGRESS)) {
			// The connect failed!
			// We close the socket anyway, and return 1
			DEBUG("DEBUG WARNING: Connection attempt failed due to reason: %s\n", strerror(errno));
			close(newsocket);
			return 1;
		}
		// The socket is opening.
		// We now have to add an event to the epollqueue, polling for the socket to be come fully open.
	} else {
		// The connect succeeded immediately. We just add the socket to the epoll queue anyway.
		DEBUG2("INFO: Connection opened immediately (faster than expected)\n");
	}
	/* Set up epolld fd and add listen socket to queue */

	ev.events = EPOLLOUT|EPOLLIN; //Specify which event to listen for
	ev.data.fd = fdindex; // Specify user data.

	if (-1 == epoll_ctl(epollfd, EPOLL_CTL_ADD, newsocket, &ev)) 							die("epoll_ctl failure on listen socket");

	/* Finally add socket to fdlist */
	fdlist[fdindex].fd = newsocket;

	return 0;
}
