/*******************************************************************************
**                                  LibChaos                                  **
**                                zcondition.h                                **
**                          (c) 2015 Charlie Waters                           **
*******************************************************************************/
#ifndef ZCONDITION_H
#define ZCONDITION_H

#include "ztypes.h"
#include "zmutex.h"

#ifdef ZMUTEX_WINTHREADS
    //struct _RTL_CONDITION_VARIABLE;
    //typedef _RTL_CONDITION_VARIABLE CONDITION_VARIABLE;
    #include <windows.h>
#else
    #include <pthread.h>
#endif

namespace LibChaos {

/*! Thread notification controller.
 *  \ingroup Thread
 */
class ZCondition {
public:
    ZCondition();
    ~ZCondition();

    ZCondition(const ZCondition &) = delete;
    ZCondition &operator=(const ZCondition &) = delete;

    void waitOnce();

    void lock();
    void wait();
    void unlock();

    void signal();

    void broadcast();

#ifdef ZMUTEX_WINTHREADS
    inline CONDITION_VARIABLE *getHandle(){
        return cond;
    }
    inline CRITICAL_SECTION *getMutex(){
        return mutex;
    }
#else
    inline pthread_cond_t *getHandle(){
        return &cond;
    }
    inline pthread_mutex_t *getMutex(){
        return &mutex;
    }
#endif

private:
#ifdef ZMUTEX_WINTHREADS
    CRITICAL_SECTION *mutex;
    CONDITION_VARIABLE *cond;
#else
    pthread_mutex_t mutex;
    pthread_cond_t cond;
#endif
};

} // namespace LibChaos

#endif // ZCONDITION_H
