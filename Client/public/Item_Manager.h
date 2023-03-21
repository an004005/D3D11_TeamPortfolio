#pragma once

#include "Client_Defines.h"
#include "Base.h"

class CItem_Manager final : public CBase
{
	DECLARE_SINGLETON(CItem_Manager)

public:
	CItem_Manager();
	virtual ~CItem_Manager() = default;

public:
	HRESULT	Initialize();
	

public:
	virtual void Free() override;
};

