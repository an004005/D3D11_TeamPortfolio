#pragma once
#include "Client_Defines.h"
#include "PostProcess.h"
#include "Timeline.h"

BEGIN(Engine)
class CModel;
class CRenderer;
class CCamera;
class CCurveFloatImpl;
END

BEGIN(Client)

/*********************
 *CPostVFX_SAS_Portrait
 *********************/
class CPostVFX_SAS_Portrait : public CPostProcess
{
protected:
	CPostVFX_SAS_Portrait(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPostVFX_SAS_Portrait(const CPostVFX_SAS_Portrait& rhs);

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;

public:
	static CPostVFX_SAS_Portrait* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
};

/*********************
 *CSAS_Portrait
 *********************/
class CSAS_Portrait : public CGameObject
{
protected:
	CSAS_Portrait(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSAS_Portrait(const CSAS_Portrait& rhs);
	virtual ~CSAS_Portrait() = default;

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void BeginTick() override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	virtual void Imgui_RenderProperty() override;

	void Start_SAS(ESASType eType);

	void ResetParams();
	void TickParams();

private:
	CRenderer*				m_pRendererCom = nullptr;
	array<CModel*, SAS_CNT> m_SAS_PortraitModels{};
	array<CCamera*, SAS_CNT> m_SAS_PortraitCams{};
	ESASType m_eCurType = ESASType::SAS_END;

	CPostVFX_SAS_Portrait* m_pPostVFX = nullptr;

	CCurveFloatImpl* m_pDissolveCurve = nullptr;
	CCurveFloatImpl* m_pColorLerpCurve = nullptr;
	CCurveFloatImpl* m_pOutlineLerpCurve = nullptr;

public:
	_bool	isFinish();

private:
	_bool	m_bFinish = false;

public:
	static CSAS_Portrait* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};


END