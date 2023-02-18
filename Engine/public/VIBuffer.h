#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer abstract : public CComponent
{
protected:
	CVIBuffer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer(const CVIBuffer& rhs);
	virtual ~CVIBuffer() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual HRESULT Render();

protected:
	/* 할당하고자하는 버퍼의 속성을 정의한다. */
	D3D11_BUFFER_DESC			m_BufferDesc;

	/* 할당시에 채워넣고자하는 데이터들. */
	D3D11_SUBRESOURCE_DATA		m_SubResourceData;

	_uint						m_iStride = 0; /* 정점하나의 바이트 크기. */
	_uint						m_iNumVertices = 0;
	_uint						m_iNumPrimitive = 0;
	_uint						m_iIndicesSizePerPrimitive = 0;
	_uint						m_iNumIndicesPerPrimitive = 0;
	_uint						m_iNumIndices = 0;
	_uint						m_iNumVertexBuffers = 0;
	DXGI_FORMAT					m_eIndexFormat;
	D3D11_PRIMITIVE_TOPOLOGY	m_eTopology;


protected: /* 버텍스 버퍼 */
	ID3D11Buffer*				m_pVB = nullptr;
	


protected: /* 인덱스 버퍼 */
	ID3D11Buffer*				m_pIB = nullptr;
	

protected:
	HRESULT	Create_VertexBuffer();
	HRESULT	Create_IndexBuffer();



public:
	virtual CComponent* Clone(void* pArg = nullptr) = 0;
	virtual void Free() override;

};

END