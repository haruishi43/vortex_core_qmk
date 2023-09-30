#include "tests.h"
#include "zerror.h"

namespace LibChaosTest {

static int userwait = 0;

void intHandle(ZError::zerror_signal sig){
    if(sig == ZError::INTERRUPT)
        userwait = 1;
    else if(sig == ZError::TERMINATE)
        userwait = 2;
}

void error_signal(){
    ZError::registerSigSegv();
    ZError::registerInterruptHandler(intHandle);
    ZError::registerSignalHandler(ZError::TERMINATE, intHandle);

    LOG("Waiting for Interrupt or Terminate...");
    while(!userwait){
        // wait for ctrl+c
    }
    RLOG(ZLog::NEWLN);
    if(userwait == 1)
        LOG("Caught Interrupt");
    else if(userwait == 2)
        LOG("Caught Terminate");
}

void error_trace(){
    ZArray<ZError::TraceFrame> trace = ZError::getStackTrace();
    for(zu64 i = 0; i < trace.size(); ++i){
        LOG(ZError::traceFrameStr(trace[i]));
    }
}

void error_exception(){
    try {
        throw ZException("test exception", 1);
    } catch(ZException e){
        TASSERT(e.what() == "test exception");
        TASSERT(e.code() == 1);
        return;
    }
    TASSERT(false);
}

ZArray<Test> error_tests(){
    return {
        { "error-signal",       error_signal,       false, {} },
        // TODO: error tests have trouble on ubuntu/trusty?
        { "error-trace",        error_trace,        false, {} },
        { "error-execption",    error_exception,    false, {} },
    };
}

}
