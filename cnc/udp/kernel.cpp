//
// kernel.cpp
//
// Circle - A C++ bare metal environment for Raspberry Pi
// Copyright (C) 2014-2015  R. Stange <rsta2@o2online.de>
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
#include "kernel.h"
#include <circle/net/dnsclient.h>
#include <circle/net/ntpclient.h>
#include <circle/net/in.h>
#include <circle/net/ipaddress.h>
#include <circle/net/socket.h>
#include <circle/net/udpconnection.h>
#include <circle/util.h>
#include <assert.h>

// Network configuration
static const u8 IPAddress[]      = {192, 168, 1, 200};
static const u8 NetMask[]        = {255, 255, 255, 0};
static const u8 DefaultGateway[] = {192, 168, 1, 254};
static const u8 DNSServer[]      = {8, 8, 8, 8};

// Time configuration
static const char NTPServer[]    = "pool.ntp.org";
static const int nTimeZone       = 0*60;		// minutes diff to UTC

static const char FromKernel[] = "kernel";

class CUDPSocket
{
public:
	CUDPSocket (CNetSubSystem *pNetSubSystem)
    :	m_pNetConfig (pNetSubSystem->GetConfig ()),
	    m_pTransportLayer (pNetSubSystem->GetTransportLayer ()),
	    m_hConnection (-1),
	    m_pBuffer (0)
    {
	    assert (m_pNetConfig != 0);
	    assert (m_pTransportLayer != 0);

	    m_pBuffer = new u8[FRAME_BUFFER_SIZE];
	    assert (m_pBuffer != 0);
    }
	~CUDPSocket (void)
    {
	    if (m_hConnection >= 0)
	    {
		    assert (m_pTransportLayer != 0);
		    m_pTransportLayer->Disconnect (m_hConnection);
		    m_hConnection = -1;
	    }

	    delete m_pBuffer;
	    m_pBuffer = 0;

	    m_pTransportLayer = 0;
	    m_pNetConfig = 0;
    }

	int Connect(u16 nOwnPort, u16 nRemotePort, boolean broadcast = FALSE)
    {
	    assert (m_pTransportLayer != 0);
	    if (m_hConnection >= 0)
	    {
		    if (m_nProtocol != IPPROTO_UDP)
			    return -1;
		    m_pTransportLayer->Disconnect(m_hConnection);
		    m_hConnection = -1;
	    }
        u8 broadcastAddress[] = {
            (u8)(m_pNetConfig->GetNetMask()[0] == 0 ? 255: m_pNetConfig->GetIPAddress()->Get()[0]),
            (u8)(m_pNetConfig->GetNetMask()[1] == 0 ? 255: m_pNetConfig->GetIPAddress()->Get()[1]),
            (u8)(m_pNetConfig->GetNetMask()[2] == 0 ? 255: m_pNetConfig->GetIPAddress()->Get()[2]),
            (u8)(m_pNetConfig->GetNetMask()[3] == 0 ? 255: m_pNetConfig->GetIPAddress()->Get()[3])
        };
        CIPAddress broadcastIP(broadcastAddress);
		u8 anyAddress[] = {	(u8)0, (u8)0, (u8)0, (u8)0 };
        CIPAddress anyAddressIP(anyAddress);
        CIPAddress normalAddress(m_pNetConfig->GetIPAddress()->Get());
        m_hConnection = m_pTransportLayer->Connect((broadcast ? anyAddressIP: normalAddress), (u16)nRemotePort, (u16)nOwnPort, IPPROTO_UDP);
	    return m_hConnection >= 0 ? 0 : m_hConnection;
    }

	int Connect (CIPAddress &rForeignIP, u16 nForeignPort)
	{
		if (nForeignPort == 0)
		{
			return -1;
		}

		assert (m_pTransportLayer != 0);

		if (m_hConnection >= 0)
		{
			if (m_nProtocol != IPPROTO_UDP)
			{
				return -1;
			}

			m_pTransportLayer->Disconnect (m_hConnection);
			m_hConnection = -1;
		}

		m_hConnection = m_pTransportLayer->Connect (rForeignIP, nForeignPort, 0, m_nProtocol);

		return m_hConnection >= 0 ? 0 : m_hConnection;
	}

	int Listen (u16 nOwnPort)
    {
	    assert (m_pTransportLayer != 0);
	    if (m_hConnection >= 0)
	    {
		    if (m_nProtocol != IPPROTO_UDP)
		    {
			    return -1;
		    }
		    m_pTransportLayer->Disconnect(m_hConnection);
		    m_hConnection = -1;
	    }
        static const u8 foreignAddress[] = {192, 168, 1, 81};
        CIPAddress fa(foreignAddress); 
        m_hConnection = m_pTransportLayer->Connect(fa, (u16)15057, (u16)15057, IPPROTO_UDP);
	    return m_hConnection >= 0 ? 0 : m_hConnection;
    }

	int Receive (void *pBuffer, unsigned nLength, int nFlags)
    {
	    if (m_hConnection < 0)
	    {
		    return -1;
	    }
	
	    if (nLength == 0)
	    {
		    return -1;
	    }
	
	    assert (m_pTransportLayer != 0);
	    assert (m_pBuffer != 0);
	    int nResult = m_pTransportLayer->Receive (m_pBuffer, nFlags, m_hConnection);
	    if (nResult < 0)
	    {
		    return nResult;
	    }

	    if (nLength < (unsigned) nResult)
	    {
		    nResult = nLength;
	    }

	    assert (pBuffer != 0);
	    memcpy (pBuffer, m_pBuffer, nResult);

	    return nResult;
    }

