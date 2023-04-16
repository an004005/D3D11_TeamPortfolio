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
	virtual void Late_Tick(_double TimeDelta) override;
	HRESULT	Render() override;
	virtual HRESULT Render_ShadowDepth() override;

public:
	void InitBC();
	void BeginBC();
	void EndBC();

private:
	HRESULT	SetUpComponents();

private:
	CModel* m_pModelCom = nullptr;
	CRenderer* m_pRendererCom = nullptr;

	//처음부터 생성
	class CParticleGroup* m_pDefaultParticle = nullptr;

	//브레인 크러쉬 시작할때 생성.
	class CEffectGroup* m_DistortionEffect = nullptr;
	class CParticleGroup* m_pStartParticle = nullptr;


public:
	static CEMBrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END
