static const struct _ndmap mapping_none[] = {
    {K_RELEASE, -1},
    // Top row of DM42 1--6
    {K00, 0}, // A/sig+
    {K01, 0}, // B/1/x
    {K03, 0}, // D/sqrt x
    {K31, 2}, // log10
    {K30, 2}, // ln
    {K30, 0}, // xeq
    // Next row of DM42 7--12
    {K10, 0}, // STO
    {K11, 0}, // RCL
    {K12, 0}, // Rdown
    {K01, 1}, // sin
    {K02, 1}, // cos
    {K03, 1}, // tan
    // Third row of DM42 13--17
    {K20, 0}, // ENTER
    {K21, 0}, // x<>y
    {K22, 0}, // +/-
    {K23, 0}, // EEX
    {K24, 0}, // <-
    // Fourth row of DM42 18--22
    {K40, 0}, // up-arrow
    {K31, 0}, // 7
    {K32, 0}, // 8
    {K33, 0}, // 9
    {K34, 0}, // /
    // Fifth row of DM42 23--27
    {K50, 0}, // down-arrow
    {K41, 0}, // 4
    {K42, 0}, // 5
    {K43, 0}, // 6
    {K44, 0}, // *
    // Sixth row of DM42 28--32
    {K_F, 0}, // shift
    {K51, 0}, // 1
    {K52, 0}, // 2
    {K53, 0}, // 3
    {K54, 0}, // -
    // Last row of DM42 33--37
    {K60, 0}, // EXIT
    {K61, 0}, // 0
    {K62, 0}, // .
    {K63, 0}, // R/S
    {K64, 0}, // +
    // Function keys, screenshot, sh_up, sh_down
    {K_NOP, 0}, // F1
    {K_NOP, 0},
    {K_NOP, 0},
    {K_NOP, 0},
    {K_NOP, 0},
    {K_NOP, 0}, // F6
    {K_NOP, 0}, // 44
    {K_NOP, 0}, // 45
    {K_NOP, 0}, // 46
  };

  static const struct _ndmap mapping_fshift[] = {
    {K_RELEASE, -1},
    // Top row of DM42 1--6
    {K54, 3}, // sig- 
    {K33, 1}, // y^x
    {K44, 2}, // x^2
    {K31, 1}, // 10^x
    {K30, 1}, // e^x
    {K30, 3}, // GTO
    // Next row of DM42 7--12
    {K_CMPLX, 0},
    {K54, 1}, // %
    {K23, 3}, // pi
    {K01, 2}, // asin
    {K02, 2}, // acos
    {K03, 2}, // atan
    // Third row of DM42 13--17
    {K20, 1}, // alpha
    {K_MULTI, 1}, // lastX not available; it's RCL L; so just RCL for now;
    {K_SETMENU, 4}, // ANGLES menu
    {K_SETMENU, 2}, // DISP menu
    {K_SETMENU, 1}, // CLR menu
    // Fourth row of DM42 18--22
    {K40, 0}, // up-arrow
    {K52, 1}, // SLV
    {K_SETMENU, 14}, // Integral/sum/product menu
    {K44, 3}, // MATRIX cat
    {K_SETMENU, 12}, // STATS menu
    // Fifth row of DM42 23--27
    {K50, 0}, // down-arrow
    {K_SETMENU, 3}, // BASE menu 
    {K04, 3}, // CONV cat
    {K_SETMENU, 10}, // FLAGS menu
    {K_SETMENU, 11}, // PROBS menu
    // Sixth row of DM42 28--32
    {K_G, 0}, // gshift
    {K_MULTI, DEFMEN}, // toggle default menu
    {K_NOP, 0}, // no function
    {K52, 3}, // P.FCN cat
    {K_NOP, 0}, // no function
    // Last row of DM42 33--37
    {K60, 3}, // off
    {K_SETMENU, 8}, // SETUP menu
    {K21, 1}, // <(
    {K63, 3}, // P/R
    {K10, 3}, // CAT cat
    // Function keys, screenshot, sh_up, sh_down
    {K_NOP, 0},
    {K_NOP, 0},
    {K_NOP, 0},
    {K_NOP, 0},
    {K_NOP, 0},
    {K_NOP, 0},
    {K_NOP, 0},
    {K_SETMENU, -1}, // last menu
    {K_SETMENU, 5}, // MISC menu!
  };

  static const struct _ndmap mapping_gshift[] = {
    {K_RELEASE, -1},
    // Top row of DM42 1--6
    {K64, 3}, // sig+
    {K34, 1}, // 1/x
    {K44, 1}, // sqrt x
    {K31, 2}, // 10^x
    {K_NOP, 0}, // no function
    {K_NOP, 0}, // no function
    // Next row of DM42 7--12
    {K_NOP, 0}, // no function
    {K11, 3}, // View
    {K12, 3}, // Rup
    {K_SETMENU, 4}, // ANGLES menu
    {K_SETMENU, 4}, // ANGLES menu
    {K_SETMENU, 4}, // ANGLES menu
    // Third row of DM42 13--17
    {K20, 2}, // fill
    {K_SETMENU, 6}, // X<>Y menu
    {K05, 3}, // MODE cat
    {K_NOP, 0}, // no function
    {K_NOP, 0}, // no function
    // Fourth row of DM42 18--22
    {K_NOP, 0}, // no function
    {K_NOP, 0}, // no function
    {K_NOP, 0}, // no function
    {K43, 3}, // SUMS catalogue
    {K42, 3}, // STAT catalogue
    // Fifth row of DM42 23--27
    {K_NOP, 0}, // no function 
    {K_NOP, 0}, // no function
    {K20, 3}, // CONST catalogue
    {K51, 3}, // TEST catalogue
    {K41, 3}, // PROB catalogue
    // Sixth row of DM42 28--32
    {K_G, -1}, // gshift (clears shift state)
    {K_NOP, 0}, // no function
    {K_NOP, 0}, // no function
    {K53, 3}, // X.FCN catalogue.
    {K_NOP, 0}, // no function
    // Last row of DM42 33--37
    {K60, 2}, // SHOW (register browser)
    {K_SETMENU, 13}, // SETUP2 menu
    {K_NOP, 0}, // no function
    {K_SETMENU, 7}, // P.FCN menu
    {K50, 3}, // Status
    // Function keys, screenshot, sh_up, sh_down
    {K_NOP, 0},
    {K_NOP, 0},
    {K_NOP, 0},
    {K_NOP, 0},
    {K_NOP, 0},
    {K_NOP, 0},
    {K_NOP, 0},
    {K_NOP, 0},
    {K_NOP, 0},
  };

