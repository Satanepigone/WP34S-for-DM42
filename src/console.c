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
#ifdef INCLUDE_STOPWATCH
#include "stopwatch.h"
#endif
#undef DM42SAFE


#ifdef DM42

#include <menu.c>

struct _ndmap no_key = {.key_34s = K_NOP, .shift = 0};
struct _ndmap heartbeat = {.key_34s = K_HEARTBEAT, .shift = -1};
struct _ndmap f_shift = {.key_34s = K_F, .shift = -1};
struct _ndmap g_shift = {.key_34s = K_G, .shift = -1};
struct _ndmap h_shift = {.key_34s = K_H, .shift = -1};
struct _ndmap release = {.key_34s = K_RELEASE, .shift = -1};
struct _ndmap do_multi (struct _ndmap r);
int KeyDown = 0;

static struct _ndmap remap (const int c) {

  struct _ndmap no_change = {.key_34s = (char) c, .shift = 0};

#include "keytran.c"

  if (c==K_HEARTBEAT) return heartbeat;

  if (c == 99) return release; // turn KEY_DOUBLE_RELEASE into ordinary release

  if (c == 44 ) {
    struct _ndmap sshot = {.key_34s = K_MULTI, .shift = SSHOT};
    return sshot;
  }
  
  if (Running | Pause) {
    return mapping_running[c];
  }

  // In alpha mode, make sure that f-shift F5, F6 can summon alpha menus
  if ( get_alpha_state() && cur_shift()==SHIFT_F) {
    if (c == KEY_F5) return (struct _ndmap) { K_ARROW, 1 };
    if (c == KEY_F6) return (struct _ndmap) { K_CMPLX, 1 };
  }

  struct _menu Ref = get_current_menu_ref();
  
  switch (c) {
  case KEY_F1:
    if (cur_shift() == SHIFT_N) return Ref.keys[0].unshifted;
    if (cur_shift() == SHIFT_F) return Ref.keys[0].shifted;
  case KEY_F2:
    if (cur_shift() == SHIFT_N) return Ref.keys[1].unshifted;
    if (cur_shift() == SHIFT_F) return Ref.keys[1].shifted;
  case KEY_F3:
    if (cur_shift() == SHIFT_N) return Ref.keys[2].unshifted;
    if (cur_shift() == SHIFT_F) return Ref.keys[2].shifted;
  case KEY_F4:
    if (cur_shift() == SHIFT_N) return Ref.keys[3].unshifted;
    if (cur_shift() == SHIFT_F) return Ref.keys[3].shifted;
  case KEY_F5:
    if (cur_shift() == SHIFT_N) return Ref.keys[4].unshifted;
    if (cur_shift() == SHIFT_F) return Ref.keys[4].shifted;
  case KEY_F6:
    if (cur_shift() == SHIFT_N) return Ref.keys[5].unshifted;
    if (cur_shift() == SHIFT_F) return Ref.keys[5].shifted;
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
  //  save_ram_file(0);
  SET_ST(STAT_PGM_END);
}


/*
 *  Dummies
 */
int is_key_pressed(void) 
{
  return !key_empty();
}

int get_key(void)
{
  return key_pop();
}

/*
 * k is a WP34s key code. I need to translate it into a DM42 keycode, and push it.
 * 
 */
int put_key( int k )
{
  const char wp34s_to_DM42[] = {
    KEY_SIGMA, KEY_INV, KEY_SQRT, KEY_LOG, KEY_F5, KEY_F6, // 0-5
    KEY_STO, KEY_RCL, KEY_RDN, KEY_SHIFT, KEY_COS, KEY_TAN, // 6-11
    KEY_ENTER, KEY_SWAP, KEY_CHS, KEY_E, KEY_BSP, 0, // 12-17
    KEY_XEQ, KEY_7, KEY_8, KEY_9, KEY_DIV, 0, // 18-23
    KEY_UP, KEY_4, KEY_5, KEY_6, KEY_MUL, 0, // 24-29
    KEY_DOWN, KEY_1, KEY_2, KEY_3, KEY_SUB, 0, // 30-35
    KEY_EXIT, KEY_0, KEY_DOT, KEY_RUN, KEY_ADD,}; // 36-40
  
  if ( ( k >= K00 ) && ( k <= K64 ) ) {
    return key_push( wp34s_to_DM42 [k] );
  }
  else {
    return -1;
  }
}

enum shifts shift_down(void)
{
	return SHIFT_N;
}

void start_key_timer (void);
long int keyticks (void);
void moveto (int line, int x);

static long int start_ticks;

void start_key_timer() {
    start_ticks = get_rtc_ticks();
    KeyDown = 1;
}

long int keyticks () {
  long int i;
  i = (get_rtc_ticks() - start_ticks)*10;
  return i >> 8;
}

/*
 * Debug function - print_debug displays two integers
 */

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
  sys_delay (1500);
  strcpy( print_string, spaces );
  moveto (3, 1);
  lcd_print (fReg , (const char*) print_string );
  lcd_refresh();
}
/*
 * End of debug code
 */

