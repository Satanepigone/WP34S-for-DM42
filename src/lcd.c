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
  dots[n] = 0; // =0 would work too.
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
  int bat = is_dot(BATTERY);
  int leq = is_dot(LIT_EQ);
  int rpn = is_dot(RPN);
  int i;
  for (i=0; i<TOP_DOTS; i++)
    //		if (i != RCL_annun && i != BATTERY && i != LIT_EQ )
    //			clr_dot(i);
    dots[i] <<= 1;
  dot(RCL_annun, rcl);
  dot(BATTERY, bat);
  dot(LIT_EQ, leq);
  dot(RPN, rpn);
}

void reset_7_segment(void) {
  for (int i = 0; i <= EXP_SIGN; ++i) {
    dots[i] <<= 1;
  }
}

#define setBlackPixel(x, y)                bitblt24(x, 1, y, 1, BLT_OR,   BLT_NONE)
#define setWhitePixel(x, y)                bitblt24(x, 1, y, 1, BLT_ANDN, BLT_NONE)

void left_side (int i, int j, int col) { //i - xleft reference; j - ytop reference
  lcd_fill_rect (i+2, j+11, 1, 9, col);
  lcd_fill_rect (i+3, j+2, 1, 18, col);
  lcd_fill_rect (i+4, j+2, 1, 17, col);
  lcd_fill_rect (i+5, j+3, 1, 15, col);
  lcd_fill_rect (i+6, j+4, 1, 7, col);
}
void left_side_top (int i, int j, int col) {
  left_side(i, j, col);
}
void left_side_bottom (int i, int j, int col) {
  left_side(i-2, j+19, col);
}
void right_side (int i, int j, int col) {
  lcd_fill_rect (i+16, j+11, 1, 7, col);
  lcd_fill_rect (i+17, j+4, 1, 15, col);
  lcd_fill_rect (i+18, j+3, 1, 17, col);
  lcd_fill_rect (i+19, j+2, 1, 18, col);
  lcd_fill_rect (i+20, j+2, 1, 9, col);
}
void right_side_top (int i, int j, int col) {
  right_side(i, j, col);
}
void right_side_bottom (int i, int j, int col) {
  right_side(i-2, j+19, col);
}
void top (int i, int j, int col) {
  lcd_fill_rect ( i+5, j, 14, 1, col );
  lcd_fill_rect ( i+4, j+1, 16, 1, col );
  lcd_fill_rect ( i+6, j+2, 12, 1, col );
  lcd_fill_rect ( i+7, j+3, 10, 1, col );
}
void middle (int i, int j, int col) {
  //  lcd_fill_rect ( i+5, j+19, 11, 3, col );
  //  setBlackPixel ( i+4, j+20 );
  //  setBlackPixel ( i+16, j+20 );
  lcd_fill_rect ( i+5, j+21, 11, 1, col );
  lcd_fill_rect ( i+4, j+20, 13, 1, col );
  lcd_fill_rect ( i+5, j+19, 11, 1, col );
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

void exp_left_side (int i, int j, int col) {
  lcd_fill_rect (i, j+6, 1, 6, col );
  lcd_fill_rect (i+1, j, 1, 11, col );
  lcd_fill_rect (i+2, j+1, 1, 9, col );
  lcd_fill_rect (i+3, j+2, 1, 4, col );
}
void exp_left_side_top (int i, int j, int col) {
  exp_left_side(i+1, j, col);
}
void exp_left_side_bottom (int i, int j, int col) {
  exp_left_side(i, j+11, col);
}
void exp_right_side (int i, int j, int col) {
  lcd_fill_rect (i+12, j, 1, 6, col );
  lcd_fill_rect (i+11, j+1, 1, 11, col );
  lcd_fill_rect (i+10, j+2, 1, 9, col );
  lcd_fill_rect (i+9, j+6, 1, 4, col );
}
void exp_right_side_top (int i, int j, int col) {
  exp_right_side(i, j, col);
}
void exp_right_side_bottom (int i, int j, int col) {
  exp_right_side(i-1, j+11, col);
}
void exp_top (int i, int j, int col) {
  lcd_fill_rect (i+4, j, 7, 1, col );
  lcd_fill_rect (i+5, j+1, 5, 1, col );
  lcd_fill_rect (i+6, j+2, 3, 1, col );
}
void exp_bottom (int i, int j, int col) {
  lcd_fill_rect (i+2, j+22, 7, 1, col );
  lcd_fill_rect (i+3, j+21, 5, 1, col );
  lcd_fill_rect (i+4, j+20, 3, 1, col );
}
void exp_middle (int i, int j, int col) {
  lcd_fill_rect (i+4, j+10, 5, 1, col );
  lcd_fill_rect (i+3, j+11, 7, 1, col );
  lcd_fill_rect (i+4, j+12, 5, 1, col );
}

#if 0
void show_disp(void) { // This function re-draws everything.
  // It may be fast enough. It does mean that clearing everything first is needed,
  // or we keep track of changes.
  // Try clearing everything first first.
        int i, j, p;
	uint32_t x;
	const uint32_t xleft = 26;
	const uint32_t ytop = 130; // was 170
	const uint32_t dwidth = 25;
	const uint32_t xleft_exp = 344;
	const uint32_t ytop_exp = 130; // was 170
	const uint32_t dwidth_exp = 18;
	const uint32_t y_ann = Y_ANNUNC;
        /* Segments 0 - 107 are the main digits */
        for (i=0; i<DISPLAY_DIGITS; i++) { // 0 -> 11 inclusive
                p = i*SEGS_PER_DIGIT;
                x = xleft + dwidth*i;
                if (dots[p]) { // top
		  top (x, ytop);
                }
                if (dots[p+1]) { // top left
		  left_side_top (x, ytop);
                }
                if (dots[p+3]) { // top right
		  right_side_top (x, ytop);
                }
                if (dots[p+2]) { // centre line
		  middle (x, ytop);
                }
                if (dots[p+4]) { // lower left
		  left_side_bottom (x, ytop);
                }
                if (dots[p+6]) { // lower right
		  right_side_bottom (x, ytop);
                }
                if (dots[p+5]) { // bottom
		  bottom (x, ytop);
                }
                if (dots[p+7]) { // point
		  decimal (x, ytop);
                }
                if (dots[p+8]) { // comma
		  comma (x, ytop);
                }
        }
        /* Segments 108 - 128 are the exponent digits */
        for (i=0; i<3; i++) {
                p = i*7+108;
                x = xleft_exp + dwidth_exp*i;
                if (dots[p]) { // top
		  exp_top (x, ytop_exp);
                }
                if (dots[p+1]) { // top left
		  exp_left_side_top (x, ytop_exp);
                }
                if (dots[p+3]) { // top right
		  exp_right_side_top (x, ytop_exp);
                }
                if (dots[p+2]) { // centre line
		  exp_middle (x, ytop_exp);
                }
                if (dots[p+4]) { // lower left
		  exp_left_side_bottom (x, ytop_exp);
                }
                if (dots[p+6]) { // lower right
		  exp_right_side_bottom (x, ytop_exp);
                }
                if (dots[p+5]) { // bottom
		  exp_bottom (x, ytop_exp);
                }
        }
        /* Segments 129 & 130 are the signs */
        if (dots[MANT_SIGN]) {
	  middle (xleft - dwidth, ytop);
        }
        if (dots[EXP_SIGN]) {
	  exp_middle (xleft_exp-dwidth_exp, ytop);
        }

	if (dots[BIG_EQ]) {
	  lcd_fill_rect (250, y_ann-34, 15, 4, 0xff);
	  lcd_fill_rect (250, y_ann-24, 15, 4, 0xff);
        }
	t20->inv = 0;
	
	draw_PRT();
	
	/* if (UState.print_on) { */
	/*   lcd_setXY(t20, 250, y_ann-70); */
	/*   lcd_writeText(t20,"PRT"); */
        /* } */

        if (dots[DOWN_ARR]) {
	  lcd_fill_rect(287,y_ann-67,5,12,0xff);
	  lcd_fill_rect(285,y_ann-55,9,1,0xff);
	  lcd_fill_rect(286,y_ann-54,7,1,0xff);
	  lcd_fill_rect(287,y_ann-53,5,1,0xff);
	  lcd_fill_rect(288,y_ann-52,3,1,0xff);
	  lcd_fill_rect(289,y_ann-51,1,1,0xff);
        }

	if (dots[INPUT]) {
	  lcd_setXY(t20, 305, y_ann-70);
	  lcd_writeText(t20,"ALPHA");
        }

	/* if (dots[LIT_EQ]) { */
	/*   lcd_setXY (t20, 360, y_ann-70); */
	/*   if (C_LOCKED) { */
	/*     lcd_writeText(t20, "C_LK"); */
	/*   } */
	/*   else { */
	/*     lcd_writeText(t20, "="); */
	/*   } */
        /* } */

	draw_LEQ();
	
	if (dots[BATTERY]) {
	  //    MOVE(70, 10);   PRINTF("####-");
        }
#ifdef MODIFY_BEG_SSIZE8
	if (dots[BEG]) {
	  lcd_setXY (t20, 285, y_ann-50);
	  lcd_writeText(t20, "S:8");
        }
	else {
	  lcd_setXY (t20, 285, y_ann-50);
	  lcd_writeText(t20, "S:4");
	}	  
#else
	if (dots[BEG]) {
	  lcd_setXY (t20, 285, y_ann-50);
	  lcd_writeText(t20, "BEG");
        }
#endif
	if (dots[STO_annun]) {
	  lcd_setXY (t20, 325, y_ann-50);
	  lcd_writeText(t20, "PGM");
        }

	/* if (dots[RCL_annun]) { */
	/*   lcd_setXY (t20, 365, y_ann-50); */
	/*   lcd_writeText(t20, "RUN"); */
        /* } */

	if (dots[RAD]) {
	  lcd_setXY (t20, 285, y_ann-30);
	  lcd_writeText(t20, "RAD");
	}

	if (dots[DEG]) {
	  lcd_setXY (t20, 325, y_ann-30);
	  lcd_writeText(t20, "DEG");
        }
	draw_RPN_RCL();
	
	/* The graphical bit last */
        for (i=0; i<BITMAP_WIDTH; i++) {
	  for (j=0; j<6; j++) {
	    if (dots[i*6+j+MATRIX_BASE]) {
	      lcd_fill_rect( xleft-dwidth+5*i+15, ytop-(10+6*6)+j*6, 4, 5, 0xff);
	    }
	  }
	}
#ifdef TOP_ROW
        for (i=0; i<BW_TOP; i++) {
	  for (j=0; j<6; j++) {
	    if (dots[i*6+j+MB_TOP]) {
	      lcd_fill_rect( xleft-dwidth+5*i+15, 70-(10+6*5)+j*5, 4, 4, 0xff);
	    }
	  }
	}
#endif
}
#endif

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
      exp_middle (XLEFT_EXP-DWIDTH_EXP, YTOP, col);
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
      return;
    case BEG:
#ifdef MODIFY_BEG_SSIZE8
      /* t20->inv = 0; */
      /* if (dots[BEG] & 1) { */
      /* 	lcd_setXY (t20, 285, Y_ANNUNC-50); */
      /* 	lcd_writeText(t20, "S:8"); */
      /* } */
      /* else { */
      /* 	lcd_setXY (t20, 285, Y_ANNUNC-50); */
      /* 	lcd_writeText(t20, "S:4"); */
      /* }	   */
      {
	int d = UState.stack_depth;
	//	print_debug(100, dots[BEG]);
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

void finish_display(void) {
  draw_PRT();
  finish_7_segment();
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

void show_progtrace(char *buf) {
}

void show_stack(void) {
}

void show_flags(void) {
}

void wait_for_display(void)
{
}

