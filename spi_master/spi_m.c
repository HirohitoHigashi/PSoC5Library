/*! @file
  @brief
  SPI master convenience library for PSoC5LP

  @version 1.0
  @date 2019/09/03 14:42:39
  @note This version only supports one interface.

<pre>
  Copyright (C) 2019 Shimane IT Open-Innovation Center.
  All Rights Reserved.

  This file is distributed under BSD 3-Clause License.
</pre>
*/


/***** Feature test switches ************************************************/
/***** System headers *******************************************************/
#include <stdint.h>

/***** Local headers ********************************************************/
#include "spi_m.h"

/***** Constant values ******************************************************/
/***** Macros ***************************************************************/
/***** Typedefs *************************************************************/
/***** Function prototypes **************************************************/
/***** Local variables ******************************************************/
//================================================================
/*! SPI transfer management data.
*/
static struct SPI_HANDLE {
  uint8_t *send_data;
  int send_size;
  int send_total;
  int send_n;

  uint8_t *recv_data;
  int recv_size;
  int recv_n;
} spi_handle;


/***** Global variables *****************************************************/
/***** Signal catching functions ********************************************/

//================================================================
/*! Intterrupt callback on byte transfer complete.
 */
void SPIM_1_TX_ISR_EntryCallback(void) {
  if( spi_handle.send_n < spi_handle.send_size ) {
    SPIM_1_WriteTxData( *spi_handle.send_data++ );
    ++spi_handle.send_n;
    return;
  }

  if( spi_handle.send_n < spi_handle.send_total ) {
    SPIM_1_WriteTxData( 0 );
    ++spi_handle.send_n;
  }
}


//================================================================
/*! Intterrupt callback on Rx FIFO not empty.
 */
void SPIM_1_RX_ISR_EntryCallback(void) {
  do {
    int data = SPIM_1_ReadRxData();

    if( spi_handle.recv_n < spi_handle.recv_size &&
	spi_handle.recv_n++ >= 0 ) {
      *spi_handle.recv_data++ = data;
    }
  } while( SPIM_1_GetRxBufferSize() != 0 );
}


/***** Local functions ******************************************************/
/***** Global functions *****************************************************/

//================================================================
/*! Perform SPI data transfer. (send and receive)

  @param  send_buf	pointer to send data buffer. or NULL.
  @param  send_size	send data size (bytes).
  @param  recv_buf	pointer to receive data buffer. or NULL.
  @param  recv_size	receive data size (bytes).
  @param  flag_include	if this flag true, including receive data when sending data
*/
void spi_transfer( void *send_buf, int send_size,
		   void *recv_buf, int recv_size, int flag_include )
{
  // check idle state.
  while( !(SPIM_1_ReadTxStatus() & SPIM_1_STS_SPI_IDLE))
    ;

  SPIM_1_DisableTxInt();
  SPIM_1_DisableRxInt();
  SPIM_1_ClearFIFO();

  spi_handle.send_data = send_buf;
  spi_handle.send_size = send_size;
  if( flag_include ) {
    spi_handle.send_total = send_size > recv_size ? send_size : recv_size;
  } else {
    spi_handle.send_total = send_size + recv_size;
  }
  spi_handle.send_n = 0;

  spi_handle.recv_data = recv_buf;
  spi_handle.recv_size = recv_buf ? recv_size : 0;
  spi_handle.recv_n = flag_include ? 0 : -send_size;

  // send SPIM_1_FIFO_SIZE (maybe 4) byte continuously.
  while( spi_handle.send_n < spi_handle.send_size ) {
    SPIM_1_WriteTxData( *spi_handle.send_data++ );
    if( ++spi_handle.send_n >= SPIM_1_FIFO_SIZE ) goto DONE;
  }
  while( spi_handle.send_n < spi_handle.send_total ) {
    SPIM_1_WriteTxData( 0 );
    if( ++spi_handle.send_n >= SPIM_1_FIFO_SIZE ) goto DONE;
  }

 DONE:
  SPIM_1_EnableTxInt();
  SPIM_1_EnableRxInt();
}
