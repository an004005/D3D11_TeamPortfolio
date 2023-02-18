#include "stdafx.h"
#include "Room.h"
#include "ClientPacketHandler.h"
#include "Player.h"
#include "GameSession.h"
#include "Protocol.pb.h"
#include "GameRoom.h"

shared_ptr<Room> GRoom = make_shared<Room>();

Room::Room()
{
	_roomPerPlayer = 6;
}

void Room::Enter(PlayerRef player)
{
	// ºó¹æ »èÁ¦
	_gameRooms.erase(remove_if(_gameRooms.begin(), _gameRooms.end(), [player](const GameRoomRef& gameRoom)
	{
		return gameRoom->Empty();
	}), _gameRooms.end());

	_players.push_back(player);

	if (_players.size() >= _roomPerPlayer)
	{
		const GameRoomRef gameRoom = make_shared<GameRoom>();
		_gameRooms.push_back(gameRoom);

		list<PlayerRef> inGamePlayers;

		for (uint32 i = 0; i < _roomPerPlayer; ++i)
		{
			auto player = _players.front();
			_players.pop_front();

			player->_gameRoom = gameRoom;
			inGamePlayers.push_back(player);
		}

		gameRoom->EnterPlayers(inGamePlayers);
	}
}

void Room::Leave(PlayerRef player)
{
	_players.erase(remove_if(_players.begin(), _players.end(), [player](const PlayerRef& inPlayer)
	{
		return inPlayer->playerId == player->playerId;
	}), _players.end());

	if (auto gameRoom = player->_gameRoom.lock())
	{
		gameRoom->DoAsync(&GameRoom::Leave, player);
	}
}

void Room::Broadcast(SendBufferRef sendBuffer)
{
	for (auto& player : _players)
	{
		if (const auto locked = player->ownerSession.lock())
		{
			locked->Send(sendBuffer);
		}
	}
}

void Room::BroadcastExcept(SendBufferRef sendBuffer, uint64 iPlayerID)
{
	for (auto& player : _players)
	{
		if (player->playerId == iPlayerID)
			continue;

		if (const auto locked = player->ownerSession.lock())
		{
			locked->Send(sendBuffer);
		}
	}
}
