#include "MainEventLoop.h"
#include "Logger.h"
#include "Object.h"

MainEventLoop::MainEventLoop() {

}

template<typename R>
bool is_ready(std::future<R> const& f)
{
	return f.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
}

void MainEventLoop::Process() {
	for (auto it = m_futures.begin(); it != m_futures.end();) {
		std::future<int> & f = it->first;
		Signal<int> * s = it->second;
		if (is_ready(f)) {
			s->fire(f.get());
			it = m_futures.erase(it);
		}
		else {
			++it;
		}
	}

	while (!m_toDelete.empty()) {
		delete m_toDelete.back();
		m_toDelete.pop_back();
	}
}

void MainEventLoop::DeleteLater(Object *obj) {
	m_toDelete.push_back(obj);
}

void MainEventLoop::Add(std::future<int> && future, Signal<int> * signal) {
	m_futures.push_back(std::make_pair(std::move(future), signal));
}

