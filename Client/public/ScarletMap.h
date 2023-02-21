#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

//맵 저장, 불러오기 담당

enum COMPONENTS {
	PSYCOKINESIS,
	COMPONENTS_END
};
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
	
public:
	void	LoadMap(const _tchar* pMapPath, LEVEL eLevel);
	void	SaveMap(const _tchar* pMapPath);
	void	SetUp_Prototypes_MapObject(LEVEL eLevel);

private:
	HRESULT SetUp_Components();
	void	Create_MapNonAnim_Object();

private:
	map<const _tchar*, CGameObject*>	m_pMapProtos;
	array<_bool, COMPONENTS_END>		m_eComponents;

	const _tchar*						m_pMapProtoTag = nullptr;

public:
	static CScarletMap* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg = nullptr) override;
	void Free() override;
};

