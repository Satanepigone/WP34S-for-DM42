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

/*
 * This module handles all load/save operations in the real build or emulator
 * Module written by MvC
 */

// Emulator definitions
#include <stdio.h>
#include <stdlib.h>
//#include <stddef.h>
//#include <string.h>
//#include <stdarg.h>

#include "features.h"
#include "dmcp.h"

#define PERSISTENT_RAM
#define SLCDCMEM
#define VOLATILE_RAM
#define BACKUP_FLASH

#ifdef C_VERSION
#define STATE_FILE "wp34s/wp34c.dat"
#define BACKUP_FILE "wp34s/wp34c-backup.dat"
#else
#define STATE_FILE "wp34s/wp34s.dat"
#define BACKUP_FILE "wp34s/wp34s-backup.dat"
#endif

#define LIBRARY_FILE "wp34s/wp34s-lib.dat"
#define FPT ppgm_fp //use this as the file pointer

#define DM42SAFE
#include "xeq.h"
#include "storage.h"
#include "display.h"
#include "stats.h"
#include "alpha.h"
#undef DM42SAFE

#define PAGE_SIZE	 256 // if saving to flash need page size of 2k on DM42
// but we aren't actually saving to flash any more!

/*
 *  Setup the persistent RAM
 */

TPersistentRam *main_ram, *backup_ram;
FLASH_REGION *library_ram;

void init_mem () { // called at start of program_main in console.c
  char *v;

  v = (char *) calloc(RAM_SIZE + RAM_SIZE + LIBRARY_SIZE,1);
  if (v == NULL) {
    perror("calloc failed");
    return;
  }
  main_ram = (TPersistentRam *) v;
  v += RAM_SIZE;
  backup_ram = (TPersistentRam *) v;
  v += RAM_SIZE;
  library_ram = (FLASH_REGION *) v;
}

/*
 *  Data that is saved in the SLCD controller during deep sleep
 */
SLCDCMEM TStateWhileOn StateWhileOn;

/*
 *  A private register area for XROM code in volatile RAM
 *  It replaces the local registers and flags if active.
 */
TXromParams XromParams;
VOLATILE_RAM TXromLocal XromLocal;

/* Private space for four registers temporarily
 */
VOLATILE_RAM REGISTER XromA2D[4];

/*
 *  The CCITT 16 bit CRC algorithm (X^16 + X^12 + X^5 + 1)
 */
unsigned short int crc16( const void *base, unsigned int length )
{
	unsigned short int crc = 0x5aa5;
	unsigned char *d = (unsigned char *) base;
	unsigned int i;

	for ( i = 0; i < length; ++i ) {
		crc  = ( (unsigned char)( crc >> 8 ) ) | ( crc << 8 );
		crc ^= *d++;
		crc ^= ( (unsigned char)( crc & 0xff ) ) >> 4;
		crc ^= crc << 12;
		crc ^= ( crc & 0xff ) << 5;
	}
	return crc;
}

/*
 *  Compute a checksum and compare it against the stored sum
 *  Returns non zero value if failure
 */
static int test_checksum( const void *data, unsigned int length, unsigned short oldcrc, unsigned short *pcrc )
{
	unsigned short crc;
	crc = crc16( data, length );
	
	if ( pcrc != NULL ) {
		*pcrc = crc;
	}
	return crc != oldcrc && oldcrc != MAGIC_MARKER;
}

/*
 *  Checksum the current program.
 */
short unsigned int checksum_program( void )
{
	update_program_bounds( 1 );
	return crc16( get_current_prog(), ProgEnd - ProgBegin + 1 );
	// Size surely needs doubling - bytes, not steps. ND.
}

/*
 *  Checksum the persistent RAM area
 *  Returns non zero value if failure
 */
int checksum_ram( void )
{
	return test_checksum( &PersistentRam, sizeof( PersistentRam ) - sizeof( short ),
			      Crc, &Crc );
}

/*
 *  Checksum the backup flash region
 *  Returns non zero value if failure
 */
int checksum_backup( void )
{
	return test_checksum( &BackupFlash, sizeof( BackupFlash ) - sizeof( short ),
		              BackupFlash._crc, NULL );
}

/*
 *  Checksum a flash region
 *  Returns non zero value if failure
 */
static int checksum_region( FLASH_REGION *fr, FLASH_REGION *header )
{
	unsigned int l = header->size * sizeof( s_opcode );
	
	return l > sizeof( fr->prog ) || test_checksum( fr->prog, l, fr->crc, &(header->crc ) );
}

/*
 *  Helper to store final END in empty program space
 */
static void stoend( void )
{
	ProgSize = 1;
	Prog[ 0 ] = ( OP_NIL | OP_END );
}

