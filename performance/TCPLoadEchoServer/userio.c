/*
 * userio.c
 *
 *  Created on: Dec 8, 2013
 *      Author: arnout
 */

#include "userio.h"


/*
 * Die "silent"
 * Print error message (appended with newline)
 */
void dies(char* fmt,...) {
	char outputstring[1024];
	va_list args;
	va_start(args, fmt);
	vsnprintf(outputstring,1024,fmt, args);
	va_end(args);
	fprintf(stderr,"%s\n",outputstring);
	exit(EXIT_FAILURE);
}


/*
 * Print error message (appended with newline)
 * Then print perror description
 */
void die(char* fmt,...) {
	char outputstring[1024];
	va_list args;
	va_start(args, fmt);
	vsnprintf(outputstring,1024,fmt, args);
	va_end(args);
	fprintf(stderr,"%s\n",outputstring);
	perror("Error description:");
	exit(EXIT_FAILURE);
}
