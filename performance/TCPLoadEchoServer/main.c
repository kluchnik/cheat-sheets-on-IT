/*
 * TCPLoadEchoServer
 *
 * A Linux C program for a functional IPv4/IPv6 TCP Echo Server
 * using the linux Epoll library to scale with many thousands of sessions.
 *
 * Copyright Arnout Diels 2013
 */

#define STR_EXPAND(tok) #tok
#define STR(tok) STR_EXPAND(tok)
#define VERSION 0.1.1

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

#include "main.h"
#include "limits.h"
#include "userio.h"
#include "linkedlist.h"

struct linkedlistitem delayed_write_memory = {0};

int main(int argc, char ** argv) {
	int listensocket, epollfd, eventstriggered, n, newconns,currfd;
	struct epoll_event events[MAXEVENTS];
	int activeconns = 0;

	/* Parse input parameters */
	parse_input_parameter(argc, argv);

	/* Check and change if needed the per-user limit of open files */
	if(input_pars.skipopenfile != 1) {
		check_user_limits(MAXCONNS+1); // TODO: do a more refined check here
	} else {
		printf("WARNING: Not checking open file limit on system (this could limit performance).\n");
	}
	/* Set up our epoll queue */
	if(-1 == (epollfd = epoll_create(MAXEVENTS))) 											die("Could not create epoll fd.");
	/* Set up our server */
	listensocket = create_listen_socket_and_add_epoll(epollfd);


	/* Start our interrupt-triggerd polling queue */

    for (;;) {
    	eventstriggered = epoll_wait(epollfd, events, MAXEVENTS, -1);
    	DEBUG2("Event triggered: %d\n",eventstriggered);
        if (eventstriggered == -1) 													die("epoll wait returned -1");

        for (n = 0; n < eventstriggered; ++n) {
            if (events[n].data.fd == listensocket) {
				newconns = listensocket_handler(epollfd,listensocket,activeconns);
				activeconns += newconns;
				DEBUG("Added %d new fd to list (currently %d connections in event loop)\n",newconns,activeconns);
				// Successfully accepted and added newfd to the list. Loop for new connections.

            } else {
            	// Since we only trigger on the listen socket or data sockets, this must be a data socket.

            	/*
            	 * See explanation in main.h
            	 * If the TCP write buffer is full, it is possible that the echo server removes
            	 * sockets from the read-event queue, and adds them to a write-event queue.
            	 * So we have to check here which event was triggered.
            	 * In normal behavior, the socket will always stay in the read-event.
            	 *
            	 * Note that errors are always possible too, so we have to check for them as well.
            	 */
            	currfd = events[n].data.fd;

            	// Check if the socket went in err/hangup (triggered by wait by default)
            	if((events[n].events & EPOLLERR) || (events[n].events & EPOLLHUP)) {
            		// Socket will automatically be removed from epoll list
            		close(currfd);
            		activeconns--;
            		DEBUG("Closed connection (currently %d connections in event loop)\n",activeconns);
            		/* Also check if we need to remove logged user data from the queue */
            		remove_linkedlist(&delayed_write_memory,currfd); // will ignore if not present
            		continue;
            	}

            	if(events[n].events & EPOLLOUT) {
            		// We actually had an event for write ready.
            		// This is used in the special case where the write buffer of the TCP stack was full.
            		if(datasocket_writeblocked_handler(epollfd, currfd)) { // Returns 1 if connection was closed
            			activeconns--;
            			DEBUG("Closed connection (currently %d connections in event loop)\n",activeconns);
            		}
            	} else if (events[n].events & EPOLLIN)  {
            		// Normal read-ready event
            		if(datasocket_handler(epollfd, currfd)) { // Returns 1 if connection was closed
						activeconns--;
						DEBUG("Closed connection (currently %d connections in event loop)\n",activeconns);
					}
            	}
            }


        }
        DEBUG2("Looping event loop\n",activeconns);
    }

    // Loop should never end
	close(listensocket);
	return 0;
}

/*
 * Print usage
 */
