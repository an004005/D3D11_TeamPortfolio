#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Client)

class CEnvironmentEffect : public CGameObject
{
	enum EEnvironmentEffectType
	{
		Env_Crush_1_1,
		Env_Crush_1_2,
		Env_Spark_Particle_10Sec,
		Boss_WaterFall,
		EndingCredit_Inbok,
		EndingCredit_Jihoon,
		EndingCredit_Jongwook,
		EndingCredit_Junghwan,
		EndingCredit_Kibum,
		EndingCredit_Suhyun,
		EndingCredit_Team,
		EndingCredit_Pic_RedRect,
		EndingCredit_Sound,
		ENVEFFECT_END
	};
private:
	CEnvironmentEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEnvironmentEffect(const CEnvironmentEffect& rhs);
	virtual ~CEnvironmentEffect() = default;

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Imgui_RenderProperty() override;
	virtual void SaveToJson(Json& json) override;
	virtual void LoadFromJson(const Json& json) override;

	void SpawnEffect();

private:
	void Clear();

private:
	EEnvironmentEffectType m_eType = Env_Crush_1_1;
	class CEffectGroup* m_pEffectGroup = nullptr;
	class CParticleGroup* m_pParticleGroup = nullptr;


public:
	virtual void Free() override;
	static CEnvironmentEffect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
};

END