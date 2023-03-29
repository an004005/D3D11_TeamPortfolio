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

	void AddLifePointLight(_float fLife, const _float4& vPos, _float fRange, const _float4& vColor);
	void AddLifeCapsuleLight(_float fLife, const _float4& vStart, const _float4& vEnd, _float fRange, const _float4& vColor);

	void Tick(_double TimeDelta);

	const _float4x4* GetShadowLightView() const { return &m_ShadowLightView; }
	const _float4x4* GetShadowLightProj() const { return &m_ShadowLightProj; }
	void SetShadowCam(class CCamera* pShadowCam);
	_float4 GetShadowCamLook();
	_bool IsShadowCamOn() const { return m_pShadowCam != nullptr; }
	_float4 GetDirectionalLightDir() { return m_vDirectionalLightDir; }

	// void	CascadeUpdate(_float4x4 CamWorlMatrix, _float4* vFustumCorners);

	void Imgui_Render();


private:
	unordered_map<string, class CLight*> m_Lights;
	list<pair<class CLight*, _float>> m_TempLights;

	CCamera* m_pShadowCam = nullptr;
	_float4x4 m_ShadowLightView;
	_float4x4 m_ShadowLightProj;
	_float4 m_vDirectionalLightDir;

public:
	virtual void Free() override;
};

END