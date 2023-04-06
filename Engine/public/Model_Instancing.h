#pragma once

#include "Component.h"
#include "Model.h"

BEGIN(Engine)

class ENGINE_DLL CModel_Instancing final : public CComponent
{
private:
	CModel_Instancing(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CModel_Instancing(const CModel_Instancing& rhs);
	virtual ~CModel_Instancing() = default;

public:
	_float4x4 GetPivotMatrix() const { return m_PivotMatrix; }
	void SetPivot(_float4x4 Pivot) { m_PivotMatrix = Pivot; }
	void Tick(_double TimeDelta);

	//Mesh Functions
	void	Add_Instance(_float4x4 WorldMatirxf4x4);
	void	Modify_Matrix(_uint iIndex, _float4x4 WorldMatirxf4x4);
	void	Delete_Instance(_uint iIndex);
	void	Map_Meshs();
	_uint	Get_NumMeshes() const { return static_cast<_uint>(m_Meshes.size()); }
	vector<class CVIBuffer_ModelInstancing*>& GetMeshes() { return m_Meshes; }

public:
	virtual HRESULT Initialize_Prototype(const char* pModelFilePath, _uint iNumInstance);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Imgui_RenderProperty() override;

public:
	HRESULT Render(class CTransform* pTransform);
	HRESULT Render(const _float4x4& WorldMatrix);
	HRESULT RenderMesh(class CTransform* pTransform, _uint iMeshIdx);
	HRESULT RenderMeshOnly(_uint iMeshIdx) const;
	HRESULT Render_MeshAlphBlend(_float4x4 WorldMatrix, _uint iMeshIdx);

	HRESULT Render_ShadowDepth(class CTransform* pTransform);

	const vector<class CMaterial*>& GetMaterials() { return m_Materials; }
	class CMaterial* FindMaterial(const _tchar* pMtrlProtoTag);

private:
	HRESULT Ready_Materials(HANDLE hFile);

private:
	static const _float4x4 s_DefaultPivot;
	static const string s_ModifyFilePath;

private:
	string								m_strName;
	_float4x4							m_PivotMatrix;

	/* 하나의 모델은 교체가 가능한 여러개의 메시로 구성되어있다. */
	vector<class CVIBuffer_ModelInstancing*>	m_Meshes;
	vector<class CMaterial*>					m_Materials;

	class CShader* m_pShadowShader = nullptr;

public:
	static CModel_Instancing* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const char* pModelFilePath, _uint iNumInstance = 300, _float4x4 PivotMatrix = s_DefaultPivot);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END