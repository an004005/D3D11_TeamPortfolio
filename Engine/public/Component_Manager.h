#pragma once

#include "Shader.h"
#include "Texture.h"
#include "Renderer.h"
#include "Transform.h"
#include "VIBuffer_Rect.h"
#include "VIBuffer_Terrain.h"
#include "VIBuffer_Mesh_Instance.h"
#include "Model.h"

/* 다양한 컴포넌트(원형)들을 담고 있는다. -> 사본컴포넌트들은 사용해야할 객체들이 보유한다. */
/* 사본을 생성하여 리턴한다. */
BEGIN(Engine)

class CComponent_Manager final : public CBase
{
	DECLARE_SINGLETON(CComponent_Manager)
private:
	CComponent_Manager();
	virtual ~CComponent_Manager() = default;

public:
	HRESULT Reserve_Manager(_uint iNumLevels);
	HRESULT Clear(_uint iLevelIndex);
	HRESULT Add_Prototype(_uint iLevelIndex, const _tchar* pPrototypeTag, class CComponent* pPrototype);
	class CComponent* Clone_Component(_uint iLevelIndex, const _tchar* pPrototypeTag, void* pArg = nullptr);

	wcmap<class CComponent*>* GetProtoTypes(_uint iLevelIndex);

private:
	_uint											m_iNumLevels = 0;

	/* 레벨별로 원형 컴포넌트(텍스쳐, 메시모델 etc )들을 보관하겠다. */
	using PROTOTYPES = wcmap<class CComponent*>;
	vector<PROTOTYPES> m_pPrototypes;

private:
	CComponent* Find_Prototype(_uint iLevelIndex, const _tchar* pPrototypeTag);
	CComponent* Handle_FindPrototypeFailed(_uint iLevelIndex, const _tchar* pPrototypeTag);

public:
	virtual void Free() override;
};

END