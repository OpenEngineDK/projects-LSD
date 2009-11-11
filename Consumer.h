#ifndef _LSD_CONSUMER_H_
#define _LSD_CONSUMER_H_

#include "LockedQueue.h"
#include <Core/Thread.h>

#include <Logging/Logger.h>

using namespace OpenEngine::Core;

class Consumer : public Thread {
    LockedQueue<float> *queue;

public:
    bool run;
    Consumer(LockedQueue<float> *queue) : queue(queue),run(true) {
        
    }

    void Run() {
        while (run) {
            while(queue->IsEmpty() && run)
                Thread::Sleep(1000);
            if (!run) return;

            float e = queue->Get();

            logger.info << e << logger.end;

            
        }
    }

};

#endif
