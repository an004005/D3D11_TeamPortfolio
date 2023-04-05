#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CRenderer;
class CModel;
END

BEGIN(Client)

class CEMBrain : public CGameObject
{
private:
	CEMBrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEMBrain(const CEMBrain& rhs);
	virtual ~CEMBrain() = default;

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void BeginTick() override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	HRESULT	Render() override;
	virtual void Imgui_RenderProperty() override;
	virtual HRESULT Render_ShadowDepth() override;

private:
	HRESULT	SetUpComponents();

private:
	CModel* m_pModelCom = nullptr;
	CRenderer* m_pRendererCom = nullptr;

public:
	static CEMBrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END
