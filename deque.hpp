#ifndef SJTU_DEQUE_HPP
#define SJTU_DEQUE_HPP

#include "exceptions.hpp"
#include <cassert>
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
        for (const_iterator it = other.cbegin(); it != other.cend(); it++)
            insert_tail(*it);
        sz = other.sz;
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
            Node* temp = p;
            for (size_t i = 0; i < n; i++)
                temp = temp->nxt;
            return iterator(temp, base);
        }
        iterator operator-(const size_t& n) const
        {
            Node* temp = p;
            for (size_t i = 0; i < n; i++)
                temp = temp->pre;
            return iterator(temp, base);
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
    };
    class const_iterator : public iterator {
    public:
        const_iterator(Node* p = nullptr, const double_list<T>* base = nullptr)
            : iterator(p, base)
        {
        }
        const_iterator(const iterator& t)
            : iterator(t)
        {
        }
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
        return const_iterator(tail->pre);
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
        return head == tail;
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
        if (pos.get_base() != this || pos == end() || pos.p == nullptr)
            throw invalid_iterator();
        sz++;
        Node* cur = new Node(new T(val));
        cur->nxt = pos.p->nxt;
        cur->pre = pos.p;
        pos.p->nxt->pre = cur;
        pos.p->nxt = cur;
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
        if (pos.get_base() != this)
            throw invalid_iterator();
        size_t cnt = 0;
        auto it = begin();
        while (it != pos) {
            cnt++;
            it++;
        }
        if (it == end())
            throw invalid_iterator();
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
    /**
     * insert an element at the tail of the list
     */
    void insert_tail(const T& val)
    {
        if (empty())
            insert_head(val);
        else
            insert(last(), val);
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
    void print() { }
};
template <class T>
class deque {
private:
    double_list<double_list<T>>* block;
    size_t length, sz;

public:
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
        iterator operator+(const size_t& n) const
        {
            typename double_list<double_list<T>>::iterator temp = block_it;
            assert(temp == base->block->begin());
            size_t cnt = n, pos = temp->get_pos(list_it);
            while (pos + cnt >= temp->size()) {
                cnt -= temp->size() - pos;
                temp++;
                pos = 0;
            }
            if (temp == base->block->cend())
                return iterator(temp, nullptr, base);
            return iterator(temp, temp->begin() + cnt, base);
        }
        iterator operator-(const size_t& n) const
        {
            typename double_list<double_list<T>>::iterator temp = block_it;
            size_t cnt = n, pos = temp->get_pos(list_it);
            while (pos - cnt < 0) {
                cnt -= pos;
                temp--;
                pos = temp->size() - 1;
            }
            return iterator(temp, temp->end() - cnt, base);
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
            if (lpos == rpos)
                return block_it->get_pos(list_it) - rhs.block_it->get_pos(rhs.list_it);
            else {
                bool greater = lpos > rpos;
                size_t cnt = 0;
                auto temp = greater ? rhs : *this;
                while (temp.block_it != (greater ? block_it : rhs.block_it)) {
                    cnt += temp.block_it->size();
                    temp.block_it++;
                }
                lpos = block_it->get_pos(list_it);
                rpos = rhs.block_it->get_pos(rhs.list_it);
                return greater ? cnt + lpos - rpos : cnt + rpos - lpos;
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
            iterator temp = *this;
            if (*this == base->clast()) {
                block_it = base->block->end();
                list_it = nullptr;
                return temp;
            }
            if (list_it == block_it->last()) {
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
            if (*this == base->clast()) {
                block_it = base->block->end();
                list_it = nullptr;
                return *this;
            }
            if (list_it == block_it->last()) {
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
            return list_it.get();
        }
        /**
         * *it
         */
        T& operator*() const
        {
            return *list_it;
        }
        /**
         * it->field
         */
        T* operator->() const noexcept
        {
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
        const T& operator*() const
        {
            return *this->list_it;
        }
        const T* operator->() const noexcept
        {
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
        length = 1;
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
        length = 1;
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
    T& at(const size_t& pos)
    {
        return *(begin() + pos);
    }
    const T& at(const size_t& pos) const
    {
        return *(cbegin() + pos);
    }
    T& operator[](const size_t& pos)
    {
        return at(pos);
    }
    const T& operator[](const size_t& pos) const
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
        return iterator(block->end(), nullptr, this);
    }
    const_iterator cend() const
    {
        return const_iterator(block->end(), nullptr, this);
    }
    void print()
    {
        for (auto temp = block->begin(); temp != block->end(); temp++) {
            for (auto temp2 = temp->begin(); temp2 != temp->end(); temp2++)
                (*temp2).print();
            std::cerr << std::endl;
        }
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
        block->clear();
        length = 1;
        sz = 0;
    }

    /**
     * split the block at pos
     */
    iterator split(iterator pos)
    {
        if (pos.block_it->size() <= length)
            return pos;
        std::cerr << "split" << std::endl;
        size_t old_pos = pos.block_it->get_pos(pos.list_it);
        typename double_list<T>::iterator temp = pos.block_it->begin();
        typename double_list<double_list<T>>::iterator block1 = block->insert(pos.block_it, double_list<T>());
        while (block1->size() + 1 <= pos.block_it->size() / 2) {
            block1->insert_tail(*temp);
            temp++;
        }
        typename double_list<double_list<T>>::iterator block2 = block->insert(block1, double_list<T>());
        while (temp != pos.block_it->end()) {
            block2->insert_tail(*temp);
            temp++;
        }
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
        if (pos.block_it == block->last() || pos.block_it->size() + (pos.block_it + 1)->size() > length)
            return pos;
        std::cerr << "merge" << std::endl;
        size_t old_pos = pos.block_it->get_pos(pos.list_it);
        double_list<T> temp = *pos.block_it;
        pos.block_it = block->erase(pos.block_it);
        for (auto it = temp.end() - 1;; it--) {
            pos.block_it->insert_head(*it);
            if (it == temp.begin())
                break;
        }
        return iterator(pos.block_it, pos.block_it->begin() + old_pos, this);
    }
    /**
     * reconstruct the block
     */
    iterator reconstruct(iterator pos)
    {
        if (block->size() <= length && block->size() > length / 4)
            return pos;
        std::cerr << "reconstruct" << std::endl;
        if (block->size() > length)
            length <<= 1;
        else
            length >>= 1;
        double_list<T> list;
        size_t new_pos = 0;
        for (auto temp = block->begin(); temp != block->end(); temp++) {
            if (temp != pos.block_it)
                new_pos += temp->size();
            else
                new_pos += pos.block_it->get_pos(pos.list_it);
            for (auto temp2 = temp->begin(); temp2 != temp->end(); temp2++)
                list.insert_tail(*temp2);
        }
        block->clear();
        size_t cnt = 0;
        typename double_list<T>::iterator temp = list.begin();
        while (temp != list.end()) {
            if (cnt % length == 0)
                block->insert_tail(double_list<T>());
            block->last()->insert_tail(*temp);
            if (cnt == new_pos)
                pos = iterator(block->last(), block->last()->last(), this);
            temp++;
            cnt++;
        }
        return pos;
    }

    /**
     * insert value before pos.
     * return an iterator pointing to the inserted value.
     * throw if the iterator is invalid or it points to a wrong place.
     */
    iterator insert(iterator pos, const T& value)
    {
        if (pos == end() || pos.base != this)
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
        if (empty())
            push_front(value);
        else
            insert(last(), value);
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
        sz++;
        block->begin()->insert_head(value);
        split(begin());
        reconstruct(begin());
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