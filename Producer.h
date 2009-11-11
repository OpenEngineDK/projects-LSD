#ifndef _LSD_PRODUCER_H_
#define _LSD_PRODUCER_H_

#include "LockedQueue.h"
#include <Core/Thread.h>

#include <Logging/Logger.h>

using namespace OpenEngine::Core;

class Producer : public Thread {
    LockedQueue<float> *queue;

public:
    bool run;
    Producer(LockedQueue<float> *queue) : queue(queue),run(true) {
        
    }

    void Run() {
        while (run) {
            
            queue->Put(42.0);

            Thread::Sleep(1000000);
        }
    }

};

#endif
