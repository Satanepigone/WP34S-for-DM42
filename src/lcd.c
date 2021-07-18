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

#ifdef DM42
#include "dmcp.h"
#endif

#define DM42SAFE
#include "lcd.h"
#include "display.h"
#include "xeq.h"

#if defined(INCLUDE_C_LOCK) || defined(INFRARED)
#include "data.h"
#include "storage.h"
#endif

#undef DM42SAFE

#include "pretty.h"


static unsigned char dots[TOP_DOTS];
/*
 * bit 1 of each char represents what's in the display buffer;
 * bit 0 is zeroed at the start of display() by the reset functions.
 * set_dot and clr_dot look at bit 1 to decide whether to draw / erase / leave
 * At the end of display(), "10" means that something is drawn which hasn't been
 * set by display(). finish_display looks for this, and erases such dots.
 */ 

/* Some wrapper routines to set segments of the display */
void set_dot(int n) {
  dots[n] |= 1;
  if (dots[n]&2) return;
  draw_dot(n);
  dots[n] = 3; // 11;
}
void clr_dot(int n) {
  dots[n] &= ~1;
  if ( !(dots[n]&2)) return;
  draw_dot(n);
  dots[n] = 0; 
}

int is_dot(int n) {
	return dots[n]&1;
}

void clear_disp(void) {
      for (int i=0; i<TOP_DOTS; i++)
	dots[i] = 0;
      lcd_clear_buf();
}
  
void reset_disp(void) {
  clr_dot(STO_annun); // gets turned back on if needed;
  int rcl = is_dot(RCL_annun);
  //  int bat = is_dot(BATTERY);
  int leq = is_dot(LIT_EQ);
  int rpn = is_dot(RPN);
  int i;
  reset_7_segment();
  for (i=128; i<TOP_DOTS; i++)
    dots[i] <<= 1;
  dot(RCL_annun, rcl);
  //  dot(BATTERY, bat);
  dot(LIT_EQ, leq);
  dot(RPN, rpn);
}

void reset_7_segment(void) {
  for (int i = 0; i <= 107; ++i) {
    dots[i] <<= 1;
  }
  clear_exponent();
}

/* This next function clears the exponent region in the buffer, 
 * and zeros that part of dots. This is needed because I currently 
 * have segments which overlap, so complete redrawing is needed for
 * small region.
 */
void clear_exponent(void) {
  lcd_fill_rect(XLEFT_EXP-DWIDTH_EXP, YTOP, 400-XLEFT_EXP, 30, 0);
  for (int i = 108; i<=128; ++i) {
    dots[i] = 0;
  }
  dots[EXP_SIGN] = 0;
}

