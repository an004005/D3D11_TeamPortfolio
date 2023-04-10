#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CShader;
class CModel;
END

BEGIN(Client)

class CConsumption_Item : public CGameObject
{
public:
	enum ITEMTYPE { SOLO_SMALL, SOLO_MID, SOLO_BIG, PARTY_SMALL, PARTY_MID, PARTY_BIG, SAS_FULL, ITEMTYPE_END };

private:
	CConsumption_Item(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CConsumption_Item(const CConsumption_Item& rhs);
	virtual ~CConsumption_Item() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void BeginTick();
	virtual void Tick(_double TimeDelta);
	virtual void Late_Tick(_double TimeDelta);
	virtual void AfterPhysX() override;
	virtual HRESULT Render();
	virtual HRESULT Render_ShadowDepth() override;

public:
	virtual void Imgui_RenderProperty() override;

private:
	void	Update_Animation(_double TimeDelta);
	void	PlayerOverlapCheck();

private:
	CRenderer* m_pRenderer = nullptr;
	CModel* m_pModel = nullptr;

private:
	HRESULT SetUp_Components(void* pArg);

private:
	_float      m_fLerpTime = 0.f;
	_float      m_fDuration = 0.2f;

	_bool		m_bOverlapCheck = false;

	// 이번에 획득한 아이템이 무엇인가? Item Infomation
	vector<wstring>		m_vecItemName;
	wstring				m_strName = L"";
	ITEMTYPE			m_eItemType = SOLO_SMALL;

public:
	static CConsumption_Item* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END