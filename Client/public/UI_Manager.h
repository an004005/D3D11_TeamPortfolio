#pragma once

#include "Client_Defines.h"
#include "Base.h"
#include "Canvas.h"

class CUI_Manager final : public CBase
{
	DECLARE_SINGLETON(CUI_Manager)

public:
	CUI_Manager();
	virtual ~CUI_Manager() = default;

public:
	map<wstring, CCanvas*> Get_Canvas() {
		return m_mapCanvass;
	}

	CCanvas* Find_CCanvas(const wstring& pCanvasTag);
	void Add_CCanvas(const wstring& pCanvasTag, CCanvas* pCanvas);

private:
	map<wstring, CCanvas*> m_mapCanvass;

public:
	virtual void Free() override;
};

