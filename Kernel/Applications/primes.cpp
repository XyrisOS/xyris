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
#include <Library/Bitset.hpp>
#include <Scheduler/tasks.hpp>
#include <Applications/primes.hpp>
#include <Devices/Graphics/console.hpp>

namespace Apps {

// this value can be tweaked based on memory constraints
#define PRIME_MAX_SQRT 4000
#define PRIME_MAX (PRIME_MAX_SQRT * PRIME_MAX_SQRT)
static Bitset<PRIME_MAX> map(1);

static size_t prime_current;

void find_primes(void)
{
    for (prime_current = 2; prime_current < PRIME_MAX_SQRT; prime_current++) {
        if (!map.Test(prime_current)) continue;
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
        Console::printf("\e[s\e[23;0fComputing primes: %%%zu\e[u", pct);
    } while (prime_current < PRIME_MAX_SQRT);

    size_t count = 0;
    for (size_t i = 2; i < PRIME_MAX; i++) {
        count += map.Test(i);
    }
    Console::printf("\e[s\e[23;0fFound %zu primes between 2 and %u.\e[u", count, PRIME_MAX);
}

}