/*
 *  Clear the program space
 */
void clpall( void )
{
	clrretstk_pc();
	stoend();
}

/*
 *  Sanity checks for program (step) deletion
 */
static int check_delete_prog( unsigned int pc ) 
{
	if ( !isRAM( pc ) || ( pc == ProgSize && getprog( pc ) == ( OP_NIL | OP_END ) ) ) {
		report_warn(ERR_READ_ONLY);
	}
	else {
		return 0;
	}
	return 1;
}

/*
 *  Clear just the current program
 */
void clrprog( void )
{
	update_program_bounds( 1 );
	if ( nLIB( ProgBegin ) == REGION_LIBRARY ) {
		/*
		 *  Porgram is in flash
		 */
		flash_remove( ProgBegin, ProgEnd + 1 - ProgBegin );
	}
	else {
		if ( check_delete_prog( ProgBegin ) ) {
			return;
		}
		clrretstk();
		xcopy( Prog_1 + ProgBegin, Prog + ProgEnd, ( ProgSize - ProgEnd ) << 1 );
		ProgSize -= ( ProgEnd + 1 - ProgBegin );
		if ( ProgSize == 0 ) {
			stoend();
		}
	}
	set_pc( ProgBegin - 1 );
	update_program_bounds( 1 );
}
 
/*
 *  Clear all - programs and registers
 */
void clrall(void) 
{
	NumRegs = TOPREALREG;
	xeq_init_contexts();
	clrreg( OP_CLREG );
	clrstk( OP_CLSTK );
	clralpha( OP_CLRALPHA );
	clrflags( OP_CLFLAGS );
	clpall();

	reset_shift();
	State2.test = TST_NONE;

	DispMsg = NULL;
}


/*
 *  Clear everything
 */
void reset( void ) 
{
  xset( &PersistentRam, 0, sizeof( PersistentRam ) );
	clrall();
	init_state();
#ifdef INFRARED
	printer_set_delay(1800);
#endif
	DispMsg = "Erased";
}

/*
 *  Store into program space.
 */
void stoprog( opcode c ) {
	const int off = isDBL( c ) ? 2 : 1;
	int i;
	unsigned int pc = state_pc();

	if ( pc == ProgSize && c != ( OP_NIL | OP_END ) )
		stoprog( OP_NIL | OP_END );

	if ( !isRAM( pc ) ) {
		report_warn( ERR_READ_ONLY );
		return;
	}
	clrretstk();
	xeq_init_contexts();
	if ( ProgFree < off ) {
		return;
	}
	ProgSize += off;
	ProgEnd += off;
	pc = do_inc( pc, 0 );	// Don't wrap on END
	for ( i = ProgSize + 1; i > (int) pc; --i ) {
		Prog_1[ i ] = Prog_1[ i - off ];
	}
	if (isDBL(c))
		Prog_1[pc + 1] = c >> 16;
	Prog_1[pc] = c;
	State.pc = pc;
}


/*
 *  Delete the current step in the program
 */
void delprog( void )
{
	int i;
	const unsigned int pc = state_pc();
	int off;

	if ( check_delete_prog( pc ) )
		return;
	if ( pc == 0 )
		return;

	off = isDBL( Prog_1[ pc ]) ? 2 : 1;
	ProgSize -= off;
	ProgEnd -= off;
	for ( i = pc; i <= (int) ProgSize; ++i )
		Prog_1[ i ] = Prog_1[ i + off ];
	decpc();
}


/*
 *  Helper to append a program in RAM.
 *  Returns non zero in case of an error.
 */
int append_program( const s_opcode *source, int length )
{
	unsigned short pc;
	int space_needed = length - ProgFree;

	if ( ProgSize == 1 ) {
		/*
		 *  Only the default END statement is present
		 */
		--space_needed;
		--ProgSize;
	}
	if ( length > NUMPROG_LIMIT ) {
		return report_err( ERR_INVALID );
	}
	if ( length > NUMPROG_LIMIT - ProgSize ) {
		return report_err( ERR_RAM_FULL );
	}

	/*
	 *  Make room if needed
	 */
	clrretstk();
	if ( space_needed > 0 && SizeStatRegs != 0 ) {
		space_needed -= SizeStatRegs;
		sigmaDeallocate();
	}
	if ( space_needed > 0 ) {
		int regs;
		if (is_dblmode())
			regs = global_regs() - ( ( space_needed + 7 ) >> 3 );
		else
			regs = NumRegs - ( ( space_needed + 3 ) >> 2 );

		if ( regs < 0 ) {
			return report_err( ERR_RAM_FULL );
		}
		cmdregs( regs, RARG_REGS );
	}
	/*
	 *  Append data
	 */
	pc = ProgSize + 1;
	ProgSize += length;
	xcopy( Prog_1 + pc, source, length << 1 );
	set_pc( pc );
	return 0;
}


