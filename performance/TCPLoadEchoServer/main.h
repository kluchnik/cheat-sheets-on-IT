/*
 * main.h
 *
 *  Created on: Dec 8, 2013
 *      Author: arnout
 */

#ifndef MAIN_H_
#define MAIN_H_

#define MAXCONNS 200000		// The maximum number of open connections. Has to be lower than 2^16
							// Note that the ACTUAL number of connections the program will support is less;
							// A normal program already has 3 FD open (stdin,out,err). But, it could also have inherited others!
							// TODO: check the number of open files during boot, and make sure we can allocate more!
#define MAXEVENTS (MAXCONNS+1)	// This number represents almost all allocated userspace memory during startup. (sizeof(struct ev) * MAXEVENTS)

#define READBUFF 256		// Defines the amount of data to read in at once from a socket. Bigger is normally more performant.

#define DEFAULTPORT 7

#define TCP_SEND_BUFFER_CORRECT 1	// Defines behaviour when the TCP send buffer is full. If set to zero, we simply drop content
									// until the socket is writable again. (wrong)
/*
 * A note on connection handling:
 * Instead of keeping a large list with data corresponding to each active socket in userspace, we just let the (more efficient)
 * kernel space do the work. It already keeps a list with file descriptors, and will make sure we don't hit the limit.
 * When connections are closed, and new ones open, the kernel has to recycle the fd efficiently, saving us the work.
 *
 * The only data we need to keep associated with the fd, is unwritten data, in case a write blocks of the TCP kernel send buffer.
 * If we didn't need this, we wouldn't even need any connection-associated userspace data.
 *
 * To store this extra amount of data, there are a few options
 * 	- Dynamically allocate memory for each now connection, and store the fd in it, (along with more room for the write buffer) (slow)
 * 	- Statically allocate the memory (but this is memory inefficient as indicated above)
 * 	- Only allocate dynamic memory for a connection if it needs to. (sounds nice)
 *
 * 	This last option is the most interesting, and is the one we implement (using a linked-list. See linkedlist.h)
 *
 */

struct input_parameter {
	struct sockaddr_in6 local_saddrin;
	int skipopenfile;
} input_pars;

void usage(int argc, char ** argv);
void parse_input_parameter(int argc, char ** argv);

int create_listen_socket_and_add_epoll(int epollfd) ;
int listensocket_handler(int epollfd,int listensocket,int activeconns);
int datasocket_handler(int epollfd,int currfd);
int datasocket_writeblocked_handler(int epollfd, int currfd);

#endif /* MAIN_H_ */
