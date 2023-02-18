#pragma once
#include "JobQueue.h"

class Room : public JobQueue
{
public:
	Room();
	// �̱۾����� ȯ���θ��� �ڵ�
	void Enter(PlayerRef player);
	void Leave(PlayerRef player);
	void Broadcast(SendBufferRef sendBuffer);
	void BroadcastExcept(SendBufferRef sendBuffer, uint64 iPlayerID);

	list<PlayerRef>_players;

	uint32 _roomPerPlayer;
	list<GameRoomRef> _gameRooms;
};

extern shared_ptr<Room> GRoom;