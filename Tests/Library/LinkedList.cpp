/**
 * @file test-linkedlist.cpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief Linked list unit tests
 * @version 0.1
 * @date 2021-07-10
 *
 * @copyright Copyright the Xyris Contributors (c) 2021
 *
 */
#include <catch2/catch.hpp>
#include <Library/LinkedList.hpp>

/**
 * @brief Test class that extends the Node class
 * to allow for a single uint8_t to be stored.
 *
 */
class TestNode : public LinkedList::Node {
public:
    TestNode()
        : Node()
        , m_Data(0)
    {
        // Default constructor
    }
    TestNode(uint8_t i)
        : Node()
        , m_Data(i) {
            // Value constructor
        };

    uint8_t Data() { return m_Data; }

private:
    uint8_t m_Data;
};

TEST_CASE("linked list operations", "[linkedlist]")
{
    LinkedList::LinkedListUnmanaged list;
    // Ensure the constructor sets the head properly
    SECTION("constructor")
    {
        LinkedList::LinkedListUnmanaged list2 = LinkedList::LinkedListUnmanaged();
        REQUIRE(list2.Head() == nullptr);
    }
    // Ensure the linked list insert data properly
    SECTION("Insertion")
    {
        for (int i = 0; i < UINT8_MAX; i++) {
            TestNode* node = new TestNode(i);
            list.InsertBack(node);
        }
        // 0 -> 254 == 255 entries
        REQUIRE(list.Count() == UINT8_MAX);
    }
    // Ensure the linked list can remove data properly
    SECTION("Insert (Back) : Removal (Back)")
    {
        for (int i = 0; i < UINT8_MAX; i++) {
            TestNode* node = new TestNode(i);
            list.InsertBack(node);
        }
        // 0 -> 254 == 255 entries
        REQUIRE(list.Count() == UINT8_MAX);
        // Remove all data
        for (int i = UINT8_MAX; i > 0; i--) {
            TestNode* node = reinterpret_cast<TestNode*>(list.RemoveBack());
            REQUIRE(node != nullptr);
            REQUIRE(node->Data() == (i - 1));
            delete node;
        }
        REQUIRE(list.Count() == 0);
    }
    // Ensure the linked list can remove data properly
    SECTION("Insert (Back) : Removal (Front)")
    {
        for (int i = 0; i < UINT8_MAX; i++) {
            TestNode* node = new TestNode(i);
            list.InsertBack(node);
        }
        // 0 -> 254 == 255 entries
        REQUIRE(list.Count() == UINT8_MAX);
        // Remove all data
        for (int i = 0; i < UINT8_MAX; i++) {
            TestNode* node = reinterpret_cast<TestNode*>(list.RemoveFront());
            REQUIRE(node != nullptr);
            REQUIRE(node->Data() == i);
            delete node;
        }
        REQUIRE(list.Count() == 0);
    }
    // Ensure the linked list can remove data properly
    SECTION("Insert (Front) : Removal (Back)")
    {
        for (int i = 0; i < UINT8_MAX; i++) {
            TestNode* node = new TestNode(i);
            list.InsertFront(node);
        }
        // 0 -> 254 == 255 entries
        REQUIRE(list.Count() == UINT8_MAX);
        // Remove all data
        for (int i = UINT8_MAX; i > 0; i--) {
            TestNode* node = reinterpret_cast<TestNode*>(list.RemoveBack());
            REQUIRE(node != nullptr);
            REQUIRE(node->Data() == (UINT8_MAX - i));
            delete node;
        }
        REQUIRE(list.Count() == 0);
    }
    // Ensure the linked list can remove data properly
    SECTION("Insert (Front) : Removal (Front)")
    {
        for (int i = 0; i < UINT8_MAX; i++) {
            TestNode* node = new TestNode(i);
            list.InsertFront(node);
        }
        // 0 -> 254 == 255 entries
        REQUIRE(list.Count() == UINT8_MAX);
        // Remove all data
        for (int i = UINT8_MAX; i > 0; i--) {
            TestNode* node = reinterpret_cast<TestNode*>(list.RemoveFront());
            REQUIRE(node != nullptr);
            REQUIRE(node->Data() == (i - 1));
            delete node;
        }
        REQUIRE(list.Count() == 0);
    }
}