void usage(int argc, char ** argv) {
	printf("TCPLoadEchoServer %s -- By Arnout Diels 2013\n",STR(VERSION));
	printf("\n");
	printf("Usage: %s -i <IP> -p <port> [-s]\n",argv[0]);
	printf("\n");
	printf("\n");
	printf("  -s disables the check on the open-file-limit.\n");
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
	int port_flag = 0;

	while ((opt = getopt(argc, argv, "i:p:hs")) != -1) {
		switch (opt) {
		case 'i':
			// Determine if this is either a valid IPv4 or IPv6 address.
			// If IPv4, convert it into IPv4-IPv6 mapped.
			if (inet_pton(AF_INET,optarg,&skaddr4.sin_addr) > 0) {
				// This was a valid IPv4 address. Convert it to IPv6
				input_pars.local_saddrin.sin6_addr.__in6_u.__u6_addr32[0] = 0;
				input_pars.local_saddrin.sin6_addr.__in6_u.__u6_addr32[1] = 0;
				input_pars.local_saddrin.sin6_addr.__in6_u.__u6_addr16[4] = 0;
				input_pars.local_saddrin.sin6_addr.__in6_u.__u6_addr16[5] = 0xFFFF;
				input_pars.local_saddrin.sin6_addr.__in6_u.__u6_addr32[3] = skaddr4.sin_addr.s_addr;
			} else {
				// Not a valid IPv4 address. Check if it is a valid IPv6 address
				if (inet_pton(AF_INET6,optarg,&input_pars.local_saddrin.sin6_addr) < 1) {
					// Not valid either. Print help.
					usage(argc,argv);
				}
			}
			// We should now have a valid IPv6 socket address
			 sin_addr_flag = 1;

			break;
		case 'p':
			input_pars.local_saddrin.sin6_port = htons(atoi(optarg));
			port_flag = 1;
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
	// Check if nothing was filled in. In that case, we fill in our values with the defaults
	if(!sin_addr_flag) input_pars.local_saddrin.sin6_addr = in6addr_any;
	if(!port_flag) input_pars.local_saddrin.sin6_port = htons(DEFAULTPORT);
	// Make sure we use IPv6 family
	input_pars.local_saddrin.sin6_family = AF_INET6;
	// Return
	return;
}

/*
 * This functions creates a listening socket, adds it to the epoll event queue, and returns the fd.
 */
int create_listen_socket_and_add_epoll(int epollfd) {
	int listensocket;
	char returnstring[256];
	struct epoll_event ev;
	int one = 1;

	/* Start server */
	if(-1 == (listensocket = socket(AF_INET6,SOCK_STREAM | SOCK_NONBLOCK ,0))) 										die("Could not allocate listening socket.");
	if(-1 == setsockopt(listensocket, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one)))									die("Could not change socket option");
	if(-1 == bind(listensocket, (struct sockaddr*) &input_pars.local_saddrin,sizeof(input_pars.local_saddrin)))		die("Could not bind to address.");
	if(-1 == listen(listensocket,MAXCONNS))											 								die("Could not start listener.");

	/* Print info */
	if (input_pars.local_saddrin.sin6_addr.__in6_u.__u6_addr32[0] == 0 &&
		  input_pars.local_saddrin.sin6_addr.__in6_u.__u6_addr32[1] == 0 &&
		  input_pars.local_saddrin.sin6_addr.__in6_u.__u6_addr16[4] == 0 &&
		  input_pars.local_saddrin.sin6_addr.__in6_u.__u6_addr16[5] == 0xFFFF) {
		// Print v4 mode
		inet_ntop(AF_INET,&input_pars.local_saddrin.sin6_addr.__in6_u.__u6_addr32[3],returnstring,256);
		printf("Listening on %s port %d\n",returnstring,ntohs(input_pars.local_saddrin.sin6_port));
	} else  {
		// Print v6 mode
		inet_ntop(AF_INET6,&input_pars.local_saddrin.sin6_addr,returnstring,256);
		printf("Listening on %s port %d\n",returnstring,ntohs(input_pars.local_saddrin.sin6_port));
	}

	/* Set up epolld fd and add listen socket to queue */

	ev.events = EPOLLIN; //Specify which event to listen for
	ev.data.fd = listensocket; // Specify user data.

	if (-1 == epoll_ctl(epollfd, EPOLL_CTL_ADD, listensocket, &ev)) 				die("epoll_ctl failure on listen socket");
	return listensocket;
}

/*
 * This function accepts incoming connections, and adds read-ready events for them to the event queue.
 * The function returns the number of newly opened connections
 */
int listensocket_handler(int epollfd,int listensocket,int activeconns) {
	int addrlen, newfd,newconncount;
	struct sockaddr_in caddr; //Structs for Client and server Address in the Internet
	struct epoll_event ev;

	// We have an event for the listensocket. Normally only one thing could have happened: a new connection.
	// (if there were more events, we'd have to check the .event tab and AND it with the specific event).
	newconncount = 0;
	while (1) {
		addrlen = sizeof(caddr);
		newfd = accept4(listensocket,(struct sockaddr *) &caddr, &addrlen, SOCK_NONBLOCK );
		// TODO: use accept and fctl to make it more portable?
		if (newfd == -1) {
			if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) break; // No more new connections
			die("Could not accept new connection"); // We did not break, error occurred
		}
		DEBUG("Accepted new connection (fd=%d)\n",newfd);
		/* Set reuse socket option */
		// Not done on the new connections (not sure if its even needed/useful?)
		//int one = 1;
		//if(-1 == setsockopt(newfd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one))) die ("Could not change socket option.");
		// We cannot accept more than MAXEVENTS connections, since our event queue would be full
		if ((activeconns+1) >= MAXEVENTS) {
			// Too many connections for this program
			dies("Too many open connections (new fd was %d)",newfd);
		}
		ev.events = EPOLLIN ; // available for input and non edge_triggered
		ev.data.fd = newfd;
		if (-1 == epoll_ctl(epollfd, EPOLL_CTL_ADD, newfd, &ev)) 	die("epoll_ctl failure on fd %d",newfd);
		// Successfully accepted and added newfd to the list. Loop for new connections.
		newconncount++;
	}
	if(newconncount == 0) die("Error, listening socket triggered event, but no new connection accepted!");
	return newconncount;
}

