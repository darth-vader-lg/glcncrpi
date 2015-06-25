#include "encoder.h"

volatile unsigned CEncoder::m_gpios = 0;

/// <summary>
/// Costruttore
/// </summary>
/// <param name="cha">Ingresso canale A.</param>
/// <param name="chb">Ingresso canale B.</param>
/// <param name="chz">Ingresso canale Z.</param>
/// <param name="pGPIOManager">Eventuale manager degli I/O.</param>
CEncoder::CEncoder(int cha, int chb, int chz, CGPIOManager* pGPIOManager) :
    m_pina(cha, GPIOModeInput, pGPIOManager),
    m_pinb(chb, GPIOModeInput, pGPIOManager),
    m_pinz(chz, GPIOModeInput, 0/*pGPIOManager*/)
{
    m_counter = 0;
    m_a = m_b = m_z = m_aPrev = m_bPrev = m_zPrev = 0;
    m_maska = (unsigned)1 << cha;
    m_maskb = (unsigned)1 << chb;
    m_maskz = (unsigned)1 << chz;
#ifdef INTERRUPTS
    if (pGPIOManager) {
        m_pina.ConnectInterrupt(EdgeA, this);
        m_pina.EnableInterrupt(GPIOInterruptOnRisingEdge);
        m_pinb.ConnectInterrupt(EdgeB, this);
        m_pinb.EnableInterrupt(GPIOInterruptOnRisingEdge);
//        m_pinz.ConnectInterrupt(CEncoder::DecodeInterrupt, &m_pinz);
//        m_pinz.EnableInterrupt(GPIOInterruptOnRisingEdge);
//        m_pinz.EnableInterrupt(GPIOInterruptOnFallingEdge);
    }
#endif

}

/// <summary>
/// Effettua la decodifica dei canali ed incrementa/decrementa il contatore
/// </summary>
void CEncoder::Decode(void)
{
    m_a = (m_gpios & m_maska) != 0;
    m_b = (m_gpios & m_maskb) != 0;
    m_z = m_gpios & m_maskz;
    if (m_a != m_aPrev)
        m_counter += m_a == m_b ? 1: -1;
    if (m_b != m_bPrev)
        m_counter += m_b == m_a ? -1: 1;
//        if (m_z && ! m_zPrev)
//            m_counter = 0;
    m_aPrev = m_a;
    m_bPrev = m_b;
    m_zPrev = m_z;
}

#ifdef INTERRUPTS
/// <summary>
/// Gestore interrupt su fronte del canale A
/// </summary>
/// <param name="pEncoder">Encoder da cui viene generato l'interrupt</param>
void CEncoder::EdgeA(void* pEncoder)
{
    CEncoder& encoder = *(CEncoder*)pEncoder;
    if (!encoder.m_a) {
        encoder.m_a = 1;
        encoder.m_counter += encoder.m_a == encoder.m_b ? 1: -1;
        encoder.m_pina.DisableInterrupt();
        encoder.m_pina.EnableInterrupt(GPIOInterruptOnFallingEdge);
    }
    else {
        encoder.m_a = 0;
        encoder.m_counter += encoder.m_a == encoder.m_b ? 1: -1;
        encoder.m_pina.DisableInterrupt();
        encoder.m_pina.EnableInterrupt(GPIOInterruptOnRisingEdge);
    }
}
#endif

#ifdef INTERRUPTS
/// <summary>
/// Gestore interrupt su fronte del canale B
/// </summary>
/// <param name="pEncoder">Encoder da cui viene generato l'interrupt</param>
void CEncoder::EdgeB(void* pEncoder)
{
    CEncoder& encoder = *(CEncoder*)pEncoder;
    if (!encoder.m_b) {
        encoder.m_b = 1;
        encoder.m_counter += encoder.m_b == encoder.m_a ? -1: 1;
        encoder.m_pinb.DisableInterrupt();
        encoder.m_pinb.EnableInterrupt(GPIOInterruptOnFallingEdge);
    }
    else {
        encoder.m_b = 0;
        encoder.m_counter += encoder.m_b == encoder.m_a ? -1: 1;
        encoder.m_pinb.DisableInterrupt();
        encoder.m_pinb.EnableInterrupt(GPIOInterruptOnRisingEdge);
    }
}
#endif
