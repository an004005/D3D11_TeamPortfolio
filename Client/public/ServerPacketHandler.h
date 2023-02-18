#pragma once
#include "Protocol.pb.h"

using PacketHandlerFunc = std::function<bool(PacketSessionRef&, BYTE*, int32)>;
extern PacketHandlerFunc GPacketHandler[UINT16_MAX];

enum : uint16
{
	PKT_C_LOGIN = 1000,
	PKT_S_LOGIN = 1001,
	PKT_S_PLAYER_EXIT = 1002,
	PKT_C_PLAYER_ACTION_SYNC = 1003,
	PKT_S_PLAYER_ACTION_SYNC = 1004,
	PKT_C_PLAYER_ACTION_SYNC_MATRIX = 1005,
	PKT_S_PLAYER_ACTION_SYNC_MATRIX = 1006,
};

// Custom Handlers
bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len);
bool Handle_S_LOGIN(PacketSessionRef& session, Protocol::S_LOGIN& pkt);
bool Handle_S_PLAYER_EXIT(PacketSessionRef& session, Protocol::S_PLAYER_EXIT& pkt);
bool Handle_S_PLAYER_ACTION_SYNC(PacketSessionRef& session, Protocol::S_PLAYER_ACTION_SYNC& pkt);
bool Handle_S_PLAYER_ACTION_SYNC_MATRIX(PacketSessionRef& session, Protocol::S_PLAYER_ACTION_SYNC_MATRIX& pkt);

class ServerPacketHandler
{
public:
	static void Init()
	{
		for (int32 i = 0; i < UINT16_MAX; i++)
			GPacketHandler[i] = Handle_INVALID;
		GPacketHandler[PKT_S_LOGIN] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_LOGIN>(Handle_S_LOGIN, session, buffer, len); };
		GPacketHandler[PKT_S_PLAYER_EXIT] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_PLAYER_EXIT>(Handle_S_PLAYER_EXIT, session, buffer, len); };
		GPacketHandler[PKT_S_PLAYER_ACTION_SYNC] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_PLAYER_ACTION_SYNC>(Handle_S_PLAYER_ACTION_SYNC, session, buffer, len); };
		GPacketHandler[PKT_S_PLAYER_ACTION_SYNC_MATRIX] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_PLAYER_ACTION_SYNC_MATRIX>(Handle_S_PLAYER_ACTION_SYNC_MATRIX, session, buffer, len); };
	}

	static bool HandlePacket(PacketSessionRef& session, BYTE* buffer, int32 len)
	{
		PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
		return GPacketHandler[header->id](session, buffer, len);
	}
	static SendBufferRef MakeSendBuffer(Protocol::C_LOGIN& pkt) { return MakeSendBuffer(pkt, PKT_C_LOGIN); }
	static SendBufferRef MakeSendBuffer(Protocol::C_PLAYER_ACTION_SYNC& pkt) { return MakeSendBuffer(pkt, PKT_C_PLAYER_ACTION_SYNC); }
	static SendBufferRef MakeSendBuffer(Protocol::C_PLAYER_ACTION_SYNC_MATRIX& pkt) { return MakeSendBuffer(pkt, PKT_C_PLAYER_ACTION_SYNC_MATRIX); }

private:
	template<typename PacketType, typename ProcessFunc>
	static bool HandlePacket(ProcessFunc func, PacketSessionRef& session, BYTE* buffer, int32 len)
	{
		PacketType pkt;
		if (pkt.ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)) == false)
			return false;

		return func(session, pkt);
	}

	template<typename T>
	static SendBufferRef MakeSendBuffer(T& pkt, uint16 pktId)
	{
		const uint16 dataSize = static_cast<uint16>(pkt.ByteSizeLong());
		const uint16 packetSize = dataSize + sizeof(PacketHeader);

		SendBufferRef sendBuffer = GSendBufferManager->Open(packetSize);
		PacketHeader* header = reinterpret_cast<PacketHeader*>(sendBuffer->Buffer());
		header->size = packetSize;
		header->id = pktId;
		ASSERT_CRASH(pkt.SerializeToArray(&header[1], dataSize));
		sendBuffer->Close(packetSize);

		return sendBuffer;
	}
};