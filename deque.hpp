#ifndef SJTU_DEQUE_HPP
#define SJTU_DEQUE_HPP

#include "exceptions.hpp"

#include <cstddef>

namespace sjtu {
template <class T>
class double_list {
public:
    /**
     * the nodes of double_list
     */
    struct Node {
        T* val;
        Node *pre, *nxt;
        Node(T* val, Node* pre = nullptr, Node* nxt = nullptr)
            : val(val)
            , pre(pre)
            , nxt(nxt)
        {
        }
        ~Node()
        {
            if (val != nullptr)
                delete val;
        }
    };
    Node *head, *tail;

    /**
     * constructors and destructors
     * here we denote tail as a node points to nothing
     */
    double_list()
    {
        head = tail = new Node(nullptr);
    }
    double_list(const double_list<T>& other)
    {
        head = tail = new Node(nullptr);
        Node* temp = other.head;
        while (temp != other.tail) {
            insert_tail(*(temp->val));
            temp = temp->nxt;
        }
    }
    ~double_list()
    {
        destroy();
    }
    double_list& operator=(const double_list& other)
    {
        if (this == &other)
            return *this;
        destroy();
        head = tail = new Node(nullptr);
        Node* temp = other.head;
        while (temp != other.tail) {
            insert_tail(*(temp->val));
            temp = temp->nxt;
        }
        return *this;
    }

    /**
     * the iterator of double_list
     * directly point to the Node of double_list
     */
    class iterator {
    public:
        Node* p;
        iterator(Node* p = nullptr)
            : p(p)
        {
        }
        iterator(const iterator& t)
        {
            p = t.p;
        }
        ~iterator()
        {
            p = nullptr;
        }
        Node* at()
        {
            return p;
        }
        /**
         * iter++
         */
        iterator operator++(int)
        {
            if (p == nullptr || p->nxt == nullptr)
                throw invalid_iterator();
            Node* temp = p;
            p = p->nxt;
            return iterator(temp);
        }
        /**
         * ++iter
         */
        iterator& operator++()
        {
            if (p == nullptr || p->nxt == nullptr)
                throw invalid_iterator();
            p = p->nxt;
            return *this;
        }
        /**
         * iter--
         */
        iterator operator--(int)
        {
            if (p == nullptr || p->pre == nullptr)
                throw invalid_iterator();
            Node* temp = p;
            p = p->pre;
            return iterator(temp);
        }
        /**
         * --iter
         */
        iterator& operator--()
        {
            if (p == nullptr || p->pre == nullptr)
                throw invalid_iterator();
            p = p->pre;
            return *this;
        }
        /**
         * if the iter didn't point to a value
         * throw
         */
        T& operator*() const
        {
            if (p == nullptr || p->nxt == nullptr)
                throw invalid_iterator();
            return *(p->val);
        }
        T* operator->() const noexcept
        {
            if (p == nullptr || p->nxt == nullptr)
                throw invalid_iterator();
            return p->val;
        }
        /**
         * other operation
         */
        bool operator==(const iterator& rhs) const
        {
            return p == rhs.p;
        }
        bool operator!=(const iterator& rhs) const
        {
            return p != rhs.p;
        }
    };

    /**
     * return an iterator point at the beginning
     * const or not
     */
    iterator begin()
    {
        return iterator(head);
    }
    iterator begin() const
    {
        return iterator(head);
    }
    /**
     * return an iterator point at the last element
     * maybe not exist
     */
    iterator last()
    {
        if (tail->pre == nullptr)
            throw invalid_iterator();
        return iterator(tail->pre);
    }
    /**
     * return an iterator point at the ending
     * const or not
     */
    iterator end()
    {
        return iterator(tail);
    }
    iterator end() const
    {
        return iterator(tail);
    }

    /**
     * if didn't contain anything, return true,
     * otherwise false
     */
    bool empty() const
    {
        return head != tail;
    }
    /**
     * destroy the whole double_list
     * can't be used after destroy
     */
    void destroy()
    {
        Node* temp = head;
        while (temp != tail) {
            Node* temp2 = temp->nxt;
            delete temp;
            temp = temp2;
        }
        delete tail;
        return;
    }
    /**
     * clear the whole double_list
     * can be used after clear
     */
    void clear()
    {
        destroy();
        head = tail = new Node(nullptr);
        return;
    }

