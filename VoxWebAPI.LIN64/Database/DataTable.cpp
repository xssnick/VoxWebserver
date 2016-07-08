//
// Created by kek on 07.07.16.
//

#include "DataTable.h"

DataTable::DataTable(string file)
{
	FILE *hconf = fopen(("./database/" + file + ".voxtconf").c_str(), "rb");
	if (!hconf)
	{
		printf(("./database/" + file + ".voxtconf" + " file not found!").c_str());
		return;
	}

	FILE *hFind = fopen(("./database/" + file + ".voxdbt").c_str(), "ab+");
	if (hFind)
	{
		//HFile = hFind;
		pthread_mutex_init(&cs, NULL);
		fseek(hFind, 0, SEEK_END);
		FileSz = (size_t) ftell(hFind);
		rewind(hFind);

		First = new TPart();
		if (FileSz > 0)
		{
			First->data_sz = FileSz;
			First->data = new char[FileSz];
			fread(First->data, 1, FileSz, hFind);
		}

		pthread_t thread;
		int err = 0;//pthread_create(&thread, NULL, &FileWorker, this);
		if (err != 0)
			printf("\ncan't create thread :[%s]", strerror(err));
	} else
		printf(("./database/" + file + ".voxdb" + " cant access!").c_str());
}

void *DataTable::FileWorker(void *ccr)
{
	DataTable *cur = (DataTable *) ccr;
	while (true)
	{
		if (cur->FileQueue.empty())
		{
			sleep(1);
			continue;
		}
		pthread_mutex_lock(&cur->cs);
		QItem it = cur->FileQueue.front();
		cur->FileQueue.pop();
		pthread_mutex_unlock(&cur->cs);

		fseek(cur->HFile, it.offset, SEEK_SET);
		fwrite((void *) ((unsigned long long) it.data), it.data_sz, 1, cur->HFile);
		fflush(cur->HFile);

		delete[] it.data;

	}
}