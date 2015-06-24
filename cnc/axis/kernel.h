//
// kernel.h
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
#ifndef _kernel_h
#define _kernel_h

//#define INTERRUPTS
#define MULTICORE

#include <circle/memory.h>
#include <circle/actled.h>
#include <circle/types.h>
#include <circle/gpiopin.h>
#include <circle/timer.h>
#include <circle/bcm2835.h>
#ifdef INTERRUPTS
#include <circle/interrupt.h>
#include <circle/gpiomanager.h>
#endif

enum TShutdownModeSC
{
    ShutdownNoneSC,
    ShutdownHaltSC,
    ShutdownRebootSC
};


class CKernel
{
public:
	CKernel (void);
	~CKernel (void);

	boolean Initialize (void);

    TShutdownModeSC Run (void);
#ifdef INTERRUPTS
    TShutdownModeSC RunEncodersInterrupts (void);
#endif
    TShutdownModeSC RunEncodersPolling (void);
private:
	// do not change this order
    CMemorySystem m_Memory;
    CActLED m_ActLED;
#ifdef INTERRUPTS
    CInterruptSystem m_Interrupt;
    CGPIOManager m_GPIOManager;
#endif

};

#endif