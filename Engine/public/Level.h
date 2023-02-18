#pragma once

#include "Base.h"

/* 클라이언트에 정의할 레벨들의 부모가 되는 클래스이다.  */
/* 클라이언트에 정의한 레벨들은 이 레벨안에서 사용되는 객체들을 생성한다.etc */
/* 객체생성 시 주로 필요한 장치객체들을 보관한다. */


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