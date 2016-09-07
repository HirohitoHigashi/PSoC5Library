/*! @file
  @brief
  UART wrapper for PSoC5LP. Multi component version.

  @version 1.0
  @date Wed Sep  7 16:19:45 2016

  <pre>
  Copyright (C) 2016 Shimane IT Open-Innovation Center.
  All Rights Reserved.

  This file is distributed under BSD 3-Clause License.
  </pre>
*/

#ifndef MRUBYC_UARTWRAP2_H_
#define MRUBYC_UARTWRAP2_H_
#ifdef __cplusplus
extern "C" {
#endif

/***** System headers *******************************************************/
#include <stdint.h>


/***** Local headers ********************************************************/
/***** Constant values ******************************************************/
#define UART_WRITE_NONBLOCK 0x01

//! size of FIFO buffer for receive.
#ifndef UART_SIZE_RXFIFO
# define UART_SIZE_RXFIFO   128
#endif


/***** Macros ***************************************************************/

//! Convenience macro to define the interrupt handler for TX only.
#define UART_ISR_TX(uh, NAME)   \
  CY_ISR(isr_ ## NAME ## _Tx) { \
    uart_isr_tx(uh);            \
  }

//! Convenience macro to define the interrupt handler for RX only.
#define UART_ISR_RX(uh, NAME)   \
  CY_ISR(isr_ ## NAME ## _Rx) { \
    uart_isr_rx(uh);            \
  }

//! Convenience macro to define the interrupt handler for Full UART (TX + RX)
#define UART_ISR(uh, NAME) \
  UART_ISR_TX(uh, NAME)    \
  UART_ISR_RX(uh, NAME)


//! Initializer macro for Full UART (TX + RX)
#define uart_init(uh, NAME)                           \
  do {                                                \
    uart_init_m(uh,                                   \
                NAME ## _TX_STS_COMPLETE,             \
                NAME ## _RX_STS_FIFO_NOTEMPTY,        \
                NAME ## _Start,                       \
                NAME ## _Stop,                        \
                NAME ## _ClearTxBuffer,               \
                NAME ## _ClearRxBuffer,               \
                NAME ## _ReadTxStatus,                \
                NAME ## _ReadRxStatus,                \
                NAME ## _WriteTxData,                 \
                NAME ## _ReadRxData);                 \
    isr_ ## NAME ## _Tx_StartEx(isr_ ## NAME ## _Tx); \
    isr_ ## NAME ## _Rx_StartEx(isr_ ## NAME ## _Rx); \
  } while( 0 )


//! Initializer macro for TX only.
#define uart_init_tx(uh, NAME)                        \
  do {                                                \
    uart_init_m(uh,                                   \
                NAME ## _TX_STS_COMPLETE,             \
                0,                                    \
                NAME ## _Start,                       \
                NAME ## _Stop,                        \
                NAME ## _ClearTxBuffer,               \
                0,                                    \
                NAME ## _ReadTxStatus,                \
                0,                                    \
                NAME ## _WriteTxData,                 \
                0);                                   \
    isr_ ## NAME ## _Tx_StartEx(isr_ ## NAME ## _Tx); \
  } while( 0 )


//! Initializer macro for RX only.
#define uart_init_rx(uh, NAME)                        \
  do {                                                \
    uart_init_m(uh,                                   \
                0,                                    \
                NAME ## _RX_STS_FIFO_NOTEMPTY,        \
                NAME ## _Start,                       \
                NAME ## _Stop,                        \
                0,                                    \
                NAME ## _ClearRxBuffer,               \
                0,                                    \
                NAME ## _ReadRxStatus,                \
                0,                                    \
                NAME ## _ReadRxData);                 \
    isr_ ## NAME ## _Rx_StartEx(isr_ ## NAME ## _Rx); \
  } while( 0 )

/***** Typedefs *************************************************************/

//================================================
/*!@brief
  UART Handler
*/
typedef struct UART_HANDLER {
  //! @privatesection
  // for transmit
  const char       *p_txbuf;                  // pointer of tx buffer.
  uint16_t          size_txbuf;               // size of tx buffer.
  volatile uint16_t tx_rd;                    // index of sendout bytes.
  volatile char     flag_tx_finished;
  uint8_t           mode;                     // work mode.

  // for receive.
  int16_t           delimiter;                //!<@public delimiter of read line (gets). default '\\n'.
  volatile uint16_t rx_rd;                    // index of rxfifo for read.
  volatile uint16_t rx_wr;                    // index of rxfifo for write.
  volatile char     rxfifo[UART_SIZE_RXFIFO]; // FIFO for received data.

  // constant table
  uint8_t TX_STS_COMPLETE;
  uint8_t RX_STS_FIFO_NOTEMPTY;

  // function table
  void (*Start)(void);
  void (*Stop)(void);
  void (*ClearTxBuffer)(void);
  void (*ClearRxBuffer)(void);
  uint8_t (*ReadRxStatus)(void);
  uint8_t (*ReadTxStatus)(void);
  void (*WriteTxData)(uint8_t);
  uint8_t (*ReadRxData)(void);
} UART_HANDLER;


/***** Global variables *****************************************************/
/***** Function prototypes **************************************************/
void uart_isr_tx(UART_HANDLER *uh);
void uart_isr_rx(UART_HANDLER *uh);
void uart_init_m(UART_HANDLER *uh,
                 uint8_t       TX_STS_COMPLETE,
                 uint8_t       RX_STS_FIFO_NOTEMPTY,
                 void         *Start,
                 void         *Stop,
                 void         *ClearTxBuffer,
                 void         *ClearRxBuffer,
                 void         *ReadTxStatus,
                 void         *ReadRxStatus,
                 void         *WriteTxData,
                 void         *ReadRxData);
void uart_set_mode(UART_HANDLER *uh, int mode);
void uart_clear_tx_buffer(UART_HANDLER *uh);
void uart_clear_rx_buffer(UART_HANDLER *uh);
int uart_write(UART_HANDLER *uh, const char *buf, size_t size);
int uart_read(UART_HANDLER *uh, char *buf, size_t size);
int uart_puts(UART_HANDLER *uh, const char *buf);
int uart_gets(UART_HANDLER *uh, char *buf, size_t size);
int uart_putc(UART_HANDLER *uh, int ch);
int uart_getc(UART_HANDLER *uh);

int uart_is_write_finished(UART_HANDLER *uh);
int uart_is_readable(UART_HANDLER *uh);
int uart_bytes_available(UART_HANDLER *uh);
int uart_can_read_line(UART_HANDLER *uh);


#ifdef __cplusplus
}
#endif
#endif // ifndef MRUBYC_UARTWRAP2_H_
