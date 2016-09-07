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

/***** System headers *******************************************************/
#include <project.h>
#include <string.h>

/***** Local headers ********************************************************/
#include "uart.h"

/***** Constant values ******************************************************/
/***** Macros ***************************************************************/
/***** Typedefs *************************************************************/
/***** Function prototypes **************************************************/
int uart_check_timeout(void);
void uart_stop_timeout(void);


/***** Global variables *****************************************************/
/***** Local variables ******************************************************/

//! set pointer of UART_HANDER for communicate interrupt handler.
static UART_HANDLER *p_uart_handler;


/***** Interrupt handler ****************************************************/

//================================================================
/*! Tx interrupt handler.

*/
CY_ISR(isr_UART_1_Tx)
{
  UART_HANDLER *uh = p_uart_handler;
  int sts          = UART_1_ReadTxStatus();

  while( sts & UART_1_TX_STS_COMPLETE ) { // not loop, insted of "if"
    if( uh->tx_rd >= uh->size_txbuf ) uh->flag_tx_finished = 1;
    if( uh->flag_tx_finished ) break;

    UART_1_WriteTxData(uh->p_txbuf[uh->tx_rd++]);
    break;
  }
}


//================================================================
/*! Rx interrupt handler.

*/
CY_ISR(isr_UART_1_Rx)
{
  UART_HANDLER *uh = p_uart_handler;
  int sts          = UART_1_ReadRxStatus();

  for(; sts != 0; sts = UART_1_ReadRxStatus()) {
    if( sts & UART_1_RX_STS_FIFO_NOTEMPTY ) {
      uh->rxfifo[uh->rx_wr++] = UART_1_ReadRxData();

      // check rollover write index.
      if( uh->rx_wr < sizeof(uh->rxfifo)) {
        if( uh->rx_wr == uh->rx_rd ) {
          --uh->rx_wr;   // buffer full
        }
      }
      else {
        if( uh->rx_rd == 0 ) {
          --uh->rx_wr;   // buffer full
        }
        else {
          uh->rx_wr = 0; // roll over.
        }
      }
    }

    // and any more check other status?
  }
}


/***** Local functions ******************************************************/
/***** Global functions *****************************************************/

//================================================================
/*! initialize

  @memberof UART_HANDLER
  @param  uh            Pointer of UART_HANDLER.
*/
void uart_init(UART_HANDLER *uh)
{
  uh->p_txbuf          = NULL;
  uh->delimiter        = '\n';
  uh->flag_tx_finished = 1;
  uh->mode             = 0;
  uh->rx_rd            = 0;
  uh->rx_wr            = 0;

  p_uart_handler = uh;

  UART_1_Start();
  UART_1_ClearTxBuffer();
  isr_UART_1_Tx_StartEx(isr_UART_1_Tx);
  UART_1_ClearRxBuffer();
  isr_UART_1_Rx_StartEx(isr_UART_1_Rx);
}


//================================================================
/*! set work mode

  @memberof UART_HANDLER
  @param  uh            Pointer of UART_HANDLER.
  @param  mode          mode.
*/
void uart_set_mode(UART_HANDLER *uh, int mode)
{
  uh->mode = mode;
}


//================================================================
/*! Clear transmit buffer.

  @memberof UART_HANDLER
  @param  uh            Pointer of UART_HANDLER.
*/
void uart_clear_tx_buffer(UART_HANDLER *uh)
{
  UART_1_ClearTxBuffer();
}


//================================================================
/*! Clear receive buffer.

  @memberof UART_HANDLER
  @param  uh            Pointer of UART_HANDLER.
*/
void uart_clear_rx_buffer(UART_HANDLER *uh)
{
  UART_1_ClearRxBuffer();
  uh->rx_rd = 0;
  uh->rx_wr = 0;
}


//================================================================
/*! Transmit binary data.

  @memberof UART_HANDLER
  @param  uh            Pointer of UART_HANDLER.
  @param  buf           Pointer of buffer.
  @param  size          Size of buffer.
  @return               Size of transmitted.
*/
int uart_write(UART_HANDLER *uh, const char *buf, size_t size)
{
  if( !uh->flag_tx_finished ) return 0;  // TODO: or -1 ??
  if( size == 0 ) return 0;

  uh->p_txbuf          = buf;
  uh->size_txbuf       = size;
  uh->tx_rd            = 1;
  uh->flag_tx_finished = 0;

  UART_1_WriteTxData(*buf); // send first byte.
  if( uh->mode & UART_WRITE_NONBLOCK ) return 0;

  do {
    CyPmAltAct(PM_ALT_ACT_TIME_NONE, PM_ALT_ACT_SRC_PICU);
#ifdef UART_CHECK_TIMEOUT
    if( uart_check_timeout()) {
      uart_stop_timeout();
      uh->flag_tx_finished = 1;
      return -1;
    }
#endif
  } while( !uh->flag_tx_finished );

#ifdef UART_CHECK_TIMEOUT
  uart_stop_timeout();
#endif
  return uh->tx_rd;
}


