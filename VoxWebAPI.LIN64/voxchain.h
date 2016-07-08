//
// Created by root on 15.06.16.
//

#ifndef VOXWEBSERVER_LIN64_VOXCHAIN_H
#define VOXWEBSERVER_LIN64_VOXCHAIN_H

#include <zconf.h>
#include <pthread.h>

#define CHITEM typename voxchain<T>::chain_item

template<typename T>
class voxchain
{
public:
	struct chain_item
	{
		T *Data;
		//char* DataState;
		CHITEM *Previous;
		CHITEM *Next;

		~chain_item()
		{
			if (Data)
				delete[] Data;
		}
	};

	voxchain(int container_sz);
	//Disable copy, use only reference
	//voxchain(voxchain<T>& cop) = delete;
	CHITEM *GetFirst();

	CHITEM *GetLast();

	int GetContainerSize();

	int GetSize();

	int GetContainersNum();

	T &operator[](int index);

	void Destroy(); // for can copy
	//Thread safety???
	//void RemoveContainer(int index);
	int Add(T data);

private:
	pthread_mutex_t cs;
	CHITEM *First = 0;
	CHITEM *CurContainer = 0;
	int cont_sz = 0;
	int cont_num = 0;
	int size = 0;

};

template<typename T>
inline voxchain<T>::voxchain(int container_sz)
{
	pthread_mutex_init(&cs, NULL);
	cont_sz = container_sz;
}

template<typename T>
inline CHITEM *voxchain<T>::GetFirst()
{
	return First;
}

template<typename T>
inline CHITEM *voxchain<T>::GetLast()
{
	return CurContainer;
}

template<typename T>
inline int voxchain<T>::GetContainerSize()
{
	return cont_sz;
}

template<typename T>
inline int voxchain<T>::GetSize()
{
	return size;
}

template<typename T>
inline int voxchain<T>::GetContainersNum()
{
	return cont_num;
}

template<typename T>
inline T &voxchain<T>::operator[](int index)
{
	if (index >= size)
		return T();

	int cnt = index / cont_sz;
	CHITEM *fs = First;
	while (cnt)
	{
		fs = fs->Next;
		cnt--;
	}
	return fs->Data[index % cont_sz];
}

template<typename T>
inline void voxchain<T>::Destroy()
{
	CHITEM *it = First;
	while (it)
	{
		it = it->Next;
		delete it;
	}
}

/*
template<typename T>
inline void voxchain<T>::RemoveContainer(int index)
{
	if (index < cont_num)
	{
		CHITEM* fs = First;
		while (index)
		{
			fs = fs->Next;
			index--;
		}

		if (fs->Previous)
			fs->Previous->Next = fs->Next;

		delete fs;
		cont_num--;
	}
}
*/
template<typename T>
inline int voxchain<T>::Add(T data)
{
	pthread_mutex_lock(&cs);
	if (CurContainer)
	{
		if (size % cont_sz == 0)
		{
			CHITEM *old = CurContainer;
			CurContainer = new CHITEM();
			CurContainer->Data = new T[cont_sz]();
			//CurContainer->DataState = new char[cont_sz];
			old->Next = CurContainer;
			CurContainer->Previous = old;
			cont_num++;
		}

		CurContainer->Data[size++ % cont_sz] = data;
	} else
	{
		CurContainer = new CHITEM();
		CurContainer->Data = new T[cont_sz]();
		CurContainer->Data[size++] = data;

		First = CurContainer;
		cont_num++;
	}

	pthread_mutex_unlock(&cs);
	return size;
}

#endif //VOXWEBSERVER_LIN64_VOXCHAIN_H
