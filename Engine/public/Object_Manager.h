#pragma once

#include "Base.h"

/* ���ӳ��� �ʿ��� ��ü���� �� ����(CLayer)�� ���� ������ �����Ѵ�. */
/* ��ü���� ������ �����ϰ� �ִ� CLayer���� �����ϴ� Ŭ�����̴�. */
/* ��� ��ü���� ����(Tick, Late_Tick)�� ����Ѵ�. */

BEGIN(Engine)
class CObject_Manager final : public CBase
{
	DECLARE_SINGLETON(CObject_Manager);
private:
	CObject_Manager();
	virtual ~CObject_Manager() = default;

public:
	HRESULT Reserve_Manager(_uint iNumLevels);
	HRESULT Clear(_uint iLevelIndex);
public:
	HRESULT Add_Prototype(_uint iLevelIndex, const _tchar* pPrototypeTag, class CGameObject* pPrototype);
	HRESULT Clone_GameObject(_uint iLevelIndex, const _tchar* pLayerTag, const _tchar* pPrototypeTag, void* pArg = nullptr);
	CGameObject* Clone_GameObject_Get(_uint iLevelIndex, const _tchar* pLayerTag, const _tchar* pPrototypeTag, void* pArg = nullptr);
	CGameObject* Clone_GameObject_NoLayer(_uint iLevelIndex, const _tchar* pPrototypeTag, void* pArg = nullptr);
	wcmap<class CLayer*>& GetLayers(_uint iLevelIndex);
	class CLayer* GetLayer(_uint iLevelIndex, const _tchar* pLayerTag);
	CGameObject* Find_ObjectByPredicator(_uint iLevelIndex, std::function<_bool(CGameObject*)> Pred);
	list<CGameObject*> Find_AllObjectByPredicator(_uint iLevelIndex, std::function<_bool(CGameObject*)> Pred);
	class CGameObject* Find_Prototype(_uint iLevelIndex, const _tchar* pPrototypeTag);

	void Tick(_double TimeDelta);
	void Late_Tick(_double TimeDelta);

	void SetUpdatedLevel(_uint iLevel) { m_iUpdatedLevel = iLevel; }

public: /* imgui */
	// ��� ���� Object�� Imgui�� ����Ѵ�.
	// ������ ������ �̸��� szSelectedProto�� �����ش�.
	void Imgui_ProtoViewer(OUT const _tchar*& szSelectedProto);
	
	// iLevel�� �ִ� ��� �纻 Object�� Layer���� Imgui�� ����Ѵ�.
	// ������ Object�� pSelectedObject�� �����ش�.
	// �ش� Object�� ���ų� ������� nullptr�� �����ش�.
	void Imgui_ObjectViewer(_uint iLevel, OUT CGameObject*& pSelectedObject);

	// iLevel�� �ִ� Layer�� �̸��� ImGui::Combo�� �����Ѵ�.
	void Imgui_LayerCombo(_uint iLevel, OUT const _tchar*& pLayerName);

private: /* ������ü������ ��Ƴ��´�. */

	typedef wcmap<class CGameObject*>	PROTOTYPES;
	vector<PROTOTYPES> m_vecPrototypes;

private: /* �纻��ü���� �����ϱ����� �����̳�. */
	typedef wcmap<class CLayer*>	LAYERS;
	vector<LAYERS> m_vecLayers;

	_uint m_iNumLevels = 0;
	_uint m_iUpdatedLevel = 0;

	vector<list<CGameObject*>> m_vecBeginTicks;

private:
	class CLayer* Find_Layer(_uint iLevelIndex, const _tchar* pLayerTag);

public:
	virtual void Free() override;
};

END