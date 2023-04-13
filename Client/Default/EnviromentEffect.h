#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Client)

class CEnviromentEffect : public CGameObject
{
private:
	CEnviromentEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEnviromentEffect(const CEnviromentEffect& rhs);
	virtual ~CEnviromentEffect() = default;

public:
	HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void BeginTick() override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	virtual void Imgui_RenderProperty() override;


public:
	virtual void Free() override;
	static CEnviromentEffect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
};

END