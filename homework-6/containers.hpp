// OTUS C++ basic course homework#6
// Containers
// Classes descriptions
//

#pragma once

#include <cstring>

// Abstract class for Container interface
//
template <typename T>
class iContainer
{
public:
    virtual   ~iContainer() {}

    virtual int push_back (T item) = 0;             // append an item to container
    virtual int insert (size_t pos, T item) = 0;    // insert an item at specified position
    virtual int erase (size_t pos) = 0;             // remove an item at specified position
    virtual size_t size () = 0;                     // return the current size
    virtual T   operator[] (size_t pos) = 0;        // return an item at specified position
    virtual int print () = 0;                       // print container
    virtual char * name() = 0;                      // return name of container
};

// Array/vector Container
//
template <typename T>
class ArrContainer : public iContainer <T>
{
private:
    size_t  m_size;     // current size - last element index
    size_t  m_capacity; // current capacity
    T *     m_data;     // data array itself

public: 
    ArrContainer(size_t capacity = 5) : m_size(0), m_capacity(capacity) {
        m_data = new T [m_capacity];
    }
    ~ArrContainer() {
        delete [] m_data;
    }

    // name
    //
    char*  name() override {    return "Sequential Container"; }

    // get size
    //
    size_t  size() override {    return m_size; }

    // check capacity to be able to get a new item
    //
    int check_capacity()    {       // check for capacity is enough
        if (m_size == m_capacity)   // overflow -> need to increase capacity
        {
            T * tmp = m_data;                       // save old location
            m_capacity += m_capacity/2;             // increase capacity
            m_data = new T [m_capacity];            // allocate new capacity
            memcpy (m_data, tmp, m_size*sizeof(T)); // copy data
            delete [] tmp;                          // release old location
        }
        return 0;
    }

    // append an item to container
    //
    int push_back (T item) override
    {
        check_capacity();

        m_data [m_size] = item;
        ++m_size;
        return 0;
    }

    // insert an item at specified position
    //
    int insert (size_t pos, T item) override    
    {
        check_capacity();

        for (size_t i = m_size; i > pos; --i)   // move data - find location for new item
            m_data [i] = m_data [i-1];
        m_data [pos] = item;
        ++m_size;    
        return 0;
    }
    
    // remove an item at specified position
    //
    int erase (size_t pos) override
    {
        for (size_t i = pos; i < m_size; ++i)
            m_data [i] = m_data [i+1];
        if (pos < m_size)     // have found specified position
            --m_size;       // have really delete an item

        return 0;
    }

    // return an item at specified position
    //
    T operator[] (size_t pos) override
    {
        if (pos < m_size)
            return m_data [pos];
        return 0;
    }

    // print container content
    //
    int print () override
    {
        for (size_t i = 0; i < m_size; ++i)
        {
            if (i > 0) std::cout << " ";
            std::cout << m_data [i];
        }
        std::cout << std::endl;
        return 0;
    }
};
  
// One direction List
//
template <typename T>
class ListContainer : public iContainer <T>
{
private:
    class ListItem
    {
    private:
        T           m_item;
        ListItem*   m_next;
    public:
        T           getValue()  {   return m_item; }
        ListItem*   getNext()   {   return m_next; }
        void        setNext(ListItem* next)   {   m_next = next; }
        ListItem (T value, ListItem* next = nullptr) : m_item(value), m_next(next) {}
        ~ListItem() {}
    };

    size_t       m_size;     // current size - last element index
    ListItem*    m_head;     // head of list
    ListItem*    m_rear;     // rear of list

public: 
    ListContainer::ListContainer() : m_size(0), m_head(nullptr), m_rear(nullptr) {}
    ~ListContainer() {
        ListItem * curr;
        ListItem * next = m_head;
        while (next != nullptr)
        {
            curr = next;
            next = next->getNext();
            delete curr;
        }
    }

    // name
    //
    char*  name() override {    return "One Direction List Container"; }

    // get size
    //
    size_t  size() override {    return m_size; }

