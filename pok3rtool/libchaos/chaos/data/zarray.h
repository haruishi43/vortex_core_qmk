/*******************************************************************************
**                                  LibChaos                                  **
**                                  zarray.h                                  **
**                          (c) 2015 Charlie Waters                           **
*******************************************************************************/
#ifndef ZARRAY_H
#define ZARRAY_H

#include "ztypes.h"
#include "zallocator.h"
#include "yindexedaccess.h"
#include "ypushpopaccess.h"
#include "ziterator.h"
//#include "zhash.h"

#include <initializer_list>

#define ZARRAY_INITIAL_CAPACITY 16

namespace LibChaos {

/*! Dynamic contiguous array sequence container.
 *  ZArray push/pop paradigm is FILO
 */
template <typename T> class ZArray : public YIndexedAccess<T>, public YPushPopAccess<T> {
public:
    enum { NONE = ZU64_MAX };

    class ZArrayIterator;
    class ZArrayConstIterator;

public:
    //! ZArray default constructor, optional user allocator.
    ZArray(ZAllocator<T> *alloc = new ZAllocator<T>) : _alloc(alloc), _data(nullptr), _size(0), _realsize(0){
        reserve(ZARRAY_INITIAL_CAPACITY);
    }

    //! ZArray initializer list constructor.
    ZArray(std::initializer_list<T> ls) : ZArray(){
        reserve(ls.size());
        zu64 i = 0;
        for(auto item = ls.begin(); item < ls.end(); ++item){
            _alloc->construct(&_data[i], *item);
            ++i;
        }
        _size = ls.size();
    }

    //! ZArray size constructor.
    ZArray(zu64 size) : ZArray(){
        resize(size);
    }

    //! ZArray buffer constructor.
    ZArray(const T *raw, zu64 size) : ZArray(){
        reserve(size);
        for(zu64 i = 0; i < size; ++i)
            _alloc->construct(&_data[i], raw[i]);
        _size = size;
    }

    //! ZArray copy constructor.
    ZArray(const ZArray<T> &other) : ZArray(){
        reserve(other._size);
        for(zu64 i = 0; i < other._size; ++i)
            _alloc->construct(&_data[i], other[i]);
        _size = other._size;
    }

    //! ZArray destructor.
    ~ZArray(){
        clear();
        delete _alloc;
    }

    //! Destroy all elements in array.
    void clear(){
        _alloc->destroy(_data, _size); // Destroy objects
        _size = 0;
        _alloc->dealloc(_data); // Delete memory
        _data = nullptr;
    }

    //
    // Array Assignment
    //

    ZArray<T> &assign(const ZArray<T> &other){
        _alloc->destroy(_data, _size); // Destroy contents
        reserve(other.size()); // Make space
        for(zu64 i = 0; i < other.size(); ++i)
            _alloc->construct(&_data[i], other[i]); // Copy objects
        _size = other.size();
        return *this;
    }
    inline ZArray<T> &operator=(const ZArray<T> &other){ return assign(other); }

    void swap(ZArray<T>& other){
        ZAllocator<T> *alloc = _alloc;
        zu64 size =_size;
        zu64 realsize = _realsize;
        T *data = _data;

        _alloc = alloc;
        _size = other._size;
        _realsize = other._realsize;
        _data = other._data;

        other._alloc = alloc;
        other._size = size;
        other._realsize = realsize;
        other._data = data;
    }

    //
    // Array Comparison
    //

    bool equals(const ZArray<T> &other) const {
        if(size() != other.size())
            return false;
        for(zu64 i = 0; i < size(); ++i){
            if(!(operator[](i) == other[i]))
                return false;
        }
        return true;
    }

    inline bool operator==(const ZArray<T> &other) const { return equals(other); }
    inline bool operator!=(const ZArray<T> &other) const { return !equals(other); }

    //
    // Element Insertion
    //

    //! Add \a value to the back (end) of the array.
    ZArray<T> &pushBack(const T &value){
        reserve(_size + 1);
        _alloc->construct(_data + _size, value);
        ++_size;
        return *this;
    }

    //! Add \a value to the front (begin) of the array.
    ZArray<T> &pushFront(const T &value){
        return insert(0, value);
    }

    inline void push(const T &value){ pushBack(value); }

    //! Insert \a value at \a pos, shifting subsequent elements.
    ZArray<T> &insert(zu64 pos, const T &value){
        reserve(_size + 1);
        _alloc->rawmove(_data + pos, _data + pos + 1, _size - pos);
        _alloc->construct(_data + pos, value);
        ++_size;
        return *this;
    }

    //! Append another array to the end of this array.
    ZArray<T> &append(const ZArray<T> &in){
        reserve(_size + in.size());
        for(zu64 i = 0; i < in.size(); ++i)
            _alloc->construct(_data + _size + i, in[i]);
        _size += in.size();
        return *this;
    }

    //! Concatenate two arrays in a new array.
    ZArray<T> concat(const ZArray<T> &in) const {
        ZArray<T> tmp(*this);
        tmp.append(in);
        return tmp;
    }

    //
    // Element Removal
    //

    ZArray<T> &erase(zu64 index, zu64 count = 1){
        _alloc->destroy(_data + index, count);
        _alloc->rawmove(_data + index + count, _data + index, _size - index - count);
        _size -= count;
        return *this;
    }

    ZArray<T> &remove(zu64 index){
        return erase(index);
    }
    ZArray<T> &popFront(){
        return remove(0);
    }
    ZArray<T> &popFrontCount(zu64 conut){
        return erase(0, conut);
    }
    ZArray<T> &popBack(){
        return resize(_size - 1);
    }
    inline void pop(){ popBack(); }

    //
    // Element Access
    //

    inline T &at(zu64 index){
#if LIBCHAOS_BUILD != LIBCHAOS_RELEASE
        if(index >= size())
            throw zexception("ZArray: Index out of range");
#endif
        return _data[index];
    }
    inline T &operator[](zu64 index){ return at(index); }

    inline const T &at(zu64 index) const {
#if LIBCHAOS_BUILD != LIBCHAOS_RELEASE
        if(index >= size())
            throw zexception("ZArray: Index out of range");
#endif
        return _data[index];
    }
    inline const T &operator[](zu64 index) const { return at(index); }

    inline T &front(){ return at(0); }
    inline const T &front() const { return at(0); }
    inline T &back(){ return at(_size - 1); }
    inline const T &back() const { return at(_size - 1); }

    T &peek(){
        return back();
    }
    const T &peek() const {
        return back();
    }

    bool contains(const T &test) const {
        for(zu64 i = 0; i < _size; ++i){
            if(_data[i] == test)
                return true;
        }
        return false;
    }
    zu64 find(const T &test) const {
        for(zu64 i = 0; i < _size; ++i){
            if(_data[i] == test)
                return i;
        }
        return NONE;
    }

    //
    // Array Operations
    //

    ZArray<T> &reverse(){
        ZArray<T> tmp;
        tmp.reserve(_size);
        for(zu64 i = _size; i > 0; --i)
            tmp.pushBack(operator[](i-1));
        assign(tmp);
        return *this;
    }

    //
    // Size Management
    //

    //! Change logical number of elements inside real buffer.
    ZArray<T> &resize(zu64 size, const T &value = T()){
        reserve(size);
        if(size > _size){
            _alloc->construct(_data + _size, value, size - _size); // Construct new objects
        } else if(size < _size){
            _alloc->destroy(_data + size, _size - size); // Destroy extra objects
        }
        _size = size;
        return *this;
    }

    /*! Resize the real buffer.
     *  IMPORTANT: this is the only place memory is allocated.
     *  If new size is larger, adds uninitialized space for more elements (subsequent resizes may not have to reallocate).
     *  Never reallocates a smaller buffer! resize() assumes the new buffer is never smaller so it can destroy objects.
     */
    void reserve(zu64 size){
        if(size > _realsize){
            zu64 newsize = MAX(_size * 2, size);
            T *data = _alloc->alloc(newsize);
            _alloc->rawcopy(_data, data, _size); // Copy data to new buffer
            _alloc->dealloc(_data); // Delete old buffer without calling destructors
            _data = data;
            _realsize = newsize;
        }
    }

    //! Add \a count objects to array.
    ZArray<T> &extend(zu64 count){
        return resize(_size + count);
    }
    //! Remove \a count objects from array.
    ZArray<T> &contract(zu64 count){
        return resize(_size - count);
    }

    //! Get an iterator starting at the beginning of the array.
    ZArrayIterator begin(){
        return ZArrayIterator(this, 0);
    }
    //! Get an iterator starting at the end of the array.
    ZArrayIterator end(){
        return ZArrayIterator(this, (size() ? size()-1 : 0));
    }

    //! Get a const iterator starting at the beginning of the array.
    ZArrayConstIterator cbegin() const {
        return ZArrayConstIterator(this, 0);
    }
    //! Get a const iterator starting at the end of the array.
    ZArrayConstIterator cend() const {
        return ZArrayConstIterator(this, (size() ? size()-1 : 0));
    }

    inline bool isEmpty() const { return (_size == 0); }
    inline bool empty() const { return isEmpty(); }

    inline zu64 size() const { return _size; }
    inline zu64 realsize() const { return _realsize; }
    inline zu64 capacity() const { return realsize(); }

    inline T *ptr() const { return _data; }
    inline T *raw() const { return ptr(); }

public:
    class ZArrayConstIterator : public ZRandomConstIterator<T> {
    public:
        ZArrayConstIterator(const ZArray<T> *array, zu64 index) : _array(array), _index(index){}

        const T &get() const override {
            return _array->at(_index);
        }

        bool more() const override {
            return (_index < _array->size());
        }
        void advance() override {
            ++_index;
        }

        bool less() const override {
            return (_index >= 0 && _index < _array->size());
        }
        void recede() override {
            --_index;
        }

        zu64 size() const override {
            return _array->size();
        }

        const T &at(zu64 i) const override {
            return _array->at(i);
        }

        //bool compare(ZListIterator it) const {
        //    return (_list == it._list && _node == it._node);
        //}
        //ZITERATOR_COMPARE_OVERLOADS(ZListIterator)

    private:
        const ZArray<T> *_array;
        zu64 _index;
    };

    class ZArrayIterator : public ZRandomIterator<T> {
    public:
        ZArrayIterator(ZArray<T> *array, zu64 index) : _array(array), _index(index){}

        T &get() override {
            return _array->at(_index);
        }
        const T &get() const override {
            return _array->at(_index);
        }

        bool more() const override {
            return (_index < _array->size());
        }
        void advance() override {
            ++_index;
        }

        bool less() const override {
            return (_index >= 0 && _index < _array->size());
        }
        void recede() override {
            --_index;
        }

        zu64 size() const override {
            return _array->size();
        }

        T &at(zu64 i) override {
            return _array->at(i);
        }
        const T &at(zu64 i) const override {
            return _array->at(i);
        }

        //bool compare(ZListIterator it) const {
        //    return (_list == it._list && _node == it._node);
        //}
        //ZITERATOR_COMPARE_OVERLOADS(ZListIterator)

    private:
        ZArray<T> *_array;
        zu64 _index;
    };

private:
    //! Buffer memory allocator.
    ZAllocator<T> *_alloc;
    T *_data;
    zu64 _size;
    zu64 _realsize;
};

//ZHASH_USER_SPECIALIAZATION(ZArray<T>, (ZArray<T> array), (), {})

//template <typename T, ZHashBase::hashMethod M> class ZHash<ZArray<T>, M> : public ZHashMethod<M> {
//public:
//    ZHash(const ZArray<T> &array) : ZHashMethod<M>(){
//        for(zu64 i = 0; i < array.size(); ++i){
//            ZHash<T, M>(array[i])
//        }
//    }
//};
//template <> class ZHash<TYPE, ZHashBase::defaultHash> : public ZHashMethod<ZHashBase::defaultHash> {
//public:
//    ZHash(const ZArray<T> &array) : ZHashMethod<ZHashBase::defaultHash>(){
//
//    }
//};

}

#endif // ZARRAY_H
