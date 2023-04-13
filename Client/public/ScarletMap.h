#pragma once
#include "GameObject.h"
#include "Client_Defines.h"
#include "Level_Maptool.h"

//�� ����, �ҷ�����, ������Ʈ ����
//Map Editor ����(�׷��Ÿ� �� �����͸� ������..)
BEGIN(Client)
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

	static _bool s_bMapEditor;

	void SetVisible_MapObjects(_bool bVisible);
	void SetPos_MapObjects(_float4 vPos);
	vector<class CMapObject*>& GetMapObjects() { return m_pMapObjects; }
	
private:
	HRESULT SetUp_Components();   
	_float4	SetUp_InitPosition();
	void	ClearMap();
	void	CreateMapObjectFromLoad(Json& json);

	//�������� �ν��Ͻ� �ƴ� �ֵ��� �־ ���X
	//��� ���� �ν��Ͻ��̶�� ���� �Ͽ� ���� �Լ�
	void	RayPicking();

private:
	vector<pair<wstring, PROTOINFO>>	m_pModelProtos;
	vector<class CMapObject*>			m_pMapObjects;

	pair<wstring, PROTOINFO>			m_pModelProtoInfo;
	CGameObject* 						m_pGameObject = nullptr;

	_int								m_iProtoInfo = INSTANCE;
	_bool								m_bPick = false;
public:
	static CScarletMap* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg = nullptr) override;
	void Free() override;
};

END

