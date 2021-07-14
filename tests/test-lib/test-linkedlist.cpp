/**
 * @file test-linkedlist.cpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief Linked list unit tests
 * @version 0.1
 * @date 2021-07-10
 * 
 * @copyright Copyright the Panix Contributors (c) 2021
 * 
 */
#include <catch2/catch.hpp>
// Linked list is header-only template
#include <lib/LinkedList.hpp>

TEST_CASE("linked list can insert", "[linkedlist]") {
    LinkedList::LinkedList<uint8_t> list;
    // Ensure the constructor sets the head properly
    SECTION("constructor") {
        list = LinkedList::LinkedList<uint8_t>(0);
        auto node = list.Head();
        REQUIRE(*(node->Data()) == 0);
    }
    // Ensure the linked list insert data properly
    SECTION("Insertion") {
        for (int i = 0; i < UINT8_MAX; i++) {
            list.InsertBack(i);
        }
        // 0 -> 255 == 256 entries
        REQUIRE(list.Count() == UINT8_MAX);
    }
    // Ensure the linked list can remove data properly
    SECTION("Removal (Back)") {
        // Account for the zero already in place
        for (int i = 0; i < UINT8_MAX; i++) {
            list.InsertBack(i);
        }
        // 0 -> 255 == 256 entries
        REQUIRE(list.Count() == UINT8_MAX);
        // Remove all data
        for (int i = UINT8_MAX; i > 0; i--) {
            auto node = list.RemoveBack();
            REQUIRE(*(node->Data()) == (i - 1));
        }
        REQUIRE(list.Count() == 0);
    }
    // Ensure the linked list can remove data properly
    SECTION("Removal (Front)") {
        // Account for the zero already in place
        for (int i = 0; i < UINT8_MAX; i++) {
            list.InsertBack(i);
        }
        // 0 -> 255 == 256 entries
        REQUIRE(list.Count() == UINT8_MAX);
        // Remove all data
        for (int i = 0; i < UINT8_MAX; i++) {
            auto node = list.RemoveFront();
            REQUIRE(*(node->Data()) == i);
        }
        REQUIRE(list.Count() == 0);
    }
}