/*
 *  Emulate the flash in a file wp34s-lib.dat or wp34c-backup.dat
 *  Page numbers are relative to the start of the user flash
 *  count is in pages, destination % PAGE_SIZE needs to be 0.
 */

static char* get_region_path(int region)
{
	return region == REGION_BACKUP ? BACKUP_FILE : LIBRARY_FILE;
}


static int program_flash( void *destination, void *source, int count )
{
  char *name;
  char *dest = (char *) destination;
  int offset;
  uint x=0;

  /*
   *  Copy the source to the destination memory
   */

  xcopy( dest, source, count * PAGE_SIZE ); 

  /*
   *  Update the correct region file
   */

  if ( dest >= (char *) &BackupFlash && dest < (char *) &BackupFlash + sizeof( BackupFlash ) ) {
    name = get_region_path( REGION_BACKUP );
    offset = dest - (char *) &BackupFlash;
  }
  else if ( dest >= (char *) &UserFlash && dest < (char *) &UserFlash + sizeof( UserFlash ) ) {
    name = get_region_path( REGION_LIBRARY );
    offset = dest - (char *) &UserFlash;
  }
  else {
    // Bad address
    report_err( ERR_ILLEGAL );
    return 1;
  }
  FRESULT f;

  f = check_create_wp34sdir ();
  if ( f != FR_OK ) {
    return f;
  }
  sys_disk_write_enable(1);
  f = f_open( FPT, name, FA_READ | FA_WRITE );
  if ( f != FR_OK ) {
    f = f_open( FPT, name, FA_CREATE_ALWAYS | FA_READ | FA_WRITE );
  }
  if ( f != FR_OK ) {
    sys_disk_write_enable(0);
    return 1;
  }
  f_lseek( FPT, offset );
  f = f_write( FPT, dest, PAGE_SIZE*count, &x);
  if (f != FR_OK) {
    f_close( FPT );
    sys_disk_write_enable(0);
    return 1;
  }
  f = f_close( FPT );
  if ( f != FR_OK ) {
    sys_disk_write_enable(0);
    return 1;
  }
  sys_disk_write_enable(0);
  return 0;
}

int check_create_wp34sdir(void) {
  FRESULT f;
  sys_disk_write_enable(1);
  f = check_create_dir ("/wp34s");
  sys_disk_write_enable(0);
  return f;
}


/*
 *  Initialize the library to an empty state if it's not valid
 */
void init_library( void )
{
	if ( checksum_region( &UserFlash, &UserFlash ) ) {
	  struct {
			unsigned short crc;
			unsigned short size;
			s_opcode prog[ 126 ];
		} lib;
		lib.size = 0;
		lib.crc = MAGIC_MARKER;
		xset( lib.prog, 0xff, sizeof( lib.prog ) );
		program_flash( &UserFlash, &lib, 1 );
	}
}


/*
 *  Add data at the end of user flash memory.
 *  Update crc and counter when done.
 *  All sizes are given in steps.
 */
static int flash_append( int destination_step, const s_opcode *source, int count, int size )
{
  char *dest = (char *) ( UserFlash.prog + destination_step );
  char *src = (char *) source;
#ifdef REALBUILD
  int offset_in_page = (int) dest & 0xff;
#else
  int offset_in_page = ( dest - (char *) &UserFlash ) & 0xff;
#endif
  char buffer[ PAGE_SIZE ];
  FLASH_REGION *fr = (FLASH_REGION *) buffer;
  count <<= 1;

  if ( offset_in_page != 0 ) {
    /*
     *  We are not on a page boundary
     *  Assemble a buffer from existing and new data
     */
    const int bytes = PAGE_SIZE - offset_in_page;
    xcopy( buffer, dest - offset_in_page, offset_in_page );
    xcopy( buffer + offset_in_page, src, bytes );
    if ( program_flash( dest - offset_in_page, buffer, 1 ) ) {
      return 1;
    }
    src += bytes;
    dest += bytes;
    count -= bytes;
  }

  if ( count > 0 ) {
    /*
     *  Move multiples of complete pages
     */
#ifdef DM42
    count = ( count + ( PAGE_SIZE - 1 ) ) / PAGE_SIZE; // Should work for DM42 or not
#else
    count = ( count + ( PAGE_SIZE - 1 ) ) >> 8; // Original code seems to assume that page_size = 2^8
#endif
    if ( program_flash( dest, src, count ) ) {
      return 1;
    }
  }

  /*
   *  Update the library header to fix the crc and size fields.
   */
  xcopy( fr, &UserFlash, PAGE_SIZE );
  fr->size = size;
  checksum_region( &UserFlash, fr );
  return program_flash( &UserFlash, fr, 1 );
}


