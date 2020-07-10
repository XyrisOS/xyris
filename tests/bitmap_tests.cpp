#include <cstdio>
#include <lib/bitmap.hpp>

#define TEST_BITMAP_BITS 128
#define NEW_BITMAP(NAME, SIZE) bitmap_t NAME[BITMAP_SIZE((SIZE))] = { 0 }

typedef bool(*test_func)(void) ;
struct test_t {
    char *name;
    test_func func;
};

#define TEST(FUNC) { .name = #FUNC, .func = (FUNC) }

#define BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  (byte & 0x80 ? '1' : '0'), \
  (byte & 0x40 ? '1' : '0'), \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0')

static inline bool assert_eq_b(size_t value, size_t expected)
{
    if (value != expected) {
        printf("expected 0b" BINARY_PATTERN ", got 0b" BINARY_PATTERN "...",
            BYTE_TO_BINARY(expected), BYTE_TO_BINARY(value));
        return false;
    }
    return true;
}

static inline bool assert_eq(size_t value, size_t expected)
{
    if (value == expected) return true;
    printf("expected %u, got %u...", expected, value);
    return false;
}

static bool test_bitmap_set()
{
    NEW_BITMAP(bitmap, TEST_BITMAP_BITS);

    bitmap_set_bit(bitmap, 7);
    if (!assert_eq_b(bitmap[0], 0b10000000)) goto fail;
    
    bitmap_set_bit(bitmap, 0);
    if (!assert_eq_b(bitmap[0], 0b10000001)) goto fail;

    bitmap_set_bit(bitmap, 32);
    if (!assert_eq_b(bitmap[1], 0b1)) goto fail;

    return true;
fail:
    return false;
}

static bool test_bitmap_clear()
{
    NEW_BITMAP(bitmap, TEST_BITMAP_BITS);
    
    bitmap[0] = 0b10010010;
    bitmap_clear_bit(bitmap, 4);
    if (!assert_eq_b(bitmap[0], 0b10000010)) goto fail;

    bitmap[1] = (1 << 31) | 0b1;
    bitmap_clear_bit(bitmap, 32);
    if (!assert_eq_b(bitmap[1] >> 24, 0b10000000)) goto fail;

    return true;
fail:
    return false;
}

static bool test_bitmap_find_first_bit()
{
    NEW_BITMAP(bitmap, TEST_BITMAP_BITS);

    bitmap[0] = 0b00111111;
    if (!assert_eq(bitmap_find_first_bit_clear(bitmap, TEST_BITMAP_BITS), 6)) return false;

    return true;
}

static bool test_bitmap_find_first_range()
{
    NEW_BITMAP(bitmap, TEST_BITMAP_BITS);

    bitmap[0] = 0b1100111111;
    if (!assert_eq(bitmap_find_first_range_clear(bitmap, TEST_BITMAP_BITS, 3), 10)) return false;

    return true;
}

int main()
{
    test_t tests[] = {
        TEST(test_bitmap_set),
        TEST(test_bitmap_clear),
        TEST(test_bitmap_find_first_bit),
        TEST(test_bitmap_find_first_range)
    };
    int idx, passed = 0, failed = 0;

    printf("beginning bitmap tests...\n");

    printf("BYTE_SIZE = %u\nSIZE_T_MAX_VALUE = %u\nBITS_PER_BITMAP_T = %u\n",
                BYTE_SIZE, SIZE_T_MAX_VALUE, BITS_PER_BITMAP_T);

    for (idx = 0; idx < sizeof(tests) / sizeof(test_t); idx++) {
        if (tests[idx].name == NULL) break;
        printf("running %s...", tests[idx].name);
        if (tests[idx].func()) {
            passed++;
            printf("passed\n");
        } else {
            failed++;
            printf("failed\n");
        }
    }

    printf("%d tests ran, %d passed, %d failed\n", idx, passed, failed);

    printf("finished bitmap tests\n");
}

