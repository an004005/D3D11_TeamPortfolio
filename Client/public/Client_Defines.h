#pragma once

#include <Process.h>
#include "Imgui_AppLog.h"
#include "ClientUtils.h"

namespace Client
{
	enum LEVEL { LEVEL_LOGO, LEVEL_GAMEPLAY, LEVEL_MAPTOOL, LEVEL_ANIM, LEVEL_CONVERTER, LEVEL_MULTIPLAY, LEVEL_PLAYERTEST, LEVEL_EFFECT, LEVEL_UI, LEVEL_ENEMIESTEST, LEVEL_END };
}

extern HWND			g_hWnd;
extern HINSTANCE	g_hInst;
extern bool			g_bFullScreen;
extern bool			g_bNeedResizeSwapChain;
extern unsigned int	g_iWinSizeX;
extern unsigned int	g_iWinSizeY;
extern _float g_fTimeDelta;

using namespace Client;