/*
 *  Remove steps from user flash memory.
 */
int flash_remove( int step_no, int count )
{
	const int size = UserFlash.size - count;
	step_no = offsetLIB( step_no );
	return flash_append( step_no, UserFlash.prog + step_no + count,
			     size - step_no, size );
}


/*
 *  Simple backup / restore
 *  Started with ON+STO or ON+RCL or the SAVE/LOAD commands
 *  The backup area is the last 2KB of flash (pages 504 to 511) // only in REALBUILD
 */
void flash_backup( enum nilop op )
{
  if ( not_running() ) {
    process_cmdline_set_lift();
    init_state();
    checksum_all();
    if ( program_flash( &BackupFlash, &PersistentRam, sizeof( BackupFlash ) / PAGE_SIZE ) ) {
      error_message( ERR_IO );
      DispMsg = "Error";
    }
    else {
      DispMsg = "Saved";
    }
  }
}


void flash_restore( enum nilop op )
{
  if ( not_running() ) {
    if ( checksum_backup() ) {
      error_message( ERR_INVALID );
    }
    else {
      xcopy( &PersistentRam, &BackupFlash, sizeof( PersistentRam ) );
      init_state();
      DispMsg = "Restored";
    }
  }
}


/*
 *  Load the user program area from the backup.
 *  Called by PLOAD.
 */
void load_program( enum nilop op )
{
	if ( not_running() ) {
		if ( checksum_backup() ) {
			/*
			 *  Not a valid backup
			 */
			report_err( ERR_INVALID );
			return;
		}
		clpall();
		append_program( BackupFlash._prog, BackupFlash._prog_size );
	}
}


/*
 *  Load registers from backup
 */
void load_registers( enum nilop op )
{
	int count;
	if ( checksum_backup() ) {
		/*
		 *  Not a valid backup region
		 */
		report_err( ERR_INVALID );
		return;
	}
	count = NumRegs;
	if ( is_dblmode() ) {
		// Don't clobber the stack in DP mode
		count -= EXTRA_REG + STACK_SIZE;
	}
	if ( count > BackupFlash._numregs ) {
		count = BackupFlash._numregs;
	}
	xcopy( get_reg_n(0), get_flash_reg_n(0), count << 3 );
}


/*
 *  Load the statistical summation registers from backup
 */
void load_sigma( enum nilop op )
{
	if ( checksum_backup() ) {
		/*
		 *  Not a valid backup region
		 */
		report_err( ERR_INVALID );
		return;
	}
	if ( ! BackupFlash._state.have_stats ) {
		/*
		 *  Backup has no data
		 */
		report_err( ERR_MORE_POINTS );
		return;
	}
	sigmaCopy( ( (char *)( BackupFlash._regs + TOPREALREG - BackupFlash._numregs ) - sizeof( STAT_DATA ) ) );
}


/*
 *  Load the configuration data from the backup
 */
void load_state( enum nilop op )
{
	if ( not_running() ) {
		if ( checksum_backup() ) {
			/*
			 *  Not a valid backup region
			 */
			report_err( ERR_INVALID );
			return;
		}
		xcopy( &RandS1, &BackupFlash._rand_s1, (char *) &Crc - (char *) &RandS1 );
		init_state();
		clrretstk_pc();
	}
}


/*
 *  Save a user program to the library region. Called by PSTO.
 */
void store_program( enum nilop op )
{
	opcode lbl; 
	unsigned int pc;
	int space_needed, count, free;

	if ( not_running() ) {
		/*
		 *  Don't copy from library or XROM
		 */
		pc = nLIB( state_pc() );
		if ( pc == REGION_LIBRARY || pc == REGION_XROM ) {
			report_err( ERR_ILLEGAL );
			return;
		}
		/*
		 *  Check if program is labeled
		 */
		update_program_bounds( 1 );
		lbl = getprog( ProgBegin );
		if ( !isDBL(lbl) || opDBL(lbl) != DBL_LBL ) {
			report_err( ERR_NO_LBL );
			return;
		}
		/*
		 *  Compute space needed
		 */
		count = space_needed = 1 + ProgEnd - ProgBegin;
		free = NUMPROG_FLASH_MAX - UserFlash.size;

		/*
		 *  Find a duplicate label in the library and delete the program
		 */
		pc = find_opcode_from( addrLIB( 0, REGION_LIBRARY ), lbl, 0 );
		if ( pc != 0 ) {
			/*
			 *  CLP in library
			 */
			unsigned int old_pc = state_pc();
			set_pc( pc );
			space_needed -= 1 + ProgEnd - ProgBegin;
			if ( space_needed <= free ) {
				clrprog();
			}
			set_pc( old_pc );
		}
		if ( space_needed > free ) {
			report_err( ERR_FLASH_FULL );
			return;
		}
		// 3. Append program
		flash_append( UserFlash.size, get_current_prog(), count, UserFlash.size + count );
	}
}


