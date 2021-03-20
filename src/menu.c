#include <menu.h>

void set_menu ( int new_menu ) {
  int m = current_menu;
  if (new_menu == current_menu) {
    current_menu = 0;
  }
  else if (new_menu == -1) {
    current_menu = last_menu;
  }
  else {
    current_menu = new_menu;
  }
  last_menu = m;
}

int get_menu () {
  return current_menu;
}

void display_current_menu () {
  display_menu (current_menu);
}

void set_last_menu () {
  set_menu (last_menu);
}

static unsigned char mdots[200][2];

static const struct _menu Menus[] = {
  {
    "Blank", // 0
    { 
      { NO_KEY, NO_KEY, "", "" },
      { NO_KEY, NO_KEY, "", "" },
      { NO_KEY, NO_KEY, "", "" },
      { NO_KEY, NO_KEY, "", "" },
      { ARROW_KEY, NO_KEY, "--\015", "" },
      { CMPLX_KEY, NO_KEY, "CPX", "" },
    }    
  },
  {
    "Clear", // 1
    { 
      { { K24, 1 }, NO_KEY, "CLProg", "" },
      { { K24, 2 }, NO_KEY, "CL\221", "" },
      { { K24, 3 }, NO_KEY, "CL X", "" },
      { NO_KEY, NO_KEY, "", "" },
      { ARROW_KEY, NO_KEY, "--\015", "" },
      { CMPLX_KEY, NO_KEY, "CPX", "" },
    }    
  },
  {
    "DISP", // 2
    { 
      { { K00, 3 }, { K05, 1 }, "ALL", "a b/c" },
      { { K01, 3 }, { K05, 2 }, "FIX", "d/c" },
      { { K02, 3 }, { K10, 1 }, "SCI", "H.MS" },
      { { K03, 3 }, { K11, 1 }, "ENG", "H.d" },
      { ARROW_KEY, { K62, 3 }, "--\015", "./," },
      { CMPLX_KEY, NO_KEY, "CPX", "" },
    }
  },
  {
    "BASE", // 3
    { 
      { { K22, 1 }, { K22, 3 }, "BIN", "not" },
      { { K22, 2 }, { K31, 3 }, "OCT", "and" },
      { { K23, 1 }, { K32, 3 }, "DEC", "or" },
      { { K23, 2 }, { K33, 3 }, "HEX", "xor" },
      { ARROW_KEY, { K34, 3}, "--\015", "RMDR" },
      { CMPLX_KEY, { K11, 1}, "CPX", "H.d" },
    }
  },
  {
    "ANGLES", // 4
    { 
      { { K10, 2 }, { K04, 1 }, "DEG", "P\015R" },
      { { K11, 2 }, { K04, 2 }, "RAD", "R\015P" },
      { { K12, 2 }, { K00, 1 }, "GRAD", "HYP" },
      { { K10, 1 }, { K11, 1 }, "H.MS", "H.d" },
      { ARROW_KEY, { K00, 2 }, "--\015", "HYP\235" },
      { CMPLX_KEY, NO_KEY, "CPX", "" },
    }
      
  },
  {
    "MISC", // 5
    { 
      { { K61, 1 }, { K32, 1 }, "|X|", "2\234" },
      { { K61, 2 }, { K32, 2 }, "RND", "Log\272" },
      { { K62, 1 }, { K33, 2 }, "IPart", "Log\213" },
      { { K62, 2 }, { K34, 2 }, "FPart", "||el" },
      { ARROW_KEY, { K54, 2 }, "--\015", "\203%" },
      { CMPLX_KEY, NO_KEY, "CPX", "" },
    }
  },
  {
    "X<>Y Key", // 6
    { 
      { { K21, 1 }, NO_KEY, "Show\016", "" },
      { { K21, 2 }, NO_KEY, "Show\015", "" },
      { { K21, 3 }, NO_KEY, "x\027?", "" },
      { NO_KEY, NO_KEY, "", "" },
      { ARROW_KEY, NO_KEY, "--\015", "" },
      { CMPLX_KEY, NO_KEY, "CPX", "" },
    }    
  },
  {
    "Program", // 7
    { 
      { { K63, 1 }, { K64, 1 }, "LBL", "DSE" },
      { { K63, 2 }, { K64, 2 }, "RTN", "ISG" },
      { { K61, 3 }, NO_KEY, "Pause", "" },
      { { K30, 3 }, NO_KEY, "GTO", "" },
      { ARROW_KEY, NO_KEY, "--\015", "" },
      { CMPLX_KEY, NO_KEY, "CPX", "" },
    }
  },
  {
    "Setup 1", // 8
    { 
      { { K_EXIT, 0}, NO_KEY, "EXIT", "" },
      { { K_MULTI, HELP }, NO_KEY, "HELP", "" },
      { { K_MULTI, ONSTO }, { K_MULTI, WRTST}, "OnSTO", "SvRAM" },
      { { K_MULTI, ONRCL }, { K_MULTI, LDST}, "OnRCL", "LdRAM" },
      { ARROW_KEY, NO_KEY, "--\015", "" },
      { CMPLX_KEY, NO_KEY, "CPX", "" },
    }
  },
  {
    "Arrow", // 9
    { 
      { { K22, 1 }, { K10, 2 }, "Bin", "DEG" },
      { { K22, 2 }, { K11, 2 }, "Oct", "RAD" },
      { { K23, 1 }, { K12, 2 }, "Dec", "Grad" },
      { { K23, 2 }, { K10, 1 }, "Hex", "2HMS" },
      { ARROW_KEY, { K11, 1 }, "--\015", "HMS2" },
      { CMPLX_KEY, NO_KEY, "CPX", "" },
    }
  },
  {
    "Flags", // 10
    { 
      { { K50, 1 }, NO_KEY, "SF_", "" },
      { { K50, 2 }, NO_KEY, "CF_", "" },
      { { K51, 1 }, NO_KEY, "x=?", "" },
      { { K51, 2 }, NO_KEY, "x\013?", "" },
      { ARROW_KEY, NO_KEY, "--\015", "" },
      { CMPLX_KEY, NO_KEY, "CPX", "" },
    }
  },
  {
    "Probability", // 11
    { 
      { { K40, 3 }, { K41, 1 }, "Fact!", "\224" },
      { { K40, 1 }, { K41, 2 }, "Cy,x", "\224\235" },
      { { K40, 2 }, NO_KEY, "Py,x", "" },
      { { K12, 1 }, NO_KEY, "RAN#", "" },
      { ARROW_KEY, NO_KEY, "--\015", "" },
      { CMPLX_KEY, NO_KEY, "CPX", "" },
    }
  },
  {
    "Statistics", // 12
    { 
      { { K42, 1 }, { K43, 3 }, "Xbar", "SUMS" },
      { { K43, 1 }, NO_KEY, "Yhat", "" },
      { { K42, 2 }, NO_KEY, "s", "" },
      { { K43, 2 }, NO_KEY, "r", "" },
      { ARROW_KEY, NO_KEY, "--\015", "" },
      { CMPLX_KEY, NO_KEY, "CPX", "" },
    }
  },
  {
    "Setup 2", // 13
    { 
      { { K_MULTI, WRLIB}, NO_KEY, "SvLIB", "" },
      { { K_MULTI, LLIB }, NO_KEY, "LdLIB", "" },
      { { K_MULTI, SVPRG }, NO_KEY, "SvCPRG", "" },
      { { K_MULTI, LDPRG }, NO_KEY, "LdPRG", "" },
      { ARROW_KEY, NO_KEY, "--\015", "" },
      { CMPLX_KEY, NO_KEY, "CPX", "" },
    }
  },
  {
    "Integ/Sum/Prod", // 14
    { 
      { { K52, 1 }, NO_KEY, "SLV", "" },
      { { K52, 2 }, NO_KEY, "\004 dx", "" },
      { { K53, 1 }, NO_KEY, "SUM", "" },
      { { K53, 2 }, NO_KEY, "PROD", "" },
      { ARROW_KEY, NO_KEY, "--\015", "" },
      { CMPLX_KEY, NO_KEY, "CPX", "" },
    }
  },
};  

