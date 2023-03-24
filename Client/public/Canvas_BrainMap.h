#pragma once

#include "Canvas.h"
#include "Client_Defines.h"

BEGIN(Client)

class CCanvas_BrainMap : public CCanvas
{
public:
	enum BRAINTYPE { DRIVE, ENHANCE, EXPANSION, ASSISTANCE, BRAINFIELD, BRAINTYPE_END };
	enum BRAINCOLOR { GREEN, YELLOW, RAD, PURPLE, BRAINCOLOR_END };

	typedef struct tagBrainInfo
	{
		BRAINTYPE	eType = { BRAINTYPE_END };
		BRAINCOLOR	eColor = { BRAINCOLOR_END };

		_uint		iBP = { 0 };
		_uint		iLevel = { 0 };
		_tchar*		szBrainName = {};
		_tchar*		szBrainEx[4] = {};

	}BRAININFO;

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
	void	Brain_Intiialize();
	
	void	OnIcon_Tick();

private:
	vector<class CMain_OnMouseUI*>	m_vecIconUI;
	vector<BRAININFO>	m_vecBrain;
	vector<CUI*>	m_vecLinkUI;

	BRAININFO	m_BrainInfo;

	_uint	m_iIconCount = { 0 };
	_uint	m_iLinkCount = { 0 };
	
	_bool	m_bAcquired = { false };


public:
	static CCanvas_BrainMap* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCanvas* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END