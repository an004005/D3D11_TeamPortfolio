#include "stdafx.h"
#include "GameRoom.h"
#include "ClientPacketHandler.h"
#include "GameSession.h"
#include "Player.h"
#include "Protocol.pb.h"



GameRoom::GameRoom()
{
}

void GameRoom::PrintRoomInfo()
{

}

void GameRoom::EnterPlayers(const list<PlayerRef>& players)
{
}

void GameRoom::Leave(PlayerRef player)
{
	Protocol::S_PLAYER_EXIT exitPkt;
	exitPkt.mutable_player()->set_id(player->playerId);

	_players.erase(player->playerId);

	DoAsync(&GameRoom::Broadcast, ClientPacketHandler::MakeSendBuffer(exitPkt));
}

void GameRoom::Broadcast(SendBufferRef sendBuffer)
{
	for (auto& p : _players)
	{
		if (const auto locked = p.second->ownerSession.lock())
		{
			locked->Send(sendBuffer);
		}
	}
}

void GameRoom::BroadcastExcept(SendBufferRef sendBuffer, uint64 iPlayerID)
{
	for (auto& p : _players)
	{
		if (p.first == iPlayerID)
			continue;

		if (const auto locked = p.second->ownerSession.lock())
		{
			locked->Send(sendBuffer);
		}
	}
}

bool GameRoom::IsPlayerIn(PlayerRef player)
{
	return _players.find(player->playerId) != _players.end();
}

