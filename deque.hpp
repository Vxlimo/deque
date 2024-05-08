#ifndef SJTU_DEQUE_HPP
#define SJTU_DEQUE_HPP

#include "exceptions.hpp"
#include <cmath>
#include <cstddef>
#include <iostream>

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
    size_t sz;

    /**
     * constructors and destructors
     * here we denote tail as a node points to nothing
     */
    double_list()
    {
        head = tail = new Node(nullptr);
        sz = 0;
    }
    double_list(const double_list<T>& other)
    {
        head = tail = new Node(nullptr);
        sz = 0;
        for (const_iterator it = other.cbegin(); it != other.cend(); it++)
            insert_tail(*it);
    }
    ~double_list()
    {
        destroy();
    }
    double_list& operator=(const double_list& other)
    {
        if (this == &other)
            return *this;
        clear();
        for (const_iterator it = other.cbegin(); it != other.cend(); it++)
            insert_tail(*it);
        sz = other.sz;
        return *this;
    }

    /**
     * the iterator of double_list
     * directly point to the Node of double_list
     */
    class const_iterator;
    class iterator {
    public:
        Node* p;
        const double_list<T>* base;
        iterator(Node* p = nullptr, const double_list<T>* base = nullptr)
            : p(p)
            , base(base)
        {
        }
        iterator(const iterator& t)
        {
            p = t.p;
            base = t.base;
        }
        ~iterator()
        {
            p = nullptr;
            base = nullptr;
        }
        iterator operator+(const size_t& n) const
        {
            iterator temp = *this;
            for (size_t i = 0; i < n; i++)
                temp++;
            return temp;
        }
        iterator operator-(const size_t& n) const
        {
            iterator temp = *this;
            for (size_t i = 0; i < n; i++)
                temp--;
            return temp;
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
            return iterator(temp, base);
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
            return iterator(temp, base);
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
        T* get() const
        {
            if (p == nullptr)
                throw invalid_iterator();
            return p->val;
        }
        const double_list<T>* get_base() const
        {
            return base;
        }

        /**
         * if the iter didn't point to a value
         * throw
         */
        T& operator*() const
        {
            if (p == nullptr)
                throw invalid_iterator();
            return *(p->val);
        }
        T* operator->() const noexcept
        {
            if (p == nullptr)
                throw invalid_iterator();
            return p->val;
        }
        /**
         * other operation
         */
        bool operator==(const iterator& rhs) const
        {
            return p == rhs.p && base == rhs.base;
        }
        bool operator!=(const iterator& rhs) const
        {
            return p != rhs.p || base != rhs.base;
        }
        bool operator==(const const_iterator& rhs) const
        {
            return p == rhs.p && base == rhs.base;
        }
        bool operator!=(const const_iterator& rhs) const
        {
            return p != rhs.p || base != rhs.base;
        }
    };
    class const_iterator : public iterator {
    public:
        const_iterator() = default;
        const_iterator(Node* p = nullptr, const double_list<T>* base = nullptr)
            : iterator(p, base)
        {
        }
        const_iterator(const iterator& t)
            : iterator(t)
        {
        }
        ~const_iterator() = default;
        const T& operator*() const
        {
            if (this->p == nullptr)
                throw invalid_iterator();
            return *(this->p->val);
        }
        const T* operator->() const noexcept
        {
            if (this->p == nullptr)
                throw invalid_iterator();
            return this->p->val;
        }
        const T* get() const
        {
            if (this->p == nullptr)
                throw invalid_iterator();
            return this->p->val;
        }
    };

    /**
     * return an iterator point at the beginning
     * const or not
     */
    iterator begin()
    {
        return iterator(head, this);
    }
    const_iterator cbegin() const
    {
        return const_iterator(head, this);
    }
    /**
     * return an iterator point at the last element
     * maybe not exist
     */
    iterator last()
    {
        return iterator(tail->pre, this);
    }
    const_iterator clast() const
    {
        if (empty())
            throw container_is_empty();
        return const_iterator(tail->pre, this);
    }
    /**
     * return an iterator point at the ending
     * const or not
     */
    iterator end()
    {
        return iterator(tail, this);
    }
    const_iterator cend() const
    {
        return const_iterator(tail, this);
    }

    /**
     * if didn't contain anything, return true,
     * otherwise false
     */
    bool empty() const
    {
        return sz == 0;
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
        sz = 0;
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
        sz = 0;
        return;
    }
    /**
     * the size of the double_list
     */
    size_t size() const
    {
        return sz;
    }

    /* insert an element after iterator pos */
    iterator insert(iterator pos, const T& val)
    {
        if (pos.get_base() != this || pos.p == nullptr)
            throw invalid_iterator();
        if (pos == begin()) {
            insert_head(val);
            return begin();
        }
        sz++;
        Node* cur = new Node(new T(val));
        cur->nxt = pos.p;
        cur->pre = pos.p->pre;
        cur->nxt->pre = cur;
        cur->pre->nxt = cur;
        return iterator(cur, this);
    }
    iterator insert_ptr(iterator pos, T* val)
    {
        if (pos.get_base() != this || pos.p == nullptr)
            throw invalid_iterator();
        if (pos == begin()) {
            insert_head_ptr(val);
            return begin();
        }
        sz++;
        Node* cur = new Node(val);
        cur->nxt = pos.p;
        cur->pre = pos.p->pre;
        cur->nxt->pre = cur;
        cur->pre->nxt = cur;
        return iterator(cur, this);
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
        if (pos.get_base() != this || pos == end() || pos.p == nullptr)
            throw invalid_iterator();
        if (pos == begin()) {
            delete_head();
            return begin();
        } else {
            sz--;
            Node* cur = pos.p;
            cur->pre->nxt = cur->nxt;
            cur->nxt->pre = cur->pre;
            pos++;
            delete cur;
            return pos;
        }
    }
    /**
     * get the pos of iterator pos, 0-based
     */
    size_t get_pos(iterator pos)
    {
        if (pos.get_base() != this || pos.p == nullptr)
            throw invalid_iterator();
        size_t cnt = 0;
        auto it = begin();
        while (it != pos) {
            cnt++;
            if (it == end())
                break;
            it++;
        }
        return cnt;
    }

    /**
     * insert an element at the head of the list
     */
    void insert_head(const T& val)
    {
        sz++;
        Node* cur = new Node(new T(val));
        cur->nxt = head;
        cur->nxt->pre = cur;
        head = cur;
        return;
    }
    void insert_head_ptr(T* val)
    {
        sz++;
        Node* cur = new Node(val);
        cur->nxt = head;
        cur->nxt->pre = cur;
        head = cur;
        return;
    }
    /**
     * insert an element at the tail of the list
     */
    void insert_tail(const T& val)
    {
        insert(end(), val);
        return;
    }
    void insert_tail_ptr(T* val)
    {
        insert_ptr(end(), val);
        return;
    }
    /**
     * delete the head of the list
     */
    void delete_head()
    {
        if (empty())
            throw container_is_empty();
        sz--;
        Node* cur = head;
        head = head->nxt;
        head->pre = nullptr;
        delete cur;
        return;
    }
    /**
     * delete the tail of the list
     */
    void delete_tail()
    {
        if (empty())
            throw container_is_empty();
        erase(last());
        return;
    }
    std::pair<double_list<T>*, double_list<T>*> split(size_t pos)
    {
        double_list<T>* list1 = new double_list<T>();
        double_list<T>* list2 = new double_list<T>();
        if (pos == 0) {
            list2->head = head;
            delete list2->tail;
            list2->tail = tail;
            list2->sz = sz;
            head = tail = new Node(nullptr);
            sz = 0;
            return std::make_pair(list1, list2);
        }
        if (pos >= sz) {
            list1->head = head;
            delete list1->tail;
            list1->tail = tail;
            list1->sz = sz;
            head = tail = new Node(nullptr);
            sz = 0;
            return std::make_pair(list1, list2);
        }
        auto it1 = begin() + pos - 1, it2 = begin() + pos;
        list1->head = head;
        list1->tail->pre = it1.p;
        it1.p->nxt = list1->tail;
        list1->sz = pos;
        list2->head = it2.p;
        delete list2->tail;
        list2->tail = tail;
        it2.p->pre = nullptr;
        list2->sz = sz - pos;
        head = tail = new Node(nullptr);
        sz = 0;
        return std::make_pair(list1, list2);
    }
    void merge(double_list<T>* list)
    {
        if (empty()) {
            head = list->head;
            delete tail;
            tail = list->tail;
            sz = list->sz;
            list->head = list->tail = new Node(nullptr);
            list->sz = 0;
            return;
        }
        auto it = last();
        it.p->nxt = list->head;
        list->head->pre = it.p;
        delete tail;
        tail = list->tail;
        sz += list->sz;
        list->head = list->tail = new Node(nullptr);
        list->sz = 0;
        return;
    }
};
template <class T>
class deque {
public:
    double_list<double_list<T>>* block;
    size_t length, sz;

