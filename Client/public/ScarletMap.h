#pragma once
#include "GameObject.h"
#include "Client_Defines.h"
#include "Level_Maptool.h"

//맵 저장, 불러오기, 오브젝트 생성
//Map Editor 역할(그럴거면 맵 에디터를 만들지..)
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

	//마을맵은 인스턴싱 아닌 애들이 있어서 사용X
	//모든 모델이 인스턴싱이라는 가정 하에 만든 함수
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

