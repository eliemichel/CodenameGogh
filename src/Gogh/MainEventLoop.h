#ifndef H_MAINEVENTLOOP
#define H_MAINEVENTLOOP

#include "Signal.h"
#include <future>
#include <utility>
#include <list>

class Object;

/**
 * Singleton
 */
class MainEventLoop {
public:
	static MainEventLoop & GetInstance() {
		static MainEventLoop instance;
		return instance;
	}

public:
	void Process();
	void Add(std::future<int> && future, Signal<int> * signal);
	void DeleteLater(Object *obj);

private:
	MainEventLoop();

public:
	MainEventLoop(const MainEventLoop &) = delete;
	void operator=(const MainEventLoop &) = delete;

private:
	std::list<std::pair<std::future<int>, Signal<int>*>> m_futures;
	std::list<Object*> m_toDelete;
};

#endif H_MAINEVENTLOOP
