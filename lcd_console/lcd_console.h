/*! @file
  @brief
  LCD console library
  Emulate character console (display only).

  @version 1.0
  @date Sun Aug 28 15:06:32 2016

  <pre>
  Copyright (C) 2016 Shimane IT Open-Innovation Center.
  All Rights Reserved.

  This file is distributed under BSD 3-Clause License.
  </pre>
*/

#ifndef MRUBYC_LCDCONSOLE_H_
#define MRUBYC_LCDCONSOLE_H_
#ifdef __cplusplus
extern "C" {
#endif

/***** System headers *******************************************************/
#include <stdint.h>

/***** Local headers ********************************************************/
/***** Constant values ******************************************************/
/***** Macros ***************************************************************/
/***** Typedefs *************************************************************/
#ifndef LCDCON_WIDTH
#define LCDCON_WIDTH 20
#endif
#ifndef LCDCON_HEIGHT
#define LCDCON_HEIGHT 4
#endif
#ifndef LCDCON_SCROLL_HEIGHT
#define LCDCON_SCROLL_HEIGHT 20
#endif
#if LCDCON_HEIGHT > LCDCON_SCROLL_HEIGHT
#error Must be LCDCON_HEIGHT > LCDCON_SCROLL_HEIGHT
#endif
#ifndef LCDCON_TABWIDTH
#define LCDCON_TABWIDTH 5
#endif


/***** Global variables *****************************************************/


/***** Function prototypes **************************************************/
void lcdcon_init( void );
void lcdcon_clear( void );
void lcdcon_locate( int row, int column );
void lcdcon_putc( int ch );
void lcdcon_puts( const char *s );
void lcdcon_scroll( int row );

#ifdef __cplusplus
}
#endif
#endif