void do_now (int key, int shift) { // key and shift are like the output of remapped
  struct _ndmap temp;
  temp.key_34s = key;
  temp.shift = shift;
  process_keycode_with_shift (temp);
}

struct _ndmap do_multi (struct _ndmap r) {
  switch(r.shift) {
  case ONSTO: // flash_backup()
    flash_backup(OP_SAVE);
    reset_shift();
    r = no_key;
    break;
  case ONRCL: // flash_restore()
    flash_restore(OP_LOAD);
    reset_shift();
    r = no_key;
    break;
  case WRLIB: // save library file
    save_lib_file(1);
    reset_shift();
    r = no_key;
    break;
  case LLIB: // load library file
    load_lib_file(1);
    reset_shift();
    r = no_key;
    break;
  case WRTST: // save state file
    save_ram_file(1);
    reset_shift();
    r = no_key;
    break;
  case LDST:
    load_ram_file(1);
    reset_shift();
    r = no_key;
    break;
  case LDPRG:
    load_prog_file();
    reset_shift();
    r = no_key;
    break;
  case SVPRG:
    save_prog_file();
    reset_shift();
    r = no_key;
    break;
  case HELP:
#ifdef C_VERSION
    display_help_file("/HELP/wp34c_help.html");
#else
    display_help_file("/HELP/wp34s_help.html");
#endif
    reset_shift();
    r = no_key;
    break;
  case DOTS:
    do_all_dots();
    reset_shift();
    r = no_key;
    break;
  case SSHOT: // comes after f-key already pressed, but not released
    do_now(K_RELEASE,0); //release f-key; restore display
    do_now(K_SSHOT,0); //activates screenshot routine in keys
    r = no_key;
    break;
  case DEFMEN:
    toggle_default_menu();
    reset_shift();
    r = no_key;
    break;
  case SETUMEN:
    //    build_user_menu();
    reset_shift();
    r = no_key;;
    break;
  case KCPX:
    if (C_LOCKED) {
      set_menu(M_C_Lock);
      reset_shift();
    }
    else {
      do_now (K_CMPLX, 0);
      do_now (K_RELEASE, 0);
      r = no_key;
    }
  default:
    r = no_key;
  }
  return r;
}

/*
 *  Main loop
 */
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
  lcd_clear_buf();
  t20->newln = 0;
  t20->lnfill = 0;
  t20->fixed = 1;
  t20->xspc = -2;
  t20->bgfill = 0;
  run_menu_item_app = run_menu_item;
  menu_line_str_app = menu_line_str;

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
      if ( (Pause > 0) || KeyDown == 1 ) { // Only start timer if needed for pause or keypress
	sys_timer_start(0, 100);  // Timer 0: wake up for heartbeat 
	sys_sleep();
	if (sys_timer_timeout(0)) { // If timer has completed...
	  key_push (K_HEARTBEAT);
	  if (Pause > 0) Pause--;
	}
	sys_timer_disable(0); // stop timer
      }
      else {
	sys_sleep();
      }	
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
      key_pop_all(); //try clearing the ON key to stop double flashing of RPN
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
#ifdef INCLUDE_STOPWATCH
    if ( KeyCallback != NULL ) {
      c = (*KeyCallback)( c );
    }
    else if ( StopWatchRunning && ( Ticker % STOPWATCH_BLINK ) == 0 ) {
      dot( LIT_EQ, !is_dot( LIT_EQ ) );
      lcd_refresh();
    }
#endif
    if ( (c != K_HEARTBEAT) && (c >= 0) ) {
      reset_auto_off();
    }
    if (c >= 0) {
      remapped = remap(c);
      if (remapped.key_34s == K_SETMENU) {
	set_menu ( remapped.shift );
	display_current_menu ();
	reset_shift();
	remapped = no_key;
      }
      if (remapped.key_34s == K_SYS) {
	SET_ST(STAT_MENU);
	handle_menu(&MID_MENU, MENU_RESET, 0); // App menu
	CLR_ST(STAT_MENU);
	reset_shift();
	remapped =  no_key;
	clear_disp();
	display_current_menu();
      }
      if (remapped.key_34s == K_MULTI) {
	remapped = do_multi (remapped);
	clear_disp();
	display_current_menu();
      }
      if (remapped.key_34s == K_EXIT) break;
      process_keycode_with_shift(remapped);
      if ( ((remapped.key_34s != K_HEARTBEAT) && (remapped.key_34s != K_RELEASE)) || JustStopped) start_key_timer(); 
    }
  }
  save_ram_file(0);
  return;
}
