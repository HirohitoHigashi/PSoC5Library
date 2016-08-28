/*! @file
 @brief
 LCD Character display module controller for mruby/c devkit-01

 @author Shimane IT Open-Innovation Center.
 @version v1.10
 @date Sun Apr 10 12:10:32 2016
 @note
 <pre>
  Copyright (C) 2016 Shimane IT Open-Innovation Center.
  Original author: Shimane Institute for Industrial Technology.

   This file is destributed under BSD 3-Clause License.

  Pin assigns.

  all pin continues mode. (default)
    DB4 LCD_Write() 0bit (0x01)
    DB5 LCD_Write() 1bit (0x02)
    DB6 LCD_Write() 2bit (0x04)
    DB7 LCD_Write() 3bit (0x08)
    E   LCD_Write() 4bit (0x10)
    RS  LCD_Write() 5bit (0x20)
    R/W (LOW - Write Only)

  separate control pin mode.
    DB4 LCD_DATA() 0bit (0x01)
    DB5 LCD_DATA() 1bit (0x02)
    DB6 LCD_DATA() 2bit (0x04)
    DB7 LCD_DATA() 3bit (0x08)
    RS  LCD_RS()
    E   LCD_E()
    R/W (LOW - Write Only)

 </pre>
*/

/***** Feature test switches ************************************************/
/***** System headers *******************************************************/
/***** Local headers ********************************************************/
#include "CyLib.h"
#include "LCD.h"
#include "lcdc.h"

/***** Constat values *******************************************************/
#define RS_CTRL 0
#define RS_DATA 1

/***** Macros ***************************************************************/
#define DELAY_us(us)    CyDelayUs(us)
#define DELAY_ms(ms)    CyDelay(ms)

#if 0
#define LCD_RS(rs)      LCD_RS_Write(rs)
#define LCD_E(e)        LCD_E_Write(e)
#define LCD_DATA(d)     LCD_Write(d)
#endif

/***** Typedefs *************************************************************/
/***** Function prototypes **************************************************/
/***** Local variables ******************************************************/
/***** Global variables *****************************************************/
/***** Signal catching functions ********************************************/
/***** Local functions ******************************************************/
/***** Global functions *****************************************************/

//================================================================
/*! initialize a LCD panel

*/
void lcd_init( void )
{
  DELAY_ms( 20 );                       // >15ms
  lcd_write4( RS_CTRL, 0x03 );
  DELAY_ms( 5 );                        // >4.1ms
  lcd_write4( RS_CTRL, 0x03 );
  DELAY_us( 200 );                      // >100us
  lcd_write4( RS_CTRL, 0x03 );
  DELAY_us( 53 );                       // >37us (but typ.)
  lcd_write4( RS_CTRL, 0x02 );
  DELAY_us( 53 );

  lcd_write8( RS_CTRL, 0x28 );          // 2 lines, 5x8 dots
  lcd_write8( RS_CTRL, 0x08 );          // display off
  lcd_write8( RS_CTRL, 0x01 );          // display clear
  DELAY_us( 2160 );
  lcd_write8( RS_CTRL, 0x06 );          // cursor increment, display shift off
  lcd_write8( RS_CTRL, 0x0c );          // display on
}



//================================================================
/*! clear all

*/
void lcd_clear( void )
{
  lcd_write8( RS_CTRL, 0x01 );
  DELAY_ms( 2 );                        // >1.52ms
}



//================================================================
/*! set display position

  @param        row     Row ( 0 to 3 )
  @param	column  Column ( 0 to 19 )
*/
void lcd_location( int row, int column )
{
  static const uint8_t tbl_row[] = { 0x00, 0x40, 0x14, 0x54 };
  lcd_write8( RS_CTRL, (tbl_row[ row ] + column) | 0x80 );
}



//================================================================
/*! Put a character

  @param	ch      character code.
*/
void lcd_putc( int ch )
{
  lcd_write8( RS_DATA, ch );
}



//================================================================
/*! Put a string

  @param	s       String
*/
void lcd_puts( const char *s )
{
  int ch;
  while( (ch = *s++) != '\0' ) {
    lcd_write8( RS_DATA, ch );
  }
}



//================================================================
/*! Set character generator RAM

  @param	code    character code (0-7)
  @param	bitmap5x8 bitmap data. 4-0 bits x 8datas. upper to lower.
*/
void lcd_set_cgram( int code, uint8_t *bitmap5x8 )
{
  int i;
  lcd_write8( RS_CTRL, 0x40 | ((code & 0x07) * 8) );
  for( i = 0; i < 8; i++ ) {
    lcd_write8( RS_DATA, bitmap5x8[i] );
  }
}



//================================================================
/*! Write a nibble data to LCD contol or data register.

  @param	rs      Select a register 0:Control, 1:Data.
  @param	data    data (LSB 4bits)
*/
void lcd_write4( uint8_t rs, uint8_t data )
{
#ifdef LCD_RS
  /*
    separate control pin mode.
  */
  LCD_RS( rs );
  LCD_E( 0 );
  LCD_DATA( data );
  DELAY_us( 0 );                //   tAS (>140ns)

  LCD_E( 1 );
  DELAY_us( 1 );                //   PWEH (>450ns)

  LCD_E( 0 );
  DELAY_us( 1 );                //   tCYCE (>1000ns)

#else
  /*
    all pin continues mode.
  */
  data &= 0x0f;
  if( rs ) data |= 0x20;
  LCD_Write( data );            // RS, /E
  DELAY_us( 0 );                //   tAS (>140ns)

  LCD_Write( data | 0x10 );     // E
  DELAY_us( 1 );                //   PWEH (>450ns)

  LCD_Write( data );            // /E
  DELAY_us( 1 );                //   tCYCE (>1000ns)
#endif
}



//================================================================
/*! Write a data to LCD control or data register.

  @param	rs      Select a register 0:Control, 1:Data.
  @param	data    data (8bits)
*/
void lcd_write8( uint8_t rs, uint8_t data )
{
  lcd_write4( rs, data >> 4 );          // High 4 bits.
  lcd_write4( rs, data & 0x0f );        // Low 4 bits.
  DELAY_us( 53 );
}
