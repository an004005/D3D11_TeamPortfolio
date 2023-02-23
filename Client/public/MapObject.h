#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CModel;
class CRenderer;
class CShader;
END

BEGIN(Client)

class CMapObject abstract : public CGameObject
{
protected:
	CMapObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMapObject(const CMapObject& rhs);
	virtual ~CMapObject() = default;

public:
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void* pArg) override;
	void BeginTick() override;
	void Tick(_double TimeDelta) override;
	void Late_Tick(_double TimeDelta) override;
	HRESULT Render() override;
	virtual void Imgui_RenderProperty() override;
	virtual void LoadFromJson(const Json& json) override;

public:
	const wstring&	Get_ModelTag() { return m_strModelTag; }
	
private:
	HRESULT	SetUp_Components();

protected:
	CModel*					m_pModelCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;

protected:
	wstring					m_strModelTag;
	
public:
	CGameObject* Clone(void* pArg = nullptr) override { return nullptr; }
	void Free() override;

};

END