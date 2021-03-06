#include "ThreadPool.hpp"

namespace Chaf
{
ThreadPool::ThreadPool(uint32_t max_threads_num)
{
	for (uint32_t i = 0; i < max_threads_num; i++)
	{
		m_workers.emplace_back(
		    [this]() {
			    while (true)
			    {
				    std::function<void()> task;

				    {
					    std::unique_lock<std::mutex> lock(m_mutex);
					    m_condition.wait(lock, [this]() { return m_stop || !m_task_queue.empty(); });
					    if (m_stop && m_task_queue.empty())
					    {
						    return;
					    }
					    m_task_queue.tryPop(task);
				    }

				    task();
			    }
		    });
	}
}

ThreadPool ::~ThreadPool()
{
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		m_stop = true;
	}

	m_condition.notify_all();
	for (auto &worker : m_workers)
	{
		worker.join();
	}
}

size_t ThreadPool::getThreadCount() const
{
	return m_workers.size();
}
}        // namespace Chaf