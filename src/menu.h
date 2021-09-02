#define NO_KEY {K_NOP, -1}
#define ARROW_KEY {K_ARROW, -1}
#define CMPLX_KEY {K_CMPLX, -1}

//K_MULTI defines

//#define LASTX 1
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
#define SETUMEN 14
#define KCPX 15

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

extern struct _menu UserMenu;
extern struct _menu MEMMenu;

extern const char* arrow_key_string;
extern const char* cmplx_key_string;

#define MENU_WIDTH 32

extern menu_name current_menu;
extern menu_name last_menu;
extern menu_name default_menu;

extern void all_menu_dots ( void );
extern void build_user_menu(void);
extern void build_user_menu_from_program(int);
extern void build_menu_of_labels(int);
extern unsigned int find_next_label (unsigned int);
extern int is_label_at ( unsigned int );
extern void display_menu (struct _menu Ref);
extern void display_current_menu ( void );
extern menu_name get_menu ();
extern struct _menu get_current_menu_ref(void);
extern menu_name get_last_menu ();
extern void mdot(int i, int j, int s, int on);
extern void set_menu (menu_name new_menu);
extern void set_default_menu(void);
extern void set_last_menu ( void );
