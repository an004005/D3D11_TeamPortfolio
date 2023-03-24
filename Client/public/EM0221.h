#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

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
	void Set_Fall() { 
		m_bFall = true;
		m_bTime = 0.0;
	}
	const string& Get_TargetBoneName() { return m_szTargetBoneName; }
private:
	void Falling(_double TimeDelta);
private:
	CRenderer*				m_pRendererCom = nullptr;
	CModel*					m_pModelCom = nullptr;

	string					m_szTargetBoneName = "";
	_float4x4				m_AddPivotMatrix;

	//fall
	_bool					m_bFall = false;
	_double					m_bTime = 0.0;
	_float					m_fFallSpeed = 0.f;
public:
	static CEM0221* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END