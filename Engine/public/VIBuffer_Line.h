#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class CVIBuffer_Line : public CVIBuffer
{
protected:
	CVIBuffer_Line(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Line(const CVIBuffer_Line& rhs);
	virtual ~CVIBuffer_Line() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual HRESULT Render() override;

	void SetLines(const vector<VTXLINE>& lines);

private:
	_uint m_iLineCnt = 0;

public:
	static CVIBuffer_Line* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END