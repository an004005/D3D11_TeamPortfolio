// stdafx.h : ���� ��������� ���� ��������� �ʴ�
// ǥ�� �ý��� ���� ���� �Ǵ� ������Ʈ ���� ���� ������
// ��� �ִ� ���� �����Դϴ�.
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // ���� ������ �ʴ� ������ Windows ������� �����մϴ�.
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

// Windows ��� ����:
#include <windows.h>

// C ��Ÿ�� ��� �����Դϴ�.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include "Engine_Defines.h"
#include "Client_Defines.h"
using namespace Client;

// TODO: ���α׷��� �ʿ��� �߰� ����� ���⿡�� �����մϴ�.