/*
 * This function is called when a socket could not be written to, but data was already read from it.
 * The result is a situation where we are left with data, that we need to store somewhere, until the
 * socket becomes writable.
 *
 * This function stores this data, and change the epoll queue to handle this.
 * ----------
 * We store what still needs to be sent in a dynamically allocated buffer,
 * and replace the read-ready event by a write-ready event.
 *  Note that we cannot be notified of a closed connection via the read command anymore this way!
 */
void mark_datasocket_writeblocked(int epollfd, int currfd, char* readbuffer, int bytesread, int bytesssent) {
	if(!TCP_SEND_BUFFER_CORRECT) return; // Will simply not mark the socket, discard the data, and keep the socket in the loop.
	struct epoll_event ev;
	DEBUG("WARNING: Socket became write blocked (still %d/%d bytes to send). Allocating memory to avoid echo loss\n",bytesssent,bytesread);
	/* Check if we already have an entry (we shouldn't) */
	if(NULL != find_linkedlist(&delayed_write_memory,currfd)) dies("Error, socket became write blocked, but still had data left in userspace memory!");
	/* Add the data to the list */
	add_linkedlist(&delayed_write_memory,currfd,readbuffer+bytesssent,(bytesread-bytesssent));

	/* Remove the socket from the queue */
	if (-1 == epoll_ctl(epollfd, EPOLL_CTL_DEL, currfd, NULL)) 	die("epoll_ctl removal failure on fd %d",currfd);

	/* Readd the socket to the queue */
	ev.events = EPOLLOUT ; // available for output and non edge_triggered
	ev.data.fd = currfd;
	if (-1 == epoll_ctl(epollfd, EPOLL_CTL_ADD, currfd, &ev)) 	die("epoll_ctl failure on readdition of fd %d",currfd);

	return;
}

/*
 * Process a datasocket ready for reading (echo data)
 * Return 1 if the connection was closed in the process.
 */
