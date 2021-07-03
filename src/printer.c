/* This file is part of 34S.
 *
 * 34S is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * 34S is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with 34S.  If not, see <http://www.gnu.org/licenses/>.
 */



#include "dmcp.h"

#define DM42SAFE

#include "printer.h"
#include "xeq.h"
#include "stats.h"
#include "display.h"
#include "complex.h"
#include "storage.h"
#include "lcd.h"

#undef DM42SAFE

#ifdef INFRARED

#define RETURN_IF_PRINT_OFF if (!UState.print_on) return
#define BREAK_IF_EXIT  if (key_pop() == KEY_EXIT) break

/*
void goto_new_line(void) {
  print('\n');
  prepare_new_line();
}
*/

void prepare_new_line(void) {
  int i = 0;
  UState.print_on = 0;
  finish_PRT();
  printer_advance_buf(PRINT_GRA_LN);
  while ( printer_busy_for (PRINT_GRA_LN) && i<50 ) { // return after 5.0s anyway 
    sys_timer_start(0, 100);  // Timer 0: wake up for heartbeat 
    sys_sleep();
    sys_timer_disable(0); // stop timer
    i++;
  }
  UState.print_on = 1;
  finish_PRT();
}

void put_ir( int byte ) {
  if (UState.print_on) print_byte( (uint8_t) byte);
}

/*
 *  Where will the next data be printed?
 *  Columns are in pixel units from 0 to 165
 */
unsigned int PrinterColumn;

/*
 *  Print to IR or serial port, depending on the PMODE setting. FINE.
 */
void print( int c ) { // prints a single character
  const int mode = UState.print_mode;
  // Serial mode code removed for DM42
  if ( c == '\n' && ( mode == PMODE_GRAPHICS || mode == PMODE_SMALLGRAPHICS ) ) {
    // better LF for graphics printing
    put_ir( 0x04 );
    return;
  }
  put_ir( c );
}


/*
 *  New line
 */
void print_advance( int mode ) {
  PrinterColumn = 0;
  print( mode ? 0x04 : '\n' );
  if (UState.print_blank_line) print( mode ? 0x04 : '\n' ); 
  prepare_new_line();
}


/*
 *  New line if tracing is active
 */
static void advance_if_trace()
{
  if ( PrinterColumn != 0 && Tracing ) {
    print_advance( 0 );
  }
}


/*
 *  Move to column
 */
void print_tab( unsigned int col ) // pixel-aligned column
{
  if ( PrinterColumn > col ) {
    print_advance( 0 );
  }
  if ( PrinterColumn < col ) {
    int i = col - PrinterColumn;
    int j = i / 7;
    i %= 7;
    PrinterColumn = col;
    if ( i ) {
      put_ir( 27 );
      put_ir( i );
      while ( i-- )
	put_ir( 0 );
    }
    while ( j-- )
      put_ir( ' ' );
  }
}


/* Print a buffer from the given tab position and add a new line
 */
static void print_string_from_tab(const char *s, int tab)
{
  print_tab(tab);
  print_line(s, 1);
}


/*
 *  Print a graphic sequence
 */
void print_graphic( int glen, const unsigned char *graphic ) {
  if ( glen > 0 ) {
    put_ir( 27 );
    put_ir( glen );
    while ( glen-- ) {
      put_ir( *graphic++ );
    }
  }
}


/*
 *  Determine the length of a string in printer pixels based on the current mode.
 */
static int buffer_width(const char *buff)
{
  const int mode = UState.print_mode;
  unsigned int c;
  int l = 0;

  while ((c = 0xff & *buff++) != '\0') {
    switch (mode) {
    default:
      l += 7;
      break;

    case PMODE_SMALLGRAPHICS:
      c += 256;
    case PMODE_GRAPHICS:
      l += charlengths(c);
      break;
    }
  }
  return l;
}


/*
 *  Wrap if line is full
 */
static void wrap( int width ) {
  if ( PrinterColumn + width > PAPER_WIDTH ) {
    print_advance (0);
    if ( width == 7 ) width = 5;
  }
  PrinterColumn += width;
}

