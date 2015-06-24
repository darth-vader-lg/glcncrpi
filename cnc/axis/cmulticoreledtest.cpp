#include <circle/bcm2835.h>
#include <circle/gpiopin.h>
#include <circle/timer.h>
#include "cmulticoreledtest.h"
#include "encoder.h"

CMulticoreLedTest::CMulticoreLedTest(CMemorySystem *pMemorySystem) :
    CMultiCoreSupport(pMemorySystem)
{
}

void CMulticoreLedTest::Run(unsigned nCore)
{
    switch (nCore) {
    case 0: {
        CGPIOPin led(17, GPIOModeOutput);
        led.SetMode(GPIOModeOutput);
        CEncoder encoder0(6, 6, 13);
        CEncoder encoder1(6, 6, 13);
        CEncoder encoder2(6, 6, 13);
        CEncoder encoder3(6, 6, 13);
        CEncoder encoder4(6, 6, 13);
        CEncoder encoder5(6, 6, 13);
        //unsigned ledMask = (unsigned)1 << 17;
        int angle;
        while (true) {
            CEncoder::m_gpios = *(volatile u32*)ARM_GPIO_GPLEV0;
            encoder0.Decode();
            encoder1.Decode();
            encoder2.Decode();
            encoder3.Decode();
            encoder4.Decode();
            encoder5.Decode();
            angle = encoder0.m_counter % 10000;
            if (angle < 0)
                angle = -angle;
//            if (angle < 100 || angle > 9900)
//                *(volatile u32*)ARM_GPIO_GPSET0 = ledMask;
//            else
//                *(volatile u32*)ARM_GPIO_GPCLR0 = ledMask;
        }
        break;
    }
    case 1: {
//        CGPIOPin led(17, GPIOModeOutput);
//        for (int i = 0; i < 10; i++) {
//            led.Write(HIGH);
//            CTimer::SimpleMsDelay(100);
//            led.Write(LOW);
//            CTimer::SimpleMsDelay(100);
//            led.Write(HIGH);
//            CTimer::SimpleMsDelay(100);
//            led.Write(LOW);
//            CTimer::SimpleMsDelay(500);
//        }
        CGPIOPin led(17, GPIOModeOutput);
        led.SetMode(GPIOModeOutput);
        CEncoder encoder0(27, 22, 18);
        CEncoder encoder1(27, 22, 18);
        CEncoder encoder2(27, 22, 18);
        CEncoder encoder3(27, 22, 18);
        CEncoder encoder4(27, 22, 18);
        CEncoder encoder5(27, 22, 18);
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
            angle = encoder0.m_counter % 10000;
            if (angle < 0)
                angle = -angle;
            if (angle < 100 || angle > 9900)
                *(volatile u32*)ARM_GPIO_GPSET0 = ledMask;
            else
                *(volatile u32*)ARM_GPIO_GPCLR0 = ledMask;
        }
        break;
    }
    case 2: {
        break;
    }
    case 3: {
        break;
    }

    }
}
