#include "../src/containers.hpp"

#include <gtest/gtest.h>

// 1. creation
//
TEST(List, Empty) {
    // Arrange
    List<size_t> list;

    // Act (empty for this test)

    // Assert
    ASSERT_EQ(list.size(), 0);
    ASSERT_TRUE(list.empty());
}

// 2. size
//
TEST(List, Size) {
    // Arrange
    const size_t count = 10;
    List<size_t> list;

    // Act
    for (size_t i = 0; i < count; ++i) 
        list.push_back(i);

    // Assert
    ASSERT_EQ(list.size(), count);
}

// 3. add to the end
//
TEST(List, PushBack) {
    // Arrange
    const size_t count = 10;
    List<size_t> list;

    // Act
    for (size_t i = 0; i < count; ++i) {
        list.push_back(i);
        ASSERT_EQ(list.size(), i + 1);
    }

    // Assert
    ASSERT_EQ(list.size(), count);
    ASSERT_FALSE(list.empty());
}

// 4. add to the begin
//
TEST(List, InsertAtBegin) {
    // Arrange
    const size_t count = 10;
    List<size_t> list;

    // Act
    for (size_t i = 0; i < count; ++i) {
        list.insert(0, i);
        ASSERT_EQ(list.size(), i + 1);
    }

    // Assert
    ASSERT_EQ(list.size(), count);
    ASSERT_FALSE(list.empty());
}

// 5. add into the middle
//
TEST(List, InsertIntoMiddle) {
    // Arrange
    const size_t count = 10;
    List<size_t> list;

    // Act
    for (size_t i = 0; i < count - 1; ++i) {
        list.push_back(i);
    }

    list.insert(list.size()/2, 10);

    // Assert
    ASSERT_EQ(list.size(), count);
    ASSERT_FALSE(list.empty());
}

// 6. remove from the end
//
TEST(List, PopBack) {
    // Arrange 
    const size_t count = 10;
    List<size_t> list;

    for (size_t i = 0; i < count; ++i) {
        list.push_back(i);
    }

    ASSERT_FALSE(list.empty());

    for (size_t i = 0; i < count; ++i) {
        list.pop_back();
        ASSERT_EQ(list.size(), count - i - 1);
    }

    // Assert
    ASSERT_EQ(list.size(), 0);
    ASSERT_TRUE(list.empty());
}

// 7. remove from the begin
//
TEST(List, RemoveFromBegin) {
    // Arrange 
    const size_t count = 10;
    List<size_t> list;

    // Act
    for (size_t i = 0; i < count; ++i) {
        list.push_back(i);
    }

    ASSERT_FALSE(list.empty());

    for (size_t i = 0; i < count; ++i) {
        list.erase(0);
        ASSERT_EQ(list.size(), count - i - 1);
    }

    // Assert
    ASSERT_EQ(list.size(), 0);
    ASSERT_TRUE(list.empty());
}

// 8. remove from the middle
//
TEST(List, RemoveFromMiddle) {
    // Arrange 
    const size_t count = 10;
    List<size_t> list;

    // Act
    for (size_t i = 0; i < count; ++i) {
        list.push_back(i);
    }

    ASSERT_FALSE(list.empty());

    for (size_t i = 0; i < count; ++i) {
        list.erase(list.size()/2);
        ASSERT_EQ(list.size(), count - i - 1);
    }

    // Assert
    ASSERT_EQ(list.size(), 0);
    ASSERT_TRUE(list.empty());
}

// 9. access the content
//
TEST(List, AccessContent) {
    // Arrange 
    const size_t count = 10;
    List<size_t> list;

    // Act
    for (size_t i = 0; i < count; ++i) {
        list.push_back(i);
    }

    // Assert
    for (size_t i = 0; i < count; ++i) {
        ASSERT_EQ(list[i], i);
    }
}

// 10. coping content creation
//
TEST(List, CreationCopyingContent) {
    // Arrange 
    const size_t count = 10;
    List<size_t> list1;

    // Act
    for (size_t i = 0; i < count; ++i) {
        list1.push_back(i);
    }

    ASSERT_FALSE(list1.empty());

    List<size_t> list2 (list1);

    // Assert
    ASSERT_FALSE(list2.empty());
    ASSERT_EQ(list2.size(), list1.size());
    ASSERT_EQ(list2.size(), count);

    for (size_t i = 0; i < count; ++i) {
        ASSERT_EQ(list2[i], list1[i]);
    }
}

// 11. coping content assignment
//
TEST(List, AssignCopyingContent) {
    // Arrange 
    const size_t count = 10;
    List<size_t> list1;

    // Act
    for (size_t i = 0; i < count; ++i) {
        list1.push_back(i);
    }

    ASSERT_FALSE(list1.empty());
    ASSERT_EQ(list1.size(), count);

    List<size_t> list3 ;

    ASSERT_TRUE(list3.empty());

    for (size_t i = 0; i < 2*count; ++i) {
        list3.push_back(i);
    }

    ASSERT_FALSE(list3.empty());
    ASSERT_EQ(list3.size(), 2*count);

    list3 = list1;

    // Assert

    ASSERT_FALSE(list3.empty());
    ASSERT_EQ(list3.size(), list1.size());
    ASSERT_EQ(list3.size(), count);

    for (size_t i = 0; i < count; ++i) {
        ASSERT_EQ(list3[i], list1[i]);
    }
}

// 12. empty list exception
//
TEST(List,PopBackFromEmptyList)
{
    // Arrange
    List<size_t> list;

    // Act (no action for this test)

    // Assert

    EXPECT_THROW(list.pop_back(), std::exception);
    EXPECT_NO_THROW(list.size());
}

// 13. invalid index in empty list exception
//
TEST(List,InvalidIndexEmptyList)
{
    // Arrange
    List<size_t> list;

    // Act (no action for this test)

    // Assert

    EXPECT_THROW(list.insert(2, 10), std::exception);
    EXPECT_THROW(list[5], std::exception);
}

// 14. invalid index in non-empty list exception
//
TEST(List,InvalidIndexNonEmptyList)
{
    // Arrange
    const size_t count = 10;
    List<size_t> list;

    // Act (no action for this test)
    for (size_t i = 0; i < count; ++i) {
        list.push_back(i);
    }

    // Assert

    EXPECT_THROW(list.insert(12, 100), std::exception);
    EXPECT_THROW(list[15], std::exception);
}
