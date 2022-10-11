/*
 * limits.c
 *
 *  Created on: Dec 8, 2013
 *      Author: arnout
 */

#include "limits.h"
#include "userio.h"

void check_user_limits(int limit) {
	// Check and change if needed the per-user limit of open files
	struct rlimit limitstruct;
	if(-1 == getrlimit(RLIMIT_NOFILE,&limitstruct))						    die("Could not establish user limits of open files.");
	DEBUG("Polled user limits for maximum number of open files:  soft: %d; hard: %d\n",(int) limitstruct.rlim_cur, (int) limitstruct.rlim_max);
	if(limitstruct.rlim_max < limit) {
		// The maximum value of the maximum number of open files is currently to low.
		// We can try to increase this, but this probably will only work as root.
		// A better durable solution is to use the /etc/security/limits.conf

		// Attempt to increase the limits
		limitstruct.rlim_cur = limit;
		limitstruct.rlim_max = limit;
		if(-1 == setrlimit(RLIMIT_NOFILE,&limitstruct))	{
			// This failed
			dies("Could not increase hard user limit of open files to %d.\n"
					"You can either try to run this program as root, or more recommended,\n"
					"change the user limits on the system (e.g. /etc/security.limits.conf)",limit);
		}
		DEBUG("Changed hard & soft limit to %d.\n",limit);
	} else if (limitstruct.rlim_cur < limit) {
		// The maximum limit is high enough, but the current limit might not be.
		// We should be able to increase this.
		limitstruct.rlim_cur = limit;
		if(-1 == setrlimit(RLIMIT_NOFILE,&limitstruct))	{
			dies("Could not increase soft user limit of open files to %d.\n"
					"You can either try to run this program as root, or more recommended,\n"
					"change the user limits on the system (e.g. /etc/security.limits.conf)",limit);
		}
		DEBUG("Changed soft limit to %d\n",limit);
	} else {
		DEBUG("Limit was high enough\n");
	}
	return;
}
