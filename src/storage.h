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

#ifndef __STORAGE_H__
#define __STORAGE_H__


// The actual size will be shorter on the device
#ifdef DM42
#define NUMPROG_FLASH	8190 //chosen to get length of user flash area multiple of 2k - short ints are 2 bytes, each step is 2 bytes too

#if defined (FOUR_K)
#define RAM_SIZE 4096 // in bytes
#else
#define RAM_SIZE 2048 // in bytes
#endif

#define LIBRARY_SIZE 16384 // in bytes
#else
#define NUMPROG_FLASH   9999
#endif

#ifdef REALBUILD
// Actual size of user flash area, Linker symbol on the device
extern char UserFlashSize;
#define NUMPROG_FLASH_MAX (((int) &UserFlashSize >> 1) - 2)
#else
#define NUMPROG_FLASH_MAX NUMPROG_FLASH
#endif
typedef struct _flash_region {
        unsigned short crc;
        unsigned short size;
        s_opcode prog[ NUMPROG_FLASH ];
} FLASH_REGION;

#ifdef DM42

extern TPersistentRam *main_ram, *backup_ram;
extern FLASH_REGION *library_ram;

#define PersistentRam (*main_ram)
#define BackupFlash (*backup_ram)
#define UserFlash (*library_ram)

#else

extern FLASH_REGION UserFlash;
extern TPersistentRam BackupFlash;

#endif

#ifndef REALBUILD
// Flag for "Export Program..."
extern int UseAliasNames;
#endif

extern unsigned short int crc16(const void *base, unsigned int length);
extern unsigned short int checksum_program(void);
extern int checksum_ram(void);
#define checksum_all() checksum_ram()
extern int checksum_backup(void);
extern void init_library(void);
extern int append_program(const s_opcode *source, int length);
extern void stoprog(opcode);
extern void delprog(void);

extern int init_34s(void);
extern void clrprog(void);
extern void clpall(void);
extern void clrall(void);
extern void reset(void);

extern void flash_backup(enum nilop op);
extern void flash_restore(enum nilop op);
extern int flash_remove( int step_no, int count );
extern void sam_ba_boot(void);
extern void save_program(enum nilop op);
extern void load_program(enum nilop op);
extern void load_registers(enum nilop op);
extern void load_sigma(enum nilop op);
extern void load_state(enum nilop op);
extern void store_program(enum nilop op);
extern void recall_program(enum nilop op);

#if !defined(REALBUILD) && !defined(IOS) &&!defined(DM42)
extern char StateFile[];
extern char ComPort[];
extern char Assembler[];
extern void save_statefile( const char *filename );
extern void load_statefile( const char *filename );
extern void import_textfile( const char *filename );
extern void export_textfile( const char *filename );
extern void set_assembler(const char* assembler);
#endif

#ifdef DM42
//extern void save_statefile( int i );
//extern void load_statefile( void );
//extern void import_textfile( const char *filename );
//extern void export_textfile( const char *filename );

//extern void load_statefile_state( int i );
//extern void load_statefile_backup( void );
//extern void load_statefile_library( void );
//extern void save_libraryfile( void );

extern void init_mem(void);
extern int check_create_wp34sdir(void);
extern void store_program_from_buffer (FLASH_REGION* fr);
extern void load_prog_file (void);
extern void save_prog_file (void);

extern void save_lib_file(int i);
extern void load_lib_file(int i);
extern void save_ram_file(int i);
extern int  load_ram_file(int i);
extern void load_backup_file(int i);
extern int open_selected_file (const char * fpath, const char * fname, void * data); 

extern void import_program (void);
extern int load_program_file (const char * fpath, const char * fname, void * data);
#endif

#endif
