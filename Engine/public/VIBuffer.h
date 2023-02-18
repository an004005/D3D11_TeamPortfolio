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
	/* �Ҵ��ϰ����ϴ� ������ �Ӽ��� �����Ѵ�. */
	D3D11_BUFFER_DESC			m_BufferDesc;

	/* �Ҵ�ÿ� ä���ְ����ϴ� �����͵�. */
	D3D11_SUBRESOURCE_DATA		m_SubResourceData;

	_uint						m_iStride = 0; /* �����ϳ��� ����Ʈ ũ��. */
	_uint						m_iNumVertices = 0;
	_uint						m_iNumPrimitive = 0;
	_uint						m_iIndicesSizePerPrimitive = 0;
	_uint						m_iNumIndicesPerPrimitive = 0;
	_uint						m_iNumIndices = 0;
	_uint						m_iNumVertexBuffers = 0;
	DXGI_FORMAT					m_eIndexFormat;
	D3D11_PRIMITIVE_TOPOLOGY	m_eTopology;


protected: /* ���ؽ� ���� */
	ID3D11Buffer*				m_pVB = nullptr;
	


protected: /* �ε��� ���� */
	ID3D11Buffer*				m_pIB = nullptr;
	

protected:
	HRESULT	Create_VertexBuffer();
	HRESULT	Create_IndexBuffer();



public:
	virtual CComponent* Clone(void* pArg = nullptr) = 0;
	virtual void Free() override;

};

END