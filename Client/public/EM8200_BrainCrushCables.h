#pragma once
#include "GameObject.h"
#include "Client_Defines.h"
#include "HelperClasses.h"

BEGIN(Engine)
class CModel;
class CPxModel;
class CAnimation;
class CFSMComponent;
class CRenderer;
class CCurveFloatImpl;
class CMaterial;
END

BEGIN(Client)

class CEM8200_BrainCrushCables : public CGameObject
{
private:
	enum EBrainFieldCable
	{
		HEAD_1, // 머리 아래
		HEAD_2, // 머리 오른쪽 위
		HEAD_3, // 머리 왼쪽 위
		BODY_1,
		BODY_2,
		BODY_3,
		BODY_4,
		BODY_5,
		BODY_6,
		BODY_7,
		BODY_8,
		BODY_9,
		BODY_10,
		BODY_11,
		BODY_12,
		CABLE_END
	};

private:
	CEM8200_BrainCrushCables(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEM8200_BrainCrushCables(const CEM8200_BrainCrushCables& rhs);
	virtual ~CEM8200_BrainCrushCables() = default;

public:
	HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	virtual void Imgui_RenderProperty() override;

public:
	void Activate(_bool bActive);
public:
	void SetTargetInfo(CTransform* pTargetTransform, CModel* pTargetModel);

	_float GetHeadPlayRatio();

private:
	void SetUpFSM();

private:
	array<CPxModel*, CABLE_END> m_CableModels{};
	array<_float4x4, CABLE_END> m_CablePivots{};
	array<string, CABLE_END> m_CableBones{};

	CFSMComponent* m_pFSM = nullptr;

	_bool m_bActive = false;

	CDoOnce m_PhysxHeadOnce;
	CDoOnce m_PhysxBodyOnce;
	_float m_fPhysXDelay = 1.f;


	CTransform* m_pTargetTransform = nullptr;
	CModel*		m_pTargetModel = nullptr;
	CRenderer* m_pRendererCom = nullptr;

public:
	virtual void Free() override;
	static CEM8200_BrainCrushCables* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg = nullptr) override;
};

END