#include "util.h"

int main() {
    printf("%ld\n", rte_get_timer_cycles());
    printf("%ld\n", rte_get_timer_hz());
    // printf("%lf", timestamp());
}