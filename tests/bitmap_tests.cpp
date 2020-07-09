#include <stdio>
#include <unistd>
#include <lib/bitmap.hpp>
#include <sys/types.hpp>

#define TEST_BITMAP_BITS 128
#define NEW_BITMAP(NAME, SIZE) bitmap_t NAME[BITMAP_SIZE((SIZE))] = { 0 }

static bool test_bitmap_set()
{
    NEW_BITMAP(bitmap, TEST_BITMAP_BITS);
    printf("testing bitmap_set_bit...")

    // TODO: test code here

    printf("passed\n");
    return true;
fail:
    printf("failed!\n");
    return false;
}

static void test_bitmap_clear()
{

}

int main()
{
    printf("beginning bitmap tests...\n");

    // TODO: run tests here    

    printf("finished bitmap tests\n");
}
