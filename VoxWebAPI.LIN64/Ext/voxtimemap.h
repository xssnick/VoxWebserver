//
// Created by kek on 02.07.16.
//

#ifndef VOXWEBSERVER_LIN64_VOXTIMEMAP_H
#define VOXWEBSERVER_LIN64_VOXTIMEMAP_H

#include <map>
#include <time.h>

template<typename T, typename N>
class voxtimemap
{
	typedef std::pair<T, std::pair<time_t, N>> DPar;
	typedef std::map<T, std::pair<time_t, N>> DMap;
public:
	voxtimemap(time_t lifetime_sec, time_t checktime_sec = 15);

	voxtimemap(voxtimemap &ma);

	~voxtimemap();

	void Add(T key, N val);

	bool Get(T key, N &rs);

private:
	DMap Data;

	static void MapWorker(voxtimemap<T, N> *datan);

	bool enabled = true;
	time_t timepp;
	time_t checkt;
	pthread_mutex_t cs;
};

template<typename T, typename N>
inline voxtimemap<T, N>::voxtimemap(time_t lifetime_sec, time_t checktime_sec)
{
	pthread_mutex_init(&cs, NULL);
	timepp = lifetime_sec;
	checkt = checktime_sec;
	pthread_t trd;
	pthread_create(&trd, NULL, (void *(*)(void *)) MapWorker, &Data);
}

template<typename T, typename N>
inline voxtimemap<T, N>::voxtimemap(voxtimemap &ma)
{
	pthread_mutex_init(&cs, NULL);
	timepp = ma.timepp;
	checkt = ma.checkt;
	Data = ma.Data;
	enabled = ma.enabled;
	pthread_t trd;
	pthread_create(&trd, NULL, (void *(*)(void *)) MapWorker, &Data);
}

template<typename T, typename N>
inline voxtimemap<T, N>::~voxtimemap()
{
	enabled = false;
	pthread_mutex_destroy(&cs);
}

template<typename T, typename N>
inline void voxtimemap<T, N>::Add(T key, N val)
{
	pthread_mutex_lock(&cs);
	time_t tme = time(NULL) + timepp;
	std::pair<time_t, N> pr = std::pair<time_t, N>(tme, val);
	Data[key] = pr;
	pthread_mutex_unlock(&cs);
}

template<typename T, typename N>
inline bool voxtimemap<T, N>::Get(T key, N &rs)
{
	if (!Data.count(key))
		return false;
	Data[key].first = time(NULL) + timepp;
	rs = Data[key].second;
	return true;
}

template<typename T, typename N>
inline void voxtimemap<T, N>::MapWorker(voxtimemap<T, N> *datan)
{
	struct timespec ts;
	ts.tv_nsec = 0;
	while (datan->enabled)
	{
		ts.tv_sec = datan->timepp;

		nanosleep(&ts, NULL);

		pthread_mutex_lock(&datan->cs);
		for (auto it = datan->Data.begin(); it != datan->Data.end();)
		{
			if (it->second.first < time(0))
			{
				datan->Data.erase(it++);
			} else
			{
				++it;
			}
		}

		pthread_mutex_unlock(&datan->cs);
	}
}

#endif //VOXWEBSERVER_LIN64_VOXTIMEMAP_H
