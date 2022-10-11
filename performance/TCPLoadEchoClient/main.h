/*
 * main.h
 *
 *  Created on: Dec 8, 2013
 *      Author: arnout
 */

#ifndef MAIN_H_
#define MAIN_H_

#define MAXCONNS 200000
#define MAXEVENTS MAXCONNS

#define READBUFF 256		// Defines the amount of data to read in at once from a socket. Bigger is normally more performant.

struct input_parameter {
	struct sockaddr_in6 local_saddrin[256];
	struct sockaddr_in6 remote_saddrin;
	int local_addresses;
	int total_sessions;
	int pushes_per_second;
	int conns_per_second;
	int skipopenfile;
} input_pars;

struct fdlist {
	int fd;
	int flags; // LSB = connectionEstablished. LSB+1 = error
};

void parse_input_parameter(int argc, char ** argv);
void usage(int argc, char ** argv);
int open_conn_add2epoll(struct fdlist * fdlist, int epollfd, int fdindex);
int datasocket_handler(int epollfd,int currfd);

#endif /* MAIN_H_ */
