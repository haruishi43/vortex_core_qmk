/*******************************************************************************
**                                  LibChaos                                  **
**                                  zlist.h                                   **
**                          (c) 2015 Charlie Waters                           **
*******************************************************************************/
#ifndef ZLIST_H
#define ZLIST_H

#include "ztypes.h"
#include "zallocator.h"
#include "ziterator.h"
#include "yindexedaccess.h"
#include "ypushpopaccess.h"

#include <initializer_list>

// For debug()
//#include "zstring.h"
//#include "zlog.h"

namespace LibChaos {

/*! Linked list sequence container.
 *  Implemented as a circular doubly-linked list.
 *  ZList push/pop paradigm is FIFO.
 */
template <typename T> class ZList : public YIndexedAccess<T>, public YPushPopAccess<T> {
    friend class ZIterator<ZList<T>>;
public:
    struct Node {
        Node *prev;
        Node *next;
        T data;
    };

    class ZListIterator;
    class ZListConstIterator;

public:
    //! ZList default constructor.
    ZList(ZAllocator<Node> *alloc = new ZAllocator<Node>()) : _alloc(alloc), _size(0), _head(nullptr){}

    //! ZList initializer list constructor.
    ZList(std::initializer_list<T> ls) : ZList(){
        for(auto item = ls.begin(); item < ls.end(); ++item)
            pushBack(*item);
    }

    //! ZList array initializer.
    ZList(const T *data, zu64 size) : ZList(){
        for(zu64 i = 0; i < size; ++i)
            pushBack(data[i]);
    }

    //! ZList copy constructor.
    ZList(const ZList<T> &other) : ZList(){
//        for(auto i = other.cbegin(); i.more(); ++i){
//            pushBack(i.get());
//        }
        Node *current = other._head;
        for(zu64 i = 0; current != nullptr && i < other.size(); ++i){
            pushBack(current->data);
            current = current->next;
        }
    }

    ~ZList(){
        if(_head != nullptr){
            _head->prev->next = nullptr; // Break circular link
            Node *current = _head;
            while(current != nullptr){
                Node* next = current->next;
                _talloc.destroy(&(current->data));
                _alloc->dealloc(current);
                current = next;
            }
        }
        delete _alloc;
    }

    /*! Insert \a data before \a node.
     *  Will not update head.
     */
    Node *insert(const T &data, Node *node){
        Node *newnode = newNode(data);
        newnode->next = node;
        newnode->prev = node->prev;
        node->prev->next = newnode;
        node->prev = newnode;
        ++_size;
        return newnode;
    }

    //! Add \a data to the front of the list.
    void pushFront(const T &data){
        if(_head == nullptr){
            _head = newNode(data);
            _head->prev = _head;
            _head->next = _head;
            ++_size;
        } else {
            _head = insert(data, _head);
        }
    }
    //! Add \a data to the back of the list.
    void pushBack(const T &data){
        if(_head == nullptr){
            _head = newNode(data);
            _head->prev = _head;
            _head->next = _head;
            ++_size;
        } else {
            insert(data, _head);
        }
    }
    inline void push(const T &data){ pushBack(data); }

    //! Add each element in \a list to the back of the list.
    void append(const ZList<T> &list){
        for(auto it = list.cbegin(); it.more(); ++it){
            pushBack(it.get());
        }
    }

    /*! Remove \a node from the list.
     *  Will not update head.
     */
    void remove(Node *node){
        node->prev->next = node->next;
        node->next->prev = node->prev;
        --_size;
        _talloc.destroy(&(node->data));
        _alloc->dealloc(node);
    }

    //! Remove an element from the front of the list.
    void popFront(){
        if(_head != nullptr){
            Node *next = _head->next;
            remove(_head);
            _head = (_size ? next : nullptr);
        }
    }
    //! Remove an element from the back of the list.
    void popBack(){
        if(_head != nullptr){
            remove(_head->prev);
            if(!_size)
                _head = nullptr;
        }
    }
    inline void pop(){ popFront(); }

    //! Move the head of the list to the next element.
    void rotate(){
        _head = _head->next;
    }

    T &peekFront(){
        return _head->data;
    }
    const T &peekFront() const {
        return _head->data;
    }
    T &peekBack(){
        return _head->prev->data;
    }
    const T &peekBack() const {
        return _head->prev->data;
    }
    inline T &peek(){ return peekFront(); }
    inline const T &peek() const { return peekFront(); }

    //! Swap data but not allocators of two lists.
    void swap(ZList &other){
        zu64 tmpsize = _size;
        Node *tmphead = _head;
        _size = other._size;
        _head = other._head;
        other._size = tmpsize;
        other._head = tmphead;
    }

    //! Get an iterator starting at the beginning of the list.
    ZListIterator begin(){
        return ZListIterator(this, _head);
    }
    //! Get an iterator starting at the end of the list.
    ZListIterator end(){
        return ZListIterator(this, (_head ? _head->prev : nullptr));
    }

    //! Get a const iterator starting at the beginning of the list.
    ZListConstIterator cbegin() const {
        return ZListConstIterator(this, _head);
    }
    //! Get a const iterator starting at the end of the list.
    ZListConstIterator cend() const {
        return ZListConstIterator(this, (_head ? _head->prev : nullptr));
    }

//    ZString debug() const {
//        ZString str;
//        Node *current = _head;
//        for(zu64 i = 0; i < _size; ++i){
//            str += ZString::ItoS((zu64)current->prev, 16) + " ";
//            str += ZString::ItoS((zu64)current, 16);
//            str += " " + ZString::ItoS((zu64)current->next, 16);
//            str += ", ";
//            current = current->next;
//        }
//        return str;
//    }

    inline T &operator[](zu64 index){ return getNode(index)->data; }
    inline const T &operator[](zu64 index) const { return getNode(index)->data; }

    inline T &front(){
        if(_head == nullptr)
            throw zexception("ZList: Cannot reference head of empty ZList");
        return _head->data;
    }
    inline const T &front() const {
        if(_head == nullptr)
            throw zexception("ZList: Cannot reference head of empty ZList");
        return _head->data;
    }
    inline T &back(){
        if(_head == nullptr)
            throw zexception("ZList: Cannot reference back of empty ZList");
        return _head->prev->data;
    }
    inline const T &back() const {
        if(_head == nullptr)
            throw zexception("ZList: Cannot reference back of empty ZList");
        return _head->prev->data;
    }

    inline bool isEmpty() const { return (_size == 0); }
    inline zu64 size() const { return _size; }

private:
    Node *newNode(const T &data){
       Node *node = _alloc->alloc();
       node->prev = nullptr;
       node->next = nullptr;
       _talloc.construct(&(node->data), data);
       return node;
    }

    Node *getNode(zu64 index) const {
        Node *current = _head;
        for(zu64 i = 0; i < index; ++i){
            current = current->next;
        }
        return current;
    }

public:
    class ZListConstIterator : public ZDuplexConstIterator<T> {
    public:
        ZListConstIterator(const ZList<T> *list, const typename ZList<T>::Node *start_node) : _list(list), _node(start_node), _prev(nullptr){}

        const T &get() const override {
            if(!_node)
                throw zexception("ZListConstIterator: Cannot reference iterator of empty list");
            return _node->data;
        }

        bool more() const override {
            return (_node && (_prev == nullptr || _node != _list->_head));
        }
        void advance() override {
            if(_node){
                _prev = _node;
                _node = _node->next;
            }
        }

        bool less() const override {
            return (_node && (_prev == nullptr || _node != _list->_head->prev));
        }
        void recede() override {
            if(_node){
                _prev = _node;
                _node = _node->prev;
            }
        }

        //bool compare(ZListIterator it) const {
        //    return (_list == it._list && _node == it._node);
        //}
        //ZITERATOR_COMPARE_OVERLOADS(ZListIterator)

    private:
        const ZList<T> *_list;
        const typename ZList<T>::Node *_node;
        const typename ZList<T>::Node *_prev;
    };

    class ZListIterator : public ZDuplexIterator<T> {
    public:
        ZListIterator(ZList<T> *list, typename ZList<T>::Node *start_node) : _list(list), _node(start_node), _prev(nullptr){}

        T &get() override {
            if(!_node)
                throw zexception("ZListIterator: Cannot reference iterator of empty list");
            return _node->data;
        }
        const T &get() const override {
            if(!_node)
                throw zexception("ZListIterator: Cannot reference iterator of empty list");
            return _node->data;
        }

        bool more() const override {
            return (_node && (_prev == nullptr || _node != _list->_head));
//            return (_node != nullptr);
        }
        void advance() override {
            if(_node){
                _prev = _node;
                _node = _node->next;
            }
        }

        bool less() const override {
            return (_node && (_prev == nullptr || _node != _list->_head->prev));
        }
        void recede() override {
            if(_node){
                _prev = _node;
                _node = _node->prev;
            }
        }

        //bool compare(ZListIterator it) const {
        //    return (_list == it._list && _node == it._node);
        //}
        //ZITERATOR_COMPARE_OVERLOADS(ZListIterator)

    private:
        ZList<T> *_list;
        typename ZList<T>::Node *_node;
        typename ZList<T>::Node *_prev;
    };

private:
    //! Node memory allocator.
    ZAllocator<Node> *_alloc;
    //! Allocator only for constructing T inside nodes.
    ZAllocator<T> _talloc;
    //! Number of nodes in list.
    zu64 _size;
    //! Pointer to first node in list.
    Node *_head;
};

}

#endif // ZLIST_H
