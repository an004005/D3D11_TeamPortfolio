#include "stdafx.h"
#include "ClientPacketHandler.h"

#include "GameRoom.h"
#include "Player.h"
#include "Room.h"
#include "GameSession.h"
#include "Service.h"

PacketHandlerFunc GPacketHandler[UINT16_MAX];


bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len)
{
	PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
	// TODO : Log
	return false;
}

bool Handle_C_LOGIN(PacketSessionRef& session, Protocol::C_LOGIN& pkt)
{
	GameSessionRef gameSession = static_pointer_cast<GameSession>(session);

	Protocol::S_LOGIN loginPkt;

	static Atomic<uint64> idGenerator{1};

	{
		PlayerRef playerRef = MakeShared<Player>();
		playerRef->playerId = idGenerator++;
		clog << "Player : " << playerRef->playerId << " login "<<endl;
		playerRef->ownerSession = gameSession;
		
		gameSession->_currentPlayer = playerRef;

		loginPkt.set_playerid(playerRef->playerId);
	}

	auto sendBuffer = ClientPacketHandler::MakeSendBuffer(loginPkt);
	session->Send(sendBuffer);
	
	return true;
}

/*****************
 * 인게임 프로토콜
 *******************/

bool Handle_C_PLAYER_ACTION_SYNC(PacketSessionRef& session, Protocol::C_PLAYER_ACTION_SYNC& pkt)
{
	GameSessionRef gameSession = static_pointer_cast<GameSession>(session);
	if (auto gameRoom = gameSession->_currentPlayer->_gameRoom.lock())
	{
		Protocol::S_PLAYER_ACTION_SYNC syncPkt;
		syncPkt.mutable_player()->CopyFrom(pkt.player());
		syncPkt.mutable_syncdata()->CopyFrom(pkt.syncdata());

		auto sendBuffer = ClientPacketHandler::MakeSendBuffer(syncPkt);
		gameRoom->DoAsync(&GameRoom::BroadcastExcept, sendBuffer, gameSession->_currentPlayer->playerId);
	}
	return true;
}

bool Handle_C_PLAYER_ACTION_SYNC_MATRIX(PacketSessionRef& session, Protocol::C_PLAYER_ACTION_SYNC_MATRIX& pkt)
{
	GameSessionRef gameSession = static_pointer_cast<GameSession>(session);
	if (auto gameRoom = gameSession->_currentPlayer->_gameRoom.lock())
	{
		Protocol::S_PLAYER_ACTION_SYNC_MATRIX syncPkt;
		syncPkt.mutable_player()->CopyFrom(pkt.player());
		syncPkt.mutable_syncdata()->CopyFrom(pkt.syncdata());
		syncPkt.mutable_world()->CopyFrom(pkt.world());

		auto sendBuffer = ClientPacketHandler::MakeSendBuffer(syncPkt);
		gameRoom->DoAsync(&GameRoom::BroadcastExcept, sendBuffer, gameSession->_currentPlayer->playerId);
	}
	return true;
}

