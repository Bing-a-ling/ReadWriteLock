# ReadWriteLock
a simple RAII write-first read-write lock. (c++11)

Usage：
```c++
#include "rw_lock.hpp"

ReadWriteLock mylock;

void yourReadFunc(){
// define readlock.
    ReadLock lock(mylock);
}

void yourWriteFunc(){
// define writelock.
    WriteLock lock(mylock);
}
```
