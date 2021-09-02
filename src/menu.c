//#include <menu.h>
extern const char SvnRevision[4];

struct _menu UserMenu =
  {
    "User Menu", 
    {
      { NO_KEY, NO_KEY, "", "" },
      { NO_KEY, NO_KEY, "", "" },
      { NO_KEY, NO_KEY, "", "" },
      { NO_KEY, NO_KEY, "", "" },
      { ARROW_KEY, NO_KEY, "--\015", "" },
      { CMPLX_KEY, { K_MULTI, DOTS }, "CPX", "" },
    }
  };

struct _menu MEMMenu =
  {
    "MEM Menu", 
    {
      { NO_KEY, NO_KEY, "", "" },
      { NO_KEY, NO_KEY, "", "" },
      { NO_KEY, NO_KEY, "", "" },
      { NO_KEY, NO_KEY, "", "" },
      { NO_KEY, NO_KEY, "", "" },
      { NO_KEY, NO_KEY, "", "" },
    }
  };

const char* arrow_key_string = "--\015";
const char* cmplx_key_string = "CPX";

menu_name current_menu = 0;
menu_name last_menu = 0;
menu_name default_menu = 0;

static void set_menu_label (const char *str, int smallp, int dotcol, int shifted);



/* 
 * System menu stuff
 */

#define MI_ABOUT_PGM 11

const uint8_t mid_menu[] = {
    MI_SYSTEM_ENTER,
    MI_MSC,
    MI_ABOUT_PGM,
    0 }; // Terminator

const smenu_t MID_MENU = { "System!",  mid_menu,   NULL, NULL };

void disp_about(void);

int run_menu_item(uint8_t line_id) {
  int ret = 0;

  switch(line_id) {
    case MI_ABOUT_PGM:
      disp_about();
      break;

     default:
      ret = MRET_UNIMPL;
      break;
  }

  return ret;
}

const char * menu_line_str(uint8_t line_id, char * s, const int slen) {
  const char * ln;

  switch(line_id) {

  case MI_ABOUT_PGM:    ln = "About >";              break;

  default:
    ln = NULL;
    break;
  }

  return ln;
}

void disp_about() {
#ifdef C_VERSION
  char title[] = "WP 34C calculator for DM42: v     ";
  xcopy ((title + 29), SvnRevision, 4);
#elif defined(FOUR_K)
  char title[] = "WP 34S_4k calculator for DM42: v     ";
  xcopy ((title + 32), SvnRevision, 4);
#else
  char title[] = "WP 34S calculator for DM42: v     ";
  xcopy ((title + 29), SvnRevision, 4);
#endif


  clear_disp(); // clears buffer and zeros dots[]
  lcd_writeClr(t24);

  lcd_setXY(t24, 0, 5);
  lcd_putsR(t24, title);
  lcd_setXY(t20, t24->x, t24->y);
  t20->newln = 1;
  t20->inv = 0;
#ifdef TOP_ROW
  lcd_puts(t20, "With longer alpha display and top row,");
#elif defined(BIGGER_DISPLAY)
  lcd_puts(t20, "With a longer alpha display,");
#else
  lcd_puts(t20, "Same display layout as the original WP34S,");
#endif
  lcd_puts(t20, "and some annunciators renamed.");
  lcd_puts(t20, "Includes printing, stopwatch,");
  lcd_puts(t20, "entry RPN, extended file handling,");
#ifdef FOUR_K
  lcd_puts(t20, "user-defined menus, 4k of RAM,");
  lcd_puts(t20, "and complex lock mode.");
#else
  lcd_puts(t20, "user-defined menus, and");
  lcd_puts(t20, "complex lock mode.");
#endif
  
  t20->y += 5;

  t20->bgfill = 1;
  lcd_putsR(t20, "Neither provided by          ");
  lcd_putsR(t20, "nor supported by SwissMicros.");
  t20->bgfill = 0;
  t20->newln = 0;
  
  t24->y = LCD_Y - lcd_lineHeight(t24)-5;
  t24->x = 20;
  lcd_printR(t24, "    Press EXIT key to continue...");

  lcd_refresh();

  wait_for_key_press();
}

/*
 * WP34s menu code
 */

