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
		BRAINTYPE	eType = { BRAINTYPE::BRAINTYPE_END };
		BRAINCOLOR	eColor = { BRAINCOLOR::BRAINCOLOR_END };

		_uint		iBP = { 0 };
		_uint		iLevel = { 0 };
		_tchar*	szBrainName = {};
		_tchar*	szBrainEx[4] = {};
		_bool		bUse = { false };	
		_float2		vOnIconIndex = { 0.0f, 0.0f };
		_float2		vOffIconIndex = { 0.0f, 0.0f };
		
		array<_int, 3>	arrNeighbor;

	}BRAININFO;

	typedef struct tagLickInfo
	{
		_bool	bSLink = { false };
		size_t	iIconIndex0 = { 0 };
		size_t	iIconIndex1 = { 0 };
		size_t	iLickIndex = { 0 };

	}LINKINFO;


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

public:
	// 35 : 공중 추가 공격 
	// 36 : 추가 공격 확장 2
	// 53, 57 : 브레인 필드: 데미지 경감
	_bool	Get_IconUse(const size_t iIndex) {
		return m_vecBrain[iIndex].bUse;
	}

private:
	void	Brain_Intiialize();
	void	ChildUI_Intiialize(); // 이미 맵에 저장된 UI들 이긴 하지만, 각각 따로 돌려야 하는 경우가 많아서 필요한 Icon 그리고 Lick 끼리 모아두기 위해서
	void	Link_Initialize();

	void	OnIcon_Tick();
	void	IconPick(const size_t iIndex);
	void	SkillAcquisition_Tick();	// 계속 알림창이 뜰 때를 계속 확인한다.
	void	OnLick_Tick();

private:
	vector<BRAININFO>	m_vecBrain;
	vector<LINKINFO>	m_vecOnLinkInfo;
	vector<class CMain_OnMouseUI*>	m_vecIconUI;

	BRAININFO	m_CurrentBrainInfo;

	_uint	m_iIconCount = { 0 };
	_uint	m_iLinkCount = { 0 };
	_uint	m_iLinkSCount = { 0 };

	size_t	m_iCurrentIndex = { 0 };
	
	_bool	m_bSkillAcquisition = { false }; // BP가 부족해서 뜨는 알림창
	array<_uint, BRAINTYPE_END>	m_arrCurrentHighLevel;

	_tchar*	m_szAlarmText = L"";

	_int	m_iStartPriority = { 0 };

public:
	static CCanvas_BrainMap* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCanvas* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END