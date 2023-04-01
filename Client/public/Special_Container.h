#pragma once
#include "SpecialObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CModel;
END

BEGIN(Client)

class CSpecial_Container : public CSpecialObject
{
private:
	CSpecial_Container(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSpecial_Container(const CSpecial_Container& rhs);
	virtual ~CSpecial_Container() = default;

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
	void Container_ChangeIndex(_uint iIndex);
	void Lerp_to_TargetPoint(_float4 vTargetPos, _float fRatio);
	void Container_Press();
	void Container_Press_Maintain(_float4 vTargetPos, _double TimeDelta);
	void Container_Press_Finish();

	_bool Container_isCollision() { return m_bCollision; }

	void Container_Reposition(CTransform* pTransform, _float fRatio, _float fForce);

private:
	void Container_Input_Damage(DAMAGE_PARAM tParam);

private:
	_bool  m_bDeadCheck = false;
	_float m_fDeadTime = 0.f;

private:
	_int m_iModelIndex = 0;
	_bool m_bCollision = false;
	_float m_fLerpTime = 0.f;


private:
	HRESULT	SetUp_Components(void* pArg);

public:
	static CSpecial_Container* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg = nullptr)override;
	void Free() override;
};

END
