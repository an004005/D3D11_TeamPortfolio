#pragma once
#include "Base.h"

BEGIN(Engine)

class CEngineThreadPool : public CBase
{
	DECLARE_SINGLETON(CEngineThreadPool)
private:
	CEngineThreadPool() = default;
	virtual ~CEngineThreadPool() = default;

public:
	void Initialize(size_t iNumThread);
	template<typename T>
	void EnqueueJob(T* obj, void (T::*memFunc)());

	// ���Լ� ����Ǹ� ��� job�� ���� ������ ���(main�����忡�� �����Ѵ�.)
	void WaitFinished();

private:
	size_t m_iNumThread;
	std::vector<std::thread> m_Workers;
	std::queue<std::function<void()>> m_Jobs;
	std::condition_variable m_cvJobQ;
	std::mutex m_mJob;

	std::condition_variable m_cvFinish;
	atomic<_uint> m_iBusy {0};

	atomic<_bool> m_bStopAll {false};

public:
	virtual void Free() override;
};

template <typename T>
void CEngineThreadPool::EnqueueJob(T* obj, void(T::* memFunc)())
{
	if (m_bStopAll)
		return;

	std::lock_guard<std::mutex> lock(m_mJob);
	m_Jobs.push([obj, memFunc]()
	{
		(obj->*memFunc)();
	});
	m_cvJobQ.notify_one();
}

END