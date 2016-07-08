//
// Created by kek on 07.07.16.
//

#ifndef VOXWEBSERVER_LIN64_DATATABLE_H
#define VOXWEBSERVER_LIN64_DATATABLE_H

#include "VoxWebAPI.h"

class DataTable
{
public:
	DataTable(string file);

private:
	pthread_mutex_t cs;
	FILE *HFile;
	size_t FileSz = 0;

	struct QItem
	{
		char *data;
		size_t offset;
		size_t data_sz;
	};

	struct TPart
	{
		char *data;
		size_t data_sz;
		TPart *Next;
	};
	TPart *First;
	queue<QItem> FileQueue;

	void *FileWorker(void *ccr);

};


#endif //VOXWEBSERVER_LIN64_DATATABLE_H
