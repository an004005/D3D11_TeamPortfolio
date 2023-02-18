#pragma once

/* ���� ȭ�鿡 ��������� ������ �ּҸ� ������ �մ´�. */
/* �Ҵ�� ������ ƽ�� ������ ȣ�߶���. */
/* ���������� ���ƴ� �ڿ��� �����Ѵ�. */
#include "Base.h"

BEGIN(Engine)

class CLevel_Manager final : public CBase
{
	DECLARE_SINGLETON(CLevel_Manager)
private:
	CLevel_Manager();
	virtual ~CLevel_Manager() = default;

public:
	HRESULT Open_Loading(_uint iNewLevelIdx, class CLoadingLevel* pLoadingLevel);
	HRESULT Finish_Loading(class CLevel* pNewLevel);
	HRESULT Open_Level(_uint iLevelIndex, class CLevel* pNewLevel);
	void Tick(_double TimeDelta);
	void Late_Tick(_double TimeDelta);
	HRESULT Render();

	_uint GetCurLevelIdx() const { return m_iLevelIndex; }
	_uint GetUpdatedLevel() const;
	_bool IsLoading() const { return m_bLoading; }
	class CLevel* GetLevel() { return m_pCurrentLevel; }

private:
	class CLevel*				m_pCurrentLevel = nullptr;
	_uint						m_iLevelIndex = 0;
	atomic<_bool> m_bLoading{false};
public:
	virtual void Free() override;
};

END