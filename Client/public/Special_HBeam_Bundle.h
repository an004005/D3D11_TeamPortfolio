#pragma once
#include "SpecialObject.h"
#include "Client_Defines.h"
#include "HelperClasses.h"

BEGIN(Engine)
class CModel;
END

BEGIN(Client)

class CSpecial_HBeam_Bundle : public CSpecialObject
{
private:
	CSpecial_HBeam_Bundle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSpecial_HBeam_Bundle(const CSpecial_HBeam_Bundle& rhs);
	virtual ~CSpecial_HBeam_Bundle() = default;

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
	void	HBeam_Drift();
	void	HBeam_Throw(_float4 vDir);
	void	HBeam_Decompose();
	void	HBeam_Single_Catch();
	void	HBeam_Single_Turn();
	void	HBeam_Single_Finish();
	void	HBeam_Single_SetTrigger(_bool bKinetic);

	void	HBeam_Collision();
	void	HBeam_Explosion();

	void	HBeam_SetDeadTimer();

private:
	_bool  m_bDeadCheck = false;
	_float m_fDeadTime = 0.f;
	_float m_fCollisionTime = 0.f;

public:
	_bool	HBeam_isDecomposed() { return m_bRenderOption; }	// false면 디컴포즈된거임

private:
	HRESULT	SetUp_Components(void* pArg);
	void	Create_ChildBeam();

private:
	CGameObject*	m_pHBeam_Single[8];

private:
	_bool	m_bThrow = false;
	_bool	m_bRenderOption = true;	//	true면 번들 렌더, false면 싱글 렌더
	CDoOnce tt;
	_float	m_fThrowPower = 3000.f;
	_float	m_fFloatPower = 11.5f;

public:
	static CSpecial_HBeam_Bundle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg = nullptr)override;
	void Free() override;
};

END