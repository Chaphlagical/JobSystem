#include "JobSystem.hpp"

#include <unordered_set>

namespace Chaf
{
void JobSystem::initialize()
{
	instance().m_thread_pool = std::make_unique<ThreadPool>(std::thread::hardware_concurrency() - 1);
}

size_t JobSystem::getThreadCount()
{
	return instance().m_thread_pool->getThreadCount();
}

void JobSystem::execute(JobHandle &handle, JobGraph &graph)
{
	std::unordered_set<JobNode *> finished_nodes;

	handle.m_counter.fetch_add(static_cast<uint32_t>(graph.m_nodes.size()));

	while (handle.m_counter > 0)
	{
		for (auto &node : graph.m_nodes)
		{
			if (node->m_unfinish_dependents == 0 && finished_nodes.find(node) == finished_nodes.end())
			{
				finished_nodes.insert(node);
				instance().m_thread_pool->addTask([&node, &handle]() {
					node->run();
					handle.m_counter.fetch_sub(1);
				});
			}
		}
	}
}

void JobSystem::execute(JobHandle &handle, JobNode &node)
{
	handle.m_counter.fetch_add(1);

	instance().m_thread_pool->addTask([&node, &handle]() { node.run();handle.m_counter.fetch_sub(1); });
}

void JobSystem::dispatch(JobHandle &handle, uint32_t job_count, uint32_t group_size, const std::function<void(uint32_t)> &task)
{
	uint32_t group_count = (job_count + group_size - 1) / group_size;

	handle.m_counter.fetch_add(group_count);

	for (uint32_t group_id = 0; group_id < group_count; group_id++)
	{
		instance().m_thread_pool->addTask([task, &handle, group_id]() {
			task(group_id);
			handle.m_counter.fetch_sub(1);
		});
	}
}

bool JobSystem::isBusy(const JobHandle &handle)
{
	return handle.m_counter.load() > 0;
}

void JobSystem::wait(const JobHandle &handle)
{
	while (isBusy(handle))
	{
		// wait...
	}
}

JobSystem &JobSystem::instance()
{
	static JobSystem job_system;
	return job_system;
}
}        // namespace Chaf