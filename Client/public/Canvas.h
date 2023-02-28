#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CTexture;
class CRenderer;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CCanvas : public CUI
{
protected:
	CCanvas(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCanvas(const CCanvas& rhs);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_double TimeDelta) override;
	virtual void	Late_Tick(_double TimeDelta) override;
	
	virtual void	Imgui_RenderProperty() override;
	virtual void	SaveToJson(Json& json) override;
	virtual void	LoadFromJson(const Json& json) override;

protected:
	CUI* Find_ChildUI(const _tchar* pChildTag);
	CUI* Add_ChildUI(_uint iLevelIndex, const _tchar* pPrototypeTag, const _tchar* pChildTag, void* pArg = nullptr);

public:
	map<wstring, CUI*> Get_Map() {
		return m_mapChildUIs;
	}

protected:
	map<wstring, CUI*> m_mapChildUIs;

	CUI*	m_pUI = { nullptr };

public:
	static CCanvas* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUI* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END