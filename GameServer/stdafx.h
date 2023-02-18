// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once


#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // ���� ������ �ʴ� ������ Windows ������� �����մϴ�.
#pragma warning(disable : 4800)

#ifdef _DEBUG
#pragma comment(lib, "ServerCore\\Debug\\ServerCore.lib")
// #pragma comment(lib, "Protobuf\\Release\\libprotobuf.lib")

#else
#pragma comment(lib, "ServerCore\\Release\\ServerCore.lib")
// #pragma comment(lib, "Protobuf\\Debug\\libprotobufd.lib")	
#endif


#include "CorePch.h"
#include "Enum.pb.h"

using GameSessionRef = shared_ptr<class GameSession>;
using PlayerRef = shared_ptr<class Player>;
using GameRoomRef = shared_ptr<class GameRoom>;