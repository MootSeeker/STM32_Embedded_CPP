/*
 * usart.cpp
 *
 *  Created on: May 26, 2024
 *      Author: kevin
 */



#include "usart.h"

USART::USART(UsartInstance usart)
{
    usart_ = reinterpret_cast<USART_TypeDef *>(usart);
}

USART::~USART()
{
    // Destructor implementation (if needed)
}

void USART::setUsartState(UsartState state)
{
    if (state == Enabled)
    {
        usart_->CR1 |= USART_CR1_UE;
    }
    else
    {
        usart_->CR1 &= ~USART_CR1_UE;
    }
}

void USART::setWordLength(WordLength m)
{
    if (m == Start1_Data9_StopN)
    {
        usart_->CR1 |= USART_CR1_M;
    }
    else
    {
        usart_->CR1 &= ~USART_CR1_M;
    }
}

void USART::setWakeupMethod(WakeupMethod method)
{
    if (method == AddressMark)
    {
        usart_->CR1 |= USART_CR1_WAKE;
    }
    else
    {
        usart_->CR1 &= ~USART_CR1_WAKE;
    }
}

void USART::setParityControl(ParityControlState state)
{
    if (state == Enabled)
    {
        usart_->CR1 |= USART_CR1_PCE;
    }
    else
    {
        usart_->CR1 &= ~USART_CR1_PCE;
    }
}

void USART::setParity(Parity parity)
{
    if (parity == ParityOdd)
    {
        usart_->CR1 |= USART_CR1_PS;
    }
    else
    {
        usart_->CR1 &= ~USART_CR1_PS;
    }
}

void USART::setInterruptState(CR1Interrupt intr, InterruptState state)
{
    if (state == Enabled)
    {
        usart_->CR1 |= intr;
    }
    else
    {
        usart_->CR1 &= ~intr;
    }
}

void USART::setTransmitterState(TransmitterState state)
{
    if (state == Enabled)
    {
        usart_->CR1 |= USART_CR1_TE;
    }
    else
    {
        usart_->CR1 &= ~USART_CR1_TE;
    }
}

void USART::setReceiverState(ReceiverState state)
{
    if (state == Enabled)
    {
        usart_->CR1 |= USART_CR1_RE;
    }
    else
    {
        usart_->CR1 &= ~USART_CR1_RE;
    }
}

void USART::set_baudrate(baud_rate_t baudrate)
{
    // Calculate and set the baud rate
	uint32_t usartdiv = 64000000 / (8*2*115200);
	usart_->BRR = usartdiv;
}

void USART::setStopBits(StopBits nStop)
{
    usart_->CR2 &= ~USART_CR2_STOP; // Clear existing stop bits
    usart_->CR2 |= (nStop << USART_CR2_STOP_Pos); // Set new stop bits
}


void USART::tx_char(uint8_t ch)
{
    while (!(usart_->ISR & USART_ISR_TXE));
    usart_->TDR = ch;
}

void USART::tx_str(const char *str)
{
    while (*str)
    {
        tx_char(*str++);
    }
}

uint16_t USART::rx_char()
{
    while (!(usart_->ISR & USART_ISR_RXNE));
    return usart_->RDR;
}

