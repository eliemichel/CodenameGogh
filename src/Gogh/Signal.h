#ifndef H_SIGNAL
#define H_SIGNAL

#include <functional>
#include <map>

template <typename... Args>
class Signal {
public:
	Signal() : m_nextId(0) {}

	Signal(Signal const& other) = delete;

	template <typename T>
	int connect(T *inst, void (T::*func)(Args...)) {
		return connect([=](Args&&... args) {
			(inst->*func)(std::forward<Args>(args)...);
		});
	}

	template <typename T>
	int connect(T *inst, void (T::*func)(Args...) const) {
		return connect([=](Args&&... args) {
			(inst->*func)(std::forward<Args>(args)...);
		});
	}

	int connect(std::function<void(Args...)> const& slot) {
		m_slots.insert(std::make_pair(++m_nextId, slot));
		return m_nextId;
	}

	void disconnect(int id) const {
		m_slots.erase(id);
	}

	void disconnect_all() const {
		m_slots.clear();
	}

	void fire(Args&&... args) {
		for (auto it : m_slots) {
			it.second(std::forward<Args>(args)...);
		}
	}

	Signal& operator=(Signal const& other) {
		disconnect_all();
	}

private:
	std::map<int, std::function<void(Args...)>> m_slots;
	int m_nextId;
};

#endif // H_SIGNAL
