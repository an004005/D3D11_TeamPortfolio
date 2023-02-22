#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Shader.h"

BEGIN(Client)

class CUI_Manager final : public CGameObject
{
private:
	CUI_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Manager(const CUI_Manager& rhs);
	virtual ~CUI_Manager() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_double TimeDelta) override;
	virtual void	Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

	virtual void	Imgui_RenderProperty() override;
	virtual void	SaveToJson(Json& json) override;
	virtual void	LoadFromJson(const Json& json, const _tchar* pPrototypeTag);

private:
	HRESULT			Create_UI();
	HRESULT			Create_Canvas(const _tchar* pPrototypeTag);

private:
	void			Imgui_UI();

private:
	
	list<class CUI*>		m_pUIs;
	list<class CUI*>		m_pCanvases;

	CUI*					m_pUI = nullptr;		// List Box 에서 선택한 객체
	CUI*					m_pCanvas = nullptr;
public:
	static CUI_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END