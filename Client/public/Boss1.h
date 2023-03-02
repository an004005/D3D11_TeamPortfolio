#pragma once
#include "Monster.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
class CFSMComponent;
END

BEGIN(Client)

class CBoss1 : public CMonster
{
private:
	CBoss1(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBoss1(const CBoss1& rhs);
	virtual ~CBoss1() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void BeginTick() override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	virtual void Imgui_RenderProperty() override;

	_bool IsMove() const { return m_vMoveAxis != _float3::Zero; }
	_float3 GetMoveAxis() const { return m_vMoveAxis; }
	_float GetTurnRemain() const { return m_fTurnRamain; }

private:
	CRenderer*				m_pRendererCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CFSMComponent*			m_pFSM = nullptr;
	class CBoss1_AnimationInstance*		m_pASM = nullptr;

	_float3 m_vMoveAxis;
	_float m_fTurnRamain = 0.f;


	// for test
	class CController*		m_pController = nullptr;
	class CTestCamera*		m_pCam = nullptr;

public:
	static CBoss1* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END