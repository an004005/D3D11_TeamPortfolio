#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "ParticleGroup.h"
#include "EffectGroup.h"

BEGIN(Client)

class CEMCable : public CGameObject
{
private:
	CEMCable(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEMCable(const CEMCable& rhs);
	virtual ~CEMCable() = default;

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void BeginTick() override;
	virtual void Tick(_double TimeDelta) override;

	void Set_Dest(_float4 vDest) {
		m_Dest = vDest;
	}
private:
	CEffectGroup* m_pCableEff = nullptr;

private:
	_float4			m_Dest;
	_float4			m_Depart;

	_bool			m_bReverse = false;
	_double		m_Delay = 0.0;
public:
	static CEMCable* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END