/*
 *  Load a user program from any region. Called by PRCL.
 */
void recall_program( enum nilop op )
{
	if ( not_running() ) {
		if ( state_pc() == 0 ) {
			State.pc = 1;
		}
		update_program_bounds( 1 );
		append_program( get_current_prog(), ProgEnd - ProgBegin + 1 );
	}
}


/*
 *  Lots of file routines for DM42
 */

#ifdef DM42

#define DISP_NEW 1
#define OVERWRITE_CHECK 1
#define NO_DISP_NEW 0
#define NO_OVERWRITE_CHECK 0
#define WRITE 1
#define READ 2

void save_lib_file ( int i ) {
  FRESULT f;
  int fss;
  int data = WRITE;
  uint x=0;

  sys_disk_write_enable(1);
  if (i == 0) {
    f = f_open (FPT, LIBRARY_FILE, FA_CREATE_ALWAYS | FA_READ | FA_WRITE);
    if (f != FR_OK) {
      f_close (FPT);
      sys_disk_write_enable(0);
      DispMsg = "Err slf1";
      return;
    }
  }
  else {
    fss = file_selection_screen ("Save Library File", "/wp34s", ".lib", open_selected_file, DISP_NEW, OVERWRITE_CHECK, &data );
    if (fss != 1) return;
  }
  // File is now open with correct permissions
  f = f_write (FPT, (char *) &UserFlash, sizeof (UserFlash), &x);
  if ( f != FR_OK ) {
    DispMsg = "Err slf2";
  }
  f_close( FPT );
  sys_disk_write_enable(0);
}

void load_lib_file ( int i ) {
  FRESULT f;
  int fss;
  int data = READ;
  uint x=0;

  if (i == 0) {
    f = f_open (FPT, LIBRARY_FILE, FA_READ);
    if (f != FR_OK) {
      f_close (FPT);
      //      DispMsg = "No file?";
      return;
    }
  }
  else {
    fss = file_selection_screen ("Load Library File", "/wp34s", ".lib", open_selected_file, NO_DISP_NEW, NO_OVERWRITE_CHECK, &data );
    if (fss != 1) return;
  }
  // File is now open with correct permissions
  f = f_read (FPT, (char *) &UserFlash, sizeof (UserFlash), &x);
  if ( f != FR_OK ) {
    DispMsg = "Err slf2";
  }
  if ( !(f_eof(FPT)) ) {
    DispMsg = "File too big";
  }
  f_close( FPT );
}

void save_ram_file ( int i ) {
  FRESULT f;
  int fss;
  int data = WRITE;
  uint x=0;

    process_cmdline_set_lift();
    init_state();
    checksum_all();

    sys_disk_write_enable(1);
    if (i == 0) {
      f = f_open (FPT, STATE_FILE, FA_CREATE_ALWAYS | FA_READ | FA_WRITE);
      if (f != FR_OK) {
	f_close (FPT);
	sys_disk_write_enable(0);
	DispMsg = "Err srf1";
	return;
      }
    }
    else {
      fss = file_selection_screen ("Save RAM File", "/wp34s", ".dat", open_selected_file, DISP_NEW, OVERWRITE_CHECK, &data );
      if (fss != 1) return;
    }
    // File is now open with correct permissions
    f = f_write (FPT, (char *) &PersistentRam, sizeof (PersistentRam), &x);
    if ( f != FR_OK ) {
      DispMsg = "Err srf2";
    }
    f_close( FPT );
    sys_disk_write_enable(0);
}

int load_ram_file ( int i ) { // this will load backup files too!
  FRESULT f;
  int fss;
  int data = READ;
  uint x=0;
  
  if (i == 0) {
    f = f_open (FPT, STATE_FILE, FA_READ);
    if (f != FR_OK) {
      f_close (FPT);
      //      DispMsg = "No file?";
      return 1;
    }
  }
  else {
    fss = file_selection_screen ("Load RAM File", "/wp34s", ".dat", open_selected_file, NO_DISP_NEW, NO_OVERWRITE_CHECK, &data );
    if (fss != 1) return 1;
  }
  // File is now open with correct permissions
  f = f_read (FPT, (char *) &PersistentRam, sizeof (PersistentRam), &x);
  if ( f != FR_OK ) {
    DispMsg = "Err lrf2";
      f_close( FPT );
      return 1;
  }
  if ( !(f_eof(FPT)) ) {
    DispMsg = "File too big";
      f_close( FPT );
      return 1;
  }
  f_close( FPT );
  return 0;
}

