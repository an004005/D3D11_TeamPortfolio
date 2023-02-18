#pragma once
#include "UI.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CPositionMarker : public CUI
{
private:
	CPositionMarker(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPositionMarker(const CPositionMarker& rhs);

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Render() override;
	void SetPosition(_bool bAttacker)
	{
		m_bAttacker = bAttacker;
	}
	void SetColor(_float4 vColor) { m_vColor = vColor; }

private:
	CShader*				m_pShaderCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CVIBuffer_Rect*			m_pVIBufferCom = nullptr;

	_bool m_bAttacker = false;
	_float4 m_vColor = _float4::One;

public:
	virtual void Free() override;
	DECLARE_CREATE(CPositionMarker)
	DECLARE_CLONE(CPositionMarker)
};

END