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
#undef DM42SAFE

#include "pretty.h"


#if defined(USECURSES) || defined(DM42) // want this in DM42
static unsigned char dots[400];
#endif

#if defined(USECURSES) && !defined(DM42)
static void dispreg(const char n, int index) {
  char buf[64];
        if (is_intmode())
                sprintf(buf, "%llx", (unsigned long long int)get_reg_n_int(index));
        else {
		if (is_dblmode())
			decimal128ToString(&(get_reg_n(index)->d), buf);
		else
			decimal64ToString(&(get_reg_n(index)->s), buf);
	}
        PRINTF("%c: %s", n, buf);
}
#endif

#if defined(USECURSES) || defined(DM42) // want this in DM42
/* Some wrapper routines to set segments of the display */
void set_dot(int n) {
        dots[n] = 1;
}
void clr_dot(int n) {
        dots[n] = 0;
}
int is_dot(int n) {
	return dots[n];
}
#endif

int setuptty(int reset) {
#ifdef CONSOLE
#if defined(USECURSES) && !defined(DM42) //don't want this in DM42
        if (reset)
                endwin();
        else {
                initscr();
                cbreak();
                noecho();
                //keypad(stdscr, TRUE);
				}
#endif
#endif
        return 0;
}


void reset_disp(void) {
#ifdef DM42
	int rcl = is_dot(RCL_annun);
	int bat = is_dot(BATTERY);
	int leq = is_dot(LIT_EQ);
	int rpn = is_dot(RPN);
	int i;
        for (i=0; i<400; i++)
		if (i != RCL_annun && i != BATTERY && i != LIT_EQ )
			clr_dot(i);
	dot(RCL_annun, rcl);
	dot(BATTERY, bat);
	dot(LIT_EQ, leq);
	dot(RPN, rpn);
#else  
#ifndef CONSOLE
	int rcl = is_dot(RCL_annun);
	int bat = is_dot(BATTERY);
	int leq = is_dot(LIT_EQ);
	int rpn = is_dot(RPN);
#ifndef REALBUILD
	*LastDisplayedText = '\0';
#endif
        wait_for_display();
#if defined(QTGUI) || defined(IOS)
    	xset(LcdData, 0, sizeof(LcdData));
    	xset(LastDisplayedNumber, ' ', NUMBER_LENGTH);
    	LastDisplayedNumber[NUMBER_LENGTH]=0;
    	xset(LastDisplayedExponent, ' ', EXPONENT_LENGTH);
    	LastDisplayedExponent[EXPONENT_LENGTH]=0;
#else
        // terrible code which assumes int are 4 bytes long. Works fine for realbuild and for WINGUI though
	xset((void *) AT91C_SLCDC_MEM, 0, 4 * 20);
#endif
	dot(RCL_annun, rcl);
	dot(BATTERY, bat);
	dot(LIT_EQ, leq);
	dot(RPN, rpn);
#else
// Console
#ifdef USECURSES
	int i;
        for (i=0; i<400; i++)
		if (i != RCL_annun && i != BATTERY && i != LIT_EQ )
			clr_dot(i);

	erase();
        MOVE(0, 4);
#else
        putchar('\r');
        for (i=0; i<70; i++)
                putchar(' ');
        putchar('\r');
        putchar(' ');
#endif
#endif
        State2.invalid_disp = 0;
#endif
}

#if defined(DM42)
  #define setBlackPixel(x, y)                bitblt24(x, 1, y, 1, BLT_OR,   BLT_NONE)
  #define setWhitePixel(x, y)                bitblt24(x, 1, y, 1, BLT_ANDN, BLT_NONE)

/* void left_side (int i, int j) { //i - xleft reference; j - ytop reference */
/*   lcd_fill_rect ( i+2, j+11, 4, 8, 0xff ); */
/*   lcd_fill_rect ( i+3, j+3, 4, 8, 0xff ); */
/*   setBlackPixel ( i+4, j+2 ); */
/*   setBlackPixel ( i+3, j+19 ); */
/*   setWhitePixel ( i+6, j+3 ); */
/*   setWhitePixel ( i+5, j+18 ); */
/* } */

