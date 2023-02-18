// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 또는 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
#pragma warning(disable : 4800)

#ifdef _DEBUG
#pragma comment(lib, "ServerCore\\Debug\\ServerCore.lib")
// #pragma comment(lib, "Protobuf\\Debug\\libprotobufd.lib")
#else
#pragma comment(lib, "ServerCore\\Release\\ServerCore.lib")
// #pragma comment(lib, "Protobuf\\Release\\libprotobuf.lib")
#endif
#include "CorePch.h"
#include "Enum.pb.h"
// #include "Struct.pb.h"
// #include "Protocol.pb.h"

// Windows 헤더 파일:
#include <windows.h>

// C 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include "Engine_Defines.h"
#include "Client_Defines.h"
using namespace Client;

// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.
