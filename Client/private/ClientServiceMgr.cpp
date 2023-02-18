#include "stdafx.h"
#include "ClientServiceMgr.h"
#include "ThreadManager.h"
#include "ServerPacketHandler.h"
#include "Service.h"
#include "Level_MultiPlay.h"
#include "Controller.h"

_bool g_bOnline = false;
shared_ptr<CClientServiceMgr> GClientService = make_shared<CClientServiceMgr>();

enum
{
	WORKER_TICK = 64
};

CClientServiceMgr::CClientServiceMgr()
{
}


CClientServiceMgr::~CClientServiceMgr()
{
	ClearClientObjects();

	m_bStop.store(true);
	if (g_bOnline)
		GThreadManager->Join();
	g_bOnline = false;
}

HRESULT CClientServiceMgr::ReadyClientService()
{
	g_bOnline = true;
	ServerPacketHandler::Init();
	this_thread::sleep_for(1s);

	_service = MakeShared<ClientService>(
		NetAddress(L"192.168.1.68", 7777),
		MakeShared<IocpCore>(),
		MakeShared<ServerSession>, // TODO : SessionManager 등
		1);
	ASSERT_CRASH(_service->Start());

	// 패킷 우선 스레드
	for (int32 i = 0; i < 2; i++)
	{
		GThreadManager->Launch([this]()
			{
				while (m_bStop.load() == false)
				{
					LEndTickCount = ::GetTickCount64() + WORKER_TICK / 2;
					_service->GetIocpCore()->Dispatch(20);
					ThreadManager::DistributeReservedJobs();
					ThreadManager::DoGlobalQueueWork();
				}
			});
	}

	// 잡 우선 스레드
	GThreadManager->Launch([this]()
		{
			while (m_bStop.load() == false)
			{
				LEndTickCount = ::GetTickCount64() + WORKER_TICK * 2;
				_service->GetIocpCore()->Dispatch(1);
				ThreadManager::DistributeReservedJobs();
				ThreadManager::DoGlobalQueueWork();
			}
		});

	return S_OK;
}

void CClientServiceMgr::Broadcast(const SendBufferRef sendBuffer)
{
	_service->Broadcast(sendBuffer);
}

void CClientServiceMgr::ClearClientObjects()
{
}

/*-----------------
 *  ServerSession
 -----------------*/
void ServerSession::OnConnected()
{
	Protocol::C_LOGIN pkt;
	auto sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt);
	Send(sendBuffer);
}

void ServerSession::OnRecvPacket(BYTE* buffer, int32 len)
{
	PacketSessionRef session = GetPacketSessionRef();
	PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);

	// TODO : packetId 대역 체크
	ServerPacketHandler::HandlePacket(session, buffer, len);
}