    class const_iterator;
    class iterator {
    public:
        /**
         * add data members.
         * just add whatever you want.
         */
        typename double_list<double_list<T>>::iterator block_it;
        typename double_list<T>::iterator list_it;
        const deque* base;
        iterator() = default;
        iterator(typename double_list<double_list<T>>::iterator block_it, typename double_list<T>::iterator list_it, const deque* base)
            : block_it(block_it)
            , list_it(list_it)
            , base(base)
        {
        }
        ~iterator() = default;
        iterator(const iterator& other)
            : block_it(other.block_it)
            , list_it(other.list_it)
            , base(other.base)
        {
        }
        iterator operator=(const iterator& other)
        {
            block_it = other.block_it;
            list_it = other.list_it;
            base = other.base;
            return *this;
        }

        /**
         * return a new iterator which points to the n-next element.
         * if there are not enough elements, the behaviour is undefined.
         * same for operator-.
         */
        iterator operator+(const int& n) const
        {
            if (n < 0)
                return *this - (-n);
            typename double_list<double_list<T>>::iterator temp = block_it;
            int cnt = n, pos = temp->get_pos(list_it);
            while (temp != base->block->end() && pos + cnt >= temp->size()) {
                cnt -= temp->size() - pos;
                temp++;
                pos = 0;
            }
            if (temp == base->block->end()) {
                if (cnt)
                    throw invalid_iterator();
                return base->cend();
            }
            return iterator(temp, temp->begin() + (pos + cnt), base);
        }
        iterator operator-(const int& n) const
        {
            if (n < 0)
                return *this + (-n);
            typename double_list<double_list<T>>::iterator temp = block_it;
            int cnt = n, pos = temp->get_pos(list_it);
            while (pos - cnt < 0) {
                cnt -= (pos + 1);
                temp--;
                pos = temp->size() - 1;
            }
            return iterator(temp, temp->begin() + (pos - cnt), base);
        }

