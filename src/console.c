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
 *  This is the console emulator part
 */
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#ifdef DM42
#include "dmcp.h"
#endif

#define DM42SAFE

#include "keys.h"
#include "display.h"
#include "lcd.h"
#include "int.h"
#include "consts.h"
#include "storage.h"
#include "catalogues.h"
#undef DM42SAFE


#ifdef DM42

#include <menu.c>

const uint8_t mid_menu[] = {
    MI_SYSTEM_ENTER,
    MI_MSC,
    0 }; // Terminator

const smenu_t MID_MENU = { "System!",  mid_menu,   NULL, NULL };

struct _ndmap no_key = {.key_34s = K_NOP, .shift = 0};
struct _ndmap heartbeat = {.key_34s = K_HEARTBEAT, .shift = -1};
struct _ndmap f_shift = {.key_34s = K_F, .shift = -1};
struct _ndmap g_shift = {.key_34s = K_G, .shift = -1};
struct _ndmap h_shift = {.key_34s = K_H, .shift = -1};
struct _ndmap release = {.key_34s = K_RELEASE, .shift = -1};

struct _ndmap do_multi (struct _ndmap r);

static struct _ndmap remap (const int c) {

  struct _ndmap no_change = {.key_34s = (char) c, .shift = 0};

#include "keytran.c"

  if (c==K_HEARTBEAT) return heartbeat;

  if (c == 99) return release; // turn KEY_DOUBLE_RELEASE into ordinary release

  if (c == 44 ) {
    // Start click 
    start_buzzer_freq(4400); sys_delay(10); stop_buzzer();
    // Make screenshot - allow to report errors
    if ( create_screenshot(1) == 2 ) {
      // Was error just wait for confirmation
      wait_for_key_press();
    }
    // End click
    start_buzzer_freq(8800); sys_delay(10); stop_buzzer();
    return no_key;
  }

  
  if ( get_alpha_state() ) {
    if (c == KEY_SHIFT) { //deal with shift keys
      switch (cur_shift()) { 
      case SHIFT_N:
	return mapping_none[c]; // N -> F
	break;
      case SHIFT_F:
	return mapping_fshift[c]; // F -> G
	break;
      case SHIFT_G:
	return h_shift; // G -> H
	break;
      case SHIFT_H:
	return h_shift; // H -> N
      default:
	return no_change;
      }
    }
    return mapping_alpha [c];
  }
    
  switch (c) {
  case KEY_F1:
    if (cur_shift() == SHIFT_N) return Menus[current_menu].keys[0].unshifted;
    if (cur_shift() == SHIFT_F) return Menus[current_menu].keys[0].shifted;
  case KEY_F2:
    if (cur_shift() == SHIFT_N) return Menus[current_menu].keys[1].unshifted;
    if (cur_shift() == SHIFT_F) return Menus[current_menu].keys[1].shifted;
  case KEY_F3:
    if (cur_shift() == SHIFT_N) return Menus[current_menu].keys[2].unshifted;
    if (cur_shift() == SHIFT_F) return Menus[current_menu].keys[2].shifted;
  case KEY_F4:
    if (cur_shift() == SHIFT_N) return Menus[current_menu].keys[3].unshifted;
    if (cur_shift() == SHIFT_F) return Menus[current_menu].keys[3].shifted;
  case KEY_F5:
    if (cur_shift() == SHIFT_N) return Menus[current_menu].keys[4].unshifted;
    if (cur_shift() == SHIFT_F) return Menus[current_menu].keys[4].shifted;
  case KEY_F6:
    if (cur_shift() == SHIFT_N) return Menus[current_menu].keys[5].unshifted;
    if (cur_shift() == SHIFT_F) return Menus[current_menu].keys[5].shifted;
  }
  
  switch (cur_shift()) {
  case SHIFT_N:    
    return mapping_none[c];
    break;
  case SHIFT_F:
    return mapping_fshift[c];
    break;
  case SHIFT_G:
    return mapping_gshift[c];
    break;
  default:
    return no_change;
  }
}

#endif

void shutdown( void )
{
#ifndef DM42
  checksum_all();
  setuptty( 1 );
  save_statefile( NULL );
  exit( 0 );
#else
  //  save_ram_file(0);
  SET_ST(STAT_PGM_END);
#endif
}


/*
 *  Dummies
 */
int is_key_pressed(void) 
{
#ifdef DM42
  return !key_empty();
#else
  return 0;
#endif
}

int get_key(void)
{
  #ifdef DM42
  return key_pop();
  #else
  return 0;
  #endif
}

int put_key( int k )
{
  #ifdef DM42
  return key_push(k);
  #else
  return k;
  #endif
}

