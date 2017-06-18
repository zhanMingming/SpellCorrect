#ifndef INCLUDE_CONDITION_H_
#define INCLUDE_CONDITION_H_

#include <pthread.h>
#include "MutexLock.h"
#include "NoneCopyable.h"

class Condition : public NoneCopyable{
public:

	Condition(MutexLock *p_lock);
	~Condition();

	void wait();
	void notify();
	void notify_all(); //谨慎使用

private:
	pthread_cond_t _cond;
	MutexLock *_p_lock;  //这里的lock要用指针

};

#endif /* CONDITION_H_ */
