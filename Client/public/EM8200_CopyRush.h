#pragma once
#include "GameObject.h"
#include "Client_Defines.h"
#include "Enemy_AnimInstance.h"
#include "Timeline.h"
#include "SoundStore.h"

BEGIN(Engine)
class CRenderer;
class CModel;
class CFSMComponent;
END

BEGIN(Client)

class CEM8200_CopyRushAnimInstance;

class CEM8200_CopyRush : public CGameObject
{
private:
	CEM8200_CopyRush(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEM8200_CopyRush(const CGameObject& rhs);
	virtual ~CEM8200_CopyRush() = default;

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	virtual void Imgui_RenderProperty() override;

	void StartRush(class CEM8200* pOwner, class CScarletCharacter* pTarget, _fmatrix WorldMatrix, _uint iDamage);

public:
	virtual _float4x4 GetBoneMatrix(const string& strBoneName, _bool bPivotapply = true) override;
	virtual _float4x4 GetPivotMatrix() override;

private:
	void HitOverlap();
	_bool CheckDamagedTarget(CScarletCharacter* pTarget);
	void ClearDamagedTarget();
	_double GetRandomTickPerSec();

private:
	CRenderer*				m_pRendererCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CFSMComponent*			m_pFSM = nullptr;
	CEM8200_CopyRushAnimInstance* m_pASM = nullptr;
	class CEffectSystem* m_pKarenMaskEf = nullptr;

	CSimpleTimeline			m_TPStart;
	CSimpleTimeline			m_TPEnd;

	_bool m_bActive = false;
	_bool m_bRushStart = false;
	
	class CEffectGroup* m_pDashEF = nullptr;

	class CScarletCharacter* m_pTarget = nullptr;
	class CEM8200* m_pCastedOwner = nullptr;
	_uint m_iDamage = 0;

	_double m_DefaultTicPerSec = 0.0;
	set<CScarletCharacter*> m_DamagedTargetList;

	CSoundStore m_SoundStore;
	
public:
public:
	static CEM8200_CopyRush*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg = nullptr) override;
	virtual void			Free() override;
};

class CEM8200_CopyRushAnimInstance : public CEnemy_AnimInstance
{
public:
	virtual HRESULT Initialize(CModel* pModel, CGameObject* pGameObject) override;
	virtual void UpdateTargetState(_double TimeDelta) override;

public:
	static CEM8200_CopyRushAnimInstance* Create(CModel* pModel, CGameObject* pGameObject);
	virtual void Free() override;
};

END