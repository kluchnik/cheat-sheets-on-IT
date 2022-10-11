/*
 * linkedlist.c
 *
 *  Created on: Dec 9, 2013
 *      Author: arnout
 */


#include "linkedlist.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Removes and frees the linked item (unless it is the first one) and frees the pointer memory
 */
int remove_linkedlist(struct linkedlistitem * delayed_write_memory,int fd) {
	// Traverse the items, until we hit the nullpointer
	struct linkedlistitem * localitem = delayed_write_memory;
	struct linkedlistitem * previousitem = delayed_write_memory;
	if(!delayed_write_memory) return -1;
	if(localitem->next == NULL && localitem->fd == fd) {
		// This is the first item. We never delete this one
		localitem->fd = -99;
		return 0;
	}
	while ((localitem = localitem->next)) {
		if(localitem->fd == fd) {
			// We have to remove it, and attach the other two sides together.
			if (localitem->next == NULL) {
				// This was the last (but not first) item.
				previousitem->next = NULL;
				free(localitem->ptr);
				free(localitem);
			} else {
				// This is some item in between
				previousitem->next = localitem->next;
				free(localitem);
				free(localitem->ptr);
			}
			return 0;
		}
		previousitem=localitem;
	}
	return -1;
}
/*
 * Allocates a linked item + allocates memory for the pointer and copies its contents
 */
void * add_linkedlist(struct linkedlistitem * delayed_write_memory ,int fd, void * ptr, int len) {
	// Traverse the items, until we hit the nullpointer
	struct linkedlistitem * localitem = delayed_write_memory;
	if(!localitem) return NULL;
	if(localitem->fd == fd) return NULL;
	while ((localitem->next)) {
		if(localitem->next->fd == fd) return NULL;
		localitem = localitem->next;
	}
	// We have an empty item. Allocate memory here
	localitem->next = malloc(sizeof(struct linkedlistitem));
	localitem->next->next = NULL;
	localitem->next->fd = fd;
	localitem->next->ptr = malloc(len);
	memcpy(localitem->next->ptr,ptr,len);
	localitem->next->ptrlen = len;
	return localitem->next;
}
void * find_linkedlist(struct linkedlistitem * delayed_write_memory ,int fd) {
	// Traverse the items, until we hit the nullpointer
	struct linkedlistitem * localitem = delayed_write_memory;
	if(!delayed_write_memory) return NULL;
	if(localitem->fd == fd) return localitem->ptr;
	while ((localitem = localitem->next)) {
		if(localitem->fd == fd) return localitem->ptr;
	}
	return NULL;
}
int findsize_linkedlist(struct linkedlistitem * delayed_write_memory ,int fd) {
	// Traverse the items, until we hit the nullpointer
	struct linkedlistitem * localitem = delayed_write_memory;
	if(!delayed_write_memory) return -1;
	if(localitem->fd == fd) return localitem->ptrlen;
	while ((localitem = localitem->next)) {
		if(localitem->fd == fd) return localitem->ptrlen;
	}
	return -1;
}