void left_side (int i, int j) { //i - xleft reference; j - ytop reference
  lcd_fill_rect (i+2, j+11, 1, 9, 0xff);
  lcd_fill_rect (i+3, j+2, 1, 18, 0xff);
  lcd_fill_rect (i+4, j+2, 1, 17, 0xff);
  lcd_fill_rect (i+5, j+3, 1, 15, 0xff);
  lcd_fill_rect (i+6, j+4, 1, 7, 0xff);
}
void left_side_top (int i, int j) {
  left_side(i, j);
}
void left_side_bottom (int i, int j) {
  left_side(i-2, j+19);
}
void right_side (int i, int j) {
  lcd_fill_rect (i+16, j+11, 1, 7, 0xff);
  lcd_fill_rect (i+17, j+4, 1, 15, 0xff);
  lcd_fill_rect (i+18, j+3, 1, 17, 0xff);
  lcd_fill_rect (i+19, j+2, 1, 18, 0xff);
  lcd_fill_rect (i+20, j+2, 1, 9, 0xff);
}
void right_side_top (int i, int j) {
  right_side(i, j);
}
void right_side_bottom (int i, int j) {
  right_side(i-2, j+19);
}
void top (int i, int j) {
  lcd_fill_rect ( i+5, j, 14, 1, 0xff );
  lcd_fill_rect ( i+4, j+1, 16, 1, 0xff );
  lcd_fill_rect ( i+6, j+2, 12, 1, 0xff );
  lcd_fill_rect ( i+7, j+3, 10, 1, 0xff );
}
void middle (int i, int j) {
  lcd_fill_rect ( i+5, j+19, 11, 3, 0xff );
  setBlackPixel ( i+4, j+20 );
  setBlackPixel ( i+16, j+20 );
}
void bottom (int i, int j) {
  lcd_fill_rect ( i+2, j+40, 14, 1, 0xff );
  lcd_fill_rect ( i+1, j+39, 16, 1, 0xff );
  lcd_fill_rect ( i+3, j+38, 12, 1, 0xff );
  lcd_fill_rect ( i+4, j+37, 10, 1, 0xff );
}
void decimal (int i, int j) {
  lcd_fill_rect (i+19, j+40, 5, 5, 0xff);
}
void comma (int i, int j) {
  lcd_fill_rect (i+21, j+45, 3, 3, 0xff);
  lcd_fill_rect (i+19, j+48, 3, 3, 0xff);
}

void exp_left_side (int i, int j) {
  lcd_fill_rect (i, j+6, 1, 6, 0xff );
  lcd_fill_rect (i+1, j, 1, 11, 0xff );
  lcd_fill_rect (i+2, j+1, 1, 9, 0xff );
  lcd_fill_rect (i+3, j+2, 1, 4, 0xff );
}
void exp_left_side_top (int i, int j) {
  exp_left_side(i+1, j);
}
void exp_left_side_bottom (int i, int j) {
  exp_left_side(i, j+11);
}
void exp_right_side (int i, int j) {
  lcd_fill_rect (i+12, j, 1, 6, 0xff );
  lcd_fill_rect (i+11, j+1, 1, 11, 0xff );
  lcd_fill_rect (i+10, j+2, 1, 9, 0xff );
  lcd_fill_rect (i+9, j+6, 1, 4, 0xff );
}
void exp_right_side_top (int i, int j) {
  exp_right_side(i, j);
}
void exp_right_side_bottom (int i, int j) {
  exp_right_side(i-1, j+11);
}
void exp_top (int i, int j) {
  lcd_fill_rect (i+4, j, 7, 1, 0xff );
  lcd_fill_rect (i+5, j+1, 5, 1, 0xff );
  lcd_fill_rect (i+6, j+2, 3, 1, 0xff );
}
void exp_bottom (int i, int j) {
  lcd_fill_rect (i+2, j+22, 7, 1, 0xff );
  lcd_fill_rect (i+3, j+21, 5, 1, 0xff );
  lcd_fill_rect (i+4, j+20, 3, 1, 0xff );
}
void exp_middle (int i, int j) {
  lcd_fill_rect (i+4, j+10, 5, 1, 0xff );
  lcd_fill_rect (i+3, j+11, 7, 1, 0xff );
  lcd_fill_rect (i+4, j+12, 5, 1, 0xff );
}
  
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
	  lcd_fill_rect (235, ytop-34, 15, 4, 0xff);
	  lcd_fill_rect (235, ytop-24, 15, 4, 0xff);
        }
	/* t20->newln = 0; */
	/* t20->lnfill = 0; */
	/* t20->fixed = 1; */
	/* t20->xspc = -2; */
	t20->inv = 0;
	
        if (dots[DOWN_ARR]) {
	  lcd_fill_rect(277,ytop_exp-70,5,12,0xff);
	  lcd_fill_rect(275,ytop_exp-58,9,1,0xff);
	  lcd_fill_rect(276,ytop_exp-57,7,1,0xff);
	  lcd_fill_rect(277,ytop_exp-56,5,1,0xff);
	  lcd_fill_rect(278,ytop_exp-55,3,1,0xff);
	  lcd_fill_rect(279,ytop_exp-54,1,1,0xff);
        }

	if (dots[INPUT]) {
	  lcd_setXY(t20, 297, ytop_exp-70);
	  lcd_writeText(t20,"INPUT");
        }

	if (dots[LIT_EQ]) {
	  lcd_setXY (t20, 357, ytop_exp-70);
	  lcd_writeText(t20, "=");
        }

	if (dots[BATTERY]) {
	  //    MOVE(70, 10);   PRINTF("####-");
        }

	if (dots[BEG]) {
	  lcd_setXY (t20, 275, ytop_exp-50);
	  lcd_writeText(t20, "BEG");
        }

	if (dots[STO_annun]) {
	  lcd_setXY (t20, 320, ytop_exp-50);
	  lcd_writeText(t20, "STO");
        }

	if (dots[RCL_annun]) {
	  lcd_setXY (t20, 365, ytop_exp-50);
	  lcd_writeText(t20, "RCL");
        }

	if (dots[RAD]) {
	  lcd_setXY (t20, 275, ytop_exp-30);
	  lcd_writeText(t20, "RAD");
	}

	if (dots[DEG]) {
	  lcd_setXY (t20, 320, ytop_exp-30);
	  lcd_writeText(t20, "360");
        }
        if (dots[RPN]) {
	  lcd_setXY (t20, 365, ytop_exp-30);
	  lcd_writeText(t20, "RPN");
        }

	/* The graphical bit last */
        for (i=0; i<BITMAP_WIDTH; i++) {
	  for (j=0; j<6; j++) {
	    if (dots[i*6+j+MATRIX_BASE]) {
	      //  lcd_fill_rect( xleft-dwidth+2*i, ytop-(5+3*6)+j*2, 2, 2, 0xff);
	      lcd_fill_rect( xleft-dwidth+5*i, ytop-(10+5*6)+j*5, 4, 4, 0xff);
	    }
	  }
	}
}
#endif