/*
 *  Print a complete line using character set translation
 */
void print_line( const char *buff, int with_lf )
{
  const int mode = UState.print_mode;
  unsigned int c;
  unsigned short int posns[ 257 ];
  unsigned char pattern[ 6 ];	// Rows
  unsigned char graphic[ PAPER_WIDTH ];	// Columns
  unsigned char glen = 0;
  unsigned char i, j, m, w = 0;

  // Import code from generated file font.c
  //  extern const unsigned char printer_chars[ 31 + 129 ];
  /* Translation table to printer character set
   */

  const unsigned char printer_chars[31+129] = {
    0x00, 0x00, 0x83, 0x84, 0xB3, 0x20, 0x00, 0xFE, 0x89, 0x8A, 0x8B, 0x00,
    0x8D, 0x8E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xD2, 0xD6, 0x00, 0xDE,
    0x00, 0x00, 0x00, 0x82, 0x00, 0xBB, 0xBC, 0x00, 0x98, 0x00, 0x00, 0x00,
    0xE3, 0xE4, 0x00, 0x00, 0xD3, 0xD7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x85, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x97, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x8C, 0x00, 0x00, 0x88, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x8F, 0x00, 0x00, 0x00, 0x87, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x95, 0x96, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xA1, 0xE0, 0xA2, 0xD8, 0xD0, 0x00, 0x00, 0xB4, 0xA4, 0xDC, 0xA4, 0xA5,
    0xE6, 0xE5, 0xA6, 0xA7, 0xB6, 0xE8, 0xE7, 0x00, 0xDA, 0x00, 0xEB, 0x00,
    0xAD, 0xED, 0xAE, 0xDA, 0x00, 0xB1, 0xEE, 0x00, 0xC8, 0xC4, 0xC0, 0xCC,
    0xD4, 0x00, 0x00, 0xB5, 0xC9, 0xC5, 0xC1, 0xCD, 0xD9, 0xD5, 0xD1, 0xDD,
    0xB7, 0xCA, 0xC6, 0xC2, 0xCE, 0x00, 0xEC, 0x00, 0xCB, 0xC7, 0xC4, 0xCF,
    0x00, 0xB2, 0xEF, 0x00, };

  // Determine character sizes and pointers
  findlengths( posns, mode == PMODE_SMALLGRAPHICS );

  // Print line
  while ( ( c = *( (const unsigned char *) buff++ ) ) != '\0') {

    w= 0;
    switch ( mode ) {

    case PMODE_DEFAULT:			// Mixed character and graphic printing
      if ( c == 006 && *buff == 006 ) {
	// merge small spaces
	continue;
      }
      i = c < ' ' ? printer_chars[ c - 1 ]
	: c > 126 ? printer_chars[ c - 127 + 31 ]
	: c;
#ifdef INCLUDE_C_LOCK
      if (C_LOCKED && POLAR_DISPLAY && c == 160) { // pass angle char through untranslated
	i = c;
      }
#endif
      if ( i != 0 ) {
	// Use printer character set
	w = PrinterColumn == 0 || PrinterColumn == 160 ? 6 : 7;
	print_graphic( glen, graphic );
	glen = 0;
	wrap( w );
	put_ir( i );
      }
      else {
	// graphic printing of characters unknown to the printer
	w = 6;
	if ( PrinterColumn > 0 && PrinterColumn < PAPER_WIDTH ) {
	  // Add horizontal spacing
	  graphic[ glen++ ] = 0;
	  ++PrinterColumn;
	  if ( PrinterColumn == 161 ) {
	    w = 5;
	  }
	}
	goto graphic_print;
      }
      break;

    case PMODE_SMALLGRAPHICS:		// Small font
      c += 256;
      
    case PMODE_GRAPHICS:			// Standard font
    graphic_print:
      // Spit out the character as a graphic
#ifdef INCLUDE_C_LOCK
      if ( ((C_LOCKED) && (POLAR_DISPLAY)) && c == (256+'<') ) {
	w = 4;
	pattern[0] = 8;
	pattern[1] = 4;
	pattern[2] = 2;
	pattern[3] = 15;
	pattern[4] = 0;
	pattern[5] = 0;
      }
      else {
	unpackchar( c, pattern, mode == PMODE_SMALLGRAPHICS, posns );
      }
#else
      unpackchar( c, pattern, mode == PMODE_SMALLGRAPHICS, posns );
#endif
      if ( w == 0 ) {
	w = charlengths( c );
	if ( PrinterColumn + w == 167 ) {
	  // drop last column on last character in line
	  --w;
	}
      }
      if ( PrinterColumn + w > PAPER_WIDTH ) {
	print_graphic( glen, graphic ); 
	glen = 0;
      }
      wrap( w );
      // Transpose the pattern
      m = 1;
      for ( i = 0; i < w; ++i ) {
	c = 0;
	for ( j = 0; j < 6; ++j ) {
	  if ( pattern[ j ] & m ) {
	    c |= ( 2 << j );
	  }
	}
	graphic[ glen++ ] = c;
	m <<= 1;
      }
      break;
    }
  }
  print_graphic( glen, graphic );
  if ( with_lf ) {
    print_advance( 0 );
  }
}


