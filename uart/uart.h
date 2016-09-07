/*! @file
  @brief
  UART wrapper for PSoC5LP

  @version 1.1
  @date Wed Sep  7 11:38:17 2016

  <pre>
  Copyright (C) 2016 Shimane IT Open-Innovation Center.
  All Rights Reserved.

  This file is distributed under BSD 3-Clause License.
  </pre>
*/

#ifndef MRUBYC_UARTWRAP_H_
#define MRUBYC_UARTWRAP_H_
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
} UART_HANDLER;


/***** Global variables *****************************************************/


/***** Function prototypes **************************************************/
void uart_isr_tx(UART_HANDLER *uh);
void uart_isr_rx(UART_HANDLER *uh);
void uart_init(UART_HANDLER *uh);
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
#endif /* ifndef MRUBYC_UARTWRAP_H_ */