        /**
         * return the distance between two iterators.
         * if they point to different vectors, throw
         * invaild_iterator.
         */
        int operator-(const iterator& rhs) const
        {
            if (base != rhs.base)
                throw invalid_iterator();
            size_t lpos = base->block->get_pos(block_it), rpos = rhs.base->block->get_pos(rhs.block_it);
            if (lpos == rpos) {
                return (int)block_it->get_pos(list_it) - (int)rhs.block_it->get_pos(rhs.list_it);
            } else {
                bool greater = lpos > rpos;
                int cnt = 0;
                auto temp = greater ? rhs : *this;
                while (temp.block_it != (greater ? block_it : rhs.block_it)) {
                    cnt += temp.block_it->size();
                    temp.block_it++;
                }
                lpos = block_it->get_pos(list_it);
                rpos = rhs.block_it->get_pos(rhs.list_it);
                return greater ? cnt + lpos - rpos : -(cnt + rpos - lpos);
            }
        }
        iterator& operator+=(const int& n)
        {
            iterator temp = (*this) + n;
            *this = temp;
            return *this;
        }
        iterator& operator-=(const int& n)
        {
            iterator temp = (*this) - n;
            *this = temp;
            return *this;
        }

        /**
         * iter++
         */
        iterator operator++(int)
        {
            if (*this == base->cend())
                throw invalid_iterator();
            iterator temp = *this;
            if (list_it == block_it->last() && block_it != base->block->last()) {
                block_it++;
                list_it = block_it->begin();
            } else
                list_it++;
            return temp;
        }
        /**
         * ++iter
         */
        iterator& operator++()
        {
            if (*this == base->cend())
                throw invalid_iterator();
            if (list_it == block_it->last() && block_it != base->block->last()) {
                block_it++;
                list_it = block_it->begin();
            } else
                list_it++;
            return *this;
        }
        /**
         * iter--
         */
        iterator operator--(int)
        {
            iterator temp = *this;
            if (list_it == block_it->begin()) {
                block_it--;
                list_it = block_it->last();
            } else
                list_it--;
            return temp;
        }
        /**
         * --iter
         */
        iterator& operator--()
        {
            if (list_it == block_it->begin()) {
                block_it--;
                list_it = block_it->last();
            } else
                list_it--;
            return *this;
        }

        T* get() const
        {
            if (*this == base->cend())
                throw invalid_iterator();
            return list_it.get();
        }
        /**
         * *it
         */
        T& operator*() const
        {
            if (*this == base->cend())
                throw invalid_iterator();
            return *list_it;
        }
        /**
         * it->field
         */
        T* operator->() const noexcept
        {
            if (*this == base->cend())
                throw invalid_iterator();
            return list_it.get();
        }

