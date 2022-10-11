/*
 * time_functions.h
 *
 *  Created on: May 6, 2013
 *      Author: arnout
 */
#include <sys/time.h>

#ifndef TIME_FUNCTIONS_H_
#define TIME_FUNCTIONS_H_

int timeval_subtract (struct timeval *result, struct timeval *x,struct timeval *y);

#endif /* TIME_FUNCTIONS_H_ */
