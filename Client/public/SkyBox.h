#pragma once
#include "GameObject.h"
#include "Client_Defines.h"
#include "Shader.h"

BEGIN(Engine)
class CRenderer;
class CModel;
class CVIBuffer_Sphere;
END

BEGIN(Client)

class CSkyBox : public CGameObject
{
private:
	CSkyBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSkyBox(const CSkyBox& rhs);
	virtual ~CSkyBox() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	virtual void Imgui_RenderProperty() override;
	virtual void SaveToJson(Json& json) override;
	virtual void LoadFromJson(const Json& json) override;

public:
	ShaderParams& GetParams() { return m_tParams; }


private:
	CRenderer*				m_pRendererCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CVIBuffer_Sphere*		m_pBuffer = nullptr;
	CShader*				m_pShader = nullptr;
	ShaderParams			m_tParams;
	// CTexture*				m_pTexture = nullptr;

public:
	static CSkyBox* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END