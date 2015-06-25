#ifndef CMULTICORELEDTEST_H
#define CMULTICORELEDTEST_H

#include <circle/actled.h>
#include <circle/multicore.h>

class CMulticoreLedTest : public CMultiCoreSupport
{
public:
    CMulticoreLedTest(CMemorySystem* pMemorySystem);
#ifndef ARM_ALLOW_MULTI_CORE
    boolean Initialize (void)	{ return TRUE; }
#endif

    void Run (unsigned nCore);
private:
    CActLED m_ActLED;
};

#endif // CMULTICORELEDTEST_H
