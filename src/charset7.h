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
 * 
 * 
 * This file is automatically generated.
 * Changes will not be preserved.
 */

static const unsigned char digtbl[122] = {
	0                                                           /* 0000 */,
	D_TOP                                                       /* 0001 */,
	D_MIDDLE                                                    /* 0002 */,
	D_TOP | D_MIDDLE                                            /* 0003 */,
	D_BOTTOM                                                    /* 0004 */,
	D_TOP | D_BOTTOM                                            /* 0005 */,
	D_MIDDLE | D_BOTTOM                                         /* 0006 */,
	D_TOP | D_MIDDLE | D_BOTTOM                                 /* 0007 */,
	D_TL | D_TR | D_BL | D_BR                                   /* 0010 */,
	0                                                           /* 0011 */,
	0                                                           /* 0012 */,
	0                                                           /* 0013 */,
	0                                                           /* 0014 */,
	0                                                           /* 0015 */,
	0                                                           /* 0016 */,
	0                                                           /* 0017 */,
	0                                                           /* 0020 */,
	0                                                           /* 0021 */,
	0                                                           /* 0022 */,
	0                                                           /* 0023 */,
	0                                                           /* 0024 */,
	0                                                           /* 0025 */,
	0                                                           /* 0026 */,
	0                                                           /* 0027 */,
	D_MIDDLE | D_BL | D_BR | D_BOTTOM                           /* 0030 */,
	D_BR                                                        /* 0031 */,
	0                                                           /* 0032 */,
	0                                                           /* 0033 */,
	0                                                           /* 0034 */,
	0                                                           /* 0035 */,
	0                                                           /* 0036 */,
	0                                                           /* 0037 */,
	0                                                           /*      */,
	0                                                           /*  !   */,
	D_TL | D_TR                                                 /*  "   */,
	0                                                           /*  #   */,
	0                                                           /*  $   */,
	0                                                           /*  %   */,
	0                                                           /*  &   */,
	D_TL                                                        /*  '   */,
	D_TOP | D_TL | D_BL | D_BOTTOM                              /*  (   */,
	D_TOP | D_TR | D_BR | D_BOTTOM                              /*  )   */,
	0                                                           /*  *   */,
	0                                                           /*  +   */,
	0                                                           /*  ,   */,
	D_MIDDLE                                                    /*  -   */,
	0                                                           /*  .   */,
	D_TR | D_MIDDLE | D_BL                                      /*  /   */,
	D_TOP | D_TL | D_TR | D_BL | D_BR | D_BOTTOM                /*  0   */,
	D_TR | D_BR                                                 /*  1   */,
	D_TOP | D_TR | D_MIDDLE | D_BL | D_BOTTOM                   /*  2   */,
	D_TOP | D_TR | D_MIDDLE | D_BR | D_BOTTOM                   /*  3   */,
	D_TL | D_TR | D_MIDDLE | D_BR                               /*  4   */,
	D_TOP | D_TL | D_MIDDLE | D_BR | D_BOTTOM                   /*  5   */,
	D_TOP | D_TL | D_MIDDLE | D_BL | D_BR | D_BOTTOM            /*  6   */,
	D_TOP | D_TR | D_BR                                         /*  7   */,
	D_TOP | D_TL | D_TR | D_MIDDLE | D_BL | D_BR | D_BOTTOM     /*  8   */,
	D_TOP | D_TL | D_TR | D_MIDDLE | D_BR | D_BOTTOM            /*  9   */,
	0                                                           /*  :   */,
	0                                                           /*  ;   */,
	D_BL | D_BOTTOM                                             /*  <   */,
	D_MIDDLE | D_BOTTOM                                         /*  =   */,
	D_BR | D_BOTTOM                                             /*  >   */,
	0                                                           /*  ?   */,
	D_TOP | D_TL | D_TR | D_MIDDLE                              /*  @   */,
	D_TOP | D_TL | D_TR | D_MIDDLE | D_BL | D_BR                /*  A   */,
	D_TL | D_MIDDLE | D_BL | D_BR | D_BOTTOM                    /*  B   */,
	D_TOP | D_TL | D_BL | D_BOTTOM                              /*  C   */,
	D_TR | D_MIDDLE | D_BL | D_BR | D_BOTTOM                    /*  D   */,
	D_TOP | D_TL | D_MIDDLE | D_BL | D_BOTTOM                   /*  E   */,
	D_TOP | D_TL | D_MIDDLE | D_BL                              /*  F   */,
	D_TOP | D_TL | D_BL | D_BR | D_BOTTOM                       /*  G   */,
	D_TL | D_TR | D_MIDDLE | D_BL | D_BR                        /*  H   */,
	D_TL | D_BL                                                 /*  I   */,
	D_TR | D_BL | D_BR | D_BOTTOM                               /*  J   */,
	D_TOP | D_TL | D_MIDDLE | D_BL | D_BR                       /*  K   */,
	D_TL | D_BL | D_BOTTOM                                      /*  L   */,
	D_TOP | D_TL | D_TR | D_BL                                  /*  M   */,
	D_TOP | D_TL | D_TR | D_BL | D_BR                           /*  N   */,
	D_TOP | D_TL | D_TR | D_BL | D_BR | D_BOTTOM                /*  O   */,
	D_TOP | D_TL | D_TR | D_MIDDLE | D_BL                       /*  P   */,
	D_TOP | D_TL | D_TR | D_MIDDLE | D_BR                       /*  Q   */,
	D_MIDDLE | D_BL                                             /*  R   */,
	D_TOP | D_TL | D_MIDDLE | D_BR | D_BOTTOM                   /*  S   */,
	D_TL | D_MIDDLE | D_BL | D_BOTTOM                           /*  T   */,
	D_TL | D_TR | D_BL | D_BR | D_BOTTOM                        /*  U   */,
	0                                                           /*  V   */,
	D_TL | D_BL | D_BR | D_BOTTOM                               /*  W   */,
	0                                                           /*  X   */,
	D_TL | D_TR | D_MIDDLE | D_BR | D_BOTTOM                    /*  Y   */,
	0                                                           /*  Z   */,
	D_TOP | D_TL | D_BL | D_BOTTOM                              /*  [   */,
	0                                                           /*  \   */,
	D_TOP | D_TR | D_BR | D_BOTTOM                              /*  ]   */,
	0                                                           /*  ^   */,
	D_BOTTOM                                                    /*  _   */,
	0                                                           /*  `   */,
	D_TOP | D_TL | D_TR | D_MIDDLE | D_BL | D_BR                /*  a   */,
	D_TL | D_MIDDLE | D_BL | D_BR | D_BOTTOM                    /*  b   */,
	D_MIDDLE | D_BL | D_BOTTOM                                  /*  c   */,
	D_TR | D_MIDDLE | D_BL | D_BR | D_BOTTOM                    /*  d   */,
	D_TOP | D_TL | D_MIDDLE | D_BL | D_BOTTOM                   /*  e   */,
	D_TOP | D_TL | D_MIDDLE | D_BL                              /*  f   */,
	D_TOP | D_TL | D_BL | D_BR | D_BOTTOM                       /*  g   */,
	D_TL | D_MIDDLE | D_BL | D_BR                               /*  h   */,
	D_BL                                                        /*  i   */,
	D_TR | D_BL | D_BR | D_BOTTOM                               /*  j   */,
	D_TOP | D_TL | D_MIDDLE | D_BL | D_BR                       /*  k   */,
	D_TL | D_BL | D_BOTTOM                                      /*  l   */,
	D_TOP | D_TL | D_TR | D_BR                                  /*  m   */,
	D_MIDDLE | D_BL | D_BR                                      /*  n   */,
	D_MIDDLE | D_BL | D_BR | D_BOTTOM                           /*  o   */,
	D_TOP | D_TL | D_TR | D_MIDDLE | D_BL                       /*  p   */,
	D_TOP | D_TL | D_TR | D_MIDDLE | D_BR                       /*  q   */,
	D_MIDDLE | D_BL                                             /*  r   */,
	D_TOP | D_TL | D_MIDDLE | D_BR | D_BOTTOM                   /*  s   */,
	D_TL | D_MIDDLE | D_BL | D_BOTTOM                           /*  t   */,
	D_BL | D_BR | D_BOTTOM                                      /*  u   */,
	0                                                           /*  v   */,
	D_TR | D_BL | D_BR | D_BOTTOM                               /*  w   */,
	0                                                           /*  x   */,
	D_TL | D_TR | D_MIDDLE | D_BR | D_BOTTOM                    /*  y   */,
};
#define N_DIGTBL	(122)

