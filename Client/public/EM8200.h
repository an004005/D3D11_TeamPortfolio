#pragma once
#include "Client_Defines.h"
#include "Enemy.h"
#include "Controller.h"

class CEM8200 :   public CEnemy
{
private:
	CEM8200(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEM8200(const CEM8200& rhs);
	virtual ~CEM8200() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_OutlineFlag() override;
	virtual void	SaveToJson(Json& json) override;
	virtual void	LoadFromJson(const Json& json) override;
	virtual void	Imgui_RenderComponentProperties() override;
	virtual void	SetDelete() override;

public:
	virtual void		AfterPhysX() override;
	virtual void		Reset() override;
	virtual HRESULT		Initialize(void* pArg) override;
	virtual void		BeginTick() override;
	virtual void		Tick(_double TimeDelta) override;
	virtual void		Late_Tick(_double TimeDelta) override;
	virtual void		Imgui_RenderProperty() override;
	virtual HRESULT		Render_ShadowDepth() override;
	virtual void		SetUpComponents(void* pArg) override;
	virtual void		SetUpSound() override;
	virtual void		SetUpAnimationEvent() override;
	virtual void		SetUpFSM() override;
	virtual void		SetUpUI() override;
	virtual void		TakeDamage(DAMAGE_PARAM tDamageParams) override;
	virtual void		SetEnemyBatchDataStat(ENEMY_STAT tStat) override;
	virtual ENEMY_STAT	GetEnemyBatchDataStat() override;
	virtual _float4		GetKineticTargetPos() override;
	virtual void		PlayBC() override;
	virtual _float4x4	GetBoneMatrix(const string& strBoneName, _bool bPivotapply) override;
	virtual _float4x4	GetPivotMatrix() override;
	virtual _bool		Exclude() override;

protected:
	virtual void		HitEffect(DAMAGE_PARAM& tDamageParams) override;
	virtual void		CheckDeBuff(EDeBuffType eDeBuff) override;
	virtual _bool		IsWeak(CRigidBody* pHitPart) override;
	virtual void		CheckCrushGage(DAMAGE_PARAM& tDamageParams) override;
	virtual void		CheckHP(DAMAGE_PARAM& tDamageParams) override;
	virtual _bool		CheckSASType(ESASType eSASType) override;
	virtual void		SetDead() override;
	virtual void		FindTarget() override;
	virtual void		Update_DeadDissolve(_double TimeDelta) override;
	virtual void		Update_DeBuff(_double TimeDelta) override;
	virtual void		DeBuff_End() override;
	virtual void		DeBuff_Fire() override;
	virtual void		DeBuff_Oil() override;
	virtual void		MoveJsonData(Json& jsonDest, void* pArg) override;

public:
	static CEM8200*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg = nullptr) override;
	virtual void			Free() override;
};

