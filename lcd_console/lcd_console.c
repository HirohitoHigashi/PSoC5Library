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

/***** System headers *******************************************************/
#include <stdint.h>
#include <string.h>


/***** Local headers ********************************************************/
#include "lcd_console.h"
#include "lcdc/lcdc.h"


/***** Constant values ******************************************************/
/***** Macros ***************************************************************/
/***** Typedefs *************************************************************/
/***** Function prototypes **************************************************/
/***** Local variables ******************************************************/
//! Character buffer
static uint8_t cbuf_[LCDCON_SCROLL_HEIGHT][LCDCON_WIDTH];
static uint16_t row_;           //!< cursor row
static uint16_t col_;           //!< cursor column
static uint16_t cbuf_row_;      //!< display start row of cbuf_
static uint16_t cbuf_row_st_;   //!< valid area of cbuf_
static uint16_t cbuf_row_en_;   //!< valid area of cbuf_
static int16_t cbuf_scroll_;


/***** Interrupt handler ****************************************************/
/***** Local functions ******************************************************/

//================================================================
/*! shift display area in character buffer

*/
static void rollup( void )
{
  int r, dr, c;

  // shift display start
  if( ++cbuf_row_ >= LCDCON_SCROLL_HEIGHT ) cbuf_row_ = 0;

  // clear last line
  dr = cbuf_row_ + LCDCON_HEIGHT - 1;
  if( dr >= LCDCON_SCROLL_HEIGHT ) dr -= LCDCON_SCROLL_HEIGHT;
  memset( cbuf_[dr], ' ', LCDCON_WIDTH );

  // adjust valid area of cbuf_
  cbuf_row_en_ = dr;
  if( cbuf_row_st_ >= cbuf_row_en_ ) {
    cbuf_row_st_ = cbuf_row_en_ + 1;
    if( cbuf_row_st_ >= LCDCON_SCROLL_HEIGHT ) cbuf_row_st_ = 0;
  }

  // refresh LCD
  for( r = 0; r < LCDCON_HEIGHT; r++ ) {
    dr = cbuf_row_ + r;
    if( dr >= LCDCON_SCROLL_HEIGHT ) dr -= LCDCON_SCROLL_HEIGHT;

    lcd_locate( r, 0 );
    for( c = 0; c < LCDCON_WIDTH; c++ ) {
      lcd_putc( cbuf_[dr][c] );
    }
  }
}


/***** Global functions *****************************************************/

//================================================================
/*! initialize

*/
void lcdcon_init( void )
{
  lcd_init();
  lcdcon_clear();
}


//================================================================
/*! clear all

*/
void lcdcon_clear( void )
{
  memset( cbuf_, ' ', sizeof(cbuf_) );
  row_ = 0;
  col_ = 0;
  cbuf_row_ = 0;
  cbuf_row_st_ = 0;
  cbuf_row_en_ = LCDCON_HEIGHT-1;
  cbuf_scroll_ = 0;
  lcd_clear();
}


//================================================================
/*! set display position

  @param        row     Row
  @param	column  Column
*/
void lcdcon_locate( int row, int column )
{
  if( row < 0 ) return;
  if( row >= LCDCON_HEIGHT ) return;
  if( column < 0 ) return;
  if( column >= LCDCON_WIDTH ) return;

  row_ = row;
  col_ = column;
  lcd_locate( row_, col_ );
}



//================================================================
/*! put a character

  @param	ch      character code.
*/
void lcdcon_putc( int ch )
{
  // process control characters.
  if( ch < 0x20 ) {
    switch( ch ) {
    case '\0':
      return;

    case '\a':          // Bell
      return;

    case '\b':
      if( col_ != 0 ) {
        lcd_locate( row_, --col_ );
      }
      return;

    case '\t': {
      int tw = LCDCON_TABWIDTH - (col_ % LCDCON_TABWIDTH);
      if( (col_ + tw) >= LCDCON_WIDTH ) tw = LCDCON_WIDTH - col_;
      for( ; tw > 0; tw-- ) {
        lcdcon_putc( ' ' );
      }
      return;
    }

    case '\n':
      col_ = 0;
      if( row_ < LCDCON_HEIGHT ) {
        row_++;
        lcd_locate( row_, col_ );
      } else {
        rollup();
      }
      return;

    case '\f':
      lcdcon_clear();
      return;

    case '\r':
      col_ = 0;
      lcd_locate( row_, col_ );
      return;

    default:
      return;
    }
  }

  // check rollup conditions.
  if( row_ >= LCDCON_HEIGHT ) {
    rollup();
    row_ = LCDCON_HEIGHT-1;
    lcd_locate( row_, col_ );
  }

  if( col_ >= LCDCON_WIDTH ) {
    col_ = 0;
    if( row_ < LCDCON_HEIGHT-1 ) {
      row_++;
    } else {
      rollup();
    }
    lcd_locate( row_, col_ );
  }

  // shift 0x80 for coexist user defined characters and control characters.
  if( 0x80 <= ch && ch <= 0x8f ) ch -= 0x80;

  // put a character on character buffer and LCD.
  int dr = cbuf_row_ + row_;
  if( dr >= LCDCON_SCROLL_HEIGHT ) dr -= LCDCON_SCROLL_HEIGHT;
  cbuf_[dr][col_++] = ch;
  lcd_putc( ch );
}



//================================================================
/*! put a string

  @param	s       String
*/
void lcdcon_puts( const char *s )
{
  int ch;
  while( (ch = *s++) ) {
    lcdcon_putc( ch );
  }
}


//================================================================
/*! scroll

  @param	row       scroll rows. (maybe minus value. zero is reset.)
*/
void lcdcon_scroll( int row )
{
  // adjust scroll line
  if( row == 0 ) cbuf_scroll_ = 0;
  cbuf_scroll_ += row;

  // check display area.
  if( cbuf_scroll_ > 0 ) {
    cbuf_scroll_ = 0;
  } else {
    int d = cbuf_row_st_ - cbuf_row_;
    if( d > 0 ) d = -(LCDCON_SCROLL_HEIGHT - LCDCON_HEIGHT);
    if( cbuf_scroll_ < d ) cbuf_scroll_ = d;
  }

  // draw LCD
  int dr = cbuf_row_ + cbuf_scroll_;
  if( dr < 0 ) dr += LCDCON_SCROLL_HEIGHT;

  int r, c;
  for( r = 0; r < LCDCON_HEIGHT; r++ ) {
    lcd_locate( r, 0 );
    for( c = 0; c < LCDCON_WIDTH; c++ ) {
      lcd_putc( cbuf_[dr][c] );
    }
    if( ++dr >= LCDCON_SCROLL_HEIGHT ) dr -= LCDCON_SCROLL_HEIGHT;
  }
}
