/*
 * usart.h
 *
 *  Created on: May 26, 2024
 *      Author: kevin
 */

#ifndef USART_H_
#define USART_H_

#include <common.h>
#include <system.h>


class USART
{
	public:
		enum UsartInstance : uint32_t
		{
			Usart1 = USART1_BASE,
			Usart2 = USART2_BASE,
		};

		enum Parity : uint8_t
		{
			ParityEven = 0x00, ParityOdd = 0x01
		};

		typedef enum
		{
			Disabled,
			Enabled
		} ParityControlState, UsartState, InterruptState, TransmitterState,
				ReceiverState, LinModeState, CLKPinState,
				LastBitClockPulseState, CtsState, RtsState, DmaRequestState,
				SmartCardModeState, SmartCardNackState, HalfDuplexState,
				IrdaModeState;

		typedef enum
		{
			Start1_Data8_StopN = 0x00, Start1_Data9_StopN = 0x01
		} WordLength;

		typedef enum
		{
			ParityErrorInterrupt = USART_CR1_PEIE,
			TransmitDataRegisterEmptyInterrupt = USART_CR1_TXEIE,
			TransmissionCompleteInterrupt = USART_CR1_TCIE,
			ReceivedDataReadyToReadInterrupt = USART_CR1_RXNEIE,
			IdleInterrupt = USART_CR1_IDLEIE
		} CR1Interrupt;

		typedef enum BaudRate : uint32_t
		{
			BR_9600 = 9600, BR_115200 = 115200
		} baud_rate_t;

		typedef enum
		{
			IdleLine = 0x00, AddressMark = 0x01
		} WakeupMethod;

		/* CR2 */

		/* Program number of stop bits */
		typedef enum
		{
			StopBits1, StopBits0_5, StopBits2, StopBits1_5
		} StopBits;

		/* Polarity of clock output on CK pin in synchronous mode */
		typedef enum
		{
			CPOL_LOW, CPOL_HIGH
		} ClockPolarity;

		/* Phase of the clock output on the CK pin in synchronous mode */
		typedef enum
		{
			CPHA_FirstTransition, CPHA_SecondTransition
		} ClockPhase;

		typedef enum
		{
			BreakDetection10Bit, BreakDetection11Bit
		} LinBreakDetectionLength;

		typedef enum
		{
			LinBreakDetectionInterrupt = USART_CR2_LBDIE
		} CR2Interrupt;

		/* CR3 */

		typedef enum
		{
			CtsInterrupt = USART_CR3_CTSIE, ErrorInterrupt = USART_CR3_EIE
		} Cr3Interrupt;

		typedef enum
		{
			TransmitterRequest = USART_CR3_DMAT,
			ReceiverRequest = USART_CR3_DMAR
		} DmaRequest;

		typedef enum
		{
			Normal, Low
		} IrdaPowerLevel;

		USART( UsartInstance usart );
		~USART( );

		/**
		 * @brief Get reference to a USART instance
		 *
		 * @param usart required hardware usart instance
		 * @return void* pointer to the USART object
		 */
		void* operator new( size_t, UsartInstance usart )
		{
			return reinterpret_cast < void* >( usart );
		}

		/**
		 * @brief Enable or disable USART peripheral
		 * When USART is disabled, the prescalers and outputs are stopped at the end of the current
		 * byte transfer in order to reduce power consumption. This bit is set and cleared by software.
		 * @param state desired state
		 */
		void setUsartState( UsartState state );

		/**
		 * @brief Set the Word Length for data transfer
		 * @param m desired word length
		 * @note Word length must not be modified during a data transfer (Both reception and transmission).
		 */
		void setWordLength( WordLength m );

		/**
		 * @brief Sets the USART Wakeup method
		 *
		 * @param method desired wakeup method
		 */
		void setWakeupMethod( WakeupMethod method );

		/**
		 * @brief Sets the hardware parity control state. When the parity control is enabled,
		 * the computed parity is inserted at the MSB position (9th bit if M=1; 8th bit if M=0) and parity is checked on the received data.
		 * PCE is active after the current byte (in reception and in transmission).
		 * @param state desired parity control state
		 */
		void setParityControl( ParityControlState state );

		/**
		 * @brief This selects the odd or even parity when the parity generation/detection is enabled (PCE bit set).
		 * Parity will be selected after the current byte.
		 * @param parity desired parity
		 */
		void setParity( Parity parity );

		/**
		 * @brief Enable/Disable interrupts for USART (CR1 register)
		 *
		 * @param intr interrupt
		 * @param state target state of the interrupt
		 */
		void setInterruptState( CR1Interrupt intr, InterruptState state );

		/**
		 * @brief Enable/Disable transmitter circuit
		 *
		 * @param state desired state of transmitter
		 */
		void setTransmitterState( TransmitterState state );

		/**
		 * @brief Enable/Disable Receiver circuit
		 *
		 * @param state desired state of Receiver
		 * when enabled receive will start searching for the start bit
		 */
		void setReceiverState( ReceiverState state );

