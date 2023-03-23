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
	void	HBeam_Single_SetKinetic(_bool bKinetic);

public:
	_bool	HBeam_isDecomposed() { return m_bRenderOption; }	// false�� ��������Ȱ���

private:
	HRESULT	SetUp_Components(void* pArg);
	void	Create_ChildBeam();

private:
	CGameObject*	m_pHBeam_Single[8];

private:
	_bool	m_bThrow = false;
	_bool	m_bRenderOption = true;	//	true�� ���� ����, false�� �̱� ����
	CDoOnce tt;
	_float	m_fThrowPower = 680.f;

public:
	static CSpecial_HBeam_Bundle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg = nullptr)override;
	void Free() override;
};

END