        /**
         * check whether two iterators are the same (pointing to the same
         * memory).
         */
        bool operator==(const iterator& rhs) const
        {
            return block_it == rhs.block_it && list_it == rhs.list_it && base == rhs.base;
        }
        bool operator==(const const_iterator& rhs) const
        {
            return block_it == rhs.block_it && list_it == rhs.list_it && base == rhs.base;
        }
        /**
         * some other operator for iterators.
         */
        bool operator!=(const iterator& rhs) const
        {
            return block_it != rhs.block_it || list_it != rhs.list_it || base != rhs.base;
        }
        bool operator!=(const const_iterator& rhs) const
        {
            return block_it != rhs.block_it || list_it != rhs.list_it || base != rhs.base;
        }
    };

    class const_iterator : public iterator {
    public:
        const_iterator() = default;
        const_iterator(const iterator& t)
            : iterator(t)
        {
        }
        const_iterator(typename double_list<double_list<T>>::iterator block_it, typename double_list<T>::iterator list_it, const deque* base)
            : iterator(block_it, list_it, base)
        {
        }
        ~const_iterator() = default;
        const T* get() const
        {
            if (*this == this->base->cend())
                throw invalid_iterator();
            return this->list_it.get();
        }
        const T& operator*() const
        {
            if (*this == this->base->cend())
                throw invalid_iterator();
            return *this->list_it;
        }
        const T* operator->() const noexcept
        {
            if (*this == this->base->cend())
                throw invalid_iterator();
            return this->list_it.get();
        }
    };

    /**
     * constructors.
     */
    deque()
    {
        block = new double_list<double_list<T>>();
        block->insert_tail(double_list<T>());
        length = 200;
        sz = 0;
    }
    deque(const deque& other)
    {
        block = new double_list<double_list<T>>(*other.block);
        length = other.length;
        sz = other.sz;
    }

    /**
     * deconstructor.
     */
    ~deque()
    {
        delete block;
        length = 200;
        sz = 0;
    }

    /**
     * assignment operator.
     */
    deque& operator=(const deque& other)
    {
        if (this == &other)
            return *this;
        delete block;
        block = new double_list<double_list<T>>(*other.block);
        length = other.length;
        sz = other.sz;
        return *this;
    }

    /**
     * access a specified element with bound checking.
     * throw index_out_of_bound if out of bound.
     */
    T& at(const int& pos)
    {
        return *(begin() + pos);
    }
    const T& at(const int& pos) const
    {
        return *(cbegin() + pos);
    }
    T& operator[](const int& pos)
    {
        return at(pos);
    }
    const T& operator[](const int& pos) const
    {
        return at(pos);
    }

    /**
     * access the first element.
     * throw container_is_empty when the container is empty.
     */
    const T& front() const
    {
        if (empty())
            throw container_is_empty();
        return *cbegin();
    }
    /**
     * access the last element.
     * throw container_is_empty when the container is empty.
     */
    const T& back() const
    {
        if (empty())
            throw container_is_empty();
        return *clast();
    }

    /**
     * return an iterator to the beginning.
     */
    iterator begin()
    {
        return iterator(block->begin(), block->begin()->begin(), this);
    }
    const_iterator cbegin() const
    {
        return const_iterator(block->begin(), block->begin()->begin(), this);
    }

    /**
     * return an iterator to the last element
     */
    iterator last()
    {
        return iterator(block->last(), block->last()->last(), this);
    }
    const_iterator clast() const
    {
        return const_iterator(block->last(), block->last()->last(), this);
    }

    /**
     * return an iterator to the end.
     */
    iterator end()
    {
        return iterator(block->last(), block->last()->end(), this);
    }
    const_iterator cend() const
    {
        return const_iterator(block->last(), block->last()->end(), this);
    }

    /**
     * check whether the container is empty.
     */
    bool empty() const
    {
        return sz == 0;
    }

    /**
     * return the number of elements.
     */
    size_t size() const
    {
        return sz;
    }

    /**
     * clear all contents.
     */
    void clear()
    {
        delete block;
        block = new double_list<double_list<T>>();
        block->insert_tail(double_list<T>());
        length = 200;
        sz = 0;
    }

