#pragma once

#include "Shader.h"
#include "Texture.h"
#include "Renderer.h"
#include "Transform.h"
#include "VIBuffer_Rect.h"
#include "VIBuffer_Terrain.h"
#include "VIBuffer_Mesh_Instance.h"
#include "Model.h"

/* �پ��� ������Ʈ(����)���� ��� �ִ´�. -> �纻������Ʈ���� ����ؾ��� ��ü���� �����Ѵ�. */
/* �纻�� �����Ͽ� �����Ѵ�. */
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

	/* �������� ���� ������Ʈ(�ؽ���, �޽ø� etc )���� �����ϰڴ�. */
	using PROTOTYPES = wcmap<class CComponent*>;
	vector<PROTOTYPES> m_pPrototypes;

private:
	CComponent* Find_Prototype(_uint iLevelIndex, const _tchar* pPrototypeTag);
	CComponent* Handle_FindPrototypeFailed(_uint iLevelIndex, const _tchar* pPrototypeTag);

public:
	virtual void Free() override;
};

END