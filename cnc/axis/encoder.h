#ifndef ENCODER_H
#define ENCODER_H

#include <circle/gpiopin.h>
#include <circle/gpiomanager.h>

class CEncoder
{
public:
    /// <summary>
    /// Costruttore
    /// </summary>
    /// <param name="cha">Ingresso canale A.</param>
    /// <param name="chb">Ingresso canale B.</param>
    /// <param name="chz">Ingresso canale Z.</param>
    /// <param name="pGPIOManager">Eventuale manager degli I/O.</param>
    CEncoder(int cha, int chb, int chz, CGPIOManager* pGPIOManager = 0);
public:
    /// <summary>
    /// Effettua la decodifica dei canali ed incrementa/decrementa il contatore
    /// </summary>
    void Decode(void);
#ifdef INTERRUPTS
private:
    /// <summary>
    /// Gestore interrupt su fronte del canale A
    /// </summary>
    /// <param name="pEncoder">Encoder da cui viene generato l'interrupt</param>
    static void EdgeA(void* pEncoder);
private:
    /// <summary>
    /// Gestore interrupt su fronte del canale B
    /// </summary>
    /// <param name="pEncoder">Encoder da cui viene generato l'interrupt</param>
    static void EdgeB(void* pEncoder);
#endif
public:
    /// <summary>
    /// Contatore di fronti up/down
    /// </summary>
    int m_counter;
    /// <summary>
    /// Stato dei primi 32 gpio
    /// </summary>
    static volatile unsigned m_gpios;
public:
    /// <summary>
    /// Pin canale A
    /// </summary>
    CGPIOPin m_pina;
    /// <summary>
    /// Pin canale B
    /// </summary>
    CGPIOPin m_pinb;
    /// <summary>
    /// Pin canale Z
    /// </summary>
    CGPIOPin m_pinz;
    /// <summary>
    /// Stato canale A
    /// </summary>
    unsigned m_a;
    /// <summary>
    /// Stato canale B
    /// </summary>
    unsigned m_b;
    /// <summary>
    /// Stato canale Z
    /// </summary>
    unsigned m_z;
    /// <summary>
    /// Stato precedente canale A
    /// </summary>
    unsigned m_aPrev;
    /// <summary>
    /// Stato precedente canale B
    /// </summary>
    unsigned m_bPrev;
    /// <summary>
    /// Stato precedente canale Z
    /// </summary>
    unsigned m_zPrev;
    /// <summary>
    /// Maschera del bit del canale A sullo stato dei gpio
    /// </summary>
    unsigned m_maska;
    /// <summary>
    /// Maschera del bit del canale B sullo stato dei gpio
    /// </summary>
    unsigned m_maskb;
    /// <summary>
    /// Maschera del bit del canale Z sullo stato dei gpio
    /// </summary>
    unsigned m_maskz;
};

#endif // ENCODER_H
