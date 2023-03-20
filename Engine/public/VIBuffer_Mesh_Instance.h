#pragma once

#include "VIBuffer_Instancing.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Mesh_Instancing : public CVIBuffer_Instancing
{
public:
	CVIBuffer_Mesh_Instancing(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Mesh_Instancing(const CVIBuffer_Mesh_Instancing& rhs);
	virtual ~CVIBuffer_Mesh_Instancing() = default;

public:
	virtual HRESULT Initialize_Prototype(const char* pModelFilePath, _float4x4 TransformMatrix, _uint iNumInstance);
	virtual HRESULT Initialize(void* pArg);
	virtual void	Tick(_double TimeDelta);
	virtual HRESULT Render()override;

	void	Start();

	void	GetRandomVector(_float2* out, _float2* min, _float2* max);
	void	GetRandomVector(_float3* out, _float3* min, _float3* max);
	void	GetRandomVector(_float4* out, _float4* min, _float4* max);
	float	GetRandomFloat(float lowBound, float highBound);

public:
	void SetInstanceBuffer(VTXINSTANCE* pBuffer, _uint iSize);
	void SetInstanceBuffer(list<VTXINSTANCE>* pBuffer);

protected:
	_uint m_iInitNumInstance = 0;
	string m_strName = "";

	_uint				m_iMaterialIndex = 0;
	_float				m_fTimeAcc = 0.f;
	_float				m_fGenerateTime = 0.f;

private:
	_bool		m_bGenerate = false;


public:
	static CVIBuffer_Mesh_Instancing*	Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const char* pModelFilePath, _float4x4 TransformMatrix, _uint iNumInstance);
	virtual CComponent*					Clone(void* pArg = nullptr);
	virtual void						Free() override;
};

END