void left_side (int i, int j, int col) { // i - xleft reference; j - ytop reference; col - colour
  //  lcd_fill_rect (i+2, j+11, 1, 9, col);
  lcd_fill_rect (i+2, j+13, 1, 7, col);
  lcd_fill_rect (i+3, j+2, 1, 18, col);
  lcd_fill_rect (i+4, j+2, 1, 17, col);
  lcd_fill_rect (i+5, j+3, 1, 15, col);
  //  lcd_fill_rect (i+6, j+4, 1, 7, col);
  lcd_fill_rect (i+6, j+4, 1, 5, col);
}
void left_side_top (int i, int j, int col) {
  //  left_side(i, j, col);
  left_side(i, j-1, col);
}
void left_side_bottom (int i, int j, int col) {
  left_side(i-2, j+19, col);
}
void right_side (int i, int j, int col) {
  //  lcd_fill_rect (i+16, j+11, 1, 7, col);
  lcd_fill_rect (i+16, j+13, 1, 5, col);
  lcd_fill_rect (i+17, j+4, 1, 15, col);
  lcd_fill_rect (i+18, j+3, 1, 17, col);
  lcd_fill_rect (i+19, j+2, 1, 18, col);
  //  lcd_fill_rect (i+20, j+2, 1, 9, col);
  lcd_fill_rect (i+20, j+2, 1, 7, col);
}
void right_side_top (int i, int j, int col) {
  //  right_side(i, j, col);
  right_side(i, j-1, col);
}
void right_side_bottom (int i, int j, int col) {
  right_side(i-2, j+19, col);
}
void top (int i, int j, int col) {
  //  lcd_fill_rect ( i+5, j, 14, 1, col );
  //  lcd_fill_rect ( i+4, j+1, 16, 1, col );
  //  lcd_fill_rect ( i+6, j+2, 12, 1, col );
  //  lcd_fill_rect ( i+7, j+3, 10, 1, col );
  lcd_fill_rect ( i+5, j-1, 14, 1, col );
  lcd_fill_rect ( i+4, j, 16, 1, col );
  lcd_fill_rect ( i+6, j+1, 12, 1, col );
  lcd_fill_rect ( i+7, j+2, 10, 1, col );
}
void middle (int i, int j, int col) {
  //  lcd_fill_rect ( i+5, j+21, 11, 1, col );
  //  lcd_fill_rect ( i+4, j+20, 13, 1, col );
  //  lcd_fill_rect ( i+5, j+19, 11, 1, col );
  lcd_fill_rect ( i+5, j+21, 11, 1, col );
  lcd_fill_rect ( i+4, j+20, 13, 1, col );
  lcd_fill_rect ( i+4, j+19, 13, 1, col );
  lcd_fill_rect ( i+5, j+18, 11, 1, col );
}
void bottom (int i, int j, int col) {
  lcd_fill_rect ( i+2, j+40, 14, 1, col );
  lcd_fill_rect ( i+1, j+39, 16, 1, col );
  lcd_fill_rect ( i+3, j+38, 12, 1, col );
  lcd_fill_rect ( i+4, j+37, 10, 1, col );
}
void decimal (int i, int j, int col) {
  lcd_fill_rect (i+19, j+40, 5, 5, col);
}
void comma (int i, int j, int col) {
  lcd_fill_rect (i+21, j+45, 3, 3, col);
  lcd_fill_rect (i+19, j+48, 3, 3, col);
}

void exp_left_side_top (int i, int j, int col) {
  lcd_fill_rect (i+1, j+2, 1, 10, col);
  lcd_fill_rect (i+2, j+1, 2, 12, col);
}
void exp_left_side_bottom (int i, int j, int col) {
  lcd_fill_rect (i+0, j+14, 1, 9, col);
  lcd_fill_rect (i+1, j+13, 2, 11, col);
}

void exp_right_side_top (int i, int j, int col) {
  lcd_fill_rect (i+11, j+2, 1, 9, col);
  lcd_fill_rect (i+9, j+1, 2, 11, col);
}
void exp_right_side_bottom (int i, int j, int col) {
  lcd_fill_rect (i+10, j+13, 1, 10, col);
  lcd_fill_rect (i+8, j+12, 2, 12, col);
}
void exp_top (int i, int j, int col) {
  lcd_fill_rect (i+3, j+0, 7, 1, col );
  lcd_fill_rect (i+2, j+1, 9, 2, col );
}
void exp_bottom (int i, int j, int col) {
  lcd_fill_rect (i+2, j+24, 7, 1, col );
  lcd_fill_rect (i+1, j+22, 9, 2, col );
}
void exp_middle (int i, int j, int col) {
  lcd_fill_rect (i+2, j+11, 8, 3, col );
}

