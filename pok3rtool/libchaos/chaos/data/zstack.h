/*******************************************************************************
**                                  LibChaos                                  **
**                                  zstack.h                                  **
**                          (c) 2015 Charlie Waters                           **
*******************************************************************************/
#ifndef ZSTACK_H
#define ZSTACK_H

#include "ztypes.h"
#include "zarray.h"

#include "ypushpopaccess.h"

namespace LibChaos {

//! FILO stack ZArray wrapper.
template <typename T> class ZStack : public YPushPopAccess<T> {
public:
    ZStack() : _data(){}

    void push(const T &data){
        _data.pushBack(data);
    }

    T &peek(){
        return _data.back();
    }
    const T &peek() const {
        return _data.back();
    }

    void pop(){
        _data.popBack();
    }

    zu64 size() const {
        return _data.size();
    }

private:
    ZArray<T> _data;
};

}

#endif // ZSTACK_H
