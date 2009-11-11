#ifndef _LSD_LOCKED_QUEUE_H_
#define _LSD_LOCKED_QUEUE_H_

#include <queue>
#include <Core/Mutex.h>

using OpenEngine::Core::Mutex;

template <class T> class LockedQueue {


    
    std::queue<T> _queue;
    Mutex m;
public:
    LockedQueue() {
        
    }

    void Put(T e) {
        m.Lock();
        _queue.push(e);
        m.Unlock();
    }

    void Get(T e) {
        T r;
        m.Lock();
        r = _queue.pop();
        m.Unlock();
        return r;
    }
    
};

#endif