    // append an item to container
    //
    int push_back (T item) override
    {
        ListItem * nItem = new ListItem (item);
        if (m_head == nullptr && m_rear == nullptr)     // very first item
            m_head = m_rear = nItem;
        else
        {
            m_rear->setNext(nItem);
            m_rear = nItem;
        }
        ++m_size;

        return 0;
    }

    // insert an item at specified position
    //
    int insert (size_t pos, T item) override    
    {
        ListItem * nItem = new ListItem (item);

        ListItem * prev;
        size_t idx = 0;

        for (ListItem * curr = m_head; curr != nullptr; curr = curr->getNext()) 
        {
            if (idx == pos)     // found specified position
            {
                nItem->setNext(curr); 
                if (curr == m_head)         // insert at the beginning
                    m_head = nItem;
                else                        // somewhere inside
                    prev->setNext(nItem);
                break;    
            }     
            prev = curr;
            ++idx;
        }
        if (idx < m_size)  // correct position found
            ++m_size;    
        else
            delete nItem;

        return 0;
    }
    
    // remove an item at specified position
    //
    int erase (size_t pos) override
    {
        ListItem * prev = nullptr;
        size_t idx = 0;

        for (ListItem * curr = m_head; curr != nullptr; curr = curr->getNext()) 
        {
            if (idx == pos)     // found specified position
            {
                if (curr == m_head)         // remove at the beginning
                {
                    m_head = curr->getNext();
                    if (m_head == nullptr)  // last item
                        m_rear = nullptr;
                }
                else                        // somewhere inside
                {
                    if (curr == m_rear)     // the last one
                        m_rear = prev;
                    prev->setNext(curr->getNext());
                }
                delete curr;
                break;
            }     
            ++idx;
            prev = curr;
        }
        if (pos < m_size)       // have found specified position
            --m_size;           // have really delete an item

        return 0;
    }

    // return an item at specified position
    //
    T operator[] (size_t pos) override
    {
        size_t idx = 0;

        for (ListItem * curr = m_head; curr != nullptr; curr = curr->getNext()) 
        {
            if (idx == pos)     // found specified position
            {
                return curr->getValue();
            }     
            ++idx;
        }

        return 0;
    }

    // print container content
    //
    int print () override
    {
        for (ListItem * curr = m_head; curr != nullptr; curr = curr->getNext()) 
        {
            if (curr !=m_head)  std::cout << " ";
            std::cout << curr->getValue();
        }
        std::cout << std::endl;

        return 0;
    }

    class list_iterator {
      private:
        ListItem *m_node;

      public:
        list_iterator(ListItem *item = nullptr) : m_node(item) {}
        T operator*() { return m_node->getValue(); }
        list_iterator &operator++() { m_node = m_node->getNext(); return *this; }
        bool operator==(list_iterator &rhs) { return m_node == rhs.m_node; }
        bool operator!=(list_iterator &rhs) { return m_node != rhs.m_node; }
    };

    list_iterator begin() { return list_iterator(m_head); }
    list_iterator end()   { return list_iterator(m_rear->getNext()); }

};

// Bi-direction List Container
//  
template <typename T>
class BiListContainer : public iContainer <T>
{
private:
    class BiLinkListItem
    {
    private:
        T               m_item;   // item value itself
        BiLinkListItem* m_next;   // ptr to next
        BiLinkListItem* m_prev;   // ptr to previous
    public:
        T               getValue()  {   return m_item; }
        BiLinkListItem* getNext()   {   return m_next; }
        BiLinkListItem* getPrev()   {   return m_prev; }
        void            setNext(BiLinkListItem* next)   {   m_next = next; }
        void            setPrev(BiLinkListItem* prev)   {   m_prev = prev; }
        BiLinkListItem (T value, BiLinkListItem* next = nullptr, BiLinkListItem* prev = nullptr) : m_item(value), m_next(next), m_prev(prev) {}
        ~BiLinkListItem() {}
    };

    size_t              m_size;    // current size - last element index
    BiLinkListItem*     m_head;    // head of list
    BiLinkListItem*     m_rear;    // rear of list

public: 
    BiListContainer() : m_size(0), m_head(nullptr), m_rear(nullptr) {}
    ~BiListContainer() {
        BiLinkListItem * curr;
        BiLinkListItem * next = m_head;
        while (next != nullptr)
        {
            curr = next;
            next = next->getNext();
            delete curr;
        }
    }

