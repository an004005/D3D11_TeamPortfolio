#pragma once
#include "Canvas.h"
#include "Client_Defines.h"
#include "ClientUtils.h"


BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END


BEGIN(Client)

class CLoading : public CCanvas
{
private:
	CLoading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLoading(const CLoading& rhs);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

	virtual void Imgui_RenderProperty() override;

private:
	CShader*				m_pShaderCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CVIBuffer_Rect*			m_pVIBufferCom = nullptr;

	class CPartUI* m_pLoadingAnim = nullptr;
	_float m_fTime = 0.f;


public:
	virtual void Free() override;
	DECLARE_CREATE(CLoading)
	DECLARE_CLONE(CLoading)
};

END