/* This code is the set_status_sized code with small changes
 * and non-DM42 code removed.
 * MENU_WIDTH - width of menu item in "dots", not in pixels.
 */
#define MENU_WIDTH 32

static void set_menu_label (const char *str, int smallp, int dotcol, int shifted) {
  // dotcol is the number of the starting dot column
  // shifted is 1 if shifted
  unsigned short int posns[257];

  // Mark posns as uninitialized, smallp must be 0 or 1 for this to work correctly.
  int posns_state = 255;

  int x = dotcol;
  int i, j, xmax;
  xmax = ((dotcol/MENU_WIDTH)+1)*MENU_WIDTH;
  const int offset = smallp ? 256 : 0;

  forceDispPlot=0;

#ifdef RP_PREFIX
  RectPolConv = 0;
#endif
  while (*str != '\0' && x <= xmax )  {
    int c;
    int width;
    unsigned char cmap[6];

    int real_width;
    int current_smallp;

    // A 007 byte followed by a mode byte changes the way the following character is printed.
    // Bit 7 (MSB) of the mode byte is currently unused and should be set to 1.
    // Bits 6-5: 00 -> don't change font
    //           01 -> (not used)
    //           10 -> use big font
    //           11 -> use small font
    // Bits 4-0: character will be considered this wide
    if (str[0] == '\007') {
      width = str[1] & 0x1F;
      switch (str[1] & 0x60) {
      default:
      case 0x00:	current_smallp = smallp;
	break;
      case 0x40:	current_smallp = 0;
	break;
      case 0x60:	current_smallp = 1;
	break;
      }
      c = (unsigned char) str[2] + (current_smallp ? 256 : 0);
      str += 3;

      real_width = charlengths(c);
    } else {
      c = (unsigned char) *str++ + offset;
      real_width = width = charlengths(c);
      current_smallp = smallp;
    }

    if (x + real_width > xmax)
      break;

    if (posns_state != current_smallp) {
      findlengths(posns, current_smallp);
      posns_state = current_smallp;
    }
    unpackchar(c, cmap, current_smallp, posns); // cmap contains a row of c in each element

    for (i=0; i<6; i++) {// rows
      for (j=0; j<width; j++) { // width is width of character
	if (x+j >= xmax)
	  break;
	mdot(x+j, 5-i, shifted, (cmap[i] & (1 << j))?1:0); // set or reset dot at col x+j, row i
      }
    }
    x += width;
  }
}


