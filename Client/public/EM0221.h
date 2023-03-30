#pragma once
#include "GameObject.h"
#include "Client_Defines.h"
#include "EffectGroup.h"

BEGIN(Engine)
class CRenderer;
class CModel;
END

BEGIN(Client)

//베이스 포즈에 달려있는 랜턴

class CEM0221 : public CGameObject
{
private:
	CEM0221(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEM0221(const CEM0221& rhs);
	virtual ~CEM0221() = default;

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_ShadowDepth() override;

	void SetUpComponents(void* pArg);
	void SetUpMatrix(CModel* pModle, const _fmatrix TransformMatrix);

	
public:
	void Set_TargetBoneName(const string& boneName) {
		m_szTargetBoneName = boneName;
	}
	void Set_Fall();
	const string& Get_TargetBoneName() { return m_szTargetBoneName; }
private:
	void Falling(_double TimeDelta);
	void Explosion();

	//Collision
	void Explosion_Ray();
private:
	CRenderer*				m_pRendererCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CEffectGroup*		m_pFallEffect = nullptr;
	CEffectGroup*		m_pNormalEffect = nullptr;


	string					m_szTargetBoneName = "";
	_float4x4				m_AddPivotMatrix;

	_float4					m_FallStartPosition;
	//fall
	_bool					m_bFall = false;
	_bool					m_bExplosion = false;
	_float					m_fFallSpeed = 0.f;
	_double				m_dFallMaxTime = 0.0;
public:
	static CEM0221* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END