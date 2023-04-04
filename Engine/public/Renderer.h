#pragma once

#include "Component.h"

/* 화면에 그려야할 객체들을 그리는 순서대로 정리보관한다. */
/* 보관하고 있는 순서대로 객체들의 드로우콜(렌더함수를 호출한다)을 수행한다. */

BEGIN(Engine)

typedef struct tagFogDesc
{
	_float3 vFogColor;
	_float fStartDepth;
	_float3 vHighlightColor;
	_float fGlobalDensity;
	_float fHeightFalloff;
}FOG_DESC;

class ENGINE_DLL CRenderer final : public CComponent
{
public:
	enum RENDERGROUP
	{
		RENDER_PRIORITY,
		RENDER_SHADOWDEPTH,
		RENDER_NONALPHABLEND,
		RENDER_NONALPHABLEND_TOON,
		RENDER_MESH_ALPHABLEND,
		RENDER_NONLIGHT,
		RENDER_ALPHABLEND_FIRST,
		RENDER_ALPHABLEND,
		RENDER_DECAL,
		RENDER_DEBUG,
		RENDER_OUTLINE,
		RENDER_PORTRAIT,
		POSTPROCESS_VFX,
		RENDER_UI,
		RENDER_END
	};
private:
	CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);	
	virtual ~CRenderer() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Ready_ShadowDepthResources(_uint iWidth, _uint iHeight);
	void Imgui_RenderOtherWindow();

public:
	HRESULT Add_RenderGroup(RENDERGROUP eRenderGroup, class CGameObject* pGameObject);
	HRESULT Add_DebugRenderGroup(class CComponent* pComponent);
	HRESULT Draw_RenderGroup();
	void Clear();

	class CShader* GetShader() { return m_pShader; }

	void LoadFogJson(const string& strJsonPath);
	FOG_DESC& GetFogDesc() { return m_tFogDesc; }

private:
	list<class CGameObject*>			m_RenderObjects[RENDER_END];
	typedef list<class CGameObject*>	RENDEROBJECTS;

private:
	list<class CComponent*>				m_DebugObject;
	typedef list<class CComponent*>		DEBUGOBJECTS;

private:
	class CTarget_Manager*				m_pTarget_Manager = nullptr;
	class CLight_Manager*				m_pLight_Manager = nullptr;
	class CVIBuffer_Rect*				m_pVIBuffer = nullptr;
	class CShader*						m_pShader = nullptr;
	class CShader*						m_pShader_PostProcess = nullptr;
	_float4x4							m_WorldMatrix, m_ViewMatrix, m_ProjMatrix;

	ID3D11DepthStencilView* m_pShadowDepthStencilView = nullptr;
	_bool m_bVisibleTargets = false;

	class CTexture* m_pDiffuseIrradianceTexture = nullptr;
	class CTexture* m_pSpecularRadianceTexture = nullptr;
	_bool m_bShadow = true;


	_bool m_bFog;
	FOG_DESC m_tFogDesc;

	_bool m_bLightRays = false;
	_float m_fSSLRIntensity = 0.25f;

private:
	HRESULT Render_Priority();
	HRESULT Render_ShadowDepth();
	HRESULT Render_NonAlphaBlend();
	HRESULT Render_LightAcc();
	HRESULT Render_Blend();
	HRESULT Render_MeshAlphaBlend();
	HRESULT Render_NonLight();
	HRESULT Render_AlphaBlend();
	HRESULT Render_HDR();
	HRESULT Render_PostProcess();
	HRESULT Render_UI();
	HRESULT Render_Decal();

	HRESULT	Render_Distortion(const _tchar* pTargetTag);
	HRESULT Render_Outline();
	HRESULT Render_Portrait();


private:
	HRESULT Render_DebugObject();

public:
	static CRenderer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CRenderer* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

};

END