enum shifts shift_down(void)
{
	return SHIFT_N;
}

#ifndef DM42   // No serial operations for DM42
#ifndef WIN32  // Windows uses winserial.c
/*
 *  Open a COM port for transmission
 */
int open_port( int baud, int bits, int parity, int stopbits )
{
	return 0;
}


/*
 *  Close the COM port after transmission is complete
 */
extern void close_port( void )
{
}


/*
 *  Output a single byte to the serial
 */
void put_byte( unsigned char byte )
{
	report_err(ERR_PROG_BAD);
}


/*
 *  Force buffer flush
 */
void flush_comm( void )
{
}

#endif
#endif


/*
 *  Main loop
 */
#ifdef DM42
void start_key_timer (void);
int keyticks (void);
void moveto (int line, int x);

static int start_ticks;

void start_key_timer() {
    start_ticks = get_rtc_ticks();
}

int keyticks () {
  int i;
  i = (get_rtc_ticks() - start_ticks)*10;
  return i >> 8;
}

char spaces[22] = "                     ";	//21 spaces.
char print_string[22];
int n_p = 0;


void moveto(int line, int x) {//These are line number and character positions
  lcd_setLine(fReg, line-1); //line 0 is the top line
  lcd_setXY(fReg, 20 + (fReg->f)->width*(x-1), fReg->y);
}

void print_debug (int i, int j) {
  strcpy( print_string, spaces );
  sprintf ( print_string, "Debug: %4i,%4i", i, j);
  moveto (3, 1);
  lcd_print (fReg , (const char*) print_string );
  lcd_refresh();
  sys_delay (2500);
  /* wait_for_key_press(); */
  //  key_pop_all();
  // while ((key_pop()<=0) || (key_pop()==K_HEARTBEAT));;
  // key_pop_all();
  strcpy( print_string, spaces );
  moveto (3, 1);
  lcd_print (fReg , (const char*) print_string );
  lcd_refresh();
  // while (key_empty()<=0);; // wait for release
  // key_pop_all();
}

void print_debug2 (int i, char* j) {
  strcpy( print_string, spaces );
  sprintf ( print_string, "D:%4i,%13s", i, j);
  moveto (3, 1);
  lcd_print (fReg , (const char*) print_string );
  lcd_refresh();
  sys_delay (2500);
  /* wait_for_key_press(); */
  //  key_pop_all();
  // while ((key_pop()<=0) || (key_pop()==K_HEARTBEAT));;
  // key_pop_all();
  strcpy( print_string, spaces );
  moveto (3, 1);
  lcd_print (fReg , (const char*) print_string );
  lcd_refresh();
  // while (key_empty()<=0);; // wait for release
  // key_pop_all();
}

void do_now (int key, int shift) {
  struct _ndmap temp;
  temp.key_34s = key;
  temp.shift = shift;
  process_keycode_with_shift (temp);
}

struct _ndmap do_multi (struct _ndmap r) {
  switch(r.shift) {
  case LASTX: // Last x via RCL L
    do_now (K11, 0); // RCL
    do_now (K_RELEASE,0); // release
    do_now (K15,0); // TAN key
    r.key_34s = K_RELEASE;
    r.shift = 0; // set up for release;
    break;
  case ONSTO: // flash_backup()
    flash_backup(OP_SAVE);
    r = no_key;
    break;
  case ONRCL: // flash_restore()
    flash_restore(OP_LOAD);
    r = no_key;
    break;
  case WRLIB: // save library file
    save_lib_file(1);
    r = no_key;
    break;
  case LLIB: // load library file
    load_lib_file(1);
    r = no_key;
    break;
  case WRTST: // save state file
    save_ram_file(1);
    r = no_key;
    break;
  case LDST:
    load_ram_file(1);
    r = no_key;
    break;
  case LDPRG:
    load_prog_file();
    r = no_key;
    break;
  case SVPRG:
    save_prog_file();
    r = no_key;
    break;
  case HELP:
    run_help_file("/HELP/wp34s_help.html");
    r = no_key;
    break;
  default:
    r = no_key;
  }
  return r;
}

