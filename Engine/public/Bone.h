#pragma once

#include "Base.h"

BEGIN(Engine)

class CBone final : public CBase
{
private:
	CBone();
	virtual ~CBone() = default;

public:
	const char* Get_Name() const {
		return m_strName.c_str();
	}

	_matrix Get_OffsetMatrix() {
		return XMLoadFloat4x4(&m_OffsetMatrix);
	}

	_matrix Get_CombindMatrix() {
		return XMLoadFloat4x4(&m_CombindTransformMatrix);
	}

	CBone* GetParent() { return m_pParent; }

	void Set_OffsetMatrix(_float4x4 OffsetMatrix) {
		m_OffsetMatrix = OffsetMatrix;
	}

	void Set_TransformMatrix(_fmatrix TransformMatrix) {
		XMStoreFloat4x4(&m_TransformMatrix, TransformMatrix);
	}

	_float4x4 Get_TransformMatrix() const { return m_TransformMatrix; }

	void AddChildBone(CBone* pBone);

	void SetDisableAnimation(_bool bDisableAnim) { m_bDisableAnim = bDisableAnim; }
	_bool IsDisableAnim() const { return m_bDisableAnim;  }
	void DisableToParentRecursive(_bool bDisable);
	void DisableToChildRecursive(_bool bDisable);

public:
	HRESULT Initialize(const Json& jBone, CBone* pParent);
	void Compute_CombindTransformationMatrix_StartRoot();
	void Compute_CombindTransformationMatrix();

	void Imgui_SkeletalViewRecursive(string& strSelected, ImGuiTreeNodeFlags baseFlags);

	void MakeDFSTravelVector(OUT vector<CBone*>& vectorOut);

private:


private:
	string				m_strName;
	_float4x4			m_OffsetMatrix;
	_float4x4			m_TransformMatrix;
	_float4x4			m_CombindTransformMatrix;
	CBone*				m_pParent = nullptr;
	vector<CBone*>		m_Children;
	vector<string>		m_ChildrenName;

	bool				m_bDisableAnim = false;

public:
	static CBone* Create(const Json& jBone, CBone* pParent);
	virtual void Free() override;
};

END