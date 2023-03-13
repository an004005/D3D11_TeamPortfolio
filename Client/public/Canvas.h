#pragma once

#include "Client_Defines.h"
#include "UI.h"
#include "Timeline.h"

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
	virtual void	BeginTick() override;
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

	map<wstring, CUI*> Get_Map() {	// ���� ����
		return m_mapChildUIs;
	}

protected:
	void	UIMove_FSM();
	void	UIHit(const _double & TimeDelta);

protected:
	class CPlayer* m_pPlayer = { nullptr };
	CSimpleTimeline m_Timeline;

	map<wstring, CUI*> m_mapChildUIs;

	CUI*	m_pUI = { nullptr };
	CFSMComponent*	m_pUIMoveFSM = { nullptr };			// UIMove_FSM() -> ���������� �����ߴٰ� ���� �������� ���ư��� ���� �Լ�
	
protected:
	_float2			m_vMaxDestination = { 0.0f, 0.0f };	// �� ĵ���� ���� �ٸ� �ִ� ��������

private:
	_float			m_fRandomDestination = { 0.0f };
	_bool			m_bUIMove = { false };				// SASSkill_UIMove() -> UI�� �̵���Ű���� �� �� �ܺο��� Set �� �Ѵ�.
	_bool			m_bIsDestination = { false };		// ��ǥ������ ����! �������� ���� ���ư���
	_float2			m_vDestination = { 0.0f, 0.0f };	// ��������

public:
	static CCanvas* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUI* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END