#pragma once
#include "VIBuffer_Instancing.h"
#include "Model_Instancing.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_ModelInstancing : public CVIBuffer_Instancing
{
private:
	CVIBuffer_ModelInstancing(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_ModelInstancing(const CVIBuffer_ModelInstancing& rhs);
	virtual ~CVIBuffer_ModelInstancing() = default;

public:
	virtual HRESULT Initialize_Prototype(HANDLE hFile, CModel_Instancing* pModel, _uint iNumInstance);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta);
	virtual HRESULT Render();
	
public:
	void Add_Instance(_float4x4 WorldMatrixf4x4);
	void Delete_Instance(_uint iIndex);
	void Modify_Matrix(_uint iIndex, _float4x4 WorldMatrixf4x4);
	void Map_Mesh();

public:
	_uint Get_MaterialIndex() const {
		return m_iMaterialIndex;
	}

	void	Set_MaterialIndex(_uint iIndex) {
		m_iMaterialIndex = iIndex;
	}

	const string& GetName() const { return m_strName; }

	const vector<_float4x4> GetWorldMatirxs() {
		return m_WorldMatrix;
	}
private:
	HRESULT Ready_VertexBuffer_NonAnimModel(HANDLE hFile, class CModel_Instancing* pModel);

private:
	string				m_strName;
	/* 이 메시는 m_iMaterialIndex번째 머테리얼을 사용한다. */
	_uint				m_iMaterialIndex = 0;
	vector<_float4x4>		m_WorldMatrix;

public:
	static CVIBuffer_ModelInstancing* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, HANDLE hFile, CModel_Instancing* pModel, _uint iNumInstance);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END

