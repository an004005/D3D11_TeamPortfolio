#pragma once

#include "Base.h"

BEGIN(Engine)

class CLight_Manager final : public CBase
{
	DECLARE_SINGLETON(CLight_Manager)
private:
	CLight_Manager();
	virtual ~CLight_Manager() = default;

public:
	const LIGHTDESC* Get_LightDesc(_uint iIndex);

public:
	HRESULT Add_Light(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const LIGHTDESC& LightDesc);
	void Render_Light(class CVIBuffer_Rect* pVIBuffer, class CShader* pShader);
	void Clear();

	const _float4x4* GetDirectionalLightView() const { return &m_DirectionalLightView; }
	const _float4x4* GetDirectionalLightProj() const { return &m_DirectionalLightProj; }
	void SetShadowCam(class CCamera* pShadowCam);
	_float4 GetShadowCamLook();

private:

	vector<class CLight*>					m_Lights;
	typedef vector<class CLight*>			LIGHTS;

	CCamera* m_pShadowCam = nullptr;
	_float4x4 m_DirectionalLightView;
	_float4x4 m_DirectionalLightProj;

public:
	virtual void Free() override;
};

END