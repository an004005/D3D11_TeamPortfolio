#pragma once
#include "Model.h"

BEGIN(Engine)
class CAnimation;

class ENGINE_DLL CPxModel : public CModel
{
private:
	CPxModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPxModel(const CPxModel& rhs);
	virtual ~CPxModel() = default;

public:
	virtual HRESULT Initialize_Prototype(const char* pModelFilePath) override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Imgui_RenderProperty() override;
	void Update_Tick(_double TimeDelta, const _float4x4& WorldMatrix);
	void Render_PxModel(_fmatrix WorldMatrix);

	void ActivatePhysX(_bool bActive);
	_bool IsActivePhysX() const { return m_bActivatePhysX; }

private:
	void CreateJoints();
	void ReleaseJoints();


private:
	_bool m_bActivatePhysX = false;
	class CPxBone* m_pStartBone = nullptr; // root bone
	_float4x4 m_ScalePivot;
	_float4x4 m_WorldMatrix;


public:
	static CPxModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const char* pModelFilePath, _float4x4 PivotMatrix = s_DefaultPivot);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END