#include "stdafx.h"
#include "ServerPacketHandler.h"
#include "ClientServiceMgr.h"
#include "GameInstance.h"
#include "RemoteController.h"

PacketHandlerFunc GPacketHandler[UINT16_MAX];

bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len)
{
	PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
	// TODO : Log
	return false;
}

bool Handle_S_LOGIN(PacketSessionRef& session, Protocol::S_LOGIN& pkt)
{


	return true;
}

/****************
 * 인게임 프로토콜
 *****************/

bool Handle_S_PLAYER_EXIT(PacketSessionRef& session, Protocol::S_PLAYER_EXIT& pkt)
{

	// GClientService->ExitGame(pkt.player().id());
	return true;
}

bool Handle_S_PLAYER_ACTION_SYNC(PacketSessionRef& session, Protocol::S_PLAYER_ACTION_SYNC& pkt)
{

	// CPlayer* pPlayer = GClientService->GetPlayerObject(pkt.player().id());
	// if (pPlayer == nullptr) return true;
	//
	//
	// if (auto pController = pPlayer->GetComponent<CRemoteController>(L"Controller"))
	// {
	// 	INPUT_SYNC_DATA tData;
	// 	tData.iInputMask = pkt.syncdata().inputmask();
	// 	tData.vMouseAxis = CClientUtils::ToClientVector2(pkt.syncdata().mouseaxis());
	// 	pController->AddNextInputSyncData(tData);
	// }

	return true;
}

bool Handle_S_PLAYER_ACTION_SYNC_MATRIX(PacketSessionRef& session, Protocol::S_PLAYER_ACTION_SYNC_MATRIX& pkt)
{
	// CPlayer* pPlayer = GClientService->GetPlayerObject(pkt.player().id());
	// if (pPlayer == nullptr) return true;
	//
	// if (auto pController = pPlayer->GetComponent<CRemoteController>(L"Controller"))
	// {
	// 	INPUT_SYNC_DATA tData;
	// 	tData.iInputMask = pkt.syncdata().inputmask();
	// 	tData.vMouseAxis = CClientUtils::ToClientVector2(pkt.syncdata().mouseaxis());
	// 	pController->AddNextInputSyncData(tData);
	// 	pController->AddNextWorldMatrix(CClientUtils::ToClientMatrix(pkt.world()));
	// }

	return true;
}