void set_menu ( menu_name new_menu ) {
  menu_name m = current_menu;
  if (new_menu == current_menu) { // return to default
    current_menu = default_menu;
  }
  else if (new_menu == M_Last) { // go to last menu
    current_menu = last_menu;
  }
  else {
    current_menu = new_menu; // change to new menu
  }
  if ( (m != M_Arrow) && (m != M_Alpha) ) last_menu = m;
  // store menu as last menu unless arrow or alpha
}

void set_default_menu () {
  set_menu (default_menu);
}

void toggle_default_menu () {
  if (current_menu == M_Blank) {
    current_menu = default_menu = M_Blank2;
    return;
  }
  else if (current_menu == M_Blank2) {
    current_menu = default_menu = M_Blank; 
  }
  else {
    set_default_menu();
  }
}

menu_name get_menu () {
  return current_menu;
}

menu_name get_last_menu () {
  return last_menu;
}

void display_current_menu () {
  display_menu (get_current_menu_ref());
}

void set_last_menu () {
  set_menu (M_Last);
}

static unsigned char mdots[200][2];

static const struct _menu Menus[] = {
  {
    "M_Blank", // 0
    { 
      { NO_KEY, NO_KEY, "", "" },
      { NO_KEY, NO_KEY, "", "" },
      { NO_KEY, NO_KEY, "", "" },
      { NO_KEY, NO_KEY, "", "" },
      { ARROW_KEY, NO_KEY, "--\015", "" },
      { CMPLX_KEY, { K_MULTI, DOTS }, "CPX", "" },
    }    
  },
  {
    "M_Clear", // 1
    { 
      { { K24, 1 }, { K_OP, OP_NIL | OP_CLPALL }, "CLProg", "CLPAll" },
      { { K24, 2 }, { K_OP, OP_NIL | OP_CLREG }, "CL\221", "CLReg" },
      { { K24, 3 }, { K_OP, OP_NIL | OP_CLRALPHA }, "CLx", "CL\006\240" },
      { { K_OP, OP_NIL | OP_CLSTK }, { K_OP, OP_NIL | OP_CLALL }, "CLStk", "CLAll" },
      { ARROW_KEY, NO_KEY, "--\015", "" },
      { CMPLX_KEY, NO_KEY, "CPX", "" },
    }    
  },
  {
    "M_Disp", // 2
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
    "M_Base", // 3
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
    "M_Angles", // 4
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
    "M_Misc", // 5
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
    "M_Swap", // 6
    { 
      { { K21, 1 }, { K_OP, RARG_BASEOP(RARG_SHUFFLE) }, "Show\016", "[\027]" },
      { { K21, 2 }, NO_KEY, "Show\015", "" },
      { { K21, 3 }, { K_OP, RARG_BASEOP(RARG_SWAPZ) }, "x\027?", "z\027?" },
      { { K_OP, RARG_BASEOP(RARG_SWAPY) }, { K_OP, RARG_BASEOP(RARG_SWAPT) }, "y\027?", "t\027?" },
      { ARROW_KEY, NO_KEY, "--\015", "" },
      { CMPLX_KEY, NO_KEY, "CPX", "" },
    }    
  },
  {
    "M_Program", // 7
    { 
      { { K63, 1 }, { K64, 1 }, "LBL", "DSE" },
      { { K63, 2 }, { K64, 2 }, "RTN", "ISG" },
      { { K61, 3 }, { K_OP, RARG_BASEOP(RARG_DEC) }, "Pause", "DEC" },
      { { K30, 3 }, { K_OP, RARG_BASEOP(RARG_INC) }, "GTO", "INC" },
      { ARROW_KEY, { K_OP, RARG_BASEOP(RARG_BACK) }, "--\015", "BACK" },
      { CMPLX_KEY, { K_OP, RARG_BASEOP(RARG_SKIP) }, "CPX", "SKIP" },
    }
  },
  {
    "M_Setup1", // 8
    { 
      { { K_EXIT, 0}, { K_SYS, 0 }, "EXIT", "System" },
      { { K_MULTI, HELP }, NO_KEY, "HELP", "" },
      { { K_MULTI, ONSTO }, { K_MULTI, WRTST}, "OnSTO", "SvRAM" },
      { { K_MULTI, ONRCL }, { K_MULTI, LDST}, "OnRCL", "LdRAM" },
      { ARROW_KEY, NO_KEY, "--\015", "" },
      { CMPLX_KEY, NO_KEY, "CPX", "" },
    }
  },
  {
    "M_Arrow", // 9
    { 
      { { K22, 1 }, { K10, 2 }, "\015Bin", "\015DEG" },
      { { K22, 2 }, { K11, 2 }, "\015Oct", "\015RAD" },
      { { K23, 1 }, { K12, 2 }, "\015Dec", "\015Grad" },
      { { K23, 2 }, { K10, 1 }, "\015Hex", "\015HMS" },
      { ARROW_KEY, { K11, 1 }, "--\015", "HMS\015" },
      { CMPLX_KEY, NO_KEY, "CPX", "" },
    }
  },
  {
    "M_Flags", // 10
    { 
      { { K50, 1 }, { K_OP, RARG_BASEOP(RARG_FS) }, "SF_", "FS?" },
      { { K50, 2 }, { K_OP, RARG_BASEOP(RARG_FC)}, "CF_", "FC?" },
      { { K51, 1 }, { K_OP, RARG_BASEOP(RARG_TEST_LT)}, "x=?", "x<?" },
      { { K51, 2 }, { K_OP, RARG_BASEOP(RARG_TEST_LE)}, "x\013?", "x<=?" },
      { ARROW_KEY, { K_OP, RARG_BASEOP(RARG_TEST_GE)}, "--\015", "x>=?" },
      { CMPLX_KEY, { K_OP, RARG_BASEOP(RARG_TEST_GT)}, "CPX", "x>?" },
    }
  },
  {
    "M_Prob", // 11
    { 
      { { K40, 3 }, { K_OP, OP_MON | OP_LNGAMMA }, "Fact!", "Ln\006\202" },
      { { K40, 1 }, { K41, 1 }, "Cy,x", "\224" }, // phi
      { { K40, 2 }, { K41, 2 }, "Py,x", "\224\235" }, // phi^-1
      { { K12, 1 }, { K_OP, OP_NIL | OP_STORANDOM } , "RAN#", "SEED" },
      { ARROW_KEY, NO_KEY, "--\015", "" },
      { CMPLX_KEY, NO_KEY, "CPX", "" },
    }
  },
  {
    "M_Stats", // 12
    /*
     *xbar, sd;
     *yhat, xhat;
     *corr, L.R.;
     *best, linear;
     *ln, exp;
     *cmplx, power;
     *No, sums is already on g9.
     */
    { 
      { { K42, 1 }, { K42, 2 }, "\001", "S.D." },
      { { K43, 1 }, { K_OP, OP_MON | OP_xhat }, "y-hat", "x-hat" },
      { { K_OP, OP_NIL | OP_statR}, { K_OP, OP_NIL | OP_statLR}, "CORR", "L.R." },
      { { K_OP, OP_NIL | OP_BEST}, { K_OP, OP_NIL | OP_LOGF}, "BestF", "LogFit" },
      { { K_OP, OP_NIL | OP_LINF}, { K_OP, OP_NIL | OP_EXPF}, "Linear", "ExpFit" },
      { CMPLX_KEY, { K_OP, OP_NIL | OP_PWRF }, "CPX", "PwrFit" },
    }
  },
  {
    "M_Setup2", // 13
    { 
      { { K_MULTI, WRLIB}, NO_KEY, "SvLIB", "" },
      { { K_MULTI, LLIB }, NO_KEY, "LdLIB", "" },
      { { K_MULTI, SVPRG }, { K_OP, OP_NIL | OP_PSTO }, "SvCPRG", "PSTO" },
      { { K_MULTI, LDPRG }, { K_OP, OP_NIL | OP_PRCL }, "LdPRG", "PRCL" },
      { ARROW_KEY, NO_KEY, "--\015", "" },
      { CMPLX_KEY, NO_KEY, "CPX", "" },
    }
  },
  {
    "M_Solve", // 14
    { 
      { { K52, 1 }, { K_OP, RARG_BASEOP(RARG_DERIV)}, "SLV", "f'(x)" },
      { { K52, 2 }, { K_OP, RARG_BASEOP(RARG_2DERIV)}, "\004 dx", "f''(x)" },
      { { K53, 2 }, NO_KEY, "SUM", "" },
      { { K53, 1 }, NO_KEY, "PROD", "" },
      { ARROW_KEY, NO_KEY, "--\015", "" },
      { CMPLX_KEY, NO_KEY, "CPX", "" },
    }
  },
  {
    "M_Blank2", // 15
    { 
      { { K64, 3 }, NO_KEY, "\221+", "" },
      { { K34, 1 }, NO_KEY, "1/X", "" },
      { { K44, 1 }, NO_KEY, "\003X", "" },
      { { K31, 2 }, NO_KEY, "LOG", "" },
      { ARROW_KEY, NO_KEY, "--\015", "" },
      { CMPLX_KEY, NO_KEY, "CPX", "" },
    }    
  },
  {
    "M_Alpha", // 16
    { 
      { { K_OP, OP_NIL | OP_XTOALPHA }, { K_OP, OP_NIL | OP_ALPHATIME }, "X->\240", "\240TIME" },
      { { K_OP, OP_NIL | OP_ALPHATOX }, { K_OP, OP_NIL | OP_ALPHADATE }, "\240->X", "\240DATE" },
      { { K_OP, RARG_BASEOP(RARG_AIP) }, { K_OP, OP_NIL | OP_ALPHADAY }, "\240IP", "\240DAY" },
      { { K_OP, OP_NIL | OP_ALPHALEN }, { K_OP, OP_NIL | OP_ALPHAMONTH }, "\240LENG", "\240MNTH" },
      { ARROW_KEY, { K_ARROW, 1 }, "--\015", "" },
      { CMPLX_KEY, { K_CMPLX, 1 }, "CPX", "" },
    }    
  },
  {
    "M_C_Lock", // 17
    { 
      { { K40, 2 }, { K04, 2 }, "POLAR", "\015 P" },
      { { K40, 1 }, { K04, 1 }, "RECT", "\015 R" },
      { { K22, 1 }, { K22, 3 }, "+/-\006Im", "+/-\006Re" },
      { { K21, 2 }, { K00, 1 }, "Re\027Im", "HYP" },
      { ARROW_KEY, { K23, 1 }, "--\015", "\021\006\257" },
      { CMPLX_KEY, { K20, 1 }, "CPX", "\021\006CNST" },
    }    
  },
  {
    "M_Clear_C", // 18
    { 
      { NO_KEY, NO_KEY, "", "" },
      { NO_KEY, NO_KEY, "", "" },
      { { K24, 3 }, { K_OP, OP_NIL | OP_CLRALPHA }, "CLx", "CL\006\240" },
      { { K_OP, OP_NIL | OP_CLSTK }, NO_KEY, "CLStk", "" },
      { ARROW_KEY, NO_KEY, "--\015", "" },
      { CMPLX_KEY, NO_KEY, "CPX", "" },
    }    
  },
  {
    "M_Swap_C", // 19
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
    "M_Arrow_C", // 20
    { 
      { NO_KEY, { K10, 2 }, "", "DEG" },
      { NO_KEY, { K11, 2 }, "", "RAD" },
      { NO_KEY, { K12, 2 }, "", "Grad" },
      { NO_KEY, { K10, 1 }, "", "\015HMS" },
      { ARROW_KEY, { K11, 1 }, "--\015", "HMS\015" },
      { CMPLX_KEY, NO_KEY, "CPX", "" },
    }
  },
  {
    "M_Prob_C", // 21
    { 
      { { K40, 3 }, { K_OP, OP_CMON | OP_LNGAMMA }, "\024\006!", "\024\006Ln\006\202" },
      { { K_OP, OP_CDYA | OP_COMB }, NO_KEY, "\024\006Cy,x", "" },
      { { K_OP, OP_CDYA | OP_PERM }, NO_KEY, "\024\006Py,x", "" },
      { NO_KEY, NO_KEY , "", "" },
      { ARROW_KEY, NO_KEY, "--\015", "" },
      { CMPLX_KEY, NO_KEY, "CPX", "" },
    }
  },
  {
    "M_Blank2_C", // 22
    { 
      { { K00, 1 }, NO_KEY, "HYP", "" },
      { { K34, 1 }, NO_KEY, "1/X", "" },
      { { K44, 1 }, NO_KEY, "\003X", "" },
      { { K31, 2 }, NO_KEY, "LOG", "" },
      { ARROW_KEY, NO_KEY, "--\015", "" },
      { CMPLX_KEY, NO_KEY, "CPX", "" },
    }    
  },
  {
    "M_Misc_C", // 23
    { 
      { { K61, 1 }, { K32, 1 }, "|z|", "\0242\234" },
      { { K61, 2 }, { K32, 2 }, "RND", "\024Log\272" },
      { { K62, 1 }, { K33, 2 }, "0\015Re", "\024Log\213" },
      { { K62, 2 }, { K34, 2 }, "0\015Im", "\024||el" },
      { ARROW_KEY, { K44, 3 }, "--\015", "\024.\034" },
      { CMPLX_KEY, { K34, 3 }, "CPX", "\024./" },
    }
  },
  {
    "M_Usermenu", // 24
    { 
      { { K_OP, 0x0000fb00  }, NO_KEY, "UMEN", "" },
      { { K_OP, 0x314dfb55 }, { K_OP, 0x344dfb55 }, "UM1", "UM4" },
      { { K_OP, 0x324dfb55 }, { K_OP, 0x354dfb55 }, "UM2", "UM5" },
      { { K_OP, 0x334dfb55 }, { K_OP, 0x364dfb55 }, "UM3", "UM6" },
      { { K_OP, 0x4d41fb52 }, NO_KEY, "RAM", "" },
      { { K_OP, 0x4249fb4c }, NO_KEY, "LIB", "" },
    }
  },
  {
    "M_Print", // 25
    { 
      { { K_OP, OP_NIL | OP_PRINT_ON  }, { K_OP, OP_NIL | OP_PRINT_OFF }, "\222\006On", "\222\006Off" },
      { { K_OP, RARG_BASEOP(RARG_PMODE) }, { K_OP, RARG_BASEOP(RARG_DBLSP) }, "\222\006Mode", "\222\006DbSp" },
      { { K_OP, OP_NIL | OP_PRINT_PGM }, { K_OP, OP_NIL | OP_PRINT_REGS }, "\222\006Prog", "\222\006Regs" },
      { { K_OP, OP_NIL | OP_PRINT_STACK }, { K_OP, OP_NIL | OP_PRINT_SIGMA }, "\222\006Stk", "\222\006\221" },
      { ARROW_KEY, { K_OP, OP_NIL | OP_PRINT_ALPHA }, "--\015", "\222\006\240" },
      { CMPLX_KEY, { K_OP, OP_NIL | OP_PRINT_ADV }, "CPX", "\222\006ADV" },
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

struct _menu get_current_menu_ref() {
  int i = current_menu;
  if (C_LOCKED)
    switch (current_menu) {
    case (M_Clear):
      i = M_Clear_C;
      break;
    case (M_Swap):
      i = M_Swap_C;
      break;
    case (M_Arrow):
      i = M_Arrow_C;
      break;
    case (M_Prob):
      i = M_Prob_C;
      break;
    case (M_Blank2):
      i = M_Blank2_C;
      break;
    case (M_Misc):
      i = M_Misc_C;
      break;
    case (M_Base): // These menus aren't appropriate in complex mode
    case (M_Flags):// so don't display them.
    case (M_Stats):
    case (M_Setup2):
    case (M_Solve):
    case (M_Program):
      i = M_C_Lock;
    default:;
    }
  if (current_menu == M_User) {
    return UserMenu;
  }
  else if (current_menu == M_MEM) {
    return MEMMenu;
  }
  else {
    return Menus[i];
  }
}

void display_menu (struct _menu Ref) {
  int len = 0;
  lcd_fill_rect (0, 188, 400, 52, 0); // clear bottom 52 rows for menu 
  lcd_fill_rect (0, 188, 400, 1, 0xff); // lines count from line 1? No. 
  //Clear previous menu
  for (int col = 0; col < 200; col++) {
    mdots[col][0] = 0;
    mdots[col][1] = 0;
  }

  // Draw menu items
  // 
  for (int item = 0; item < 6; item++ ) {
    /*
      Starting dot in the row of 200 is item*32 - no gaps; gaps will be inserted when drawing
    */
    // Label is Ref.keys[item].unshifted_label
    // Unshifted first
      len = pixel_length (Ref.keys[item].unshifted_label, 0); // 0 means not small font
      set_menu_label ( Ref.keys[item].unshifted_label, 0, item*32 + (32 - len)/2, 0 );
      // Now shifted ...	
      len = pixel_length (Ref.keys[item].shifted_label, 0); // 0 means not small font
      len = item*32 + (32-len)/2;
      if (len<0) len = 0;
      if (len>199) len = 199;
      set_menu_label ( Ref.keys[item].shifted_label, 0, len, 1 );
  }

  for (int col = 0; col < 196; col++) {
    for (int row = 0; row < 6; row++) {
      if ((mdots[col][0] & (1 << row)) != 0) 
	//	lcd_fill_rect( col*2-1, 230 - row*3, 3, 3, 0xff);
	lcd_fill_rect( 4+col*2, 233 - row*3, 3, 3, 0xff);
      if ((mdots[col][1] & (1 << row)) != 0) 
	//	lcd_fill_rect( col*2-1, 230 - 5*3 -10 - row*3, 3, 3, 0xff);
	lcd_fill_rect( 4+col*2, 233 - 5*3 -10 - row*3, 3, 3, 0xff);
    }
  }
}

void all_menu_dots () {
  for (int col = 0; col < 200; col++) {
    for (int row = 0; row < 6; row++) {
	lcd_fill_rect( col*2, 233 - row*3, 3, 3, 0xff);
	lcd_fill_rect( col*2, 233 - 5*3 -10 - row*3, 3, 3, 0xff);
    }
  }
}  
void umen_store (int i, opcode opc, char* buf1);

/*
void build_user_menu(void){
  // find the label 'MNU'
   const int lbl = OP_DBL + (DBL_LBL << DBL_SHIFT) + 'M' + ('N' << 16) + ('U' << 24);
   build_user_menu_from_program (lbl);
}
*/

void build_user_menu_from_program(int lbl)
{
  // const int lbl = OP_DBL + (DBL_LBL << DBL_SHIFT) + 'M' + ('N' << 16) + ('U' << 24);
  unsigned int pc = findmultilbl(lbl, 0);
  int i=0;
  s_opcode op;
  opcode opc;
  char buf1[16];
  char *p = buf1;
  char c;
  int finished = 0;

  for (int j = 0; j<6; j++) { // clear user menu
      UserMenu.keys[j].unshifted_label[0] = '\0';
      UserMenu.keys[j].unshifted.shift = 0;
      UserMenu.keys[j].unshifted.key_34s = K_NOP;
      UserMenu.keys[j].shifted_label[0] = '\0';
      UserMenu.keys[j].shifted.shift = 0;
      UserMenu.keys[j].shifted.key_34s = K_NOP;
  }
  while (pc && i < 12) {

    pc = do_inc(pc, 0);
    opc = getprog(pc);
    op = (s_opcode) opc;

    if (op == (OP_NIL | OP_END))
      break;
    if (op == (OP_NIL | OP_NOP)) {
      if (i==0) {
	continue;
      }
      else if (i==4) {
	UserMenu.keys[i].unshifted = (struct _ndmap) {K_ARROW, -1};
	strncpy(UserMenu.keys[i].unshifted_label, arrow_key_string,5);
      }
      else if (i==5) {
	UserMenu.keys[i].unshifted = (struct _ndmap) {K_CMPLX, -1};
	strncpy(UserMenu.keys[i].unshifted_label, cmplx_key_string,5);
      }
      i += 1;
      continue;
    }

    if (isDBL(opc) && (opDBL(opc) == DBL_ALPHA)) { // is it double alpha?
      if ( finished==1 || p - buf1 == 6 ) { // if we've already finished an alpha...
	umen_store (i, OP_NIL | OP_NOP, buf1); // no command, so store a NOP
	i++;
    	p = buf1;
	finished = 0;
      } // ..and start reading the new alpha
      *p++ = opc & 0xff; // character 1
      c = (opc >> 16) & 0xff;
      if (c != '\0') { // if character 2 isn't null..
	*p++ = c;
	c = opc>>24;
	if (c != '\0') // if character 3 isn't null..
	  *p++ = c;
      }
      *p = '\0'; // no ++: next character can overwrite the 0.
      if (c == '\0' || p - buf1 == 6) {
	finished = 1;
      }
    }
    else { // Not a DBL_ALPHA op - the usual case
      if ( p == buf1 ) { // no alpha label to use
	if (isRARG(op) && p == buf1) {
	  const s_opcode rarg = RARG_CMD(op);
	  if ( rarg != RARG_ALPHA && rarg != RARG_CONV
	       && rarg != RARG_CONST && rarg != RARG_CONST_CMPLX
	       && ( (op & 0xff) == 0 ) ) { // argument = 0 
	    catcmd (op, buf1); // display rarg without argument
	  }
	  else {
	    prt_umen(opc, buf1); // display rarg with argument
	  }
	}
	else {
	  prt_umen(opc, buf1); // display non-rarg thing
	}
      }
      else { // there is an alpha label to use, so reset
	p = buf1;
	finished = 0;
      }
      umen_store (i, opc, buf1);
      i++;
    }
  }
}

void umen_store (int i, opcode opc, char* buf1) {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstringop-truncation"
    if (i<6) {
      strncpy(UserMenu.keys[i].unshifted_label, buf1, 7);
      UserMenu.keys[i].unshifted_label[7]='\0';
      UserMenu.keys[i].unshifted = (struct _ndmap) {K_OP, opc};
    }
    else {
      strncpy(UserMenu.keys[i-6].shifted_label, buf1, 7);
      UserMenu.keys[i-6].shifted_label[7]='\0';
      UserMenu.keys[i-6].shifted = (struct _ndmap) {K_OP, opc};
    }
#pragma GCC diagnostic pop
}  

static unsigned int last_pc = 0;

void build_menu_of_labels(int region) {
  // region = 0 for RAM, 1 for LIB,
  // 2 for carry on from last point,
  // 3 for go back to the start of the current region
  unsigned int pc;

  if (region == 0) {
    pc = 0; // just before start of RAM
    last_pc = pc;
  }
  else if (region == 1) {
    pc = addrLIB(0, REGION_LIBRARY);
    last_pc = pc;
  }
  else if (region == 2) {
    pc = last_pc;
  }
  else {
    pc = addrLIB(0, nLIB(last_pc));
    last_pc = pc;
  }
    
  int label_count = 0;
  opcode opc;
  char *p;
  char c;
  char name[] = "   "; // 3 spaces

  // Clear RAM menu
  for (int j = 0; j<6; j++) { // clear RAM menu
      MEMMenu.keys[j].unshifted_label[0] = '\0';
      MEMMenu.keys[j].unshifted.shift = 0;
      MEMMenu.keys[j].unshifted.key_34s = K_NOP;
      MEMMenu.keys[j].shifted_label[0] = '\0';
      MEMMenu.keys[j].shifted.shift = 0;
      MEMMenu.keys[j].shifted.key_34s = K_NOP;
  }

  while ( (label_count < 12) && (pc = find_next_label(pc)) ) {
    if ( getprog(do_inc(pc,0)) == (OP_NIL | OP_NOP) ) continue; // skip if next statement NOP
    // Read that label
    opc = getprog(pc);

    p = name;
    *p++ = opc & 0xff; // character 1
    c = (opc >> 16) & 0xff;
    *p++ = c;
    if (c != '\0') { // if character 2 isn't null..
      c = opc>>24;
      *p++ = c;      
      p = '\0';
    }
    // name contains string for menu entry
    opc = opc - (DBL_LBL << DBL_SHIFT) + (DBL_XEQ << DBL_SHIFT);
    // opc is now XEQ command
    
    // Make a menu entry, both XEQ command and label
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstringop-truncation"
    if (label_count<6) {
      strncpy(MEMMenu.keys[label_count].unshifted_label, name, 4);
      MEMMenu.keys[label_count].unshifted = (struct _ndmap) {K_OP, opc};
    }
    else {
      strncpy(MEMMenu.keys[label_count-6].shifted_label, name, 4);
      MEMMenu.keys[label_count-6].shifted = (struct _ndmap) {K_OP, opc};
    }
#pragma GCC diagnostic pop

    // advance count
    label_count++;
  }
  if ( pc == 0 ) {
    last_pc = addrLIB(0, nLIB(last_pc));
  }
  else {
    last_pc = pc;
  }
}

// addrLIB(0, REGION_LIBRARY)

unsigned int find_next_label ( unsigned int pc ) {
  do {
    pc = do_inc(pc, 0);
    if (PcWrapped) return 0;
  }
  while (! is_label_at (pc) );
  return pc;
}

int is_label_at ( unsigned int pc ) {
  const unsigned int op = getprog(pc);
  return (isDBL(op) && (opDBL(op) == DBL_LBL));
}
