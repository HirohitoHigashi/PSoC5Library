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
#ifndef	PSOC5_SPIMWRAP_H_
#define	PSOC5_SPIMWRAP_H_

#ifdef __cplusplus
extern "C" {
#endif

/***** System headers *******************************************************/
#include "project.h"


/***** Local headers ********************************************************/
/***** Constant values ******************************************************/
/***** Macros ***************************************************************/
/***** Typedefs *************************************************************/
/***** Global variables *****************************************************/
/***** Function prototypes **************************************************/
void spi_transfer(void *send_buf, int send_size, void *recv_buf, int recv_size, int flag_include);


/***** Inline functions *****************************************************/

//================================================================
/*! initialize
*/
static inline void spi_init(void)
{
  SPIM_1_Start();
}



//================================================================
/*! Wait for SPI transfer to done.
*/
static inline void spi_wait_done(void)
{
  while( !(SPIM_1_ReadTxStatus() & SPIM_1_STS_SPI_IDLE))
    ;
}



//================================================================
/*! Is an SPI transfer in progress?

  @return int	true or false
*/
static inline int spi_is_transfer(void)
{
  return !(SPIM_1_ReadTxStatus() & SPIM_1_STS_SPI_IDLE);
}



#ifdef __cplusplus
}
#endif
#endif