#if defined(USECURSES) && !defined(DM42)
void show_disp(void) {
        int i, j, p, x;
        const int dig_base = 16;

        /* Segments 0 - 107 are the main digits */
        for (i=0; i<DISPLAY_DIGITS; i++) {
	  p = i*SEGS_PER_DIGIT; // 9 - 7 + dot and comma
                x = 3+5*i;
                if (dots[p]) {
                        MOVE(x+1, dig_base);    PRINTF("--");
                }
                if (dots[p+1]) {
                        MOVE(x, dig_base+1);    PRINTF("|");
                        MOVE(x, dig_base+2);    PRINTF("|");
                }
                if (dots[p+3]) {
                        MOVE(x+3, dig_base+1);  PRINTF("|");
                        MOVE(x+3, dig_base+2);  PRINTF("|");
                }
                if (dots[p+2]) {
                        MOVE(x+1, dig_base+3);  PRINTF("--");
                }
                if (dots[p+4]) {
                        MOVE(x, dig_base+4);    PRINTF("|");
                        MOVE(x, dig_base+5);    PRINTF("|");
                }
                if (dots[p+6]) {
                        MOVE(x+3, dig_base+4);  PRINTF("|");
                        MOVE(x+3, dig_base+5);  PRINTF("|");
                }
                if (dots[p+5]) {
                        MOVE(x+1, dig_base+6);  PRINTF("--");
                }
                if (dots[p+7]) {
                        MOVE(x+4, dig_base+6);  PRINTF(".");
                }
                if (dots[p+8]) {
                        MOVE(x+3, dig_base+7);  PRINTF("/");
                }
        }
        /* Segments 108 - 128 are the exponent digits */
        for (i=0; i<3; i++) {
                p = i*7+108;
                x = 66 + i * 4;
                if (dots[p]) {
                        MOVE(x+1, dig_base-1);  PRINTF("-");
                }
                if (dots[p+1]) {
                        MOVE(x, dig_base);      PRINTF("|");
                }
                if (dots[p+3]) {
                        MOVE(x+2, dig_base);    PRINTF("|");
                }
                if (dots[p+2]) {
                        MOVE(x+1, dig_base+1);  PRINTF("-");
                }
                if (dots[p+4]) {
                        MOVE(x, dig_base+2);    PRINTF("|");
                }
                if (dots[p+6]) {
                        MOVE(x+2, dig_base+2);  PRINTF("|");
                }
                if (dots[p+5]) {
                        MOVE(x+1, dig_base+3);  PRINTF("-");
                }
        }
        /* Segments 129 & 130 are the signs */
        if (dots[MANT_SIGN]) {
                MOVE(0, dig_base+3);
                PRINTF("--");
        }
        if (dots[EXP_SIGN]) {
                MOVE(64, dig_base+1);
                PRINTF("-");
        }
        if (dots[BIG_EQ]) {
                MOVE(47, 12);   PRINTF("==");
        }
        if (dots[LIT_EQ]) {
                MOVE(64, 10);   PRINTF("=");
        }
        if (dots[DOWN_ARR]) {
                MOVE(52, 10);   PRINTF("v");
        }
        if (dots[INPUT]) {
                MOVE(55, 10);   PRINTF("INPUT");
        }
        if (dots[BATTERY]) {
                MOVE(70, 10);   PRINTF("####-");
        }
        if (dots[BEG]) {
                MOVE(52, 12);   PRINTF("BEG");
        }
        if (dots[STO_annun]) {
                MOVE(62, 12);   PRINTF("STO");
        }
        if (dots[RCL_annun]) {
                MOVE(72, 12);   PRINTF("RCL");
        }
        if (dots[RAD]) {
                MOVE(52, 14);   PRINTF("RAD");
        }
        if (dots[DEG]) {
                MOVE(62, 14);   PRINTF("360");
        }
        if (dots[RPN]) {
                MOVE(72, 14);   PRINTF("RPN");
        }
        /* The graphical bit last */
        for (i=0; i<BITMAP_WIDTH; i++)
                for (j=0; j<6; j++) {
                        if (dots[i*6+j+MATRIX_BASE]) {
                                MOVE(1+i, 9+j);
                                PRINTF("#");
                        }
                }
}
#endif

