#pragma once

#include "VIBuffer.h"
#include "Model.h"

/* ���� ��ü������ �� ����. */
/* �� �޽ø� �׷��������� ����, �ε��� ���۸� �����Ѵ�. */
BEGIN(Engine)

class CMesh final : public CVIBuffer
{
private:
	CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMesh(const CMesh& rhs);
	virtual ~CMesh() = default;

public:
	// For ParticleSystem
	VTXMODEL* Get_NonAnimBuffer()
	{
		return m_pNonAnimModelBufferData;
	}

	VTXANIMMODEL* Get_AnimBuffer()
	{
		return m_pAnimModelBufferData;
	}
	// ~For ParticleSystem



	_uint Get_MaterialIndex() const {
		return m_iMaterialIndex;
	}

	void	Set_MaterialIndex(_uint iIndex) {
		m_iMaterialIndex = iIndex;
	}

public:
	virtual HRESULT Initialize_Prototype(CModel::TYPE eType, HANDLE hFile, class CModel* pModel);

public:
	void SetUp_MeshBones(class CModel* pModel);
	void SetUp_BoneMatrices(_float4x4* pBoneMatrices, _fmatrix PivotMatrix);

	const string& GetName() const { return m_strName; }

private:
	CModel::TYPE		m_eType;
	string				m_strName;
	/* �� �޽ô� m_iMaterialIndex��° ���׸����� ����Ѵ�. */
	_uint				m_iMaterialIndex = 0;

	/* �� �޽��� �����鿡�� ������ �ִ� ���� ����. */
	vector<string>			m_BoneNames;

	// ���� ���� ����
	vector<class CBone*>	m_Bones;


private:
	VTXMODEL* m_pNonAnimModelBufferData = nullptr;
	VTXANIMMODEL* m_pAnimModelBufferData = nullptr;

private:
	HRESULT Ready_VertexBuffer_NonAnimModel(HANDLE hFile, class CModel* pModel);
	HRESULT Ready_VertexBuffer_AnimModel(HANDLE hFile, class CModel* pModel);

public:
	static CMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CModel::TYPE eType, HANDLE hFile, class CModel* pModel);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free();
};

END