void load_backup_file ( int i ) { // goes into backup!
  FRESULT f;
  int fss;
  int data = READ;
  uint x=0;

  if (i == 0) {
    f = f_open (FPT, BACKUP_FILE, FA_READ);
    if (f != FR_OK) {
      f_close (FPT);
      return;
    }
  }
  else {
    fss = file_selection_screen ("Load RAM Backup File", "/wp34s", ".dat", open_selected_file, NO_DISP_NEW, NO_OVERWRITE_CHECK, &data );
    if (fss != 1) return;
  }
  // File is now open with correct permissions
  f = f_read (FPT, (char *) &BackupFlash, sizeof (BackupFlash), &x);
  if ( f != FR_OK ) {
    DispMsg = "Err lbf2";
  }
  if ( !(f_eof(FPT)) ) {
    DispMsg = "File too big";
  }
  f_close( FPT );
}

int open_selected_file (const char * fpath, const char * fname, void * data) {
  FRESULT f;

  if (*(int*)data == WRITE) {
    // open file to write
    f = f_open (FPT, fpath, FA_CREATE_ALWAYS | FA_READ | FA_WRITE);
  }
  else if (*(int*)data == READ) {
    // open file to read
    f = f_open (FPT, fpath, FA_READ);
  }
  else {
    DispMsg = "data=3?";
    return 3; // serious error!
  }
  if (f != FR_OK) {
    f_close (FPT);
    sys_disk_write_enable(0);
    DispMsg = "File err";
    return 2; // file can't be opened
  }
  else {
    return 1; // file opened successfully
  }
}

void save_prog_file () {
  opcode lbl; 
  unsigned int pc;
  unsigned short steps, prog_crc;
  FRESULT f;
  int data = WRITE;
  int fss;
  uint x=0;
  char* buffer;
  FLASH_REGION* fr;

  
  if ( not_running() ) {
    /*
     *  Don't copy from library or XROM
     */
    pc = nLIB( state_pc() );
    if ( pc == REGION_XROM ) {
      error_message( ERR_ILLEGAL );
      return;
    }
    /*
     *  Check if program is labeled
     */
    update_program_bounds( 1 );
    lbl = getprog( ProgBegin );
    if ( !isDBL(lbl) || opDBL(lbl) != DBL_LBL ) {
      error_message( ERR_NO_LBL );
      return;
    }
    /*
     *  Compute steps and crc for program
     */
    steps = 1 + ProgEnd - ProgBegin;
    prog_crc = crc16( get_current_prog(), steps << 1 );
    /*
     * Get filename and open file
     */
    sys_disk_write_enable(1);

    fss = file_selection_screen ("Save current program", "/wp34s", ".dat", open_selected_file, DISP_NEW, OVERWRITE_CHECK, &data );
    if (fss != 1) return;

    // File is now open with correct permissions

    buffer = calloc (RAM_SIZE, 1);
    if (!buffer) {// calloc failure!
      f_close(FPT);
      sys_disk_write_enable(0);
      DispMsg = "Mem short";
      return;
    }

    fr = (FLASH_REGION*) buffer;
    fr->size = steps;
    fr->crc = prog_crc;

    xcopy (fr->prog, get_current_prog(), steps << 1);
   
    f = f_write (FPT, buffer, 2*sizeof(short)+(steps << 1), &x);
    free(buffer);
    
    if (f != FR_OK) { //odd?
      DispMsg = "Write err";
    }
    else {
      DispMsg = "Written";
    }
      
    f_close(FPT);
    sys_disk_write_enable(0);
    
  }
}

void load_prog_file () {
  int fss, fsize;
  int data = READ;
  uint x=0;
  FLASH_REGION* fr;
  char* buffer;

  fss = file_selection_screen ("Load PROG File", "/wp34s", ".dat", open_selected_file, NO_DISP_NEW, NO_OVERWRITE_CHECK, &data );
  if (fss != 1) return;

  // File is now open with correct permissions

  fsize = f_size(FPT);
  if (fsize > LIBRARY_SIZE) {// too big! Wrong file?
    f_close(FPT);
    DispMsg = "File too big";
    return;
  }
  buffer = calloc (((fsize>>8)+1)<<8, 1);
  if (!buffer) {// calloc failure!
    f_close(FPT);
    DispMsg = "Mem short";
    return;
  }

  f_read (FPT, buffer, fsize, &x);
  if (!f_eof(FPT)) { //odd?
    free(buffer);
    f_close(FPT);
    DispMsg = "File odd";
    return;
  }
  f_close(FPT);

  fr = (FLASH_REGION*) buffer;
    
  if (checksum_region(fr, fr)) {//crc failed - wrong filetype?
    DispMsg = "File crc err";
        free(buffer);
    return;
  }

  store_program_from_buffer (fr);
  free(buffer);
}