		/**
		 * @brief Set the baudrate for communication, (115200 is the max baudrate supported without flow-control)
		 * @param baudrate desired baudrate
		 */
		void set_baudrate( baud_rate_t baudrate );

		/* CR2 register methods */
		/**
		 * @brief Set the Lin Mode State
		 * @param state desired state of LIN mode
		 */
		void setLinModeState( LinModeState state );

		/**
		 * @brief Set the Stop Bits
		 * @param nStop number os stop bits
		 */
		void setStopBits( StopBits nStop );

		/**
		 * @brief Set the clock pin state, enable it to output clock on CK pin in synchronous mode
		 * @param state desired clock pin state
		 */
		void setClockPinState( CLKPinState state );

		/**
		 * @brief Set the phase of the clock output in the CK pin in synchronous mode
		 * This works in conjunction with CPOL to produce the desired clock/data relationship
		 * @param phase
		 */
		void setClockPhase( ClockPhase phase );

		/**
		 * @brief Set the polarity of the clock output on the CK pin in synchronous mode.
		 * It works in conjunction with the CPHA bit to produce the desired clock/data relationship
		 *
		 * @param polarity desired clock polarity
		 */
		void setClockPolarity( ClockPolarity polarity );

		/**
		 * @brief Set weather the clock pulse associated with the last data bit transmitted (MSB) has to be output in synchronous mode.
		 * The last bit is the 8th or 9th data bit transmitted depending on the 8 or 9 bit data format selected by the setWordLength() method.
		 * @param state Enabled / Disabled
		 */
		void setLastBitClockPulseState( LastBitClockPulseState state );

		/**
		 * @brief set the LIN break detection length
		 * @param length desired break detection length
		 */
		void setLINBreakDetectionLength( LinBreakDetectionLength length );

		/**
		 * @brief Enable/Disable interrupts for USART (CR2 register)
		 *
		 * @param intr interrupt
		 * @param state target state of the interrupt
		 */
		void setInterruptState( CR2Interrupt intr, InterruptState state );

		/**
		 * @brief Set the Address Of Usart Node object
		 * Address is used in multiprocessor communication during mute mode, for wakeup with address mark detection
		 * @param addr
		 */
		void setAddressOfUsartNode( uint8_t addr );

		/* CR3 register methods */
		/**
		 * @brief Set the Interrupt State for interrupts in CR3 register
		 * @param intr interrupt in CR3 register
		 * @param state Enabled / Disabled state of the register
		 */
		void setInterruptState( Cr3Interrupt intr, InterruptState state );

		/**
		 * @brief Set hardware flow control
		 * When enabled, data is only transmitted when CTS input is asserted (tied to 0).
		 * If the CTS input is deserted while the data is being transmitted, then the transmitted before stopping.
		 * If a data is written into the data register while CTS is deserted, the transmission is postpone until the CTS is reasserted.
		 * @param state Enabled / Disabled
		 */
		void setCtsState( CtsState state );

		/**
		 * @brief Set the Rts State object
		 * When enabled, data is only requested when there is space in the receive buffer.
		 * The transmission of ddata is expected to cease after currect chracter has been transmitted.
		 * The RTS signal is asserted (Tied to 0) when a data can be received.
		 * @param state Enabled / Disabled
		 */
		void setRtsState( RtsState state );

		/**
		 * @brief Set the Dma Request State for USART
		 * When enabled the transmitter will request DMA for the data to be transmitted
		 * The receiveer will request the DMA to read the received data
		 * @param dma_request Transmitter or receiver request
		 * @param state Enabled/Disabled
		 */
		void setDmaRequestState( DmaRequest dma_request,
				DmaRequestState state );

		/**
		 * @brief Set the Smart Card Mode State
		 *
		 * @param state Enabled / Disabled
		 */
		void setSmartCardModeState( SmartCardModeState state );

		/**
		 * @brief Set the Smart Card NACK State
		 *
		 * @param state Enabled / Disabled
		 */
		void setSmartCardNackState( SmartCardNackState state );

		/**
		 * @brief Set the Half duplex State
		 *
		 * @param state Enabled / Disabled
		 */
		void setHaldDuplexState( HalfDuplexState state );

		/**
		 * @brief Set the IrDA Mode State
		 *
		 * @param state Enabled / Disabled
		 */
		void setIrdaModeState( IrdaModeState state );

		/**
		 * @brief Set the Irda Power Level
		 *
		 * @param level Normal power level or low power level
		 */
		void setIrdaPowerLevel( IrdaPowerLevel level );

		/**
		 * @brief Transmit character
		 * If data register is not empty then character will be transmitted after current transaction
		 * @param ch character to be transmitted
		 */
		void tx_char( uint8_t ch );

		/**
		 * @brief Transmit string
		 *
		 * @param str pointer to NULL terminated string
		 */
		void tx_str( const char *str );

		/**
		 * @brief Receive a character
		 * The function will wait for the receiver to receive a charater and then return the received data
		 * @return uint8_t
		 */
		uint16_t rx_char( );

	private:
		USART_TypeDef *usart_; // Pointer to the USART peripheral
};




#endif /* USART_H_ */
