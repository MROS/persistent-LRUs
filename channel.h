// 作爲生產者消費者模式中的管道
#pragma once

#include <queue>
#include <memory>
#include <mutex>
#include <condition_variable>

template <typename T>
class Channel {
public:
	void add(T element) {
		while (true) {
			std::unique_lock<std::mutex> locker(mu);
			cond.wait(locker, [this](){return buffer.size() < size;});
			buffer.push(element);
			locker.unlock();
			cond.notify_all();
			return;
		}
	}
	T remove() {
		while (true)
		{
			std::unique_lock<std::mutex> locker(mu);
			cond.wait(locker, [this](){return buffer.size() > 0;});
			T front = buffer.front();
			buffer.pop();
			locker.unlock();
			cond.notify_all();
			return front;
		}
	}
	Channel() = default;
	// ???
	Channel(const Channel& other) { }
private:
	std::mutex mu;
	std::condition_variable cond;

	std::queue<T> buffer;
	static const size_t size = 20;
};