void store_program_from_buffer( FLASH_REGION* fr )
{ // buffer contains all of program file
  unsigned int pc;
  int space_needed, count, free;

  if ( not_running() ) {
    /*
     *  Check if program is labeled
     */
    opcode lbl = (fr->prog)[0];
    if ( isDBL(lbl) ) {
      lbl |= (fr->prog)[1] << 16;
    }
    
    if ( !isDBL(lbl) || opDBL(lbl) != DBL_LBL ) {
      DispMsg = "No label";
      return;
    }
    /*
     *  Compute space needed
     */
    count = space_needed = fr->size;
    free = NUMPROG_FLASH_MAX - UserFlash.size;
    
    /*
     *  Find a duplicate label in the library and delete the program
     */
    pc = find_opcode_from( addrLIB( 0, REGION_LIBRARY ), lbl, 0 );
    if ( pc != 0 ) {
      /*
       *  CLP in library
       */
      unsigned int old_pc = state_pc();
      set_pc( pc );
      space_needed -= 1 + ProgEnd - ProgBegin;
      if ( space_needed <= free ) {
	clrprog();
      }
      set_pc( old_pc );
    }
    if ( space_needed > free ) {
      error_message( ERR_FLASH_FULL );
      return;
    }
    // 3. Append program
    flash_append( UserFlash.size, fr->prog, count, UserFlash.size + count );
  }
}

#else // if DM42 not defined ...

void load_statefile(const char *filename )
{
	FILE *f;
	char buffer[ FILENAME_MAX + 1 ];
#if !defined(QTGUI) && !defined(IOS)
	char *p;
#endif

	if ( filename != NULL && *filename != '\0' ) {
		expand_filename( StateFile, filename );
	}
	f = fopen( StateFile, "rb" );
	if ( f != NULL ) {
		fread( &PersistentRam, sizeof( PersistentRam ), 1, f );
		fclose( f );
	}
	f = fopen( expand_filename( buffer, BACKUP_FILE ), "rb" );
	if ( f != NULL ) {
		fread( &BackupFlash, sizeof( BackupFlash ), 1, f );
		fclose( f );
	}
	else {
		// Emulate a backup
		BackupFlash = PersistentRam;
	}
	f = fopen( expand_filename( buffer, LIBRARY_FILE ), "rb" );
	if ( f != NULL ) {
		fread( &UserFlash, sizeof( UserFlash ), 1, f );
		fclose( f );
	}
	init_library();

#if !defined(QTGUI) && !defined(IOS)
	/*
	 *  Load the configuration
	 *  1st line: COM port
	 *  2nd line: Tools directory
	 */
	f = fopen( expand_filename( buffer, "wp34s.ini" ), "rt" );
	if ( f != NULL ) {
		// COM port
		p = fgets( buffer, FILENAME_MAX, f );
		if ( p != NULL ) {
			strtok( buffer, "#:\r\n\t " );
			if ( *buffer != '\0' ) {
				strncpy( ComPort, buffer, FILENAME_MAX );
			}
		}
		// Assembler
		p = fgets( buffer, FILENAME_MAX, f );
		if ( p != NULL ) {
			strtok( buffer, "#\r\n\t" );
			if ( *buffer != '\0' ) {
				p = buffer + strlen( buffer );
				while ( p != buffer && p[-1] == ' ' ) {
					*(--p) = '\0';
				}
				expand_filename( Assembler, buffer );
			}
		}
		fclose( f );
	}
#endif
}

/*
 *  Import text file
 */

static void show_log( char *logname, int rc )
{
	char msg[ 10000 ] = "";
	FILE *f = fopen( logname, "rt" );
	if ( f != NULL ) {
		int size = fread( msg, 1, sizeof( msg ) - 1, f );
		if ( size >= 0 ) {
			msg[ size ] = 0;
		}
		fclose( f );
	}
	remove( logname );
	if ( *msg == '\0' ) {
		sprintf( msg, "Cannot execute assembler %s, RC=%d", Assembler, rc );
	}
	ShowMessage( rc == 0 ? "Import Result" : "Import Failed", msg );
}

