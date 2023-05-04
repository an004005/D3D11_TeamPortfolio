#pragma once

#include "Client_Defines.h"
#include "Base.h"
#include "Canvas.h"
#include "SoundStore.h"

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

	void CloneSound();

	CCanvas* Find_MoveCanvas(const wstring& pCanvasTag);
	void Add_MoveCanvas(const wstring& pCanvasTag, CCanvas* pCanvas);

	CCanvas* Find_Canvas(const wstring& pCanvasTag);
	void Add_Canvas(const wstring& pCanvasTag, CCanvas* pCanvas);

	CCanvas* Find_WindowCanvas(const wstring& pCanvasTag);
	void Add_WindowCanvas(const wstring& pCanvasTag, CCanvas* pCanvas);

	void Set_TempOff(const _bool bOff);

	void Clear();

	void SetMouseActive(_bool bActive) { m_bMouseActive = bActive; }
	_bool IsMouseActive() const { return m_bMouseActive; }

	void Tick(_double TimeDelta);

public:
	void	PlaySound(const string& SoundName);

private:
	map<wstring, CCanvas*> m_mapMoveCanvass;	
	map<wstring, CCanvas*> m_mapCanvass;
	map<wstring, CCanvas*> m_mapWindowCanvass;

	CSoundStore m_SoundStore;

	_bool m_bMouseActive = false;

public:
	virtual void Free() override;
};

