//
// kerne-multicorel.cpp
//
// Circle - A C++ bare metal environment for Raspberry Pi
// Copyright (C) 2014  R. Stange <rsta2@o2online.de>
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
#include "kernel-multicore.h"

CKernelMulticore::CKernelMulticore (void) :
    m_Screen (m_Options.GetWidth (), m_Options.GetHeight ()),
    m_Timer (&m_Interrupt),
    m_Logger (m_Options.GetLogLevel (), &m_Timer),
    m_multicoreTest(&m_Memory)
{
    m_ActLED.Blink (5);	// show we are alive
}

CKernelMulticore::~CKernelMulticore (void)
{
}

boolean CKernelMulticore::Initialize (void)
{
    boolean bOK = TRUE;

    if (bOK)
    {
        bOK = m_Screen.Initialize ();
    }

//    if (bOK)
//    {
//        bOK = m_Serial.Initialize (115200);
//    }

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
        bOK = m_multicoreTest.Initialize();
    if (bOK)
        m_ActLED.Blink(10, 100, 100);
    return bOK;
}

TShutdownMode CKernelMulticore::Run (void)
{
    //m_ActLED.Blink(10, 100, 100);
    m_multicoreTest.Run (0);
    return ShutdownHalt;
}