void program_main(){
  int c;
  struct _ndmap remapped;

  init_mem();
  init_RegionTab();
  init_34s();
  State2.flags = 1;  
  check_create_wp34sdir();
  if (!load_ram_file(0)) DispMsg = CNULL; // no message if it loads ok
  load_backup_file(0);
  load_lib_file(0);
  t20->newln = 0;
  t20->lnfill = 0;
  t20->fixed = 1;
  t20->xspc = -2;
  t20->bgfill = 0;
  
  display();
  display_current_menu();
  lcd_refresh();
  JustDisplayed = 0;
  /*
    Returns a positive integer when a key is pressed. Doesn't return until a key is pressed.
    Sleeps peacefully while waiting for a key.
    Handles (in principle!) turning off and on.
    =================
    Main event loop
    =================

    Status flags:
    ST(STAT_PGM_END)   - program should turn off state (set by auto off timer)
    ST(STAT_SUSPENDED) - Program ready for off and doesn't need to be woken-up again
    ST(STAT_OFF)       - Program in off state (only [EXIT] key can wake)
    ST(STAT_RUNNING)   - OS doesn't sleep in this mode
  */
  for(;;) {

    if ( ST(STAT_PGM_END) && ST(STAT_SUSPENDED) ) // In off mode and suspended
      {
	CLR_ST(STAT_RUNNING);
	sys_sleep();
      }
    else if (!ST(STAT_PGM_END) && key_empty()) // Go to sleep if no keys available
    {
      CLR_ST(STAT_RUNNING);
      sys_timer_start(0, 100);  // Timer 0: wake up for heartbeat 
      sys_sleep();
      if (sys_timer_timeout(0)) { // If timer has completed...
	key_push (K_HEARTBEAT);
	if (Pause > 0) Pause--;
      }
      sys_timer_disable(0); // stop timer
    }
    
    // Wakeup in off state or going to sleep
    if (ST(STAT_PGM_END) || ST(STAT_SUSPENDED) ) {
      if (!ST(STAT_SUSPENDED)) {
        // Going to off mode
        lcd_set_buf_cleared(0); // Mark no buffer change region
        draw_power_off_image(1);

        LCD_power_off(0);
        SET_ST(STAT_SUSPENDED);
        SET_ST(STAT_OFF);
      }
      // Already in OFF -> just continue to sleep above
      continue; // note - this skips the rest of the loop and goes back to the beginning,
      // thus reaching the sleep()
    }

    // Well, we are woken-up
    SET_ST(STAT_RUNNING);

    // Clear suspended state, because now we are definitely reached the active state
    CLR_ST(STAT_SUSPENDED);

    // Get up from OFF state
    if ( ST(STAT_OFF) ) {
      LCD_power_on();
      rtc_wakeup_delay(); // Ensure that RTC readings after power off will be OK

      CLR_ST(STAT_OFF);

      if ( !lcd_get_buf_cleared() )
        lcd_forced_refresh(); // Just redraw from LCD buffer
    }

    // Key is ready -> clear auto off timer
    //    if ( !key_empty() )
    //  reset_auto_off(); - moved as key here might be heartbeat
    // Fetch the key
    //  < 0 -> No key event
    //  > 0 -> Key pressed
    // == 0 -> Key released
    c = key_pop();
    if ( (c != K_HEARTBEAT) && (c > 0) ) {
      reset_auto_off();
      start_key_timer();
    }
    if (c >= 0) {
      remapped = remap(c);
      if (remapped.key_34s == K_SETMENU) {
	set_menu ( remapped.shift );
	display_current_menu ();
	switch (cur_shift()) { // gets rid of the shift state from the menu-selecting key
	case SHIFT_F:
	  remapped = f_shift; // F -> N
	  break;
	case SHIFT_G:
	  remapped = g_shift; // G -> N
	  break;
	case SHIFT_H:
	  remapped = h_shift; // H -> N
	default:
	  remapped = f_shift; // shouldn't happen
	}
      }
      if (remapped.key_34s == K_SYS) {
	SET_ST(STAT_MENU);
	handle_menu(&MID_MENU, MENU_RESET, 0); // App menu
	CLR_ST(STAT_MENU);
	remapped =  no_key;
	display_current_menu();
      }
      if (remapped.key_34s == K_MULTI) {
	remapped = do_multi (remapped);
	display_current_menu();
      }
      if (remapped.key_34s == K_EXIT) break;
      process_keycode_with_shift(remapped);
    }
  }
  save_ram_file(0);
  return;
}
#else
int main(int argc, char *argv[]) {
	int c, n = 0;
	int warm = 0;
//#include "pretty.c"

	xeq_init_contexts();
#ifndef DM42
	load_statefile( NULL );
#endif      
 skipargs:
	if (!warm)
		init_34s();
	State2.flags = 1;
	if (setuptty(0) == 0) {
		display();
		JustDisplayed = 0;
		while ((c = GETCHAR()) != GETCHAR_ERR && c != CH_QUIT) {
			  c=remap(c);
			if(c != K_UNKNOWN) {
			  process_keycode(c);
			  process_keycode(K_RELEASE);
			}
	}
		setuptty(1);
	}
	shutdown();
	return 0;
}
#endif
