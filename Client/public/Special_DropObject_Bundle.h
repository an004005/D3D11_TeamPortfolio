#pragma once
#include "SpecialObject.h"
#include "Client_Defines.h"
#include "HelperClasses.h"

BEGIN(Engine)
class CModel;
END

BEGIN(Client)

// 번들 자체로는 모델 X, 싱글을 묶어주는 역할만을 한다.

class CSpecial_DropObject_Bundle : public CSpecialObject
{
private:
	CSpecial_DropObject_Bundle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSpecial_DropObject_Bundle(const CSpecial_DropObject_Bundle& rhs);
	virtual ~CSpecial_DropObject_Bundle() = default;

public:
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void* pArg) override;
	void BeginTick() override;
	void Tick(_double TimeDelta) override;
	void Late_Tick(_double TimeDelta) override;
	virtual void AfterPhysX();
	HRESULT Render() override;

	virtual void LoadFromJson(const Json& json) override;
	virtual void SaveToJson(Json& json) override;

	virtual void Imgui_RenderProperty() override;

public:
	void	Set_Kinetic(_bool bKinetic);
	void	DropObject_Floating();
	void	DropObject_Drop();

private:
	HRESULT	SetUp_Components(void* pArg);
	void	Create_ChildObject();

private:
	CGameObject*	m_pObject_Single[20];

private:
	_bool	m_bThrow = false;
	_bool	m_bDecompose = false;
	CDoOnce tt;
	_float	m_fThrowPower = 2000.f;
	_float	m_fFloatPower = 300.f;
	CDoOnce	Decompose;

	_bool  m_bDeadCheck = false;
	_float m_fDeadTime = 0.f;

private:
	vector<_float4x4>	m_vecChildMatrix;

public:
	static CSpecial_DropObject_Bundle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg = nullptr)override;
	void Free() override;
};

END