#pragma once

#include "Base.h"

/* Ŭ���̾�Ʈ�� ������ �������� �θ� �Ǵ� Ŭ�����̴�.  */
/* Ŭ���̾�Ʈ�� ������ �������� �� �����ȿ��� ���Ǵ� ��ü���� �����Ѵ�.etc */
/* ��ü���� �� �ַ� �ʿ��� ��ġ��ü���� �����Ѵ�. */


BEGIN(Engine)

class ENGINE_DLL CLevel abstract : public CBase
{
protected:
	CLevel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel() = default;

public:
	virtual HRESULT Initialize();
	virtual void Tick(_double TimeDelta);
	virtual void Late_Tick(_double TimeDelta);
	virtual HRESULT Render();

protected:
	ID3D11Device*				m_pDevice = nullptr;
	ID3D11DeviceContext*		m_pContext = nullptr;

public:
	virtual void Free();
};

END