void draw_dot (int n) {
  int x, col = 0, inverse = 1;
  if (dots[n] & 1) {
    col = 0xff;
    inverse = 0;
  }
  
  if (n <= 107) {
    x = XLEFT + DWIDTH*(n/9);
    switch (n % 9) {  
    case 0:
      top (x, YTOP, col);
      return;
    case 1:
      left_side_top (x, YTOP, col);
      return;
    case 2:
      middle (x, YTOP, col);
      return;
    case 3:
      right_side_top (x, YTOP, col);
      return;
    case 4:
      left_side_bottom (x, YTOP, col);
      return;
    case 5:
      bottom (x, YTOP, col);
      return;
    case 6:
      right_side_bottom (x, YTOP, col);
      return;
    case 7:
      decimal (x, YTOP, col);
      return;
    case 8:
      comma (x, YTOP, col);
      return;
    default:;
    }
  }
  else if (n <= 128) { // exponent digits
    x = XLEFT_EXP + DWIDTH_EXP*((n-108)/7);
    switch ((n-108) % 7) {
    case 0:
      exp_top (x, YTOP_EXP, col);
      return;
    case 1:
      exp_left_side_top (x, YTOP_EXP, col);
      return;
    case 2:
      exp_middle (x, YTOP_EXP, col);
      return;
    case 3:
      exp_right_side_top (x, YTOP_EXP, col);
      return;
    case 4:
      exp_left_side_bottom (x, YTOP_EXP, col);
      return;
    case 5:
      exp_bottom (x, YTOP_EXP, col);
      return;
    case 6:
      exp_right_side_bottom (x, YTOP_EXP, col);
      return;
    default:;
    }
  }
  else if (n <= 141) {
    switch (n) {
    case MANT_SIGN:
      middle (XLEFT - DWIDTH, YTOP, col);
      return;
    case EXP_SIGN:
      exp_middle (XLEFT_EXP-DWIDTH_EXP+2, YTOP, col);
      return;
    case BIG_EQ:
      lcd_fill_rect (250, Y_ANNUNC-34, 15, 4, col);
      lcd_fill_rect (250, Y_ANNUNC-24, 15, 4, col);
      return;
    case LIT_EQ:
      draw_LEQ();
      return;
    case DOWN_ARR:
      lcd_fill_rect(287,Y_ANNUNC-67,5,12,col);
      lcd_fill_rect(285,Y_ANNUNC-55,9,1,col);
      lcd_fill_rect(286,Y_ANNUNC-54,7,1,col);
      lcd_fill_rect(287,Y_ANNUNC-53,5,1,col);
      lcd_fill_rect(288,Y_ANNUNC-52,3,1,col);
      lcd_fill_rect(289,Y_ANNUNC-51,1,1,col);
      return;
    case INPUT:
      t20->inv = inverse;
      lcd_setXY(t20, 305, Y_ANNUNC-70);
      lcd_writeText(t20,"ALPHA");
      return;
    case BATTERY:
      x = get_lowbat_state() + 2*usb_powered();
      if (x == 3) x = 2;
      dots[BATTERY] &= ~1; // clear bit 0
      
      switch (x) {
      case 0: // neither low nor usb;
	if (dots[BATTERY] & 2) { // was on; needs clearing
	  draw_BATT (1, 1); // clear LOW
	  draw_BATT (2, 1); // clear USB
	}
	break;
      case 1: // draw LOW
	if (dots[BATTERY] & 2) draw_BATT (2, 1); // clear USB - might have been there
	draw_BATT (1, 0); // draw LOW
	dots[BATTERY] |= 1; // set bit 0
	break;
      case 2: // draw USB
	if (dots[BATTERY] & 2) draw_BATT (1, 1); // clear LOW - might have been there
	draw_BATT (2, 0); // draw USB
	dots[BATTERY] |= 1; // set bit 0
	break;
      default:;
      }
      return;
    case BEG:
#ifdef MODIFY_BEG_SSIZE8
      {
	int d = UState.stack_depth;
	if (!(dots[BEG] & 8)) { // test bit 3;
	  if (d) {
	    draw_BEG(8, 0);
	    dots[BEG] = 7; // 0111
	  }
	  else {
	    draw_BEG(4, 0);
	    dots[BEG] = 4; // 0100
	  }
	}
	else if (d == (dots[BEG] & 2)) { // no drawing needed
	  if (d) {
	    dots[BEG] = 7;
	  }
	  else {
	    dots[BEG] = 4;
	  }
	}
	else {
	  if (d) { // changing S:4 to S:8
	    draw_BEG(4, 1);
	    draw_BEG(8, 0);
	    dots[BEG] = 7;
	  }
	  else {
	    draw_BEG(8, 1);
	    draw_BEG(4, 0);
	    dots[BEG] = 4;
	  }
	}
      }
#else
      t20->inv = inverse;
      lcd_setXY (t20, 285, y_ann-50);
      lcd_writeText(t20, "BEG");
#endif
      return;
    case STO_annun:
      t20->inv = inverse;
      lcd_setXY (t20, 325, Y_ANNUNC-50);
      lcd_writeText(t20, "PGM");
      return;
    case RCL_annun:
      draw_RCL(inverse);
    case RAD:
      t20->inv = inverse;
      lcd_setXY (t20, 285, Y_ANNUNC-30);
      lcd_writeText(t20, "RAD");
      return;
    case DEG:
      t20->inv = inverse;
      lcd_setXY (t20, 325, Y_ANNUNC-30);
      lcd_writeText(t20, "DEG");
      return;
    case RPN:
      draw_RPN(inverse);
      return;
    default:;
    }
  }
  else if (n < MAX_DOTS) { // column = (n-142)/6; row = (n-142) % 6
    lcd_fill_rect ( XLEFT-DWIDTH+5*((n-142)/6)+15, YTOP-(10+6*6)+((n-142)%6)*6, 4, 5, col);
    return;
  }
  else if (n < TOP_DOTS) { // column = (n-MAX_DOTS)/6; row = ((n-MAX_DOTS)%6)
    lcd_fill_rect( XLEFT-DWIDTH+5*((n-MAX_DOTS)/6)+15, 70-(10+6*5)+((n-MAX_DOTS)%6)*5, 4, 4, col);
    return;
  }
}

