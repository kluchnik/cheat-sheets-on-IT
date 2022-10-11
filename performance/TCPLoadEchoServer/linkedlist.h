/*
 * linkedlist.h
 *
 *  Created on: Dec 9, 2013
 *      Author: arnout
 */

#ifndef LINKEDLIST_H_
#define LINKEDLIST_H_

struct linkedlistitem {
	struct linkedlistitem* next;
	int fd;
	int ptrlen;
	void * ptr;
};

int remove_linkedlist(struct linkedlistitem * delayed_write_memory,int fd);
void * add_linkedlist(struct linkedlistitem * delayed_write_memory ,int fd, void * ptr, int len);
void * find_linkedlist(struct linkedlistitem * delayed_write_memory ,int fd);
int findsize_linkedlist(struct linkedlistitem * delayed_write_memory ,int fd);

#endif /* LINKEDLIST_H_ */
