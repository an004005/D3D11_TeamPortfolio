#pragma once
#include "Client_Defines.h"
#include "ImguiObject.h"
#include "ScarletCharacter.h"
#include "HelperClasses.h"

BEGIN(Client)

class CShop_NPC : public CScarletCharacter
{
private:
	CShop_NPC(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CShop_NPC(const CShop_NPC& rhs);
	virtual ~CShop_NPC() = default;

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
	HRESULT SetUp_Components(void* pArg);
	HRESULT SetUp_Event();
	HRESULT SetUp_Sound();

private:
	void	Update_Animation(_double TimeDelta);

private:
	_double	m_fTimeDelta = 0.f;
	
private:
	CRenderer*  m_pRenderer = nullptr;
	CModel*		m_pModel = nullptr;

private:
	_float		m_fDistance_toPlayer = 0.f;
	_float4		m_vPlayerPos = { 0.f, 0.f, 0.f, 1.f };

	wstring		m_ModelName;
	_bool		m_iCheck = false;

	_bool		m_bLoop = false;
	_float		m_fTimeAcc = 0.f;

	_float      m_fLerpTime = 0.f;
	_float      m_fDuration = 0.2f;

	class CCanvas_MainTalk* m_pCanvas_MainTalk = { nullptr };

private:
	_bool		DistanceCheck();

public:
	static CShop_NPC* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END