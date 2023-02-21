#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CModel;
class CRenderer;
class CShader;
END

BEGIN(Client)

enum class MAPOBJ_TYPE{ TYPE_END};

class CMapObject abstract : public CGameObject
{
protected:
	CMapObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMapObject(const CMapObject& rhs);
	virtual ~CMapObject() = default;

public:
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void* pArg) override;
	void BeginTick() override;
	void Tick(_double TimeDelta) override;
	void Late_Tick(_double TimeDelta) override;
	HRESULT Render() override;

private:
	HRESULT	SetUp_Components();

protected:
	CModel*					m_pModelCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	class CPsycokinesis*	m_pPsycokinesisCom = nullptr;

protected:
	MAPOBJ_TYPE				m_eType = MAPOBJ_TYPE::TYPE_END;
	const _tchar*			m_pModelTag = nullptr;
	_bool					m_OnStep = false;


public:
	CGameObject* Clone(void* pArg = nullptr) override { return nullptr; }
	void Free() override;

};

END