void draw_BEG (int depth, int inverse) {
  t20->inv = inverse;
  lcd_setXY (t20, 285, Y_ANNUNC-50);
  if (depth == 4) {
    lcd_writeText(t20, "S:4");
  }
  else {
    lcd_writeText(t20, "S:8");
  }
  return;
}

void draw_BATT (int level, int inverse) { // level = 0; ok; level = 1; low power; level = 2; USB
  t20->inv = inverse; // If inverse is 1, this means clear it.
  lcd_setXY (t20, XLEFT-DWIDTH+15, Y_ANNUNC-70);
  if (level == 1) {
    lcd_writeText(t20, "LOW");
  }
  else if (level == 2) {
    lcd_writeText(t20, "USB");
  }
  return;
}

void finish_display(void) {
  draw_dot(BATTERY);
  draw_PRT();
  finish_7_segment();
  finish_alpha_row();
#ifdef TOP_ROW
  finish_top_row();
#endif
  lcd_refresh();
}

void finish_7_segment(void) {
  for (int i = 0; i <= EXP_SIGN; ++i) {
    if ((dots[i]&3) == 2) clr_dot(i);
  }
}

void finish_alpha_row(void) {
  for (int i = MATRIX_BASE; i < MAX_DOTS; ++i) {
    if ((dots[i]&3) == 2) clr_dot(i);
  }
}

#ifdef TOP_ROW
void finish_top_row(void) {
 for (int i = MB_TOP; i < TOP_DOTS; ++i) {
    if ((dots[i]&3) == 2) clr_dot(i);
  }
} 
#endif

void draw_RPN(int inverse) {
  t20->inv = inverse;
  if (ENTRY_RPN_ENABLED) {
    lcd_setXY (t20, 355, Y_ANNUNC-30);
    lcd_writeText(t20, "eRPN");
  }
  else {
    lcd_setXY (t20, 365, Y_ANNUNC-30);
    lcd_writeText(t20, "RPN");
  }
}

void draw_RCL(int inverse) {
  t20->inv = inverse;
  lcd_setXY (t20, 365, Y_ANNUNC-50);
  lcd_writeText(t20, "RUN");
  //  t20->inv = 0;
}

void draw_LEQ(void) { // little equals
  t20->inv = !(dots[LIT_EQ]&1);
  lcd_setXY (t20, 360, Y_ANNUNC-70);
#ifdef INCLUDE_C_LOCK
  if (C_LOCKED) {
    lcd_writeText(t20, "C_LK");
  }
  else {
    lcd_writeText(t20, "=");
  }
#else
  lcd_writeText(t20, "=");
#endif
  t20->inv = 0;
}

void draw_PRT (void) {
  t20->inv = !UState.print_on;
  lcd_setXY(t20, 250, Y_ANNUNC-70);
  lcd_writeText(t20,"PRT");
}

void finish_PRT (void) {
  draw_PRT();
  lcd_refresh();
}

void finish_LEQ(void) { // refreshes the LEQ flag only
  lcd_refresh();
}
  
extern void all_menu_dots (void);

void do_all_dots(void) {
  for (int i=0; i<TOP_DOTS; i++) {
    set_dot(i);
  }
  all_menu_dots();
  finish_display();
  clr_dot(RCL_annun);
  clr_dot(LIT_EQ);
}