void show_stack(void) {
#ifdef USECURSES
  int i;

        if (!State2.flags)
                return;

        // Stack display smashes the stack registers
        for (i=4; i<STACK_SIZE; i++) {
                MOVE(26, 8-i);
                PRINTF("%c ", i<stack_size()?'*':' ');
                dispreg(REGNAMES[i], regX_idx + i);
        }
        MOVE(53, 2);    dispreg(REGNAMES[regJ_idx-regX_idx], regJ_idx);
        MOVE(53, 1);    dispreg(REGNAMES[regK_idx-regX_idx], regK_idx);
        for (i=0; i<4; i++) {
                MOVE(0, 4-i);
                dispreg(REGNAMES[i], regX_idx + i);
        }
        MOVE(53, 4);
        dispreg(REGNAMES[regL_idx-regX_idx], regL_idx);
        MOVE(53, 3);
        dispreg(REGNAMES[regI_idx-regX_idx], regI_idx);
        MOVE(53, 0);
        PRINTF("stack depth: %d", stack_size());
#endif
}

void show_flags(void) {
#if defined(CONSOLE) && !defined(DM42)
	extern unsigned int get_local_flags(void);

	if (!State2.flags)
		return;
	MOVE(0, 0);
	PRINTF(" %c ", JustDisplayed ? '*' : ' ');
	MOVE(5, 0);
	switch (cur_shift()) {
	case SHIFT_F:   PRINTF("[f-shift]");    break;
	case SHIFT_G:   PRINTF("[g-shift]");    break;
	case SHIFT_H:   PRINTF("[h-shift]");    break;
	default:                                break;
	}
	if (State2.hyp) {
		MOVE(14, 0);
		if (State2.dot)
			PRINTF("[hyp]");
		else
			PRINTF("[hyp-1]");
	}
	if (!State2.runmode) {
		MOVE(21, 0);
		PRINTF("[prog]");
	}
	if (view_instruction_counter) {
		MOVE(28, 0);
		PRINTF("#%llu", instruction_count);
	}
	MOVE(0, 0);

#ifdef USECURSES
#define FLAG_BASE       5
	MOVE(10, FLAG_BASE);
	if (State2.rarg)
		PRINTF("[rcmd]");
	else if (State2.arrow)
		PRINTF("[arr]");
	if (State2.dot) {
		MOVE(18, FLAG_BASE);
		PRINTF("[dot]");
	}
	if (State2.ind) {
		MOVE(24, FLAG_BASE);
		PRINTF("[ind]");
	}
	if (State2.trace) {
		MOVE(30, FLAG_BASE);
		PRINTF("[trace]");
	}
	if (State2.cmplx) {
		MOVE(40, FLAG_BASE);
		PRINTF("[cmplx]");
	}
	if (State2.catalogue) {
		MOVE(50, FLAG_BASE);
		PRINTF("[cat %03u]", State2.catalogue);
	}
	if (State2.hms) {
		MOVE(64, FLAG_BASE);
		PRINTF("[H.MS]");
	}
	if (UState.fract) {
		MOVE(71, FLAG_BASE);
		PRINTF("[FRACT]");
	}
	if (State2.multi) {
		MOVE(71, FLAG_BASE+1);
		PRINTF("[MULTI]");
	}
	MOVE(50, FLAG_BASE+1);
	PRINTF("[RRS %03u]", ProgSize);
	if (State2.state_lift) {
		MOVE(10, FLAG_BASE+1);
		PRINTF("[lift]");
	}
	if (Running) {
		MOVE(18, FLAG_BASE+1);
		PRINTF("[running]");
	}
	MOVE(70, 5);
	PRINTF("iw = %u/%u", State2.window, IntMaxWindow);
	MOVE(30, FLAG_BASE+1);
	PRINTF("shft = %u", cur_shift());
	MOVE(40, FLAG_BASE+1);
	PRINTF("trig = %u", UState.trigmode);
	MOVE(60, FLAG_BASE+1);
	PRINTF("r = %u", ShowRegister);
//	MOVE(60, FLAG_BASE+1);
//	PRINTF("apos = %u", State2.alpha_pos);
	MOVE(10, FLAG_BASE+2);
	PRINTF("numdig = %u   alpha '%-31s'   lflags = %03o-%03o",
			State2.numdigit, Alpha, get_local_flags() >> 8,
			get_local_flags() & 0xff);
	if (State.entryp) {
		MOVE(0, FLAG_BASE+2);
		PRINTF("entryp");
	}
	MOVE(10, FLAG_BASE+3);
	PRINTF("digval=%u", State2.digval);
	MOVE(23, FLAG_BASE+3);
	PRINTF("pc = %03u", state_pc());
	MOVE(34, FLAG_BASE+3);
	PRINTF("ap = %u", State2.alpha_pos);
	MOVE(45, FLAG_BASE+3);
	PRINTF("cmddot = %u  cmdeex = %u  eol = %u",
			CmdLineDot, CmdLineEex, CmdLineLength);
	MOVE(0, FLAG_BASE+3);
	PRINTF("JG=%d", UState.jg1582?1582:1752);
#endif
#endif
}