/*
 *  Print buffer right justified
 */
void print_justified( const char *buff )
{
  const int pmode = UState.print_mode;
  int len = pmode == PMODE_DEFAULT ? slen( buff ) * 7 - 1
    : pixel_length( buff, pmode == PMODE_SMALLGRAPHICS );

  if ( len >= PAPER_WIDTH ) {
    len = PAPER_WIDTH;
  }
  if ( len > 0 ) {
    print_tab( PAPER_WIDTH - len );
  }
  print_line( buff, 1 );
}


/*
 *  Print a single register
 */
void print_reg( int reg, const char *label, int eq )
{
  char buffer[ 65 ];
  if ( label != NULL ) {
    print_line( label, 0 );
    if ( eq ) print_line( " =", 0 );
  }
  xset( buffer, '\0', sizeof( buffer ) );
  format_reg( reg, buffer );
  print_justified( buffer );
}	


/*
 *  Print a block of registers with labels
 */
void print_registers( enum nilop op )
{
  int s, n;

  RETURN_IF_PRINT_OFF;
  advance_if_trace();

#ifdef INCLUDE_C_LOCK  
  if (State2.cmplx || C_LOCKED) {
    print_registers_cmplx (op);
    State2.cmplx = 0;
    return;
  }
#else
  if (State2.cmplx) {
    print_registers_cmplx (op);
    State2.cmplx = 0;
    return;
  }
#endif
  
  if ( op == OP_PRINT_STACK ) {
    s = regX_idx;
    n = stack_size();
  }
  else {
    if ( reg_decode( &s, &n, (int *) NULL, 0 ) ) {
      return;
    }
  }

  while ( n-- ) {
    int r = s;
    char name[ 6 ], *p = name;

    if ( r >= regX_idx && r <= regK_idx ) {
      *p++ = REGNAMES[ r - regX_idx ] | 0x60; // force lower case
    }
    else {
      *p++ = 'r';
      if ( r > LOCAL_REG_BASE ) {
	*p++ = '.';
	r -= LOCAL_REG_BASE;
	if ( r >= 100 ) {
	  *p++ = '1';
	  r -= 100;
	}
      }
      p = num_arg_0( p, r, 2 );
    }
    *p = '\0';
    print_reg( s++, name, 1 );
    BREAK_IF_EXIT;
  }
  if ( op == OP_PRINT_STACK ) {
    print_reg( regL_idx, "l", 1 );
    print_reg( regI_idx, "i", 1 );
  }
}

/*
 *  Print the statistical registers
 */
