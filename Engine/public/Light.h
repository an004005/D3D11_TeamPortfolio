#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CLight final : public CBase
{
public:
	enum TRANSFORMSTATE { D3DTS_VIEW, D3DTS_PROJ, D3DTS_END };

public:
	CLight(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLight() = default;

public:
	LIGHTDESC* Get_LightDesc() {
		return &m_LightDesc;
	}

	// _bool&		Set_Enable()
	// {
	// 	return m_LightDesc.isEnable;
	// }

	// void Set_LightPos(_float4 vPos)				{ XMStoreFloat4(&m_LightDesc.vPosition, vPos); }
	// void Set_LightRange(_float fRange)			{ m_LightDesc.fRange = fRange; }
	// void Set_LightDirection(_float4 vDirection) { m_LightDesc.vDirection = vDirection; }

public:
	HRESULT Initialize(const LIGHTDESC& LightDesc);
	HRESULT Render(class CVIBuffer_Rect* pVIBuffer, class CShader* pShader);
	LIGHTDESC::TYPE GetType() const { return m_LightDesc.eType; }
	void SetDirection(_float4 vDirection) { m_LightDesc.vDirection = vDirection; }

	void Imgui_Render();

	// TYPE_DIRECTIONAL ¿ë
	// void					Update(_fmatrix CamWorldMatrix, _float4* vFrustumCorners);


private:
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pContext = nullptr;

private:
	LIGHTDESC				m_LightDesc;
	// _float4x4				m_TransformState[D3DTS_END];
	// _float4x4				m_TransformState_TP[D3DTS_END];
public:
	static CLight* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const LIGHTDESC& LightDesc);
	virtual void Free() override;
};

END