    /**
     * erase the element at iterator pos
     * if the iter didn't point to anything, do nothing
     * otherwise, delete the element pointed by the iter
     * and return the iterator point at the same "index"
     * e.g.
     * 	if the origin iterator point at the 2nd element
     * 	the returned iterator also point at the
     *  2nd element of the list after the operation
     *  or the tail if the list after the operation
     *  don't contain 2nd elememt.
     */
    iterator erase(iterator pos)
    {
        if (pos == iterator(head)) {
            delete_head();
            return iterator(head);
        } else {
            Node* cur = pos.p;
            cur->pre->nxt = cur->nxt;
            cur->nxt->pre = cur->pre;
            pos.p = cur->nxt;
            delete cur;
            return pos;
        }
    }
    /**
     * insert an element at the head of the list
     */
    void insert_head(const T& val)
    {
        Node* cur = new Node(new T(val));
        if (head == tail) {
            cur->nxt = head;
            cur->nxt->pre = cur;
            head = cur;
        } else {
            cur->nxt = head;
            cur->nxt->pre = cur;
            head = cur;
        }
        return;
    }
    /**
     * insert an element at the tail of the list
     */
    void insert_tail(const T& val)
    {
        Node* cur = new Node(new T(val));
        if (head == tail) {
            cur->nxt = tail;
            cur->nxt->pre = cur;
            head = cur;
        } else {
            cur->nxt = tail;
            cur->pre = tail->pre;
            cur->nxt->pre = cur;
            cur->pre->nxt = cur;
        }
        return;
    }
    /**
     * delete the head of the list
     */
    void delete_head()
    {
        if (head == tail)
            throw container_is_empty();
        Node* cur = head;
        if (cur->nxt == tail) {
            cur->nxt->pre = nullptr;
            delete cur;
            head = tail;
        } else {
            Node* tmp = cur->nxt;
            cur->nxt->pre = nullptr;
            delete cur;
            head = tmp;
        }
        return;
    }
    /**
     * delete the tail of the list
     */
    void delete_tail()
    {
        if (head == tail)
            throw container_is_empty();
        Node* cur = tail->pre;
        if (head->nxt == tail) {
            cur->nxt->pre = nullptr;
            delete cur;
            head = tail;
        } else {
            cur->nxt->pre = cur->pre;
            cur->pre->nxt = cur->nxt;
            delete cur;
        }
        return;
    }
};
template <class T>
class deque {
public:
    static int SIZE = 1e5;
    size_t size;
    struct Blocks {
        double_list<T> list;
        Blocks *pre, nxt;
        Blocks(Blocks* pre = nullptr, Blocks* nxt = nullptr)
            : pre(pre)
            , nxt(nxt)
        {
        }
    };
    double_list<Blocks>* blocks;
    class const_iterator;
    class iterator {
    private:
        struct Node {
            double_list<Blocks>::iterator block;
            double_list<T>::iterator element;
            Node(double_list<Blocks>::iterator block, double_list<T>::iterator element)
                : block(block)
                , element(element)
            {
            }
        };
        Node* p;
        /**
         * add data members.
         * just add whatever you want.
         */
    public:
        /**
         * return a new iterator which points to the n-next element.
         * if there are not enough elements, the behaviour is undefined.
         * same for operator-.
         */
        iterator()
        {
            p = nullptr;
        }
        iterator(Node* p)
            : p(p)
        {
        }
        iterator(const iterator& other)
        {
            p = other.p;
        }
        iterator(double_list<Blocks>::iterator block, double_list<T>::iterator element)
        {
            p = new Node(block, element);
        }
        ~iterator()
        {
            p = nullptr;
        }
        iterator operator+(const int& n) const
        {
            iterator temp = *this;
            for (int i = 0; i < n; i++)
                temp++;
            return temp;
        }
        iterator operator-(const int& n) const
        {
            iterator temp = *this;
            for (int i = 0; i < n; i++)
                temp--;
            return temp;
        }

        /**
         * return the distance between two iterators.
         * if they point to different vectors, throw
         * invaild_iterator.
         */
        int operator-(const iterator& rhs) const
        {
            int dis = 0;
            Node* temp = rhs;
            while (temp != p) {
                dis++;
                temp++;
            }
            return dis;
        }
        iterator& operator+=(const int& n)
        {
            for (int i = 0; i < n; i++)
                *this ++;
            return *this;
        }
        iterator& operator-=(const int& n)
        {
            for (int i = 0; i < n; i++)
                *this --;
            return *this;
        }

