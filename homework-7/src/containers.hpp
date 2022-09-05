// OTUS C++ basic course homework#6
// Containers
// Classes descriptions
//

#pragma once

#include <cstring>

// Array/vector List
//
template <typename T>
class ArrList
{
private:
    size_t  m_size;     // current size - last element index
    size_t  m_capacity; // current capacity
    T *     m_data;     // data array itself

public: 
    ArrList(size_t capacity = 5) : m_size(0), m_capacity(capacity) {
        m_data = new T [m_capacity];
    }
    ArrList(ArrList& arr) {
        m_size = arr.m_size;
        m_capacity = arr.m_capacity;
        m_data = new T [m_capacity];

        for (size_t i = 0; i < m_size; ++i)
            m_data [i] = arr.m_data [i];
    }
    ~ArrList() {
        delete [] m_data;
    }

    // name
    //
    char*  name() const noexcept    {    return "Sequential (Vector like) List"; }

    // get size
    //
    size_t  size() const noexcept   {    return m_size; }

    // check the container for emptiness
    //
    bool    empty() const noexcept  {   return m_size == 0; }


    // check capacity to be able to get a new item
    //
    void check_capacity()    {       // check for capacity is enough
        if (m_size == m_capacity)   // overflow -> need to increase capacity
        {
            T * tmp = m_data;                       // save old location
            m_capacity += m_capacity/2;             // increase capacity
            m_data = new T [m_capacity];            // allocate new capacity
            memcpy (m_data, tmp, m_size*sizeof(T)); // copy data
            delete [] tmp;                          // release old location
        }
    }

    // append an item to List
    //
    void push_back (const T& item)
    {
        check_capacity();

        m_data [m_size] = item;
        ++m_size;
    }

    // remove and return the last item of List
    //
    T pop_back ()
    {
        if (empty()) 
            throw std::runtime_error ("List is empty!");

        --m_size;
        return m_data [m_size];
    }

    // insert an item at specified position
    //
    void insert (size_t pos, T item) 
    {
        check_capacity();

        if (pos > m_size)
            throw std::runtime_error ("Wrong position in List!");

        for (size_t i = m_size; i > pos; --i)   // move data - find location for new item
            m_data [i] = m_data [i-1];
        m_data [pos] = item;
        ++m_size;    
    }
    
    // remove an item at specified position
    //
    void erase (size_t pos)
    {
        for (size_t i = pos; i < m_size; ++i)
            m_data [i] = m_data [i+1];
        if (pos < m_size)     // have found specified position
            --m_size;       // have really delete an item
    }

    // return an item at specified position
    //
    T operator[] (size_t pos)
    {
        if (pos < m_size)
            return m_data [pos];

        throw std::runtime_error ("Wrong position in List!");
    }

    // return an item at specified position
    //
    ArrList& operator= (const ArrList& arr)
    {
        delete [] m_data;
        m_size = arr.m_size;
        m_capacity = arr.m_capacity;
        m_data = new T [m_capacity];

        for (size_t i = 0; i < m_size; ++i)
            m_data [i] = arr.m_data [i];

        return *this;
    } 

    // print list content
    //
    void print () 
    {
        for (size_t i = 0; i < m_size; ++i)
        {
            if (i > 0) std::cout << " ";
            std::cout << m_data [i];
        }
        std::cout << std::endl;
    }
};

// ----------------------------------------- next container --------------------------------------------------------------------------
  
// One direction List
//
template <typename T>
class List
{
private:
    class ListItem
    {
    private:
        T           m_item;
        ListItem*   m_next;
    public:
        T           getValue()  {   return m_item; }
        void        setValue(T value)  {   m_item = value; }
        ListItem*   getNext()   {   return m_next; }
        void        setNext(ListItem* next)   {   m_next = next; }
        ListItem (T value, ListItem* next = nullptr) : m_item(value), m_next(next) {}
        ~ListItem() {}
    };

    size_t       m_size;     // current size - last element index
    ListItem*    m_head;     // head of list
    ListItem*    m_rear;     // rear of list

public: 
    List() : m_size(0), m_head(nullptr), m_rear(nullptr) {}
    List(const List& rhs)
    {
        ListItem * curr = m_head;
        ListItem * prev = nullptr;

        m_size = rhs.m_size;

        for (ListItem * p = rhs.m_head; p != nullptr; p = p->getNext())
        {
            curr = new ListItem (p->getValue());  // new list item
            if (prev == nullptr)        // no previous item
                m_head = curr;          // update list head 
            else
                prev->setNext(curr);
            prev = curr;
            curr = curr->getNext();
        }
        m_rear = prev;              // update rear
    }
    ~List() {
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
    char*  name() const noexcept    {    return "One Direction List"; }

    // get size
    //
    size_t  size() const noexcept   {    return m_size; }

    // check the container for emptiness
    //
    bool    empty() const noexcept  {   return m_size == 0; }

    // append an item to list
    //
    void push_back (const T& item)
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
    }

    // remove and return the last item of List
    //
    T pop_back ()
    {
        if (empty())
            throw std::runtime_error ("List is empty!");

        ListItem * prev = nullptr;
        for (ListItem * curr = m_head; curr != nullptr; curr = curr->getNext()) 
        {
            prev = curr;
            if (curr->getNext() == nullptr) // found last one
            {
                if (prev == nullptr)    // remove the last item
                    m_head = m_rear = nullptr;
                else
                {
                    m_rear = prev;
                    m_rear->setNext (nullptr);
                }
                --m_size;
                return curr->getValue();
            }     
        }
    }

    // insert an item at specified position
    //
    void insert (size_t pos, T item)
    {
        if (pos > m_size)
            throw std::runtime_error ("Wrong position in List!");

        ListItem * nItem = new ListItem (item);

        ListItem * prev = nullptr;
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
        if (idx == pos)  // correct position found
            ++m_size;    
        else
            delete nItem;
    }
    
    // remove an item at specified position
    //
    void erase (size_t pos)
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
    }

    // return an item at specified position
    //
    T operator[] (size_t pos)
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

        throw std::runtime_error ("Wrong position in List!");
    }

    // assignment operator
    //
    List& operator= (const List& rhs)
    {
        ListItem * curr = m_head;
        ListItem * prev = nullptr;
        int cnt = 0;

        for (ListItem * pRhs = rhs.m_head; pRhs != nullptr; pRhs = pRhs->getNext())
        {
            if (curr == nullptr)    
            {
                ++cnt;
                curr = new ListItem (pRhs->getValue());  // new list item
                if (prev == nullptr)        // no previous item
                    m_head = curr;          // update list head 
                else
                    prev->setNext(curr);
            }
            else
                curr->setValue (pRhs->getValue());       // update current value
            prev = curr;
            curr = curr->getNext();
        }

        m_rear = prev;              // update rear

        if (curr != nullptr)        // source list is larger than rhs
        {   // clear the list rest
            prev->setNext(nullptr); // finalize the list
            while (curr != nullptr)
            {
                prev = curr;
                curr = curr->getNext();
                delete prev;
            }
        }
        m_size = rhs.m_size;

        return *this;
    }

    // print container content
    //
    void print ()
    {
        for (ListItem * curr = m_head; curr != nullptr; curr = curr->getNext()) 
        {
            if (curr !=m_head)  std::cout << " ";
            std::cout << curr->getValue();
        }
        std::cout << std::endl;
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
    list_iterator end()   { return list_iterator(nullptr); }
};

