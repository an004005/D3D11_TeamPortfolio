#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CPxModel;
class CRenderer;
class CModel;
class CAnimation;
END

BEGIN(Client)

enum
{
	CABLE_CNT = 5
};

class CSAS_Cable : public CGameObject
{
private:
	CSAS_Cable(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSAS_Cable(const CSAS_Cable& rhs);
	virtual ~CSAS_Cable() = default;

public:
	HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	virtual void Imgui_RenderProperty() override;

public:
	void SetTargetInfo(CTransform* pTargetTransform, CModel* pTargetModel);
	void EquipCable(ESASType eType);
	void UnEquipCable();

private:
	_matrix CalcSocketMatrix(_int iIdx, _fmatrix TargetWorldMatrix);


private:
	array<CPxModel*, CABLE_CNT> m_CableModels{};
	array<_float4x4, CABLE_CNT> m_CablePivots{};
	CAnimation* m_pWatchAnim = nullptr;

	CTransform* m_pTargetTransform = nullptr;
	CModel*		m_pTargetModel = nullptr;

	CRenderer* m_pRendererCom = nullptr;

	_bool m_bActive = false;

	string socket;


public:
	static CSAS_Cable* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg = nullptr) override;
	void Free() override;
};

END