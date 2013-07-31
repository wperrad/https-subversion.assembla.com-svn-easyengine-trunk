#ifndef _SINGLETON_H_
#define _SINGLETON_H_ 

//#include <windows.h>

template<class T> class CSingleton
{
protected:
	static T* s_pData;
	CSingleton(){}
public:
	
	static T* GetInstance()
	{
		if (!s_pData)
			s_pData = new T;
		return s_pData;			
	}
};


template<class T> T* CSingleton<T>::s_pData = NULL;

#endif