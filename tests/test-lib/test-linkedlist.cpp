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

TEST_CASE("linked list operations", "[linkedlist]") {
    LinkedList::LinkedList<uint8_t> list;
    // Ensure the constructor sets the head properly
    SECTION("constructor") {
        LinkedList::LinkedList<uint8_t> list2 = LinkedList::LinkedList<uint8_t>(0);
        auto node = list2.Head();
        REQUIRE(node != NULL);
        REQUIRE(node->Data() == 0);
    }
    // Ensure the linked list insert data properly
    SECTION("Insertion") {
        for (int i = 0; i < UINT8_MAX; i++) {
            list.InsertBack(i);
        }
        // 0 -> 254 == 255 entries
        REQUIRE(list.Count() == UINT8_MAX);
    }
    // Ensure the linked list can remove data properly
    SECTION("Insert (Back) : Removal (Back)") {
        for (int i = 0; i < UINT8_MAX; i++) {
            list.InsertBack(i);
        }
        // 0 -> 254 == 255 entries
        REQUIRE(list.Count() == UINT8_MAX);
        // Remove all data
        for (int i = UINT8_MAX; i > 0; i--) {
            auto node = list.RemoveBack();
            REQUIRE(node != NULL);
            REQUIRE(node->Data() == (i - 1));
            delete node;
        }
        REQUIRE(list.Count() == 0);
    }
    // Ensure the linked list can remove data properly
    SECTION("Insert (Back) : Removal (Front)") {
        for (int i = 0; i < UINT8_MAX; i++) {
            list.InsertBack(i);
        }
        // 0 -> 254 == 255 entries
        REQUIRE(list.Count() == UINT8_MAX);
        // Remove all data
        for (int i = 0; i < UINT8_MAX; i++) {
            auto node = list.RemoveFront();
            REQUIRE(node != NULL);
            REQUIRE(node->Data() == i);
            delete node;
        }
        REQUIRE(list.Count() == 0);
    }
    // Ensure the linked list can remove data properly
    SECTION("Insert (Front) : Removal (Back)") {
        for (int i = 0; i < UINT8_MAX; i++) {
            list.InsertFront(i);
        }
        // 0 -> 254 == 255 entries
        REQUIRE(list.Count() == UINT8_MAX);
        // Remove all data
        for (int i = UINT8_MAX; i > 0; i--) {
            auto node = list.RemoveBack();
            REQUIRE(node != NULL);
            REQUIRE(node->Data() == (UINT8_MAX - i));
            delete node;
        }
        REQUIRE(list.Count() == 0);
    }
    // Ensure the linked list can remove data properly
    SECTION("Insert (Front) : Removal (Front)") {
        for (int i = 0; i < UINT8_MAX; i++) {
            list.InsertFront(i);
        }
        // 0 -> 254 == 255 entries
        REQUIRE(list.Count() == UINT8_MAX);
        // Remove all data
        for (int i = UINT8_MAX; i > 0 ; i--) {
            auto node = list.RemoveFront();
            REQUIRE(node != NULL);
            REQUIRE(node->Data() == (i - 1));
            delete node;
        }
        REQUIRE(list.Count() == 0);
    }
}
