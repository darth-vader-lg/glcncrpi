//
// kernel.cpp
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
#include "kernel.h"
#include "encoder.h"

CKernel::CKernel (void)
#ifdef INTERRUPT
    :
    m_GPIOManager(&m_Interrupt)
#endif
{
}

CKernel::~CKernel (void)
{
}

boolean CKernel::Initialize (void)
{
    boolean bOk = TRUE;
#ifdef INTERRUPTS
    m_Interrupt.Initialize();
    if (bOk)
        bOk = m_GPIOManager.Initialize();
#endif
    return bOk;
}

TShutdownModeSC CKernel::Run (void)
{
    // channels are swapped on model B+
    CGPIOPin AudioLeft (40, GPIOModeOutput);
    CGPIOPin AudioRight (45, GPIOModeOutput);

    // flash the Act LED 10 times and click on audio (3.5mm headphone jack)
    for (unsigned i = 1; i <= 50; i++)
    {
        m_ActLED.On ();
        AudioLeft.Invert ();
        AudioRight.Invert ();
        CTimer::SimpleMsDelay (200);

        m_ActLED.Off ();
        CTimer::SimpleMsDelay (500);
    }

    return ShutdownRebootSC;
}

#ifdef INTERRUPTS
TShutdownModeSC CKernel::RunEncodersInterrupts(void)
{
    CGPIOPin led(17, GPIOModeOutput);
    CEncoder encoder0(27, 22, 18, &m_GPIOManager);
    unsigned ledMask = (unsigned)1 << 17;
    int angle;
    while (true) {
        angle = encoder0.m_counter % 10000;
        if (angle < 0)
            angle = -angle;
        if (angle < 100 || angle > 9900)
            *(volatile u32*)ARM_GPIO_GPSET0 = ledMask;
        else
            *(volatile u32*)ARM_GPIO_GPCLR0 = ledMask;
    }
    return ShutdownReboot;
}
#endif

TShutdownModeSC CKernel::RunEncodersPolling(void)
{
    CGPIOPin led(17, GPIOModeOutput);
    led.SetMode(GPIOModeOutput);
    CEncoder encoder0(27, 22, 18);
    CEncoder encoder1(27, 22, 18);
    CEncoder encoder2(27, 22, 18);
    CEncoder encoder3(27, 22, 18);
    CEncoder encoder4(27, 22, 18);
    CEncoder encoder5(27, 22, 18);
    CEncoder encoder6(27, 22, 18);
    CEncoder encoder7(27, 22, 18);
    unsigned ledMask = (unsigned)1 << 17;
    int angle;
    while (true) {
        CEncoder::m_gpios = *(volatile u32*)ARM_GPIO_GPLEV0;
        encoder0.Decode();
        encoder1.Decode();
        encoder2.Decode();
        encoder3.Decode();
        encoder4.Decode();
        encoder5.Decode();
        encoder6.Decode();
        encoder7.Decode();
        angle = encoder0.m_counter % 10000;
        if (angle < 0)
            angle = -angle;
        if (angle < 100 || angle > 9900)
            *(volatile u32*)ARM_GPIO_GPSET0 = ledMask;
        else
            *(volatile u32*)ARM_GPIO_GPCLR0 = ledMask;
    }
    return ShutdownRebootSC;
}
