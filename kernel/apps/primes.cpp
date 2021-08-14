/**
 * @file primes.cpp
 * @author Micah Switzer (mswitzer@cedarville.edu)
 * @brief Prime computation tasks
 * @version 0.1
 * @date 2020-12-30
 *
 * @copyright Copyright the Xyris Contributors (c) 2021
 *
 */
#include <stddef.h>

#include <lib/bitset.hpp>
#include <lib/stdio.hpp>
#include <sys/tasks.hpp>
#include <apps/primes.hpp>

namespace apps {

// this value can be tweaked based on memory constraints
#define PRIME_MAX_SQRT 4000
#define PRIME_MAX (PRIME_MAX_SQRT * PRIME_MAX_SQRT)
#define PRIMES_SIZE (PRIME_MAX / (sizeof(size_t) * CHAR_BIT))
static size_t primes[PRIMES_SIZE];
static Bitset map = Bitset(primes, sizeof(primes));

static size_t prime_current;

void find_primes(void)
{
    for (size_t i = 0; i < PRIMES_SIZE; i++)
        primes[i] = SIZE_MAX;

    for (prime_current = 2; prime_current < PRIME_MAX_SQRT; prime_current++) {
        if (!map.Get(prime_current)) continue;
        for (size_t j = prime_current * prime_current; j < PRIME_MAX; j += prime_current) {
            map.Clear(j);
        }
    }
}

void show_primes(void)
{
    do {
        tasks_nano_sleep(1000ULL * 1000 * 1000);
        size_t pct = (prime_current * 100) / PRIME_MAX_SQRT;
        kprintf("\e[s\e[23;0fComputing primes: %%%u\e[u", pct);
    } while (prime_current < PRIME_MAX_SQRT);

    size_t count = 0;
    for (size_t i = 2; i < PRIME_MAX; i++) {
        count += map.Get(i);
    }
    kprintf("\e[s\e[23;0fFound %u primes between 2 and %u.\e[u", count, PRIME_MAX);
}

}
