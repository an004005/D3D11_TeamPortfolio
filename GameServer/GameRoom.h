#pragma once
#include "JobQueue.h"
#include "Protocol.pb.h"

class GameRoom : public JobQueue
{
public:
	GameRoom();
	void PrintRoomInfo();
	void EnterPlayers(const list<PlayerRef>& players);
	void Leave(PlayerRef player);
	void Broadcast(SendBufferRef sendBuffer);
	void BroadcastExcept(SendBufferRef sendBuffer, uint64 iPlayerID);

	bool IsPlayerIn(PlayerRef player);
	bool Empty() const { return _players.empty(); }

public:
	bool _bStart = false;
	map<uint64, PlayerRef> _players;
};

