#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CTexture;
class CRenderer;
class CVIBuffer_Rect;
class CFSMComponent;
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
	void	Set_UIMove() {
		m_bUIMove = true;
	}

	map<wstring, CUI*> Get_Map() {	// 삭제 예정
		return m_mapChildUIs;
	}

protected:
	void	UIMove_FSM();

protected:
	map<wstring, CUI*> m_mapChildUIs;

	CUI*	m_pUI = { nullptr };
	
	CFSMComponent*	m_pUIMoveFSM = { nullptr };				// UIMove_FSM() -> 도착지점에 도달했다가 원래 지점으로 돌아가기 위해 함수

private:
	static _bool	m_bUIMove;							// SASSkill_UIMove() -> UI를 이동시키고자 할 때 외부에서 Set 을 한다.
	_bool			m_bIsDestination = { false };		// 목표지점에 도달! 원점으로 이제 돌아가라
	_float2			m_vDestination = { 0.0f, 0.0f };	// 도착지점

public:
	static CCanvas* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUI* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END