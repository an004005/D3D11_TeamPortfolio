#pragma once
#include "GameObject.h"
#include "Client_Defines.h"


BEGIN(Engine)
class CRigidBody;
END

enum USAGE { CREATE, USAGE_END};

BEGIN(Client)
class CTrigger final :  public CGameObject
{
private:
	CTrigger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTrigger(const CTrigger& rhs);
	virtual ~CTrigger() = default;

public:
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void* pArg) override;
	void BeginTick() override;
	void Tick(_double TimeDelta) override;
	void Late_Tick(_double TimeDelta) override;

	virtual void SaveToJson(OUT Json& json) override;
	virtual void LoadFromJson(const Json& json) override;

	void AfterPhysX() override;
	HRESULT Render() override;
	void Imgui_RenderProperty() override;

public:
	void	Set_ForCreate(const wstring& ProtoTag, const _float4x4 WorldMatrix);
	
private:
	HRESULT SetUp_Components(void * pArg);
	void	SetUp_InitInfo(const Json& json);

	void	SetUp_Create(const Json& json);
private:
	CRigidBody*			m_pRigidBodyCom = nullptr;

	//로드때 RigidBoyd에 셋팅해줄 함수를 정하기 위해 셋팅해줌.
	USAGE				m_eUsage = USAGE_END;

	/*For Create*/
	map<wstring, vector<_float4x4>>	m_ProtoWorldMatrixes;
	wstring							m_PotoTag = L"";

public:
	static CTrigger* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg = nullptr) override;
	void Free() override;

};

END