        /**
         * iter++
         */
        iterator operator++(int)
        {
            iterator temp = *this;
            if (p->element.at() == p->block.at()->list.last().at()->val) {
                p->block++;
                p->element = p->block.at()->list.begin();
            } else
                p->element++;
            return temp;
        }
        /**
         * ++iter
         */
        iterator& operator++()
        {
            if (p->element.at() == p->block.at()->list.last().at()->val) {
                p->block++;
                p->element = p->block.at()->list.begin();
            } else
                p->element++;
            return *this;
        }
        /**
         * iter--
         */
        iterator operator--(int)
        {
            iterator temp = *this;
            if (p->element.at() == p->block.at()->list.begin().at()->val) {
                p->block--;
                p->element = p->block.at()->list.last();
            } else
                p->element--;
            return temp;
        }
        /**
         * --iter
         */
        iterator& operator--()
        {
            if (p->element.at() == p->block.at()->list.begin().at()->val) {
                p->block--;
                p->element = p->block.at()->list.last();
            } else
                p->element--;
            return *this;
        }

        /**
         * *it
         */
        T& operator*() const
        {
            return *(p->element.at());
        }
        /**
         * it->field
         */
        T* operator->() const noexcept
        {
            return p->element.at();
        }

        /**
         * check whether two iterators are the same (pointing to the same
         * memory).
         */
        bool operator==(const iterator& rhs) const
        {
            return p == rhs.p;
        }
        bool operator==(const const_iterator& rhs) const
        {
            return p == rhs.p;
        }
        /**
         * some other operator for iterators.
         */
        bool operator!=(const iterator& rhs) const
        {
            return p != rhs.p;
        }
        bool operator!=(const const_iterator& rhs) const
        {
            return p != rhs.p;
        }
    };

    class const_iterator : private iterator {
    private:
        /**
         * it should has similar member method as iterator.
         * you can copy them, but with care!
         * and it should be able to be constructed from an iterator.
         */
    public:
        const_iterator()
        {
            p = nullptr;
        }
        const_iterator(Node* p)
            : p(p)
        {
        }
        const_iterator(const const_iterator& other)
        {
            p = other.p;
        }
        const_iterator(double_list<Blocks>::iterator block, double_list<T>::iterator element)
        {
            p = new Node(block, element);
        }
        ~const_iterator()
        {
            p = nullptr;
        }
        const_iterator(iterator& other)
        {
            p = other.p;
        }
        const_iterator operator+(const int& n) const
        {
            const_iterator temp = *this;
            for (int i = 0; i < n; i++)
                temp++;
            return temp;
        }
        const_iterator operator-(const int& n) const
        {
            const_iterator temp = *this;
            for (int i = 0; i < n; i++)
                temp--;
            return temp;
        }

        /**
         * return the distance between two iterators.
         * if they point to different vectors, throw
         * invaild_iterator.
         */
        int operator-(const iterator& rhs) const
        {
            int dis = 0;
            Node* temp = rhs;
            while (temp != p) {
                dis++;
                temp++;
            }
            return dis;
        }
        iterator& operator+=(const int& n)
        {
            for (int i = 0; i < n; i++)
                *this ++;
            return *this;
        }
        iterator& operator-=(const int& n)
        {
            for (int i = 0; i < n; i++)
                *this --;
            return *this;
        }

        /**
         * iter++
         */
        iterator operator++(int)
        {
            iterator temp = *this;
            if (p->element.at() == p->block.at()->list.last().at()->val) {
                p->block++;
                p->element = p->block.at()->list.begin();
            } else
                p->element++;
            return temp;
        }
        /**
         * ++iter
         */
        iterator& operator++()
        {
            if (p->element.at() == p->block.at()->list.last().at()->val) {
                p->block++;
                p->element = p->block.at()->list.begin();
            } else
                p->element++;
            return *this;
        }
        /**
         * iter--
         */
        iterator operator--(int)
        {
            iterator temp = *this;
            if (p->element.at() == p->block.at()->list.begin().at()->val) {
                p->block--;
                p->element = p->block.at()->list.last();
            } else
                p->element--;
            return temp;
        }
        /**
         * --iter
         */
        iterator& operator--()
        {
            if (p->element.at() == p->block.at()->list.begin().at()->val) {
                p->block--;
                p->element = p->block.at()->list.last();
            } else
                p->element--;
            return *this;
        }

        /**
         * *it
         */
        const T& operator*() const
        {
            return *(p->element.at());
        }
        /**
         * it->field
         */
        const T* operator->() const noexcept
        {
            return p->element.at();
        }

        /**
         * check whether two iterators are the same (pointing to the same
         * memory).
         */
        bool operator==(const iterator& rhs) const
        {
            return p == rhs.p;
        }
        bool operator==(const const_iterator& rhs) const
        {
            return p == rhs.p;
        }
        /**
         * some other operator for iterators.
         */
        bool operator!=(const iterator& rhs) const
        {
            return p != rhs.p;
        }
        bool operator!=(const const_iterator& rhs) const
        {
            return p != rhs.p;
        }
    };

