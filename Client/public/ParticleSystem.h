#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "Shader.h"


BEGIN(Engine)
class CVIBuffer_Point_Instancing;
class CRenderer;
END

BEGIN(Client)

enum class ESpawnShape
{
	SPHERE,
	CONE,
};

enum class EPointType
{
	TEXTURE,
	FLIPBOOK
};

class CParticleSystem :	public CGameObject
{
public:
	CParticleSystem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CParticleSystem(const CParticleSystem& rhs);
	virtual ~CParticleSystem() = default;

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	virtual void SaveToJson(Json& json) override;
	virtual void LoadFromJson(const Json& json) override;
	virtual void Imgui_RenderProperty() override;
	void SetLoop(_bool bLoop) { m_bLoop = bLoop; }

private:
	void AddPoint();
	void UpdatePoints(_float fTimeDelta);

private:
	ShaderParams m_tParam;
	CRenderer* m_pRendererCom = nullptr;
	CShader* m_pShader = nullptr;
	_uint m_iPassIdx = 0;
	CVIBuffer_Point_Instancing* m_pBuffer = nullptr;

	_bool m_bLocal = true;
	_uint m_iInstanceNum = 50;

	_float m_fDuration = 5.f;
	_bool m_bLoop = true;

	_float m_fSpawnTickTime = 0.1f;
	_float m_fCurSpawnTick = 0.f;
	_int m_iBurstCnt = 1;

	_float2 m_fLifeMinMax{ 1.f, 2.f };
	_bool m_bLifeRand = false;
	_float m_fLife = 1.f;

	_float2 m_fSpeedMinMax{ 1.f, 2.f };
	_float m_fSpeed = 1.f;
	_bool m_bSpeedRand = false;

	_float2 m_fSize{ 0.1f, 0.1f };

	ESpawnShape m_eShape = ESpawnShape::SPHERE;
	_float m_fConeAngleDeg = 30.f;
	_float m_fConeOriginRadius = 1.f;
	_float m_fSphereRadius = 4.f;

	_bool m_bFromOrigin = true; // false搁 官款爹 酒公单辑 积己
	_bool m_bGravity = false;
	_bool m_bSizeDecreaseByLife = false;
	_bool m_bSizeIncreaseByLife = false;
	_float m_fIncreaseMaxSize = 1.f;


	list<VTXMATRIX> m_Points;

private:
	// For Gravity

	// _bool	m_bGravity;
	_float	m_fJumpPower;
	_float	m_fDownSpeed;



public:
	static CParticleSystem* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END
