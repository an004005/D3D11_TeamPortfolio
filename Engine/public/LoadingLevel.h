#pragma once
#include "Level.h"
#include <thread>

BEGIN(Engine)

class ENGINE_DLL CLoadingLevel : public CLevel
{
protected:
	CLoadingLevel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, std::function<CLevel*()>&& LevelCreator);

public:
	virtual HRESULT Initialize() override;
	void StartLoading();

protected:
	void SetFinish();
	_bool IsFinished() const { return m_bFinished; }

public:
	virtual void Free() override;

private:
	atomic<_bool> m_bFinished{false};
	std::function<CLevel*()> m_LevelCreator = nullptr;
	std::thread m_LoadingThread;
	CLevel* m_pCreatedLevel = nullptr;
};

END