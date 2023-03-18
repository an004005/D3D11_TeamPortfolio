#pragma once
#include "GameObject.h"
#include "Client_Defines.h"
#include "Canvas_Tutorial.h"

BEGIN(Engine)
class CRigidBody;

END

enum USAGE { CREATE, TUTORIAL, USAGE_END};

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

	void	Set_ForTutorial(CCanvas_Tutorial::TUTORIAL eTutorial);
	
private:
	HRESULT SetUp_Components(void * pArg);
	void	SetUp_InitInfo(const Json& json); //LoadFromJson에서 실행

	//Monster만 생성
	void	SetUp_Create(const Json& json);

	void	SetUp_Tutorial(const Json& json);

private:
	CRigidBody*			m_pRigidBodyCom = nullptr;

	//로드때 RigidBoyd에 셋팅해줄 함수를 정하기 위해 셋팅해줌.
	USAGE				m_eUsage = USAGE_END;

	/*For Create*/
	map<wstring, vector<_float4x4>>	m_ProtoWorldMatrixes;
	vector<class CMonster*>			m_pMonsters;
	wstring							m_PotoTag = L"";

	/*For Tutorial*/
	CGameObject* m_pTutorial = { nullptr };
	CCanvas_Tutorial::TUTORIAL		m_eTutorial = { CCanvas_Tutorial::TUTORIAL_END };

public:
	static CTrigger* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg = nullptr) override;
	void Free() override;

};

END