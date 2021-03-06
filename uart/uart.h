/*! @file
  @brief
  UART wrapper for PSoC5LP

  @version 1.3
  @date 2020/09/05 13:20:17

  <pre>
  Copyright (C) 2016-2020 Shimane IT Open-Innovation Center.
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
#include <string.h>


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
  const uint8_t    *p_txbuf;                  // pointer of tx buffer.
  uint16_t          size_txbuf;               // size of tx buffer.
  volatile uint16_t tx_rd;                    // index of sendout bytes.
  volatile char     flag_tx_finished;
  uint8_t           mode;                     // work mode.

  // for receive.
  uint8_t           rx_overflow;	      // buffer overflow flag.
  uint8_t           delimiter;                //!<@public delimiter of read line (gets). default '\\n'.

  volatile uint16_t rx_rd;                    // index of rxfifo for read.
  volatile uint16_t rx_wr;                    // index of rxfifo for write.
  volatile char     rxfifo[UART_SIZE_RXFIFO]; // FIFO for received data.
} UART_HANDLE;


/***** Global variables *****************************************************/
/***** Function prototypes **************************************************/
void uart_init(UART_HANDLE *uh);
void uart_clear_tx_buffer(UART_HANDLE *uh);
void uart_clear_rx_buffer(UART_HANDLE *uh);
int uart_write(UART_HANDLE *uh, const void *buffer, size_t size);
int uart_read(UART_HANDLE *uh, void *buffer, size_t size);
int uart_gets(UART_HANDLE *uh, char *buf, size_t size);
int uart_read_block(UART_HANDLE *uh, void *buffer, size_t size);
int uart_read_nonblock(UART_HANDLE *uh, void *buffer, size_t size);
int uart_bytes_available(UART_HANDLE *uh);
int uart_can_read_line(UART_HANDLE *uh);


/***** Inline functions *****************************************************/

//================================================================
/*! set work mode

  @memberof UART_HANDLE
  @param  uh            Pointer of UART_HANDLE.
  @param  mode          mode.
*/
static inline void uart_set_mode(UART_HANDLE *uh, int mode)
{
  uh->mode |= mode;
}


//================================================================
/*! Transmit string.

  @memberof UART_HANDLE
  @param  uh            Pointer of UART_HANDLE.
  @param  buf           Pointer of buffer.
  @return               Size of transmitted.
*/
static inline int uart_puts(UART_HANDLE *uh, const char *buf)
{
  return uart_write(uh, buf, strlen(buf));
}


//================================================================
/*! Transmit a character. (1 byte)

  @memberof UART_HANDLE
  @param  uh            Pointer of UART_HANDLE.
  @param  ch            character
  @return               Size of transmitted.
*/
static inline int uart_putc(UART_HANDLE *uh, int ch)
{
  char buf[1];

  buf[0] = ch;
  return uart_write(uh, buf, 1);
}


//================================================================
/*! Receive a character. (1 byte)

  @memberof UART_HANDLE
  @param  uh            Pointer of UART_HANDLE.
  @return int           Received character.
*/
static inline int uart_getc(UART_HANDLE *uh)
{
  char buf[1];

  uart_read(uh, buf, 1);
  return buf[0];
}


//================================================================
/*! check write finished?

  @memberof UART_HANDLE
  @param  uh            Pointer of UART_HANDLE.
  @return int           result (bool)
*/
static inline int uart_is_write_finished(const UART_HANDLE *uh)
{
  return uh->flag_tx_finished;
}


//================================================================
/*! check data can be read.

  @memberof UART_HANDLE
  @param  uh            Pointer of UART_HANDLE.
  @return int           result (bool)
*/
static inline int uart_is_readable(const UART_HANDLE *uh)
{
  return uh->rx_rd != uh->rx_wr;
}


//================================================================
/*! check Rx buffer overflow?

  @memberof UART_HANDLE
  @return int           result (bool)
*/
static inline int uart_is_rx_overflow(const UART_HANDLE *uh)
{
  return uh->rx_overflow;
}



#ifdef __cplusplus
}
#endif
#endif
