//
// Created by root on 18.06.16.
//

#ifndef VOXWEBSERVER_LIN64_DataFileStorage_H
#define VOXWEBSERVER_LIN64_DataFileStorage_H

#include "VoxWebAPI.h"

/////
#define DBLOCATION(path) DataFileStorage<DBTABLE_CLASS::DBTABLE_STORE> DBTABLE_CLASS::DataTable = DataFileStorage<DBTABLE_CLASS::DBTABLE_STORE>(path);\
voxchain<DBTABLE_CLASS*> DBTABLE_CLASS::DataTableChain = voxchain<DBTABLE_CLASS*>(25);\

#define DBINSERT ID=DataTable.Insert(GenerateTableItem(),true);
#define DBUPDATE DataTable.Update(GenerateTableItem(),ID,true);

#define DBSTOREVAR lc_ins_tp
#define DB_TO_FROMSTR(valdb, val, maxlen) memcpy(DBSTOREVAR->valdb,val.c_str(),min((long)val.size(),(long)maxlen-1));
#define DB_TO_DIRECT(t, f) DBSTOREVAR->t = f;
#define DB_TO_IDFROMTYPE(t, f) DBSTOREVAR->t = f->GetID();

#define DB_FROM_TYPEFROMID(t, f, typ) t = ((typ*)0)->GetByID(DBSTOREVAR->f);
#define DB_FROM_DIRECT(t, f) t = DBSTOREVAR->f;

