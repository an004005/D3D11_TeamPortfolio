#pragma once
#include "Session.h"
#include "Protocol.pb.h"
#include "CorePch.h"
#include "JobQueue.h"

extern _bool g_bOnline;
extern shared_ptr<class CClientServiceMgr> GClientService;

class CClientServiceMgr : public JobQueue
{
public:
	CClientServiceMgr();
	virtual ~CClientServiceMgr();

public:
	HRESULT ReadyClientService();
	void Broadcast(const SendBufferRef sendBuffer);
	
public:
	// CBase기반의 오브젝트 삭제 함수
	void ClearClientObjects();

private:
	USE_LOCK;
	ClientServiceRef _service;
	atomic<bool> m_bStop{ false };

};


/*-----------------
 *  ServerSession
 -----------------*/
class ServerSession : public PacketSession
{
public:
	~ServerSession()
	{
		cout << "~ServerSession" << endl;
	}

	virtual void OnConnected() override;
	virtual void OnRecvPacket(BYTE* buffer, int32 len) override;
	virtual void OnSend(int32 len) override
	{
		//cout << "OnSend Len = " << len << endl;
	}

	virtual void OnDisconnected() override
	{
		//cout << "Disconnected" << endl;
	}
};

