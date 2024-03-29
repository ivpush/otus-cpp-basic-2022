// OTUS C++ basic course homework#6
// Containers
// test program
//

#include <iostream>

#include "containers.hpp"

template <typename T>
void test_container(T * container)
{
    container = new T ;

    std::cout << "=============================================================================" << std::endl;
    // 1. create a container
    std::cout << " 1. " << container->name() << std::endl;
    // 2. add to container 10 items (0, 1, ..., 9)
    for (int i = 0; i < 10; ++i)
        container->push_back (i);   
    // 3. print container content
    std::cout << " 3. Container: ";
    container->print ();
    // 4. print container size
    std::cout << " 4. Container size = " << container->size() << std::endl;
    // 5. remove 3rd, 5th and 7th item from container - 1st approach
//    container->erase (3-1);        // remove 3th item, i.e. item[2]
//    container->erase (4-1);        // initial 5th becomes 4th after first remove
//    container->erase (5-1);        // initial 7th becomes 5th after 2 removes
    // 5. remove 3rd, 5th and 7th item from container - 2nd approach - removing in reverse order of indexes - start with big to small
    container->erase (7-1);        // remove 7th item, i.e. item[6]
    container->erase (5-1);        // remove 5th item, i.e. item[4]
    container->erase (3-1);        // remove 3th item, i.e. item[2]
    // 6. print container content
    std::cout << " 6. Container: ";
    container->print ();
    // 7. add item 10 at the beginning of container
    container->insert (0, 10);
    // 8. print container content
    std::cout << " 8. Container: ";
    container->print ();
    // 9. add item 20 in the middle of container
    container->insert (container->size()/2, 20);
    // 10. print container content
    std::cout << "10. Container: ";
    container->print ();
    // 11. append item 30 to the end of container
    container->push_back (30);
    // 12. print container content
    std::cout << "12. Container: ";
    container->print ();
    std::cout << std::endl;

    delete container;
}


int main ()
{
    ArrList<int> * tc1 = nullptr; 
    test_container (tc1);

    List<int> * tc2 = nullptr;
    test_container (tc2);

    return 0;
}