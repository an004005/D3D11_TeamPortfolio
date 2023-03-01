#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CLight final : public CBase
{
public:
	CLight(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLight() = default;

public:
	LIGHTDESC* Get_LightDesc() {
		return &m_LightDesc;
	}

public:
	HRESULT Initialize(const LIGHTDESC& LightDesc);
	HRESULT Render(class CVIBuffer_Rect* pVIBuffer, class CShader* pShader);
	LIGHTDESC::TYPE GetType() const { return m_LightDesc.eType; }
	void SetDirection(_float4 vDirection) { m_LightDesc.vDirection = vDirection; }

	void Imgui_Render();

private:
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pContext = nullptr;

private:
	LIGHTDESC			m_LightDesc;

public:
	static CLight* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const LIGHTDESC& LightDesc);
	virtual void Free() override;
};

END