	int Send (const void *pBuffer, unsigned nLength, int nFlags)
	{
		if (m_hConnection < 0)
		{
			return -1;
		}

		if (nLength == 0)
		{
			return -1;
		}
	
		assert (m_pTransportLayer != 0);
		assert (pBuffer != 0);
		return m_pTransportLayer->Send (pBuffer, nLength, nFlags, m_hConnection);
	}

private:
	CNetConfig	*m_pNetConfig;
	CTransportLayer	*m_pTransportLayer;

	int m_nProtocol;
	int m_hConnection;

	u8 *m_pBuffer;
};

CKernel::CKernel (void)
:	m_Screen (m_Options.GetWidth (), m_Options.GetHeight ()),
	m_Timer (&m_Interrupt),
	m_Logger (m_Options.GetLogLevel (), &m_Timer),
	m_DWHCI (&m_Interrupt, &m_Timer),
	m_Net (IPAddress, NetMask, DefaultGateway, DNSServer),
	m_nTicksNextUpdate (0)
{
	m_ActLED.Blink (5);	// show we are alive
}

CKernel::~CKernel (void)
{
}

boolean CKernel::Initialize (void)
{
	boolean bOK = TRUE;

	if (bOK)
	{
		bOK = m_Screen.Initialize ();
	}

	if (bOK)
	{
		bOK = m_Serial.Initialize (115200);
	}

	if (bOK)
	{
		CDevice *pTarget = m_DeviceNameService.GetDevice (m_Options.GetLogDevice (), FALSE);
		if (pTarget == 0)
		{
			pTarget = &m_Screen;
		}

		bOK = m_Logger.Initialize (pTarget);
	}

	if (bOK)
	{
		bOK = m_Interrupt.Initialize ();
	}

	if (bOK)
	{
		bOK = m_Timer.Initialize ();
	}

	if (bOK)
	{
		bOK = m_DWHCI.Initialize ();
	}

	if (bOK)
	{
		bOK = m_Net.Initialize ();
	}

	return bOK;
}

TShutdownMode CKernel::Run (void)
{
	m_Logger.Write (FromKernel, LogNotice, "Compile time: " __DATE__ " " __TIME__);

	// wait for Ethernet PHY to come up
	m_Timer.MsDelay (2000);

	m_Logger.Write (FromKernel, LogNotice, "Try \"ping %u.%u.%u.%u\" from another computer!",
			(unsigned) IPAddress[0], (unsigned) IPAddress[1],
			(unsigned) IPAddress[2], (unsigned) IPAddress[3]);

    CUDPSocket broadcast(&m_Net);
    int broadcastConnection = broadcast.Connect(15057, 0, TRUE);
    m_Logger.Write(FromKernel, LogNotice, "Listening broadcasts from connection %d", broadcastConnection);

	unsigned char RecvPacket[16];
    for (unsigned nCount = 0; 1; nCount++)
	{
		//if ((int) (m_nTicksNextUpdate - m_Timer.GetTicks ()) <= 0)
		//{
		//	UpdateTime ();
		//}

		m_Net.Process ();
        int received = broadcast.Receive(RecvPacket, sizeof(RecvPacket), MSG_DONTWAIT);
        if (received > 0) {
			if (RecvPacket[0] == 1) {
				CIPAddress from((const u8*)&RecvPacket[1]);
				u16 remotePort = (u16)RecvPacket[5] + (u16)RecvPacket[6] * 256;
                m_Logger.Write(
					FromKernel,
					LogNotice,
					"Connection from %u.%u.%u.%u:%u",
					(unsigned)from.Get()[0],
					(unsigned)from.Get()[1],
					(unsigned)from.Get()[2],
					(unsigned)from.Get()[3],
					(unsigned)remotePort);
				CSocket response(&m_Net, IPPROTO_UDP);
				response.Connect(from, remotePort);
				CString strResponse;
				strResponse.Format(
					"Raspberry PI Bare-metal axis at %u.%u.%u.%u:15051. nCount=%u", 
					(unsigned)IPAddress[0],
					(unsigned)IPAddress[1],
					(unsigned)IPAddress[2],
					(unsigned)IPAddress[3],
					nCount);
				response.Send((const char*)strResponse, strResponse.GetLength(), MSG_DONTWAIT); 
			}
        }

		m_Screen.Rotor (0, nCount);
	}

	return ShutdownHalt;
}

void CKernel::UpdateTime (void)
{
	CIPAddress NTPServerIP;
	CDNSClient DNSClient (&m_Net);
	if (!DNSClient.Resolve (NTPServer, &NTPServerIP))
	{
		m_Logger.Write (FromKernel, LogWarning, "Cannot resolve: %s", NTPServer);

		m_nTicksNextUpdate = m_Timer.GetTicks () + 300 * HZ;

		return;
	}
	
	CNTPClient NTPClient (&m_Net);
	NTPClient.SetTimeZone (nTimeZone);
	unsigned nTime = NTPClient.GetTime (NTPServerIP);
	if (nTime == 0)
	{
		m_Logger.Write (FromKernel, LogWarning, "Cannot get time from %s", NTPServer);

		m_nTicksNextUpdate = m_Timer.GetTicks () + 300 * HZ;

		return;
	}

	m_Timer.SetTime (nTime);

	m_Logger.Write (FromKernel, LogNotice, "System time updated");

	m_nTicksNextUpdate = m_Timer.GetTicks () + 900 * HZ;
}
