#pragma once

#include <atomic>

namespace Chaf
{
class SpinLock
{
  public:
	SpinLock();

	~SpinLock() = default;

	void lock();

	void unlock();

  private:
	std::atomic_flag m_flag;
};
}        // namespace Chaf