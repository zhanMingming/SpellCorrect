
#ifndef MUTEXLOCK_H_
#define MUTEXLOCK_H_


#include <pthread.h>
#include <stdexcept>
#include "NoneCopyable.h"

class Condition;

class MutexLock : public NoneCopyable{
	friend class Condition;
public:
	
	MutexLock();
	~MutexLock();

	void lock();
	void unlock();

private:
	pthread_mutex_t _mutex;
};

#endif /* MUTEXLOCK_H_ */
