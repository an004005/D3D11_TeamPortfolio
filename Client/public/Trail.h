#pragma once
#include "GameObject.h"
#include "Client_Defines.h"
#include "TimerHelper.h"
#include "Shader.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CVIBuffer;
END

BEGIN(Client)

class CTrail : public CGameObject
{
private:
	CTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTrail(const CTrail& rhs);
	virtual ~CTrail() = default;

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	virtual void SaveToJson(Json& json) override;
	virtual void Imgui_RenderProperty() override;

public:
	void SetActive(_bool bActive) { m_bActive = bActive; }
	_bool IsActive() const { return m_bActive; }

	void ClearBuffer();

private:
	CShader*					m_pShaderCom = nullptr;
	CRenderer*					m_pRendererCom = nullptr;
	class CVIBuffer_Instancing_Trail* m_pBuffer = nullptr;
	class CTexture*				m_pTex = nullptr;

	_bool m_bActive = true;

	_float m_fLife;
	_float m_fWidth;
	_float m_fSegmentSize;
	_float4 m_vColor;
	vector<_float4> m_vPrePoses;
	_uint m_iPass = 0;

public:
	DECLARE_CLONE(CTrail)
	DECLARE_CREATE(CTrail)
	virtual void Free() override;
};

END