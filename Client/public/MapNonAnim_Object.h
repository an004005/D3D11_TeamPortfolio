#pragma once
#include "MapObject.h"

BEGIN(Client)

class CMapNonAnim_Object : public CMapObject
{
private:
	CMapNonAnim_Object(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMapNonAnim_Object(const CMapNonAnim_Object& rhs);
	virtual ~CMapNonAnim_Object() = default;

public:
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void* pArg) override;
	void BeginTick() override;
	void Tick(_double TimeDelta) override;
	void Late_Tick(_double TimeDelta) override;
	HRESULT Render() override;

private:
	HRESULT	SetUp_Components();

public:
	static CMapNonAnim_Object* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg = nullptr) override;
	void Free() override;
};

END