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
	class CLight* Find_Light(const string& strLightTag);
	class CLight* Add_Light(const string& strLightTag, ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const LIGHTDESC& LightDesc);
	void Delete_Light(const string& strLightTag);
	void Render_Light(class CVIBuffer_Rect* pVIBuffer, class CShader* pShader);
	void Clear();

	const _float4x4* GetShadowLightView() const { return &m_ShadowLightView; }
	const _float4x4* GetShadowLightProj() const { return &m_ShadowLightProj; }
	void SetShadowCam(class CCamera* pShadowCam);
	_float4 GetShadowCamLook();

	// void	CascadeUpdate(_float4x4 CamWorlMatrix, _float4* vFustumCorners);

	void Imgui_Render();

private:
	unordered_map<string, class CLight*> m_Lights;

	CCamera* m_pShadowCam = nullptr;
	_float4x4 m_ShadowLightView;
	_float4x4 m_ShadowLightProj;

public:
	virtual void Free() override;
};

END