/*! @file
  @brief
  UART wrapper for PSoC5LP

  @version 1.2
  @date Sun Jun  3 19:54:35 2018

  <pre>
  Copyright (C) 2016-2018 Shimane IT Open-Innovation Center.
  All Rights Reserved.

  This file is distributed under BSD 3-Clause License.
  </pre>
*/

#ifndef PSOC5_UARTWRAP_H_
#define PSOC5_UARTWRAP_H_
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
# define UART_SIZE_RXFIFO 128
#endif


/***** Macros ***************************************************************/
/***** Typedefs *************************************************************/

//================================================
/*!@brief
  UART Handle
*/
typedef struct UART_HANDLE {
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
} UART_HANDLE;


/***** Global variables *****************************************************/


/***** Function prototypes **************************************************/
void uart_isr_tx(UART_HANDLE *uh);
void uart_isr_rx(UART_HANDLE *uh);
void uart_init(UART_HANDLE *uh);
void uart_set_mode(UART_HANDLE *uh, int mode);
void uart_clear_tx_buffer(UART_HANDLE *uh);
void uart_clear_rx_buffer(UART_HANDLE *uh);
int uart_write(UART_HANDLE *uh, const char *buf, size_t size);
int uart_read(UART_HANDLE *uh, char *buf, size_t size);
int uart_puts(UART_HANDLE *uh, const char *buf);
int uart_gets(UART_HANDLE *uh, char *buf, size_t size);
int uart_putc(UART_HANDLE *uh, int ch);
int uart_getc(UART_HANDLE *uh);

int uart_is_write_finished(UART_HANDLE *uh);
int uart_is_readable(UART_HANDLE *uh);
int uart_bytes_available(UART_HANDLE *uh);
int uart_can_read_line(UART_HANDLE *uh);


#ifdef __cplusplus
}
#endif
#endif
