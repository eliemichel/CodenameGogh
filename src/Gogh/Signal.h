#ifndef H_SIGNAL
#define H_SIGNAL

#include <functional>
#include <map>
#include <utility>

typedef int SignalConnection;

template <typename... Args>
class Signal {
public:
	Signal() : m_nextId(1) {} // Connection = 0 is reserved for invalid connections

	Signal(Signal const& other) = delete;

	template <typename T>
	SignalConnection connect(T *inst, void (T::*func)(Args...)) {
		return connect([=](Args... args) {
			(inst->*func)(args...);
		});
	}

	template <typename T>
	SignalConnection connect(T *inst, void (T::*func)(Args...) const) {
		return connect([=](Args... args) {
			(inst->*func)(args...);
		});
	}

	SignalConnection connect(std::function<void(Args...)> const& slot) {
		m_slots.insert(std::make_pair(++m_nextId, slot));
		return m_nextId;
	}

	void disconnect(SignalConnection co) {
		m_slots.erase(co);
	}

	void disconnectAll() const {
		m_slots.clear();
	}

	void fire(Args... args) {
		for (auto it : m_slots) {
			it.second(args...);
		}
	}

	Signal& operator=(Signal const& other) {
		disconnectAll();
	}

private:
	std::map<SignalConnection, std::function<void(Args...)>> m_slots;
	int m_nextId;
};

#endif // H_SIGNAL
