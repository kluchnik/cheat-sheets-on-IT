/*
 * userio.h
 *
 *  Created on: Dec 8, 2013
 *      Author: arnout
 */

#ifndef USERIO_H_
#define USERIO_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>


#define DEBUG(...) printf(__VA_ARGS__)
#define DEBUG2(...)


void die(char* fmt,...);
void dies(char* fmt,...);

#endif /* USERIO_H_ */
