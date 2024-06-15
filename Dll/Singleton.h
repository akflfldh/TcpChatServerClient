#pragma once

#include"Default.h"


#include<assert.h>




template<typename T>
class  Singleton
{
public:
	Singleton();
	~Singleton();

	static T* GetInstance();

private:
	static T* mInstance;

};

template<typename T>
T* Singleton<T>::mInstance = nullptr;


template<typename T>
inline Singleton<T>::Singleton()
{
	assert(!mInstance);
	mInstance =(T *)this;

}

template<typename T>
inline Singleton<T>::~Singleton()
{
	mInstance = nullptr;

}

template<typename T>
inline T* Singleton<T>::GetInstance()
{
	assert(mInstance);


	return mInstance;
}
