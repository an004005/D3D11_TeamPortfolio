#pragma once
#include "CorePch.h"


class Player
{
public:

	uint64					playerId = 0;

	weak_ptr<GameSession>			ownerSession;
	weak_ptr<GameRoom>			_gameRoom;

};