    // name
    //
    char*  name() override {    return "Bi Direction List Container"; }

    // get size
    //
    size_t  size() override {    return m_size; }

    // append an item to container
    //
    int push_back (T item) override
    {
        BiLinkListItem * nItem = new BiLinkListItem (item);
        if (m_head == nullptr && m_rear == nullptr)     // very first item
            m_head = m_rear = nItem;
        else
        {
            m_rear->setNext(nItem);
            nItem->setPrev(m_rear);
            m_rear = nItem;
        }
        ++m_size;

        return 0;
    }

    // insert an item at specified position
    //
    int insert (size_t pos, T item) override    
    {
        BiLinkListItem * nItem = new BiLinkListItem (item);

        size_t idx = 0;

        for (BiLinkListItem * curr = m_head; curr != nullptr; curr = curr->getNext()) 
        {
            if (idx == pos)     // found specified position
            {
                nItem->setNext (curr);
                nItem->setPrev (curr->getPrev());
                curr->setPrev (nItem);
                if (curr == m_head)         // insert at the beginning
                    m_head = nItem;
                else                        // somewhere inside
                    (nItem->getPrev())->setNext(nItem);
                break;    
            }     
            ++idx;
        }
        if (idx < m_size)  // correct position found
            ++m_size;      // increase size
        else
            delete nItem;

        return 0;
    }
    
    // remove an item at specified position
    //
    int erase (size_t pos) override
    {
        size_t idx = 0;

        for (BiLinkListItem * curr = m_head; curr != nullptr; curr = curr->getNext()) 
        {
            if (idx == pos)     // found specified position
            {
                if (curr == m_head)         // remove at the beginning - no prev
                {
                    m_head = curr->getNext();
                    if (m_head != nullptr)
                        m_head->setPrev(nullptr);
                    if (curr == m_rear)
                        m_rear = nullptr;
                }
                else                        // somewhere inside
                {
                    if (curr == m_rear)     // last in list
                    {
                        m_rear = curr->getPrev();
                        m_rear->setNext(nullptr);
                    }
                    else
                    {
                        (curr->getPrev())->setNext(curr->getNext());
                        (curr->getNext())->setPrev(curr->getPrev());
                    }
                }
                delete curr;
                break;
            }     
            ++idx;
        }
        if (pos < m_size)       // have found specified position
            --m_size;           // have really delete an item

        return 0;
    }

    // return an item at specified position
    //
    T operator[] (size_t pos) override
    {
        size_t idx = 0;

        for (BiLinkListItem * curr = m_head; curr != nullptr; curr = curr->getNext()) 
        {
            if (idx == pos)     // found specified position
            {
                return curr->getValue();
            }     
            ++idx;
        }

        return 0;
    }

    // print container content
    //
    int print () override
    {
        for (BiLinkListItem * curr = m_head; curr != nullptr; curr = curr->getNext()) 
        {
            if (curr !=m_head)  std::cout << " ";
            std::cout << curr->getValue();
        }
        std::cout << std::endl;

        return 0;
    }

    class bilist_iterator {
      private:
        BiLinkListItem *m_node;

      public:
        bilist_iterator(BiLinkListItem *item = nullptr) : m_node(item) {}
        T operator*() { return m_node->getValue(); }
        bilist_iterator &operator++() { m_node = m_node->getNext(); return *this; }
        bilist_iterator &operator--() { m_node = m_node->getPrev(); return *this; }
        bool operator==(bilist_iterator &rhs) { return m_node == rhs.m_node; }
        bool operator!=(bilist_iterator &rhs) { return m_node != rhs.m_node; }
    };

    bilist_iterator begin() { return bilist_iterator(m_head); }             // direct oreder
    bilist_iterator end()   { return bilist_iterator(m_rear->getNext()); }

    bilist_iterator beginr() { return bilist_iterator(m_rear); }            // reverse order
    bilist_iterator endr()   { return bilist_iterator(m_head->getPrev()); }

};
  
