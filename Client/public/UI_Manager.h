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
	map<wstring, CCanvas*> Get_MoveCanvas() {
		return m_mapMoveCanvass;
	}

public:
	CCanvas* Find_MoveCanvas(const wstring& pCanvasTag);
	void Add_MoveCanvas(const wstring& pCanvasTag, CCanvas* pCanvas);

	CCanvas* Find_Canvas(const wstring& pCanvasTag);
	void Add_Canvas(const wstring& pCanvasTag, CCanvas* pCanvas);

	CCanvas* Find_WindowCanvas(const wstring& pCanvasTag);
	void Add_WindowCanvas(const wstring& pCanvasTag, CCanvas* pCanvas);

	void Set_TempOff(const _bool bOff);

	void Clear();

private:
	map<wstring, CCanvas*> m_mapMoveCanvass;	
	map<wstring, CCanvas*> m_mapCanvass;
	map<wstring, CCanvas*> m_mapWindowCanvass;

public:
	virtual void Free() override;
};

