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
		_bool		bUse = { false };		// �� ���� Icon ���� ���� ����
		_bool		bLink = { false };
		_float2		vOnIconIndex = { 0.0f, 0.0f };
		_float2		vOffIconIndex = { 0.0f, 0.0f };

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
	void	ChildUI_Intiialize(); // �̹� �ʿ� ����� UI�� �̱� ������, ���� ���� ������ �ϴ� ��찡 ���Ƽ� �ʿ��� Icon �׸��� Lick ���� ��Ƶα� ���ؼ�

	void	OnIcon_Tick();
	void	IconPick(const size_t iIndex);
	void	IconLevel(const size_t iIndex);
	void	SkillAcquisition_Tick(const _double & TimeDelta);	// ��� �˸�â�� �� ���� ��� Ȯ���Ѵ�.

private:
	vector<BRAININFO>	m_vecBrain;
	vector<class CMain_OnMouseUI*>	m_vecIconUI;
	vector<CUI*>	m_vecLinkUI;

	BRAININFO	m_CurrentBrainInfo;

	_uint	m_iIconCount = { 0 };
	_uint	m_iLinkCount = { 0 };

	size_t	m_iCurrentIndex = { 0 };
	
	_bool	m_bSkillAcquisition = { false }; // BP�� �����ؼ� �ߴ� �˸�â
	array<_uint, BRAINTYPE_END>	m_arrCurrentLevel;

	_tchar*	m_szAlarmText = L"";

public:
	static CCanvas_BrainMap* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCanvas* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END