#define DBINITSTART \
public:\
inline DBTABLE_CLASS* GetByID(size_t id)\
{\
    voxchain<DBTABLE_CLASS*>::chain_item* item = DataTableChain.GetFirst();\
    int SZ = DataTableChain.GetSize();\
    int contSZ = DataTableChain.GetContainerSize();\
    while (item)\
    {\
        for (int i = 0; i < min(contSZ, SZ); i++)\
        {\
            if (item->Data[i] && item->Data[i]->GetID() == id)\
            {\
                return item->Data[i];\
            }\
        }\
        SZ -= contSZ;\
        item = item->Next;\
    }\
\
    DBTABLE_STORE tu;\
    if(DataTable.Load(&tu,id))\
    {\
       DBTABLE_CLASS* cls_ccs = new DBTABLE_CLASS(id,&tu);\
       DataTableChain.Add(cls_ccs);\
       return cls_ccs;\
    }\
    return 0;\
}\
private:\
DBTABLE_CLASS(size_t id,DBTABLE_STORE* DBSTOREVAR)\
{\
    ID = id;\

#define DBINITEND }

#define DBFINDALL(vectr, where, fld, param) \

#define DBTABLE_CONVERTER_START private:\
static DataFileStorage<DBTABLE_STORE> DataTable;\
public:\
static voxchain<DBTABLE_CLASS*> DataTableChain;\
private:\
DBTABLE_STORE* GenerateTableItem() \
{\
    DBTABLE_STORE* lc_ins_tp = new DBTABLE_STORE();\


#define DBTABLE_CONVERTER_END \
        return lc_ins_tp; \
}\


////
#define DBGET(type, name) \
private:\
type name;\
public:\
type Get##name()\
{\
    return name;\
}\

#define DBGETSET(type, name) \
private:\
type name;\
public:\
type Get##name()\
{\
    return name;\
}\
\
void Set##name(type val)\
{\
    name = val;\
    DataTable.Update(GenerateTableItem(),ID,true);\
}\


class TableBase
{
public:
	size_t GetID()
	{
		return ID;
	}

protected:
	TableBase()
	{
		ID = 0;
	}

	size_t ID;
};


template<typename T>
class DataFileStorage
{
public:
	DataFileStorage(string filename);

	size_t Insert(T *object, bool destroy = false);

	void DelPointer(T *object);

	size_t Size();

	void Update(T *objecter, size_t id, bool destroy = false);

	bool Load(T *object, size_t id);

	vector<T> LoadAll();

private:
	struct DBItem
	{
		DBItem *Next = 0;
		DBItem *Previous = 0;
		size_t FileAddr;
		void *Data;
		bool InDB = false;
		bool once = false;
		bool Destroy = false;
	};

	queue<DBItem *> FileQueue;
	pthread_mutex_t cs;
	FILE *HFile;
	size_t FileSz = 0;
	string File;
	bool Loaded = false;

	void Init();

	static void *FileWorker(void *ccr);
};

template<typename T>
inline DataFileStorage<T>::DataFileStorage(string filename)
{
	File = filename;
	Init();
}

template<typename T>
inline size_t DataFileStorage<T>::Insert(T *objecter, bool destroy)
{
	DBItem *im = new DBItem();
	im->Data = objecter;
	im->Destroy = destroy;
	im->once = destroy;
	im->InDB = true;
	im->FileAddr = FileSz;
	size_t ret = FileSz / sizeof(T);
	FileSz += sizeof(T);
	pthread_mutex_lock(&cs);
	FileQueue.push(im);
	pthread_mutex_unlock(&cs);
	return ret;
}

template<typename T>
inline void DataFileStorage<T>::DelPointer(T *object)
{
	DBItem *im = new DBItem();
	im->Data = object;
	im->once = true;
	im->Destroy = true;
	im->InDB = false;
	pthread_mutex_lock(&cs);
	FileQueue.push(im);
	pthread_mutex_unlock(&cs);
}


template<typename T>
void DataFileStorage<T>::Update(T *objecter, size_t id, bool destroy)
{
	DBItem *im = new DBItem();
	im->Destroy = destroy;
	im->Data = objecter;
	im->FileAddr = sizeof(T) * id;
	im->once = true;
	im->InDB = true;
	pthread_mutex_lock(&cs);
	FileQueue.push(im);
	pthread_mutex_unlock(&cs);
}


template<typename T>
inline size_t DataFileStorage<T>::Size()
{
	return FileSz / sizeof(T);
}

template<typename T>
inline void DataFileStorage<T>::Init()
{
	FILE *hFind = fopen(("./database/" + File + ".voxdb").c_str(), "ab+");
	if (hFind)
	{
		HFile = hFind;
		pthread_mutex_init(&cs, NULL);
		fseek(hFind, 0, SEEK_END);
		FileSz = (size_t) ftell(hFind);
		rewind(hFind);
		pthread_t thread;
		int err = pthread_create(&thread, NULL, &FileWorker, this);
		if (err != 0)
			printf("\ncan't create thread :[%s]", strerror(err));
		else
		{
			Loaded = true;
		}
	} else
		printf(("./database/" + File + ".voxdb" + " cant access!").c_str());
}

template<typename T>
inline void *DataFileStorage<T>::FileWorker(void *ccr)
{
	DataFileStorage<T> *cur = (DataFileStorage<T> *) ccr;
	while (true)
	{
		if (cur->FileQueue.empty())
		{
			sleep(1);
			continue;
		}
		pthread_mutex_lock(&cur->cs);
		DBItem *it = cur->FileQueue.front();
		cur->FileQueue.pop();
		pthread_mutex_unlock(&cur->cs);

		if (it->InDB)
		{
			fseek(cur->HFile, it->FileAddr, SEEK_SET);
			fwrite((void *) ((unsigned long long) it->Data), sizeof(T), 1, cur->HFile);
			fflush(cur->HFile);
		}

		if (it->Destroy)
			delete (T *) it->Data;

		if (it->once)
			delete it;

	}
}


template<typename T>
inline bool DataFileStorage<T>::Load(T *object, size_t id)
{
	size_t off = id * sizeof(T);
	bool ok = false;
	pthread_mutex_lock(&cs);
	fseek(HFile, off, SEEK_SET);
	if (fread((void *) ((unsigned long long) object), sizeof(T), 1, HFile) <= 0)
	{
		printf("ERR read id(%d) from DFS %d | %s\n", id, errno, strerror(errno));
	} else
		ok = true;
	pthread_mutex_unlock(&cs);
	return ok;
}

template<typename T>
inline vector<T> DataFileStorage<T>::LoadAll()
{
	size_t num = Size();
	vector<T> res = vector<T>(Size());
	pthread_mutex_lock(&cs);
	for (size_t id = 0; id < num; id++)
	{
		size_t off = id * sizeof(T);
		fseek(HFile, off, SEEK_SET);
		if (fread((void *) ((unsigned long long) &res[id]), sizeof(T), 1, HFile) <= 0)
		{
			printf("ERR readall id(%d) from DFS %d | %s\n", id, errno, strerror(errno));
		}
	}
	pthread_mutex_unlock(&cs);
	return res;
}

#endif //VOXWEBSERVER_LIN64_DataFileStorage_H