/* Determine the pixel length of the string if it were displayed.

int pixel_length(const char *s, int smallp)
{
  int len = 0;
  const int offset = smallp ? 256 : 0;
  while (*s != '\0') {
    if (s[0] == '\007') {
      len += s[1] & 0x1F;
      s += 3;
      continue;
    }
    len += charlengths( (unsigned char) *s++ + offset );
  }
  return len;
}
*/


void mdot(int i, int j, int s, int on) { // Column i, row j, state on
  if ( (i<0) || (i>199) ) return;
  if (on) {
    mdots[i][s] |= ( 1 << j );
  }
  else {
    mdots[i][s] &= ~( 1 << j );
  }
}


void display_menu (int current_menu) {
  int len = 0;
  lcd_fill_rect (0, 188, 400, 52, 0); // clear bottom 52 rows for menu 
  //  if (current_menu == 0) return; // Menu 0 is blank
  // Not blank now - includes arrow and cmplx like all the others.
  lcd_fill_rect (0, 188, 400, 1, 0xff); // lines count from line 1? No. 
  //Clear previous menu
  for (int col = 0; col < 200; col++) {
    mdots[col][0] = 0;
    mdots[col][1] = 0;
  }

  // Draw menu items
  // 
  for (int item = 0; item < 6; item++ ) {
    // Label is Menus[current.menu].keys[item].unshifted_label
    // Unshifted first
    len = pixel_length (Menus[current_menu].keys[item].unshifted_label, 0); // 0 means not small font
    /*
      Starting dot in the row of 200 is item*32 - no gaps; gaps will be inserted when drawing
    */
    set_menu_label ( Menus[current_menu].keys[item].unshifted_label, 0, item*32 + (32 - len)/2, 0 );
    // Now shifted ...	
    len = pixel_length (Menus[current_menu].keys[item].shifted_label, 0); // 0 means not small font
    len = item*32 + (32-len)/2;
    if (len<0) len = 0;
    if (len>199) len = 199;
    set_menu_label ( Menus[current_menu].keys[item].shifted_label, 0, len, 1 );
  }

  for (int col = 0; col < 200; col++) {
    for (int row = 0; row < 6; row++) {
      if ((mdots[col][0] & (1 << row)) != 0) 
	//	lcd_fill_rect( col*2-1, 230 - row*3, 3, 3, 0xff);
	lcd_fill_rect( col*2, 233 - row*3, 3, 3, 0xff);
      if ((mdots[col][1] & (1 << row)) != 0) 
	//	lcd_fill_rect( col*2-1, 230 - 5*3 -10 - row*3, 3, 3, 0xff);
	lcd_fill_rect( col*2, 233 - 5*3 -10 - row*3, 3, 3, 0xff);
    }
  }
}

/*
dots is a char array.

I'm currently haveing dots 2 pixels wide, with no gaps between dots. 400/6 is 66; allowing for spaces
I need three pixels between one label and the next, for a line. 5*3=15; (400-15) / 6 = 64 with one over.
So, 32 dots per label.

One long one - one byte per column.
cmap contains the character dots.
Each element in mdot

    for (i=0; i<6; i++) // rows of character. cmap[i] contains width bits.
      for (j=0; j<width; j++) { // ( (cmap[i] & (1 << j)) ? 1:0 ) is the pixel in the ith row of the jth column. If it's 1 we need to set bit j 
	if (x+j >= MENU_WIDTH)
	  break;
//	mdot(x+j) |= (cmap[i] & (1 << j))?1:0);
	mdot(x+j) |= (cmap[i] & (1 << j))?1:0);
      }
      
To set dot y in column x, row[x] = row[x] | (1 << y)
To read dot y in column x, ( row[x] & (1 << y) )
To test dot y in column x, 
void m

c5 asdfg
c4 qwert
c3 ;lkjh
c2 zxcvb
c1 poiuy
c0 12345

dots(0) - aq;zp1
...
dots(4) - 5ybhtq

void mdot(int i, int j, int on) { // Column i, row j, state on
if (on) {
mdots(i) |= ( 1 << j );
else {
mdots(i) |= ~( 1 << j );
}
}
for j = 0 to width-1
for i = 0 to 5
dots(j) bit i = c[i] 
*/