void print_sigma( enum nilop op )
{
  // Use the user commands to get the values

  static const char ops[] = {
    OP_sigmaN,
    OP_sigmaX, OP_sigmaY, 
    OP_sigmaX2, OP_sigmaY2, OP_sigmaXY, OP_sigmaX2Y, 
    OP_sigmalnX, OP_sigmalnY, 
    OP_sigmaXlnY, OP_sigmaYlnX,
    OP_sigmalnXlnX, OP_sigmalnYlnY,
    OP_sigmalnXlnY, 
  };
  int i;
  RETURN_IF_PRINT_OFF;
  advance_if_trace();
  REGISTER save_x;
  char buffer[ 16 ];

  // We need to save and restore X
  copyreg( &save_x, StackBase );

  for ( i = 0; i < sizeof( ops ); ++i ) {
    const char *p;
    sigma_val( (enum nilop) ops[ i ] );
    p = prt( OP_NIL | ops[ i ], buffer );
    print_reg( regX_idx, p, 1 );
  }
  copyreg( StackBase, &save_x );
}

/*
 *  Print the contents of the Alpha register, terminated by a LF
 */
void print_alpha( enum nilop op )
{
  RETURN_IF_PRINT_OFF;
  advance_if_trace();
  if ( op == OP_PRINT_ALPHA_JUST ) {
    print_justified( Alpha );
  }
  else {
    print_line( Alpha, op == OP_PRINT_ALPHA );
  }  
}

/*
 *  Send a LF to the printer
 */
void print_lf( enum nilop op ) {
  RETURN_IF_PRINT_OFF;
  advance_if_trace();
  print_advance( 0 );
}

/*
 *  Print a single character or control code
 */
void cmdprint( unsigned int arg, enum rarg op )
{
  char buff[ 2 ];
  RETURN_IF_PRINT_OFF;

  switch ( op ) {

  case RARG_PRINT_BYTE:
    // Transparent printing of bytes
    print( arg ); // might be a line feed, so...
    if (arg == '\n' || arg == 0x04)
      prepare_new_line();
    break;
	
  case RARG_PRINT_CHAR:
    // Character printing, depending on mode
    buff[ 0 ] = (char) arg;
    buff[ 1 ] = '\0';
    print_line( buff, 0 );
    break;

  case RARG_PRINT_TAB:
    // Move to specific column
    print_tab( arg );
    break;

  default:
    break;
  }
}


/*
 *  Print a named register
 */
void cmdprintreg( unsigned int arg, enum rarg op )
{
  RETURN_IF_PRINT_OFF;
#ifdef INCLUDE_C_LOCK
  if (State2.cmplx || C_LOCKED) {
    print_reg_cmplx( arg, CNULL, 0);
    State2.cmplx = 0;
  }
#else
  if (State2.cmplx) {
    print_reg_cmplx( arg, CNULL, 0);
    State2.cmplx = 0;
  }
#endif
  else {
    print_reg( arg, CNULL, 0 );
  }
}

/*
 *  Print a pair of registers as a complex number.
 */
void cmdprintcmplxreg( unsigned int reg, enum rarg op){
  RETURN_IF_PRINT_OFF;
  print_reg_cmplx (reg, CNULL, 0);
}