    /**
     * constructors.
     */
    deque()
    {
        blocks = new double_list<Blocks>();
        size = 0;
    }
    deque(const deque& other)
    {
        destroy();
        blocks = new double_list<Blocks>();
        size = other.size;
        double_list<Blocks>::iterator temp = other.blocks->begin();
        while (temp != other.blocks->end()) {
            Blocks* cur = new Blocks();
            cur->list = temp.at()->list;
            blocks->insert_tail(*cur);
            temp++;
        }
    }
    /**
     * deconstructor.
     */
    ~deque()
    {
        destroy();
    }

    /**
     * assignment operator.
     */
    deque& operator=(const deque& other)
    {
        if (this == &other)
            return *this;
        destroy();
        blocks = new double_list<Blocks>();
        double_list<Blocks>::iterator temp = other.blocks->begin();
        while (temp != other.blocks->end()) {
            Blocks* cur = new Blocks();
            cur->list = temp.at()->list;
            blocks->insert_tail(*cur);
            temp++;
        }
        return *this;
    }
    void destroy()
    {
        double_list<Blocks>::iterator temp = blocks->begin();
        while (temp != blocks->end()) {
            delete temp.at();
            temp++;
        }
        delete blocks;
        return;
    }

    /**
     * access a specified element with bound checking.
     * throw index_out_of_bound if out of bound.
     */
    T& at(const size_t& pos) { }
    const T& at(const size_t& pos) const { }
    T& operator[](const size_t& pos) { }
    const T& operator[](const size_t& pos) const { }

    /**
     * access the first element.
     * throw container_is_empty when the container is empty.
     */
    const T& front() const
    {
        if (blocks->empty())
            throw container_is_empty();
        return blocks->begin().at()->list.begin().at()->val;
    }
    /**
     * access the last element.
     * throw container_is_empty when the container is empty.
     */
    const T& back() const
    {
        if (blocks->empty())
            throw container_is_empty();
        return blocks->last().at()->list.last().at()->val;
    }

    /**
     * return an iterator to the beginning.
     */
    iterator begin()
    {
        return iterator(blocks->begin(), blocks->begin().at()->list.begin());
    }
    const_iterator cbegin() const
    {
        return const_iterator(blocks->begin(), blocks->begin().at()->list.begin());
    }

    /**
     * return an iterator to the end.
     */
    iterator end()
    {
        return iterator(blocks->end(), blocks->end().at()->list.begin());
    }
    const_iterator cend() const
    {
        return const_iterator(blocks->end(), blocks->end().at()->list.begin());
    }

    /**
     * check whether the container is empty.
     */
    bool empty() const
    {
        return blocks->empty();
    }

    /**
     * return the number of elements.
     */
    size_t size() const
    {
        return size;
    }

    /**
     * clear all contents.
     */
    void clear()
    {
        destroy();
        blocks = new double_list<Blocks>();
        size = 0;
        return;
    }

    /**
     * insert value before pos.
     * return an iterator pointing to the inserted value.
     * throw if the iterator is invalid or it points to a wrong place.
     */
    iterator insert(iterator pos, const T& value) { }

    /**
     * remove the element at pos.
     * return an iterator pointing to the following element. if pos points to
     * the last element, return end(). throw if the container is empty,
     * the iterator is invalid, or it points to a wrong place.
     */
    iterator erase(iterator pos) { }

    /**
     * add an element to the end.
     */
    void push_back(const T& value)
    {
        if (blocks->empty() || blocks->last()->list.size >= SIZE) {
            Blocks* cur = new Blocks();
            cur->list.insert_tail(value);
            blocks->insert_tail(*cur);
        }
        blocks->last()->list.insert_tail(value);
        size++;
        return;
    }

    /**
     * remove the last element.
     * throw when the container is empty.
     */
    void pop_back()
    {
        if (blocks->empty())
            throw container_is_empty();
        blocks->last()->list.delete_tail();
        size--;
        if (blocks->last()->list.size == 0)
            blocks->delete_tail();
        return;
    }

    /**
     * insert an element to the beginning.
     */
    void push_front(const T& value)
    {
        if (blocks->empty() || blocks->begin()->list.size >= SIZE) {
            Blocks* cur = new Blocks();
            cur->list.insert_head(value);
            blocks->insert_head(*cur);
        }
        blocks->begin()->list.insert_head(value);
        size++;
        return;
    }

    /**
     * remove the first element.
     * throw when the container is empty.
     */
    void pop_front()
    {
        if (blocks->empty())
            throw container_is_empty();
        blocks->begin()->list.delete_head();
        size--;
        if (blocks->begin()->list.size == 0)
            blocks->delete_head();
        return;
    }
};

} // namespace sjtu

#endif