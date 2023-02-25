#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

//�� ����, �ҷ�����, ������Ʈ ����
//Map Editor ����(�׷��Ÿ� �� �����͸� ������..)

class CScarletMap : public CGameObject
{
private:
	CScarletMap(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CScarletMap(const CScarletMap& rhs);
	virtual ~CScarletMap() = default;

public:
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void* pArg) override;
	void BeginTick() override;
	void Tick(_double TimeDelta) override;
	void Late_Tick(_double TimeDelta) override;
	HRESULT Render() override;
	void Imgui_RenderProperty() override;
	virtual void SaveToJson(OUT Json& json) override;
	virtual void LoadFromJson(const Json& json) override;

private:
	HRESULT SetUp_Components();
	_float4	SetUp_InitPosition();

private:
	vector<wstring>	m_pModelProtos;

	wstring								m_pModelProtoTag;
	CGameObject* 						m_pGameObject = nullptr;
public:
	static CScarletMap* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg = nullptr) override;
	void Free() override;
};