void print_reg_cmplx (int reg, const char *label, int eq) {
  decNumber x, y;
#ifdef INCLUDE_C_LOCK
  decNumber rx, ry;
  char *q;
#endif
  int lenx, leny, lenlab = 0, maxlen;
  char bufx[ 50 ];
  char bufy[ 54 ];
  char buffer[104];
  char *p, *r;

  advance_if_trace();

  if (is_intmode()) {
    bad_mode_error();
    return;
  }

  xset( bufx, '\0', sizeof( bufx ) );
  p = bufx;

  if ( label != NULL ) {
    p = scopy(p, label);
    if ( eq ) p = scopy(p, " ="); // put label and = at left of line
    lenlab = buffer_width(bufx);
    print_line (bufx, 0);
  }
  // So far as I can see this doesn't work for double precision
  // Calling set_x instead of set_x_dn would be a start (both in display.c)
  
  getRegister(&x, reg);
  getRegister(&y, reg+1);

#ifdef INCLUDE_C_LOCK
  if (C_LOCKED && POLAR_DISPLAY) {
    cmplxToPolar (&rx, &ry, &x, &y);
    cvt_rad2 (&y, &ry);
    decNumberCopy(&x, &rx); // x, y now contain r, theta
  }
#endif
  // x, y now contain the numbers to print
  
  xset( bufx, '\0', sizeof( bufx ) );
  p = bufx;
  lenx = DISPLAY_DIGITS;
  set_x_dn( &x, p, &lenx);
  p = find_char(bufx, '\0');
  scopy(p, "  ");
  lenx = buffer_width(bufx);
  
  xset( bufy, '\0', sizeof( bufy ) );
  leny = DISPLAY_DIGITS;

#ifdef INCLUDE_C_LOCK
  if (C_LOCKED && POLAR_DISPLAY) {
    if (UState.print_mode == PMODE_DEFAULT) {
      r = scopy (bufy, "\xA0 "); // assumes ROMAN8 character set; angle symbol
    }
    else if (UState.print_mode == PMODE_GRAPHICS) { // graphics modes
      r = scopy(bufy, "< "); // < is angle symbol in MODE 1; 
    }
    else {
      r = scopy(bufy, "<\006 "); // '<' one px smaller than what will be printed; 
    }
  }
  else {
    q = CPX_J ? "j " : "i ";
    r = scopy (bufy, q);
  }
#else
  r = bufy;
#endif
  set_x_dn( &y, r, &leny); // copy y after complex prefix
  leny = buffer_width(bufy); // bufy is still the start of the buffer

  if (lenx + leny + lenlab > PAPER_WIDTH) {
    if (lenx + leny > PAPER_WIDTH) {
      p[2] = '\0'; // end bufx after " " - p still points near end of bufx
      lenx = buffer_width(bufx);
      maxlen = PAPER_WIDTH - (lenx > leny ? lenx : leny);
      print_string_from_tab(bufx, maxlen);
      print_string_from_tab(bufy, maxlen);
    }
    else {
      print_advance(1); // print label by itself, followed by ...
      scopy(scopy(buffer, bufx), bufy);
      print_justified(buffer); // the two numbers
    }
  }
  else {
    scopy(scopy(buffer, bufx), bufy);
    print_justified(buffer);
  }
}

/*
 *  Print a block of registers with labels
 */
void print_registers_cmplx( enum nilop op )
{
  int s, n;

  RETURN_IF_PRINT_OFF;
  advance_if_trace();

  if ( op == OP_PRINT_STACK ) {
    s = regX_idx;
    n = stack_size();
  }
  else {
    if ( reg_decode( &s, &n, (int *) NULL, 0 ) ) {
      return;
    }
  }
  
#ifdef INCLUDE_C_LOCK
  if ( (C_LOCKED) && (s & 1) ) {
    error_message(ERR_ODD_REG);
    return;
  }
#endif  

  while ( n>0 ) { // changed from while(n) in case an odd n is entered in non-c_lock mode
    int r = s;
    char name[ 6 ], *p = name;

    n -= 2;
    
    if ( r >= regX_idx && r <= regK_idx ) {
      *p++ = "Z?U?V?W?L?J?"[ r - regX_idx ]; // no space as = used
    }
    else {
      *p++ = 'r';
      if ( r > LOCAL_REG_BASE ) {
	*p++ = '.';
	r -= LOCAL_REG_BASE;
	if ( r >= 100 ) {
	  *p++ = '1';
	  r -= 100;
	}
      }
      p = num_arg_0( p, r, 2 );
    }
    *p = '\0';
    print_reg_cmplx( s, name, 1);
    s +=2;
    BREAK_IF_EXIT;
  }
  if ( op == OP_PRINT_STACK ) {
    print_reg_cmplx( regL_idx, "L", 1);
  }
}


/*
 *  Return the width of alpha to x
 */
void cmdprintwidth(enum nilop op)
{
  setX_int_sgn(buffer_width(Alpha), 0);
}

/*
 *  Turn printing on or off
 */
void print_on_off (enum nilop op) {
  if (op == OP_PRINT_ON) {
    UState.print_on = 1;
  }
  else if (op == OP_PRINT_OFF) {
    UState.print_on = 0;
  }
}

/*
 *  Set printing modes
 */
void cmdprintmode( unsigned int arg, enum rarg op )
{
  if ( op == RARG_PMODE ) {
    UState.print_mode = arg;
  }
  else if ( op == RARG_PDELAY ) { // DM42 replacement
    printer_set_delay(arg*100); // arg is still in ticks, so convert to ms.
  }
  else if ( op == RARG_DBLSP ) {
    UState.print_blank_line = arg;
  }
}

