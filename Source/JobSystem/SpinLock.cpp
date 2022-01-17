#include "SpinLock.hpp"

namespace Chaf
{
SpinLock::SpinLock() :
    m_flag{ATOMIC_FLAG_INIT}
{
}

void SpinLock::lock()
{
	while (m_flag.test_and_set(std::memory_order_acquire))
		;
}

void SpinLock::unlock()
{
	m_flag.clear(std::memory_order_release);
}
}        // namespace Chaf
