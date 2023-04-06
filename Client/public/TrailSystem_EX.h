#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "Shader.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CVIBuffer;
END
BEGIN(Client)
class CTrailSystem_EX : public CGameObject
{
public:
	CTrailSystem_EX(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTrailSystem_EX(const CTrailSystem_EX& rhs);
	virtual ~CTrailSystem_EX() = default;

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_double TimeDelta) override;
	virtual void	Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	virtual void	SaveToJson(Json & json) override;
	virtual void	Imgui_RenderProperty() override;
	HRESULT			SetUp_ShaderResources();

public:
	void			SetActive(_bool bActive) { m_bActive = bActive; }
	_bool			IsActive() const { return m_bActive; }

	void			ClearBuffer();

private:
	CShader*		m_pShaderCom = nullptr;
	CRenderer*		m_pRendererCom = nullptr;
	class CVIBuffer_Trail_Instancing* m_pBuffer = nullptr;
	class CTexture* m_pTex = nullptr;

	_bool								m_bActive = true;

	_float								m_fLife;
	_float								m_fWidth;
	_float								m_fSegmentSize;
	_float4								m_vColor;
	vector<_float4>						m_vPrePoses;
	vector<_float4>						m_vPreLook;
	list<_vector>						m_TrailPointList;
	_uint								m_iPass = 0;
	_uint								m_iSegmentCnt = 0;

public:
	static CTrailSystem_EX* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void						Free() override;
};


END