    /**
     * split the block at pos
     */
    iterator split(iterator pos)
    {
        if (pos.block_it->size() <= length)
            return pos;
        size_t old_pos = pos.block_it->get_pos(pos.list_it);
        auto [temp1, temp2] = pos.block_it->split(pos.block_it->size() / 2);
        auto block1 = block->insert_ptr(pos.block_it, temp1);
        auto block2 = block->insert_ptr(pos.block_it, temp2);
        block->erase(pos.block_it);
        if (old_pos < block1->size())
            return iterator(block1, block1->begin() + old_pos, this);
        else
            return iterator(block2, block2->begin() + (old_pos - block1->size()), this);
    }
    /**
     * merge the block at pos and the block at pos + 1
     */
    iterator merge(iterator pos)
    {
        if (pos.block_it != block->last() && pos.block_it->size() + (pos.block_it + 1)->size() <= length) {
            size_t old_pos = pos.block_it->get_pos(pos.list_it);
            pos.block_it->merge((pos.block_it + 1).get());
            block->erase(pos.block_it + 1);
            return iterator(pos.block_it, pos.block_it->begin() + old_pos, this);
        }
        if (pos.block_it != block->begin() && pos.block_it->size() + (pos.block_it - 1)->size() <= length) {
            size_t old_pos = (pos.block_it - 1)->size() + pos.block_it->get_pos(pos.list_it);
            (pos.block_it - 1)->merge(pos.block_it.get());
            pos.block_it = pos.block_it - 1;
            block->erase(pos.block_it + 1);
            return iterator(pos.block_it, pos.block_it->begin() + old_pos, this);
        }
        return pos;
    }
    /**
     * reconstruct the block
     */
    iterator reconstruct(iterator pos)
    {
        static size_t last_construct = 0, opts = 0;
        opts++;
        if (opts - last_construct > std::min(size(), length * length)) {
            length = std::sqrt(size()) + 1;
            if (length < 200)
                length = 200;
            last_construct = opts;
        } else
            return pos;
        double_list<T>* list = new double_list<T>();
        size_t new_pos = 0;
        bool flag = false;
        for (auto temp = block->begin(); temp != block->end(); temp++) {
            if (temp != pos.block_it) {
                if (!flag)
                    new_pos += temp->size();
            } else {
                new_pos += temp->get_pos(pos.list_it);
                flag = true;
            }
            list->merge(temp.get());
        }
        delete block;
        block = new double_list<double_list<T>>();
        size_t cnt = 0;
        while (cnt < size()) {
            auto [temp1, temp2] = list->split(length);
            delete list;
            block->insert_tail_ptr(temp1);
            list = temp2;
            cnt += temp1->size();
        }
        if (list->size() || block->empty())
            block->insert_tail_ptr(list);
        delete list;
        return begin() + new_pos;
    }

    /**
     * insert value before pos.
     * return an iterator pointing to the inserted value.
     * throw if the iterator is invalid or it points to a wrong place.
     */
    iterator insert(iterator pos, const T& value)
    {
        if (pos.base != this)
            throw invalid_iterator();
        sz++;
        pos.list_it = pos.block_it->insert(pos.list_it, value);
        pos = split(pos);
        pos = reconstruct(pos);
        return pos;
    }

    /**
     * remove the element at pos.
     * return an iterator pointing to the following element. if pos points to
     * the last element, return end(). throw if the container is empty,
     * the iterator is invalid, or it points to a wrong place.
     */
    iterator erase(iterator pos)
    {
        if (pos == end() || pos.base != this)
            throw invalid_iterator();
        sz--;
        if (pos == last()) {
            pos.list_it = pos.block_it->erase(pos.list_it);
            if (pos.block_it->empty() && block->size() != 1)
                block->delete_tail();
            return end();
        }
        pos.list_it = pos.block_it->erase(pos.list_it);
        if (pos.list_it == pos.block_it->end()) {
            if (pos.block_it->empty())
                pos.block_it = block->erase(pos.block_it);
            else
                pos.block_it++;
            pos.list_it = pos.block_it->begin();
        }
        pos = merge(pos);
        pos = reconstruct(pos);
        return pos;
    }

    /**
     * add an element to the end.
     */
    void push_back(const T& value)
    {
        insert(end(), value);
        return;
    }

    /**
     * remove the last element.
     * throw when the container is empty.
     */
    void pop_back()
    {
        if (empty())
            throw container_is_empty();
        erase(last());
        return;
    }

    /**
     * insert an element to the beginning.
     */
    void push_front(const T& value)
    {
        insert(begin(), value);
        return;
    }

    /**
     * remove the first element.
     * throw when the container is empty.
     */
    void pop_front()
    {
        if (empty())
            throw container_is_empty();
        erase(begin());
        return;
    }
};

} // namespace sjtu

#endif