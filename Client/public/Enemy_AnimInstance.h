#pragma once
#include "Client_Defines.h"
#include "AnimationInstance.h"

BEGIN(Client)

class CEnemy_AnimInstance abstract : public CAnimationInstance
{
public:
	virtual HRESULT Initialize(CModel* pModel, CGameObject* pGameObject) override;
	virtual void Tick(_double TimeDelta) override;

	virtual void Imgui_RenderState() override;

	const string& GetCurStateName() { return m_pASM_Base->GetCurState()->m_strName; }

public:
	_bool isSocketEmpty(const string& strSocName) { return m_mapAnimSocket[strSocName].empty(); }
	_bool isSocketPassby(const string& strSocName, _float fPlayRatio = 1.f);
	void InputAnimSocket(const string& strSocName, list<CAnimation*> AnimList);
	void AttachAnimSocket(const string& strSocName, const list<CAnimation*>& AnimList);
	_bool CheckSocketAnim(const string& strSocName, const string& AnimName) { return (AnimName == m_mapAnimSocket[strSocName].front()->GetName()) ? true : false; }
	void ClearSocketAnim(const string& strSocName, _float fLerpTime = 0.1f);

	void InputAnimSocketOne(const string& strSocName, const string& strAnimName);
	void AttachAnimSocketOne(const string& strSocName, const string& strAnimName);

	
	const string&	GetCurSocketAnimName();
	void InputAnimSocketMany(const string& strSocName, const list<string>& AnimNameList);
	void AttachAnimSocketMany(const string& strSocName, const list<string>& AnimNameList);
	
protected:
	CAnimationStateMachine* m_pASM_Base = nullptr;

	_float	m_fLerpTime = 0.f;
	_float	m_fLerpDuration = 0.1f;
	_bool m_bAttach = false;

public:
	virtual void Free() override;
};

END