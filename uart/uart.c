/*! @file
  @brief
  UART wrapper for PSoC5LP

  @version 0.1
  @date Wed Aug 24 15:59:34 2016

  <pre>
  Copyright (C) 2016 Shimane IT Open-Innovation Center.
  All Rights Reserved.

  This file is distributed under BSD 3-Clause License.

  This is test imprement yet.
  </pre>
*/

/***** System headers *******************************************************/
#include <project.h>

/***** Local headers ********************************************************/
#include "uart.h"

/***** Constant values ******************************************************/
/***** Macros ***************************************************************/
#ifdef UART_CHECK_TIMEOUT
#undef UART_CHECK_TIMEOUT
#define UART_CHECK_TIMEOUT() uart_check_timeout()
#else
#define UART_CHECK_TIMEOUT() 0
#endif


/***** Typedefs *************************************************************/
/***** Function prototypes **************************************************/
int uart_check_timeout( void );


/***** Local variables ******************************************************/
//! set pointer of UART_HANDER for communicate interrupt handler.
static UART_HANDLER *p_uart_handler;


/***** Interrupt handler ****************************************************/
//================================================================
/*! Tx interrupt handler.

*/
CY_ISR( isr_UART1_Tx )
{
  UART_HANDLER *uh = p_uart_handler;
  int sts;

  sts = UART1_ReadTxStatus();

  if( sts & UART1_TX_STS_COMPLETE ) {
    if( uh->tx_rd >= uh->size_txbuf ) {
      uh->flag_tx_finished = 1;
      return;
    }

    UART1_WriteTxData( uh->p_txbuf[ uh->tx_rd++ ] );
  }
}



//================================================================
/*! Rx interrupt handler.

*/
CY_ISR( isr_UART1_Rx )
{
  UART_HANDLER *uh = p_uart_handler;
  int sts;

  for( sts = UART1_ReadRxStatus(); sts != 0; sts = UART1_ReadRxStatus() ) {
    if( sts & UART1_RX_STS_FIFO_NOTEMPTY ) {
      uh->rxfifo[ uh->rx_wr++ ] = UART1_ReadRxData();

      // check rollover write index.
      if( uh->rx_wr < sizeof(uh->rxfifo) ) {
        if( uh->rx_wr == uh->rx_rd ) {
          --uh->rx_wr;      // buffer full
        }
      } else {
        if( uh->rx_rd == 0 ) {
          --uh->rx_wr;      // buffer full
        } else {
          uh->rx_wr = 0;    // roll over.
        }
      }
    }

    // and any more check other status?
  }
}



/***** Global functions *****************************************************/

//================================================================
/*! initialize

  @param  uh            Pointer of UART_HANDLER.
  @retval 0             When success.
*/
int uart_init( UART_HANDLER *uh )
{
  uh->p_txbuf = NULL;
  uh->delimiter = '\n';
  uh->flag_tx_finished = 1;
  uh->rx_rd = 0;
  uh->rx_wr = 0;

  p_uart_handler = uh;

  UART1_Start();
  UART1_ClearRxBuffer();
  UART1_ClearTxBuffer();
  isr_UART1_Rx_StartEx( isr_UART1_Rx );
  isr_UART1_Tx_StartEx( isr_UART1_Tx );

  return 0;
}



//================================================================
/*! Clear receive buffer.

  @param  uh            Pointer of UART_HANDLER.
*/
void uart_clear_tx_buffer( UART_HANDLER *uh )
{
  UART1_ClearTxBuffer();
}



//================================================================
/*! Clear receive buffer.

  @param  uh            Pointer of UART_HANDLER.
*/
void uart_clear_rx_buffer( UART_HANDLER *uh )
{
  UART1_ClearRxBuffer();
  uh->rx_rd = 0;
  uh->rx_wr = 0;
}



//================================================================
/*! Transmit binary data.

  @param  uh            Pointer of UART_HANDLER.
  @param  buf           Pointer of buffer.
  @param  size          Size of buffer.
  @return               Size of transmitted.
*/
int uart_write( UART_HANDLER *uh, const char *buf, size_t size )
{
  if( !uh->flag_tx_finished ) return 0;         // TODO: or -1 ??
  if( *buf == '\0' ) return 0;

  uh->p_txbuf = buf;
  uh->size_txbuf = size;
  uh->tx_rd = 1;
  uh->flag_tx_finished = 0;

  UART1_WriteTxData( *buf );    // send first byte.
  do {
    CyPmAltAct( PM_ALT_ACT_TIME_NONE, PM_ALT_ACT_SRC_PICU );
  } while( !uh->flag_tx_finished );

  return uh->tx_rd;
}



//================================================================
/*! Receive binary data.

  @param  uh            Pointer of UART_HANDLER.
  @param  buf           Pointer of buffer.
  @param  size          Size of buffer.
  @return int           Num of received bytes.
*/
int uart_read( UART_HANDLER *uh, char *buf, size_t size )
{
  size_t cnt = size;

  while( 1 ) {
    // buffer is empty?
    if( uh->rx_rd == uh->rx_wr ) {
      CyPmAltAct( PM_ALT_ACT_TIME_NONE, PM_ALT_ACT_SRC_PICU );
      continue;
    }

    *buf++ = uh->rxfifo[ uh->rx_rd++ ];
    if( uh->rx_rd >= sizeof(uh->rxfifo) ) uh->rx_rd = 0;

    if( --cnt == 0 ) break;
  }

  return size - cnt;
}



//================================================================
/*! Transmit string.

  @param  uh            Pointer of UART_HANDLER.
  @param  buf           Pointer of buffer.
  @return               Size of transmitted.
*/
int uart_puts( UART_HANDLER *uh, const char *buf )
{
  return uart_write( uh, buf, strlen( buf ) );
}





//================================================================
/*! Receive string.

  @param  uh            Pointer of UART_HANDLER.
  @param  buf           Pointer of buffer.
  @param  size          Size of buffer.
  @return int           Num of received bytes.
*/
int uart_gets( UART_HANDLER *uh, char *buf, size_t size )
{
  size_t cnt = size - 1;

  while( 1 ) {
    // buffer is empty?
    if( uh->rx_rd == uh->rx_wr ) {
      CyPmAltAct( PM_ALT_ACT_TIME_NONE, PM_ALT_ACT_SRC_PICU );
      if( UART_CHECK_TIMEOUT() ) {
          *buf = '\0';
          return -1;
      }
      continue;
    }

    int ch = (*buf++ = uh->rxfifo[ uh->rx_rd++ ]);
    if( uh->rx_rd >= sizeof(uh->rxfifo) ) uh->rx_rd = 0;

    if( --cnt == 0 ) break;
    if( ch == uh->delimiter ) break;
  }
  *buf = '\0';

  return size - cnt - 1;
}
