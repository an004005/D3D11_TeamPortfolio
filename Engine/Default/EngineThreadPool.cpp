#include "stdafx.h"
#include "EngineThreadPool.h"

IMPLEMENT_SINGLETON(CEngineThreadPool);

void CEngineThreadPool::Initialize(size_t iNumThread)
{
	m_iNumThread = iNumThread;
	m_Workers.reserve(m_iNumThread);
	for (size_t i = 0; i < m_iNumThread; ++i)
	{
		m_Workers.emplace_back([this]
		{
			while (true) 
			{
				std::unique_lock<std::mutex> lock(m_mJob);
				m_cvJobQ.wait(lock, [this]()
				{
					return !this->m_Jobs.empty() || m_bStopAll;
				});

				if (m_bStopAll && this->m_Jobs.empty()) 
				{
				  return;
				}

				// 맨 앞의 job 을 뺀다.
				++m_iBusy;
				std::function<void()> job = std::move(m_Jobs.front());
				m_Jobs.pop();
				lock.unlock();

				// 해당 job 을 수행한다 :)
				job();
				--m_iBusy;
				lock.lock();
				m_cvFinish.notify_one();
			}
		});
	}
}

void CEngineThreadPool::WaitFinished()
{
	if (m_bStopAll)
		return;

	if (m_Jobs.empty() && (m_iBusy == 0))
		return;

    std::unique_lock<std::mutex> lock(m_mJob);
    m_cvFinish.wait(lock, [this]()
    {
	    return m_Jobs.empty() && (m_iBusy == 0);
    });
}

void CEngineThreadPool::Free()
{
	std::unique_lock<std::mutex> lock(m_mJob);
	m_bStopAll.store(true);
	m_cvJobQ.notify_all();
	lock.unlock();

	for (auto& t : m_Workers)
	{
		t.join();
	}
}
