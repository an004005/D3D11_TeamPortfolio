#pragma once
#include "ScarletCharacter.h"
#include "Client_Defines.h"
#include "Timeline.h"
#include "MathUtils.h"

BEGIN(Engine)
class CRenderer;
class CModel;
END

BEGIN(Client)

class CMonsterEx abstract : public CScarletCharacter
{
public:
	enum MONSTER_NAME { BRONJON, SKUMMYPANDOU, SKUMMYPOOL, BUDDYLUMI, FLOWERLEG, GODLYFERRY, MONSTERNAME_END };

protected:
	CMonsterEx(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonsterEx(const CScarletCharacter& rhs);
	virtual ~CMonsterEx() = default;

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual void Imgui_RenderProperty() override;
	virtual HRESULT Render_ShadowDepth() override;

	virtual void SetUpComponents(void* pArg);
	virtual void SetupSound();

	virtual void TakeDamage(DAMAGE_PARAM tDamageParams) override;
	
public:
	_float GetHpRatio() const { return (_float)m_iHP / (_float)m_iMaxHP; }
	_float GetCrushGageRatio() const { return (_float)m_iCrushGage / (_float)m_iMaxCrushGage; }
	_bool IsDead() const { return m_bDead; }
	virtual _float4	GetKineticTargetPos() { return GetColliderPosition(); }

protected:
	// 몬스터가 죽으면 실행해야할 코드들 넣기
	virtual void SetDead();

protected:
	static vector<wstring>			s_vecDefaultBlood;
	static vector<wstring>			s_vecFireBlood;
	static vector<wstring>			s_vecElecBlood;

	static vector<wstring>			s_vecDefaultHit;
	static vector<wstring>			s_vecFireHit;
	static vector<wstring>			s_vecElecHit;

protected:
	CRenderer*				m_pRendererCom = nullptr;
	CModel*					m_pModelCom = nullptr;

	_int m_iHP = 100;
	_int m_iMaxHP = 100;

	_int m_iCrushGage = 100;
	_int m_iMaxCrushGage = 100;

	_bool m_bDead = false;
	set<CScarletCharacter*> m_DamagedTargetList;
	CSimpleTimeline m_DeathTimeline;

	_uint	iMonsterLevel = { 0 };
	MONSTER_NAME m_eMonsterName = { MONSTERNAME_END };

	string m_strDeathSoundTag;
	string m_strImpactTag;
	string m_strImpactVoiceTag;

public:
	virtual void Free() override;
};

END