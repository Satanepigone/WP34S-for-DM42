#define NO_KEY {K_NOP, -1}
#define ARROW_KEY {K_ARROW, -1}
#define CMPLX_KEY {K_CMPLX, -1}

//K_MULTI defines

#define LASTX 1
#define ONSTO 2
#define ONRCL 3
#define WRLIB 4
#define LLIB 5
#define WRTST 6
#define LDST 7
#define LDPRG 8
#define HELP 9
#define SVPRG 10
#define DOTS 11
#define SSHOT 12
#define DEFMEN 13

struct _key {
  struct _ndmap unshifted;
  struct _ndmap shifted;
  char unshifted_label[8];
  char shifted_label[8];
};
  
struct _menu {
  char name[15];
  struct _key keys[6];
};

#define MENU_WIDTH 32

static int current_menu = 0;
static int last_menu = 0;
static int default_menu = 0;

static void set_menu_label (const char *str, int smallp, int dotcol, int shifted);
void mdot(int i, int j, int s, int on);
extern void all_menu_dots ( void );
