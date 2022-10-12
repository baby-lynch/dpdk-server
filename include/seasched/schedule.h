#ifndef _SCHEDULE_H_
#define _SCHEDULE_H_

#include "context.h"
#include "node.h"
#include "seanet.h"
#include "util.h"

/* schedule a request packet, i.e. put a packet node into priority queue
   return 0 on success, -1 on fail
 */
int schedule(void* arg);

#endif