void wait_for_display(void)
{
}

#if defined(USECURSES) && !defined(DM42) 
void finish_display(void) {
        show_disp();
        MOVE(0, 0);
        refresh();

}
#endif
#if defined(DM42) 
void finish_display(void) {
  //  lcd_clear_buf();
  lcd_fill_rect (0, 0, 400, 188, 0); //leave bottom 52 rows for menu 
  show_disp();
  lcd_refresh();
}
void finish_RPN(void) {//only refreshes the RPN flag
  t20->inv = !dots[RPN];
  lcd_setXY (t20, 350, 130-30); // 130 is ytop_exp
  lcd_writeText(t20, "RPN");
  lcd_refresh();
}

extern void all_menu_dots (void);

void do_all_dots(void) {
  for (int i=0; i<400; i++) {
    set_dot(i);
  }
  all_menu_dots();
  finish_display();
  clr_dot(RCL_annun);
  clr_dot(LIT_EQ);
}
#endif

#ifdef CONSOLE
#ifndef DM42
/* Take a string and cleanse all non-printing characters from it.
 * Replace them with the usual [xxx] sequences.
 */
extern const char *pretty(unsigned char);

static char *cleanse(const char *s) {
        static char res[50];
        char *p;

        for (p=res; *s != '\0'; s++) {
                unsigned char c = 0xff & *s;
                const char *m = pretty(c);
                if (m == NULL) {
                        *p++ = c;
                } else {
                        *p++ = '[';
                        p = scopy_char(p, m, ']');
                }
        }
        *p = '\0';
        return res;
}
#endif
#endif

void show_progtrace(char *buf) {
#ifndef DM42
#ifdef CONSOLE
        int pc = state_pc();

#ifdef USECURSES
        int i;

        if (!State2.flags)
                return;

        for (i=4; i>0 && pc >= 0; i--) {
                MOVE(0, i);
                if (pc) {
                        opcode op = getprog(pc);
                        PRINTF("%03d %08x: %s", pc, op, cleanse(prt(op, buf)));
                } else
                        PRINTF("000:");
                pc = do_dec(pc, 1);
        }
#endif
#endif
#endif       
}
