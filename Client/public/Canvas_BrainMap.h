#pragma once

#include "Canvas.h"
#include "Client_Defines.h"
#include "Item_Manager.h"

BEGIN(Client)

class CCanvas_BrainMap : public CCanvas
{
protected:
	CCanvas_BrainMap(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCanvas_BrainMap(const CCanvas_BrainMap& rhs);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_double TimeDelta) override;
	virtual void	Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

	virtual void	Imgui_RenderProperty() override;
	virtual void	SaveToJson(Json& json) override;
	virtual void	LoadFromJson(const Json& json) override;

private:
	void	OnIcon_Tick();

private:
	vector<class CMain_OnMouseUI*>	m_vecIconUI;
	vector<CUI*>	m_vecLinkUI;

	CItem_Manager::BRAININFO	m_BrainInfo;

	_uint	m_iIconCount = { 0 };
	_uint	m_iLinkCount = { 0 };
	
	_bool	m_bAcquired = { false };


public:
	static CCanvas_BrainMap* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCanvas* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END