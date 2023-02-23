#pragma once

#include "Base.h"

/* 게임내에 필요한 객체들을 내 기준(CLayer)에 따라 나누어 보관한다. */
/* 객체들을 나누어 저장하고 있는 CLayer들을 보관하는 클래스이다. */
/* 모든 객체들의 갱신(Tick, Late_Tick)을 담당한다. */

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
	// 모든 원본 Object를 Imgui로 출력한다.
	// 선택한 원본의 이름을 szSelectedProto로 돌려준다.
	void Imgui_ProtoViewer(OUT const _tchar*& szSelectedProto);
	
	// iLevel에 있는 모든 사본 Object를 Layer별로 Imgui로 출력한다.
	// 선택한 Object는 pSelectedObject로 돌려준다.
	// 해당 Object가 없거나 사라지면 nullptr로 돌려준다.
	void Imgui_ObjectViewer(_uint iLevel, OUT CGameObject*& pSelectedObject);

	// iLevel에 있는 Layer의 이름을 ImGui::Combo로 렌더한다.
	void Imgui_LayerCombo(_uint iLevel, OUT const _tchar*& pLayerName);

private: /* 원형객체들을ㅇ 모아놓는다. */

	typedef wcmap<class CGameObject*>	PROTOTYPES;
	vector<PROTOTYPES> m_vecPrototypes;

private: /* 사본객체들을 보관하기위한 컨테이너. */
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