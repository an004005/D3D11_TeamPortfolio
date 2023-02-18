#pragma once

/* 레퍼런스 카운트를 관리한다. */
BEGIN(Engine)

class ENGINE_DLL CBase abstract
{
protected:
	CBase();
	virtual ~CBase() = default;

public:
	unsigned long AddRef(); /* 레퍼런스 카운트를 증가시킨다. */
	unsigned long Release(); /* 레퍼런스 카운트를 감소시킨다. or 삭제한다. */

private: /* 참조갯수*/
	unsigned long			m_dwRefCnt = 0;
public:
	virtual void Free() = 0;
};

END