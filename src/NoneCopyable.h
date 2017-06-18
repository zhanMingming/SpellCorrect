#ifndef NONCOPYABLE_H_
#define NONCOPYABLE_H_

class NoneCopyable
{
protected:
	NoneCopyable(){}
	virtual ~NoneCopyable(){}
private:
	NoneCopyable(const NoneCopyable &){}
    NoneCopyable &operator=(const NoneCopyable &){
		return *this;
	}
};

#endif