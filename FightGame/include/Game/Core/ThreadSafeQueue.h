#pragma once
/**
* @file LoadQueue.h
* @brief スレッドセーフのロードを処理するためのキュー
* @author 越智晴彦
* @data 02/02
*/


#include <queue>
#include <condition_variable>
#include <mutex>

template<typename QueueDataType>
class ThreadSafeQueue
{
public:
	ThreadSafeQueue();
	~ThreadSafeQueue() = default;

	/**
	* @ brief Queueにプッシュする
	* @ param QueueDataType
	*/
	void Push(QueueDataType&& a_data);

	/**
	* @ brief QueueをPopする、Queueが空の場合falseを返す
	* @ param QueueDataType
	*/
	bool TryPop(QueueDataType& a_data);

	/**
	* @ brief QueueをPopする、Queueが空の場合待機する
	* @ param QueueDataType
	*/
	QueueDataType Pop();

	bool IsEnpty();

private:
	std::queue<QueueDataType> queue;
	std::mutex mutex;
	std::condition_variable cond;
};

template<typename QueueDataType>
inline ThreadSafeQueue<QueueDataType>::ThreadSafeQueue()
{
}


template<typename QueueDataType>
inline void ThreadSafeQueue<QueueDataType>::Push(QueueDataType&& a_data)
{
	// Queueをロックしてpush
	std::unique_lock<std::mutex> lock(mutex);
	queue.push(a_data);
}

template<typename QueueDataType>
inline bool ThreadSafeQueue<QueueDataType>::TryPop(QueueDataType& a_out)
{
	std::unique_lock<std::mutex> lock(mutex);
	if (queue.empty()) return false;
	a_out = std::move(queue.front());
	queue.pop();
	return true;
}


template<typename QueueDataType>
inline QueueDataType ThreadSafeQueue<QueueDataType>::Pop()
{
	// ロックされていたら待機
	std::unique_lock<std::mutex> lock(mutex);
	cond.wait(lock, [&] {return !queue.empty(); });
	// 先頭をコピーして削除
	QueueDataType value = std::move(queue.front());
	queue.pop();

	return value;
}

template<typename QueueDataType>
inline bool ThreadSafeQueue<QueueDataType>::IsEnpty()
{
	return queue.empty();
}
