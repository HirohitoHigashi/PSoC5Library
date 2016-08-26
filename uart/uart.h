/*! @file
  @brief
  UART wrapper for PSoC5LP

  @version 0.1
  @date Wed Aug 24 15:59:34 2016

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

//! size of FIFO buffer for receive.
#ifndef UART_SIZE_RXFIFO
#define UART_SIZE_RXFIFO 128
#endif


/***** Macros ***************************************************************/
/***** Typedefs *************************************************************/

//================================================
/*!@brief
  UART Handler
*/
typedef struct UART_HANDLER {
  // for transmit
  const char       *p_txbuf;    // pointer of tx buffer.
  uint16_t          size_txbuf; // size of tx buffer.
  volatile uint16_t tx_rd;      // index of sendout character.
  volatile char     flag_tx_finished;

  // for receive.
  int16_t           delimiter;
  volatile uint16_t rx_rd;
  volatile uint16_t rx_wr;
  volatile char     rxfifo[ UART_SIZE_RXFIFO ];
} UART_HANDLER;



/***** Global variables *****************************************************/


/***** Function prototypes **************************************************/
int uart_init( UART_HANDLER *uh );
void uart_clear_tx_buffer();
void uart_clear_rx_buffer();
int uart_write( UART_HANDLER *uh, const char *buf, size_t size );
int uart_read( UART_HANDLER *uh, char *buf, size_t size );
int uart_puts( UART_HANDLER *uh, const char *buf );
int uart_gets( UART_HANDLER *uh, char *buf, size_t size );


#ifdef __cplusplus
}
#endif
#endif /* ifndef MRUBYC_UARTWRAP_H_ */
