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

#ifndef __PRINTER_H__
#define __PRINTER_H__

#include "xeq.h"

#define PRINTING_ANNUNCIATOR LIT_EQ

enum print_modes {
	PMODE_DEFAULT = 0,
	PMODE_GRAPHICS = 1,
	PMODE_SMALLGRAPHICS = 2,
#ifndef DM42
	PMODE_SERIAL = 3
#endif
};

#define TRACE_DATA_ENTRY 0xffffffff

// User visible routines
extern void print( int c );
extern void print_advance( int mode );
extern void print_tab( unsigned int col );
extern void print_line( const char *buff, int with_lf );
extern void print_justified( const char *buff );
extern void print_reg( int reg, const char *label, int eq );
extern void print_trace( opcode op, int phase );
extern void print_program( enum nilop op );
extern void print_registers( enum nilop op );
extern void print_sigma( enum nilop op );
extern void print_alpha( enum nilop op );
extern void print_lf( enum nilop op );
extern void cmdprint( unsigned int arg, enum rarg op );
extern void cmdprintreg( unsigned int arg, enum rarg op );
extern void cmdprintcmplxreg( unsigned int arg, enum rarg op );
extern void cmdprintmode( unsigned int arg, enum rarg op );
extern void cmdprintwidth(enum nilop op);
extern void cmdplotprint( unsigned int arg, enum rarg op );
extern void prepare_new_line( void );
extern void goto_new_line( void );
extern void print_on_off( enum nilop op );
extern void print_registers_cmplx( enum nilop op );
extern void print_reg_cmplx( int reg, const char *label, int eq);

// Implemented by the hardware layer
extern void put_ir( int byte );


// DM42: have removed REALBUILD print_delay stuff

extern unsigned int PrinterColumn;

#ifndef INFRARED
#define print_trace( op, phase ) /**/
#endif

#endif
