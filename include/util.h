#ifndef _UTIL_H_
#define _UTIL_H_

#include <rte_cycles.h>
#include <rte_timer.h>

/* Get the number of seconds elapsed since boot */
static double timestamp(void) {
    return (rte_get_tsc_cycles() / rte_get_tsc_hz());
}

#endif