/*
 *  Trace an instruction
 */
void print_trace( opcode op, int phase )
{
  char buffer[ 16 ];
  const char *p;
	
  if ( (Tracing || op == RARG( RARG_SF, T_FLAG )) && UState.print_on ) {
    /*
     *  We trace when flag T has been set and printing is on
     */
    if (op == (OP_SPEC | OP_CHS)) {
      if (CmdLineLength > 0)
	return;
    }
    else if (op >= (OP_SPEC | OP_EEX) && op <= (OP_SPEC | OP_F))
      return;
    else if (op == (OP_SPEC | OP_CLX)) {
      if (CmdLineLength || (phase==1 && PrinterColumn==0))
	return;
      op = OP_NIL | OP_rCLX;
    }
    else if (! Running && isRARG(op) && RARG_CMD(op) == RARG_ALPHA )
      return;

    // Format the command
    p = prt( op, buffer );

    if ( phase == 0 ) {
      // Left part of print
      if ( CmdLineLength ) {
	process_cmdline();
      }
      print_tab( 0 );
      print_line( prt( op, buffer ), 0 );
      print_line( " ", 0 );
    }
    else {
      // right part of print
      print_reg( regX_idx,
		 op == TRACE_DATA_ENTRY ? ">>>" :
		 PrinterColumn == 0     ? ( !Tracing ? p : "***"  ) :
		 CNULL,
		 0 );
      if ( State2.wascomplex ) {
	print_reg( regY_idx, "cpx", 0 );
      }
    }
  }
}

/*
 *  Print a program listing
 *  Start at the PC location
 */
void print_program( enum nilop op )
{
  unsigned int pc = state_pc();
  const int runmode = State2.runmode;
  const int pmode = UState.print_mode;
  const int numlen = isRAM( pc ) ? 3 : 4;
  const int tab = numlen * ( 6 - pmode ) + 1;

  RETURN_IF_PRINT_OFF;
  advance_if_trace();
  
  if ( runmode ) {
    pc = ProgBegin;
  }
  if ( pc == 0 ) {
    ++pc;
  }

  PcWrapped = 0;
  while ( !PcWrapped ) {
    char buffer[ 16 ];
    const char *p;
    opcode op = getprog( pc );
    unsigned int upc = user_pc( pc );
    *num_arg_0( buffer, upc, numlen ) = '\0';
    print_line( buffer, 0 );
    if ( pmode == PMODE_GRAPHICS || pmode == PMODE_SMALLGRAPHICS ) {
      print_tab( tab );
    }
    print( ' ' );
    p = prt( op, buffer );
    print_line( p, 1 );
    BREAK_IF_EXIT;
    pc = do_inc( pc, runmode );
  }
}

#ifdef INCLUDE_PLOTTING
/*
 *  Send the buffer to the printer
 */
void cmdplotprint( unsigned int arg, enum rarg op )
{
  unsigned char *p = plot_check_range( arg, 0, 0 );

  RETURN_IF_PRINT_OFF;
  if ( p != NULL ) {
    int width = (int) p[ 0 ];
    int height = (int) p[ 1 ];
    int col = PrinterColumn;
    p += 2;
    if ( PrinterColumn + width > PAPER_WIDTH ) {
      /*
       *  Data does not fit on the same line
       */
      col = 0;
    }
    while ( height != 0 ) {
      /*
       *  Print this line
       */
      //      print_tab( col ) || print_graphic( width, p );
      print_tab( col );
      print_graphic( width, p );
      PrinterColumn = col + width;
      if ( --height ) {
	/*
	 *  More lines ot print
	 */
	print_advance( 1 );
	p += width;
      }
    }
    if ( PrinterColumn == PAPER_WIDTH ) {
      /*
       *  Line is full
       */
      print_advance( 1 );
    }
  }
}


#endif

#else
/* To avoid an empty .o and a warning in ranlib */
void dummy_print()
{
}
#endif