static char* mktmpname(char* name, const char* prefix)
{
#if (defined(QTGUI) || ( defined(USECURSES) && ! defined(WIN32) )) && ! defined(DM42)
	strcpy(name, "wp34s");
	strcat(name, prefix);
	strcat(name, "_XXXXXX");
	return mktemp(name);
#else
	return tmpnam(name);
#endif
}

void set_assembler(const char* assembler)
{
	strncpy(Assembler, assembler, FILENAME_MAX);
}

#ifdef QTGUI
static char* getTmpDir()
{
#ifdef _WIN32
	return getenv("TMP");
#else
	char *tmp = getenv("TMPDIR");
	if(tmp==NULL || *tmp==0)
	{
		tmp = P_tmpdir;
	}
	if(tmp==NULL || *tmp==0)
	{
		tmp="/tmp";
	}
	return tmp;
#endif
}
#endif

#define IMPORT_BUFFER_SIZE 10000
void import_textfile( const char *filename )
{
#ifdef QTGUI
	char previousDir[ IMPORT_BUFFER_SIZE ];
#endif
	char buffer[ IMPORT_BUFFER_SIZE ];
	char tempfile[ FILENAME_MAX ];
	char logfile[ FILENAME_MAX ];
	char *tempname, *logname;
	int rc = -1;
	FILE *f;

	tempname = mktmpname( tempfile, "tmp" );
	if ( *tempname == '\\' ) {
		++tempname;
	}
	logname = mktmpname( logfile, "log" );
	if ( *logname == '\\' ) {
		++logname;
	}

	sprintf( buffer, "%s %s -pp \"%s\" -o %s 1>%s 2>&1", Assembler, ASSEMBLER_OPTIONS, filename, tempname, logname );
#ifdef QTGUI
	getcwd(previousDir, IMPORT_BUFFER_SIZE);
	chdir(getTmpDir());
#endif
	rc = system( buffer );
	show_log( logname, rc );
	if ( rc == 0 ) {
		// Assembly successful
		int size, words = 0;
		f = fopen( tempname, "rb" );

		if ( f == NULL ) {
			ShowMessage( "Import Failed", "Assembler output file error: %s", strerror( errno ) );
		}
		else {
			size = (int) fread( buffer, 2, sizeof( buffer ) / 2, f );
			fclose( f );
			if ( size >= 2 ) {
				words = (unsigned char) buffer[ 3 ] * 256 + (unsigned char) buffer[ 2 ];
			}
			if ( words != size - 3 ) {
				// Bad file size
				ShowMessage( "Import Failed", "Bad assembler output file size %d", size );
			}
			else {
				append_program( (s_opcode *) ( buffer + 4 ), words );
				update_program_bounds( 1 );
			}
		}
	}
	remove( tempname );
	remove( "wp34s_pp.lst" );
#ifdef QTGUI
	chdir(previousDir);
#endif
}

/*
 *  Export: Print current program to text file
 */
#include "pretty.h"

static const char *pretty( unsigned char z ) {
  if ( z == 32 ) {
    return "space";
  }
  if ( z < 32 ) {
    return map32[ z & 0x1f ];
  }
  if (z >= 127) {
    return maptop[ z - 127 ];
  }
  return CNULL;
}


static void write_pretty( const char *in, FILE *f ) {
  const char *p;
  const char *delim;
  char c;

  delim = strchr( in, '\'' );
  if ( delim == NULL ) {
    delim = strchr( in, 0x06 );
  }
  while ( *in != '\0' ) {
    c = *in;
    p = NULL;
    if ( in++ == delim ) {
      if ( c == 0x06 ) {
	++in;
	c = ' ';
      }
    }
    else {
      p = pretty( c );
    }
    if ( p == CNULL ) {
      fputc( c, f );
    }
    else {
      fputc( '[', f );
      while ( *p != '\0' ) {
	fputc( *p++, f );
      }
      fputc( ']', f );
    }
  }
  fputc( '\n', f );
}


extern void export_textfile( const char *filename )
{
  FILE *f;
  unsigned int pc = state_pc();
  int runmode = State2.runmode;
  int numlen = isRAM( pc ) ? 3 : 4;

  f = fopen( filename, "wt" );
  if ( f == NULL ) return;

  if ( runmode ) {
    // current program
    pc = ProgBegin;
  }
  else {
    // complete program memory
    pc = 1;
    numlen = 3;
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
    fprintf( f, "%s ", buffer );
    p = prt( op, buffer );
    write_pretty( p, f );
    pc = do_inc( pc, runmode );
  }

  fclose( f );
}
#endif //ifndef DM42

