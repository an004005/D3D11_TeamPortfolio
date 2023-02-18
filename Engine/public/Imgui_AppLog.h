#pragma once
#include "ImguiObject.h"

#ifdef DISABLE_LOG
#define IM_LOG(...) 
#define IM_WARN(...) 
#define IM_ERROR(...) 
#else
#define IM_LOG(...) Engine::Add_Log(0, __VA_ARGS__);
#define IM_WARN(...) Engine::Add_Log(1, __VA_ARGS__);
#define IM_ERROR(...) Engine::Add_Log(2, __VA_ARGS__);
#endif

BEGIN(Engine)

class ENGINE_DLL CImgui_AppLog : public CImguiObject
{
private:
	CImgui_AppLog(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

public:
	virtual HRESULT Initialize(void* pArg) override;

	virtual void Imgui_RenderWindow() override;

	static CImgui_AppLog* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg = nullptr);
	virtual void Free() override;
};

// 0 : info/ 1 : warn/ 2 : error
void ENGINE_DLL Add_Log(_uint iLevel, const char* fmt, ...);

END
