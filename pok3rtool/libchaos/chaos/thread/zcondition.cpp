/*******************************************************************************
**                                  LibChaos                                  **
**                               zcondition.cpp                               **
**                          (c) 2015 Charlie Waters                           **
*******************************************************************************/
#include "zcondition.h"
#include "zexception.h"

#ifdef ZMUTEX_WINTHREADS
    #include <windows.h>
#else
    #include <pthread.h>
#endif

namespace LibChaos {

#ifdef ZMUTEX_WINTHREADS
ZCondition::ZCondition() : mutex(new CRITICAL_SECTION), cond(new CONDITION_VARIABLE){
    InitializeCriticalSection(mutex);
    InitializeConditionVariable(cond);
}
#else
ZCondition::ZCondition(){
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);
}
#endif

ZCondition::~ZCondition(){
#ifdef ZMUTEX_WINTHREADS
    DeleteCriticalSection(mutex);
#else
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);
#endif
}

void ZCondition::waitOnce(){
#ifdef ZMUTEX_WINTHREADS
    EnterCriticalSection(mutex);
    SleepConditionVariableCS(cond, mutex, INFINITE);
    LeaveCriticalSection(mutex);
#else
    pthread_mutex_lock(&mutex);
    pthread_cond_wait(&cond, &mutex);
    pthread_mutex_unlock(&mutex);
#endif
}

void ZCondition::lock(){
#ifdef ZMUTEX_WINTHREADS
    EnterCriticalSection(mutex);
#else
    pthread_mutex_lock(&mutex);
#endif
}

void ZCondition::wait(){
#ifdef ZMUTEX_WINTHREADS
    SleepConditionVariableCS(cond, mutex, INFINITE);
#else
    pthread_cond_wait(&cond, &mutex);
#endif
}

void ZCondition::unlock(){
#ifdef ZMUTEX_WINTHREADS
    LeaveCriticalSection(mutex);
#else
    pthread_mutex_unlock(&mutex);
#endif
}

void ZCondition::signal(){
#ifdef ZMUTEX_WINTHREADS
    // FIXME: Causes crash at end of program
//    EnterCriticalSection(mutex);
    WakeConditionVariable(cond);
//    LeaveCriticalSection(mutex);
#else
//    pthread_mutex_lock(&mutex);
    pthread_cond_signal(&cond);
//    pthread_mutex_unlock(&mutex);
#endif
}

void ZCondition::broadcast(){
#ifdef ZMUTEX_WINTHREADS
//    EnterCriticalSection(mutex);
    WakeAllConditionVariable(cond);
//    LeaveCriticalSection(mutex);
#else
//    pthread_mutex_lock(&mutex);
    pthread_cond_broadcast(&cond);
//    pthread_mutex_unlock(&mutex);
#endif
}



}