//================================================================
/*! Receive binary data.

  @memberof UART_HANDLER
  @param  uh            Pointer of UART_HANDLER.
  @param  buf           Pointer of buffer.
  @param  size          Size of buffer.
  @return int           Num of received bytes.
*/
int uart_read(UART_HANDLER *uh, char *buf, size_t size)
{
  size_t cnt = size;

  while( 1 ) {
    // buffer is empty?
    if( uh->rx_rd == uh->rx_wr ) {
      CyPmAltAct(PM_ALT_ACT_TIME_NONE, PM_ALT_ACT_SRC_PICU);
#ifdef UART_CHECK_TIMEOUT
      if( uart_check_timeout()) {
        uart_stop_timeout();
        return -1;
      }
#endif
      continue;
    }

    *buf++ = uh->rxfifo[uh->rx_rd++];
    if( uh->rx_rd >= sizeof(uh->rxfifo)) uh->rx_rd = 0;

    if( --cnt == 0 ) break;
  }

#ifdef UART_CHECK_TIMEOUT
  uart_stop_timeout();
#endif
  return size - cnt;
}


//================================================================
/*! Transmit string.

  @memberof UART_HANDLER
  @param  uh            Pointer of UART_HANDLER.
  @param  buf           Pointer of buffer.
  @return               Size of transmitted.
*/
int uart_puts(UART_HANDLER *uh, const char *buf)
{
  return uart_write(uh, buf, strlen(buf));
}


//================================================================
/*! Receive string.

  @memberof UART_HANDLER
  @param  uh            Pointer of UART_HANDLER.
  @param  buf           Pointer of buffer.
  @param  size          Size of buffer.
  @return int           Num of received bytes.
*/
int uart_gets(UART_HANDLER *uh, char *buf, size_t size)
{
  size_t cnt = size - 1;

  while( 1 ) {
    // buffer is empty?
    if( uh->rx_rd == uh->rx_wr ) {
      CyPmAltAct(PM_ALT_ACT_TIME_NONE, PM_ALT_ACT_SRC_PICU);
#ifdef UART_CHECK_TIMEOUT
      if( uart_check_timeout()) {
        uart_stop_timeout();
        *buf = '\0';
        return -1;
      }
#endif
      continue;
    }

    int ch = (*buf++ = uh->rxfifo[uh->rx_rd++]);
    if( uh->rx_rd >= sizeof(uh->rxfifo)) uh->rx_rd = 0;

    if( --cnt == 0 ) break;
    if( ch == uh->delimiter ) break;
  }
  *buf = '\0';

#ifdef UART_CHECK_TIMEOUT
  uart_stop_timeout();
#endif
  return size - cnt - 1;
}


//================================================================
/*! Transmit a character. (1 byte)

  @memberof UART_HANDLER
  @param  uh            Pointer of UART_HANDLER.
  @param  ch            character
  @return               Size of transmitted.
*/
int uart_putc(UART_HANDLER *uh, int ch)
{
  char buf[1];

  buf[0] = ch;
  return uart_write(uh, buf, 1);
}


//================================================================
/*! Receive a character. (1 byte)

  @memberof UART_HANDLER
  @param  uh            Pointer of UART_HANDLER.
  @return int           Received character.
*/
int uart_getc(UART_HANDLER *uh)
{
  char buf[1];

  uart_read(uh, buf, 1);
  return buf[0];
}


//================================================================
/*! check write finished?

  @memberof UART_HANDLER
  @param  uh            Pointer of UART_HANDLER.
  @return int           result (bool)
*/
int uart_is_write_finished(UART_HANDLER *uh)
{
  return uh->flag_tx_finished;
}


//================================================================
/*! check data can be read.

  @memberof UART_HANDLER
  @param  uh            Pointer of UART_HANDLER.
  @return int           result (bool)
*/
int uart_is_readable(UART_HANDLER *uh)
{
  return uh->rx_rd != uh->rx_wr;
}


//================================================================
/*! check data length can be read.

  @memberof UART_HANDLER
  @param  uh            Pointer of UART_HANDLER.
  @return int           result (bytes)
*/
int uart_bytes_available(UART_HANDLER *uh)
{
  if( uh->rx_rd <= uh->rx_wr ) {
    return uh->rx_wr - uh->rx_rd;
  }
  else {
    return sizeof(uh->rxfifo) - uh->rx_rd + uh->rx_wr;
  }
}


//================================================================
/*! check data can be read a line.

  @memberof UART_HANDLER
  @param  uh            Pointer of UART_HANDLER.
  @return int           result (bool)
  @note
   If RX-FIFO buffer is full, return false(0).
*/
int uart_can_read_line(UART_HANDLER *uh)
{
  uint16_t idx = uh->rx_rd;

  while( idx != uh->rx_wr ) {
    if( uh->rxfifo[idx++] == uh->delimiter ) return 1;
    if( idx >= sizeof(uh->rxfifo)) idx = 0;
  }

  return 0;
}
