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

#ifndef __LCD_H__
#define __LCD_H__
#include "xeq.h"


extern int setuptty(int reset);
extern void set_dot(int n);
extern void clr_dot(int n);
extern int is_dot(int n);
//extern void set_status_grob(unsigned long long int grob[6]);
extern void show_disp(void);
extern void wait_for_display(void);
extern void finish_display(void);
extern void finish_7_segment(void);
#ifdef DM42
extern void draw_dot(int n);
extern void draw_RPN(int inverse);
extern void draw_RCL(int inverse);
extern void draw_BEG(int depth, int inverse);
extern void draw_LEQ(void);
extern void finish_LEQ(void);
extern void finish_top_row(void);
extern void draw_PRT(void);
extern void finish_PRT(void);
extern void do_all_dots(void);
#endif
extern void show_flags(void);
extern void clear_disp(void);
extern void reset_disp(void);
extern void reset_7_segment(void);
extern void show_progtrace(char *buf);
extern void show_stack(void);

#define MANT_SIGN	129
#define EXP_SIGN	130
#define BIG_EQ		131
#define LIT_EQ		132
#define DOWN_ARR	133
#define INPUT		134
#define BATTERY		135
#define BEG		136
#define STO_annun	137
#define RCL_annun	138
#define RAD		139
#define DEG		140
#define RPN		141
#define MATRIX_BASE	142

#define SEGS_PER_DIGIT		9
#define SEGS_PER_EXP_DIGIT	7

#define SEGS_EXP_BASE		(DISPLAY_DIGITS*SEGS_PER_DIGIT)

//#define BIGGER_DISPLAY
//#define TOP_ROW

#ifdef BIGGER_DISPLAY
#define BITMAP_WIDTH		74
#define ALPHA_SWITCH		20
#define Y_ANNUNC                75
#else
#define BITMAP_WIDTH		43
#define ALPHA_SWITCH		12
#define Y_ANNUNC               130
#endif

#ifdef TOP_ROW
#define BW_TOP		        43 // if not this intmode display code needs changing
#define MAX_DOTS		(MATRIX_BASE + BITMAP_WIDTH*6) // for middle row
#define TOP_DOTS 		(MATRIX_BASE + BITMAP_WIDTH*6 + BW_TOP*6) // inc. top row
#define MB_TOP 			(MATRIX_BASE + BITMAP_WIDTH*6) // where top row starts
#else
#define BW_TOP			BITMAP_WIDTH 
#define MAX_DOTS 		(MATRIX_BASE + BITMAP_WIDTH*6 )
#define TOP_DOTS 		MAX_DOTS
#define MB_TOP			MATRIX_BASE
#endif

#define XLEFT 26
#define YTOP 130
#define DWIDTH 25
#define XLEFT_EXP 344
#define YTOP_EXP 130
#define DWIDTH_EXP 18

#endif
