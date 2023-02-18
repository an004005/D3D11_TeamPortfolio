#pragma once
#include "ImguiObject.h"
#include "Client_Defines.h"
#include "GameInstance.h"
#include "Cell.h"

BEGIN(Engine)
class CNavigation;
END

BEGIN(Client)

class CFloors;

class CImgui_MapEditor final : public CImguiObject
{
	enum EMapEditorMode
	{
		FLOOR_MODE, // floor ���� �� ����
		CELL_CREATE, // ��3�� �� �� ����
		CELL_SELECT, // ���� Ÿ�� ���� �� ����
		CELL_POINT_FIX, // ���� �� ����
		COLLIDER,
		PROPERTY_VIEW,
	};
private:
	CImgui_MapEditor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

public:
	virtual HRESULT Initialize(void* pArg) override;

	virtual void Imgui_RenderTab() override;
	virtual void Imgui_RenderWindow() override;
	static CImgui_MapEditor* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg = nullptr);
	static void SaveLevel(const string& strPath);
	static void LoadLevel(const string& strPath);


private:
	_bool Adjust_CellPoints();

private:
	CGameInstance* m_pGameInstance = nullptr;
	CFloors*		   m_pFloor = nullptr;
	CNavigation*   m_pNavigation = nullptr;

	EMapEditorMode m_eMode = FLOOR_MODE;

	vector<_float3> m_CellPointsStore;
	array<class CIndicator*, 3> m_Indicators{};

	CCell* m_pSelectCell = nullptr;

	list<pair<CCell*, CCell::POINT>> m_CellPointPairs;

	// collider
	class CStaticColliders* m_pColls = nullptr;





	CGameObject* m_pSelectedObject = nullptr;

	const _tchar* m_pSelectedProto = nullptr;
	char m_szLayerName[MAX_PATH]{};
	const _tchar* m_pLayerName = nullptr;

public:
	virtual void Free() override;

};

END