static const struct _ndmap mapping_alpha[] = {
    {K_RELEASE, -1},
    // Top row of DM42 1--6
    {K00, -1},
    {K01, -1},
    {K02, -1},
    {K03, -1},
    {K04, -1},
    {K05, -1},
    // Next row of DM42 7--12
    {K10, -1},
    {K11, -1},
    {K12, -1},
    {K13, -1},
    {K14, -1},
    {K15, -1},
    // Third row of DM42 13--17
    {K20, -1},
    {K21, -1},
    {K22, -1},
    {K23, -1},
    {K24, -1},
    // Fourth row of DM42 18--22
    {K30, -1},
    {K31, -1},
    {K32, -1},
    {K33, -1},
    {K34, -1},
    // Fifth row of DM42 23--27
    {K40, -1},
    {K41, -1},
    {K42, -1},
    {K43, -1},
    {K44, -1},
    // Sixth row of DM42 28--32
    {K50, -1},
    {K51, -1},
    {K52, -1},
    {K53, -1},
    {K54, -1},
    // Last row of DM42 33--37
    {K60, -1},
    {K61, -1},
    {K62, -1},
    {K63, -1},
    {K64, -1},
    // Function keys, screenshot, sh_up, sh_down
    {K_NOP, -1},
    {K_NOP, -1},
    {K_NOP, -1},
    {K_NOP, -1},
    {K_ARROW, -1}, //F5 is always K_ARROW in all menus
    {K_NOP, -1},
    {K_NOP, -1},
    {K_NOP, -1},
    {K_NOP, -1},
};
