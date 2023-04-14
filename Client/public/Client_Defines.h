#pragma once

#include <Process.h>
#include "Imgui_AppLog.h"
#include "ClientUtils.h"

namespace Client
{
	enum LEVEL { LEVEL_LOGO,
		LEVEL_GAMEPLAY,
		LEVEL_MAPTOOL,
		LEVEL_ANIM,
		LEVEL_CONVERTER,
		LEVEL_MULTIPLAY,
		LEVEL_PLAYERTEST,
		LEVEL_EFFECT,
		LEVEL_UI,
		LEVEL_ENEMIESTEST,
		LEVEL_BATCH,
		LEVEL_KINETIC_EDITOR,

		LEVEL_DOWNTOWN_1,
		LEVEL_DOWNTOWN_2,
		LEVEL_DOWNTOWN_3,
		//아래로 20부터 시작, 새 래벨은 위에 만들것
		LEVEL_TUTORIAL = 20,
		LEVEL_CONSTRUCTIONSITE_3F, 
		LEVEL_CONSTRUCTIONSITE_2F,
		LEVEL_SUBWAY, 
		LEVEL_NAOMIROOM,
		LEVEL_HOSPITAL_1F,
		LEVEL_FINAL_STAGE,
		LEVEL_END };
}

extern HWND			g_hWnd;
extern HINSTANCE	g_hInst;
extern bool			g_bFullScreen;
extern bool			g_bNeedResizeSwapChain;
extern unsigned int	g_iWinSizeX;
extern unsigned int	g_iWinSizeY;
extern _float g_fTimeDelta;
extern _float g_fTimeDelta_Add;

using namespace Client;



