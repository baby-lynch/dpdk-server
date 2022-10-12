#ifndef _SERVE_H_
#define _SERVE_H_

#include "context.h"
#include "node.h"
#include "seanet.h"

/* serve a request packet, i.e. deque a packet node from priority queue
   swap its (src,dest) fields to implement the actual echoing of packet
 */
int serve(void* arg);

#endif