int datasocket_handler(int epollfd,int currfd) {
	int bytesread, bytessent;
	char readbuffer[READBUFF];
	// Note that because we potentially have a lot of data, we might need to do this in a few loops
	while (1) {
		if(-1 == (bytesread = read(currfd, readbuffer, sizeof readbuffer))) {
			// This is a special condition, because we are in the loop.
			// If the error was WOULDBLOCK, this means we might just have read exactly the amount of data we had to last time.
			// If it is another error, we need to stop.
			if((errno == EAGAIN) || (errno == EWOULDBLOCK)) break;
			printf("Could not read from socket (fd %d) due to unexpected reason (hence closing it). Reason: %s\n",currfd, strerror(errno));
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
		// We have successfully read our data. Now simply echo it back.

		// Note that this is a bit tricky. It is possible that the write does not return the number of bytes
		// that we tried to send to it. (This could be the case because TCP's send buffer is full, and the receiver's
		// flow control is blocking it.) Although unlikely, we do try to not loose data here, and keep a userspace
		// buffer active to send it the next time the socket becomes writable. In addition, we then have
		// remove the "read" event, and schedule the write event.
		if(-1 == (bytessent = write(currfd, readbuffer, bytesread))) {
			// Something went wrong. Either we are blocking (buffer full), or there was a real error
			if((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
				DEBUG("WARNING: Send-buffer full for connection %d\n",currfd);
				// We did not sent all the bytes we wanted to send.
				// Stop echoing, and queue the data until the socket becomes ready.
				mark_datasocket_writeblocked(epollfd,currfd,readbuffer,bytesread,0);
				break;
			} else {
				printf("Could not write to socket (fd %d) due to unexpected reason (hence closing it). Reason: %s\n",currfd, strerror(errno));
				close(currfd);
				return 1;
			}
		}
		if (bytessent < bytesread) {
			// We did not sent all the bytes we wanted to send.
			// Stop echoing, and queue the data until the socket becomes ready.
			mark_datasocket_writeblocked(epollfd,currfd,readbuffer,bytesread,bytessent);
			break;
		}
		DEBUG2("Sent %d bytes\n",bytessent);
		// Successfully sent the read buffer. Loop until we run out of stuff to read.
	}
	// Done
	return 0;
}

/*
 * Process a datasocket ready for writing
 * This function is only called in special cases, when the TCP send buffer was full,
 * and we could not non-blockingly write to the socket (and so echo all data)
 * Return 1 if the connection was closed in the process.
 */
int datasocket_writeblocked_handler(int epollfd, int currfd) {
	char * storedbuffer;
	int storedsize;
	int closedconns = 0;
	int bytessent;
	struct epoll_event ev;

	if(NULL == (storedbuffer = find_linkedlist(&delayed_write_memory,currfd))) dies("Error, socket was write blocked, but did not have any userspace memory!");
	if(-1 == (storedsize = findsize_linkedlist(&delayed_write_memory,currfd))) dies("Error, socket was write blocked, but did not have any userspace memory!");
	// Remove the entry, as this is now not needed anymore
	remove_linkedlist(&delayed_write_memory,currfd); // will ignore if not present


	// Now we can again start to attempt to write this data to the socket
	if(-1 == (bytessent = write(currfd, storedbuffer, storedsize))) {
		// Something went wrong. Either we are blocking (buffer full), or there was a real error
		if((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
			DEBUG("WARNING: write-blocked socket became ready for writing, but blocked again immediately! (fd %d)\n",currfd);
			// We again did not succeed in sending everything we wanted to send.
			// Stop echoing, and queue the data until the socket becomes ready.
			mark_datasocket_writeblocked(epollfd,currfd,storedbuffer,storedsize,0);
			return 0;
		} else {
			printf("Could not read from socket (fd %d) due to unexpected reason (hence closing it). Reason: %s\n",currfd, strerror(errno));
			close(currfd);
			return 1;
		}
	}
	if (bytessent < storedsize) {
		// We did not sent all the bytes we wanted to send.
		// Stop echoing, and queue the data until the socket becomes ready.
		DEBUG("WARNING: write-blocked socket became ready for writing, but blocked again immediately! (fd %d)\n",currfd);
		mark_datasocket_writeblocked(epollfd,currfd,storedbuffer,storedsize,bytessent);
		return 0;
	}
	// At this point, we have successfully written our data. We can now finally reenable read events.
	/* Remove the socket from the queue */
	if (-1 == epoll_ctl(epollfd, EPOLL_CTL_DEL, currfd, NULL)) 	die("epoll_ctl removal failure on fd %d",currfd);

	/* Readd the socket to the queue */
	ev.events = EPOLLIN ; // available for read and non edge_triggered
	ev.data.fd = currfd;
	if (-1 == epoll_ctl(epollfd, EPOLL_CTL_ADD, currfd, &ev)) 	die("epoll_ctl failure on readdition of fd %d",currfd);
	DEBUG("INFO: write-blocked socket became ready for writing, and userspace buffer was cleared! (fd %d)\n",currfd);
	return closedconns;
}


