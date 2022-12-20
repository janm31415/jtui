#include "jtui.h"
#include <time.h>

namespace jtui
  {

#define title_height 1
#define main_menu_height 1
#define status_window_height 2
#define body_height (LINES - title_height - main_menu_height - status_window_height)
#define body_width COLS
#define KEY_ESC    0x1b     /* Escape */

#define TITLECOLOR       1   
#define MAINMENUCOLOR    (2 | A_BOLD)
#define MAINMENUREVCOLOR (3 | A_BOLD | A_REVERSE)
#define SUBMENUCOLOR     (4 | A_BOLD)
#define SUBMENUREVCOLOR  (5 | A_BOLD | A_REVERSE)
#define BODYCOLOR        6
#define STATUSCOLOR      (7 | A_BOLD)
#define INPUTBOXCOLOR    8
#define EDITBOXCOLOR     (9 | A_BOLD | A_REVERSE)

#define A_ATTR  (A_ATTRIBUTES ^ A_COLOR) 

  static WINDOW* win_title, * win_main, * win_body, * win_status;
  static bool quit = false;
  int nexty, nextx;
  static int key = ERR;
  static int ch = ERR;

  menu make_menu(const std::string& name, const std::string& description, menu_function fun_ptr)
    {
    menu m;
    m.name = name;
    m.description = description;
    m.fun = fun_ptr;
    return m;
    }


  std::string pad_string(const std::string& str, int length)
    {
    char buff[10];
    char long_buffer[256];
    if (str.length() > length)
      {
      snprintf(buff, sizeof(buff), "%%.%ds", length);
      }
    else
      {
      snprintf(buff, sizeof(buff), "%%-%ds", length);
      }
    snprintf(long_buffer, sizeof(long_buffer), buff, str.c_str());
    std::string buffAsStdStr(long_buffer);
    return buffAsStdStr;
    }

  std::string pre_pad(const std::string& str, int length)
    {
    std::string prepadded(length, ' ');
    return prepadded.append(str);
    }

  void initcolor()
    {
    if (has_colors())
      start_color();

    /* foreground, background */

    init_pair(TITLECOLOR & ~A_ATTR, COLOR_BLACK, COLOR_CYAN);
    init_pair(MAINMENUCOLOR & ~A_ATTR, COLOR_WHITE, COLOR_CYAN);
    init_pair(MAINMENUREVCOLOR & ~A_ATTR, COLOR_WHITE, COLOR_BLACK);
    init_pair(SUBMENUCOLOR & ~A_ATTR, COLOR_WHITE, COLOR_CYAN);
    init_pair(SUBMENUREVCOLOR & ~A_ATTR, COLOR_WHITE, COLOR_BLACK);
    init_pair(BODYCOLOR & ~A_ATTR, COLOR_WHITE, COLOR_BLUE);
    init_pair(STATUSCOLOR & ~A_ATTR, COLOR_WHITE, COLOR_CYAN);
    init_pair(INPUTBOXCOLOR & ~A_ATTR, COLOR_BLACK, COLOR_CYAN);
    init_pair(EDITBOXCOLOR & ~A_ATTR, COLOR_WHITE, COLOR_BLACK);
    }

  void setcolor(WINDOW* win, chtype color)
    {
    chtype attr = color & A_ATTR;  /* extract Bold, Reverse, Blink bits */

    attr &= ~A_REVERSE;  /* ignore reverse, use colors instead! */
    wattrset(win, COLOR_PAIR(color & A_CHARTEXT) | attr);
    }

  void colorbox(WINDOW* win, chtype color, int hasbox)
    {
    int maxy;
    chtype attr = color & A_ATTR;  /* extract Bold, Reverse, Blink bits */

    setcolor(win, color);

    if (has_colors())
      wbkgd(win, COLOR_PAIR(color & A_CHARTEXT) | (attr & ~A_REVERSE));
    else
      wbkgd(win, attr);

    werase(win);


    maxy = getmaxy(win);

    if (hasbox && (maxy > 2))
      box(win, 0, 0);

    touchwin(win);
    wrefresh(win);
    }

  void title_message(const std::string& msg)
    {
    mvwaddstr(win_title, 0, 2, msg.c_str());
    wrefresh(win_title);
    }

  void setmenupos(int y, int x)
    {
    nexty = y;
    nextx = x;
    }

  void getmenupos(int& y, int& x)
    {
    y = nexty;
    x = nextx;
    }

  int menu_width(const menu* menu_items, const std::size_t number_of_menu_items)
    {
    int menu_width = 0;
    const menu* ptr_menu = menu_items;
    for (std::size_t i = 0; i < number_of_menu_items; ++i, ++ptr_menu)
      {
      if (ptr_menu->name.length() > menu_width)
        menu_width = ptr_menu->name.length();
      }
    return menu_width + 2;
    }

  void repaintmainmenu(int width, const menu* menu_items, const std::size_t number_of_menu_items)
    {
    int i;
    const menu* p = menu_items;

    for (i = 0; i < number_of_menu_items; ++i, ++p)
      {
      std::string text = pre_pad(pad_string(p->name, width - 1), 1);
      mvwaddstr(win_main, 0, i * width, text.c_str());
      }

    touchwin(win_main);
    wrefresh(win_main);
    }


  void repaintmenu(WINDOW* wmenu, const menu* menu_items, const std::size_t number_of_menu_items)
    {
    int i;
    const menu* p = menu_items;

    for (i = 0; i < number_of_menu_items; ++i, ++p)
      {
      mvwaddstr(win_main, i + 1, 2, p->name.c_str());
      }

    touchwin(wmenu);
    wrefresh(wmenu);
    }

  void idle()
    {
    char buf[256];
    time_t t;
    struct tm* tp;

    if (time(&t) == -1)
      return;  /* time not available */

    tp = localtime(&t);
    sprintf(buf, " %.4d-%.2d-%.2d  %.2d:%.2d:%.2d",
      tp->tm_year + 1900, tp->tm_mon + 1, tp->tm_mday,
      tp->tm_hour, tp->tm_min, tp->tm_sec);

    mvwaddstr(win_title, 0, body_width - strlen(buf) - 2, buf);
    wrefresh(win_title);
    }

  bool keypressed(void)
    {
    ch = wgetch(win_body);
    return ch != ERR;
    }

  int getkey(void)
    {
    int c = ch;

    ch = ERR;
    return c;
    }

  int waitforkey()
    {
    do idle(); while (!keypressed());
    return getkey();
    }

  void status_message(const std::string& msg)
    {
    std::string text = pad_string(msg, body_width - 3);
    mvwaddstr(win_status, 1, 2, text.c_str());
    wrefresh(win_status);
    }

  void main_help_1()
    {
    status_message("Use ALT to enter the menu.");
    }

  void main_help_2()
    {
    status_message("Use arrow keys to navigate the menu.");
    }

  void rmline(WINDOW* win, int nr)   /* keeps box lines intact */
    {
    std::string text = pad_string(" ", body_width - 2);
    mvwaddstr(win, nr, 1, text.c_str());
    wrefresh(win);
    }

  void rmerror(void)
    {
    rmline(win_status, 0);
    }

  void rmstatus(void)
    {
    rmline(win_status, 1);
    }

  void do_menu(const menu* menu_items, const std::size_t number_of_menu_items)
    {
    bool stop = false;
    int x, y;
    int old = -1;
    int cur = 0;
    int barlen = menu_width(menu_items, number_of_menu_items);
    WINDOW* win_menu;
    curs_set(0);
    getmenupos(y, x);
    int mheight = (int)number_of_menu_items + 2;
    int mwidth = barlen + 2;
    win_menu = newwin(mheight, mwidth, y, x);
    colorbox(win_menu, SUBMENUCOLOR, 1);
    repaintmenu(win_menu, menu_items, number_of_menu_items);

    key = ERR;

    while (!stop && !quit)
      {
      if (cur != old)
        {
        if (old != -1)
          {
          std::string text = pre_pad(pad_string(menu_items[old].name, barlen - 1), 1);
          mvwaddstr(win_menu, old + 1, 1, text.c_str());
          }

        setcolor(win_menu, SUBMENUREVCOLOR);

        std::string text = pre_pad(pad_string(menu_items[cur].name, barlen - 1), 1);
        mvwaddstr(win_menu, cur + 1, 1, text.c_str());
        setcolor(win_menu, SUBMENUCOLOR);
        status_message(menu_items[cur].description);
        old = cur;
        wrefresh(win_menu);
        }

      switch (int c = (key != ERR ? key : waitforkey()))
        {
        case '\n':
          touchwin(win_body);
          wrefresh(win_body);
          setmenupos(y + 1, x + 1);
          rmerror();

          key = ERR;
          curs_set(1);
          (menu_items[cur].fun)();   /* perform function */
          curs_set(0);

          repaintmenu(win_menu, menu_items, number_of_menu_items);

          stop = true;
          old = -1;
          break;

        case KEY_UP:
          cur = (cur + number_of_menu_items - 1) % number_of_menu_items;
          key = ERR;
          break;

        case KEY_DOWN:
          cur = (cur + 1) % number_of_menu_items;
          key = ERR;
          break;

        case KEY_ESC:
        case KEY_LEFT:
        case KEY_RIGHT:
          if (c == KEY_ESC)
            key = ERR;  /* return to prev submenu */
          else
            key = c;
          stop = true;
          break;
        }
      }

    rmerror();
    delwin(win_menu);
    touchwin(win_body);
    wrefresh(win_body);
    }

  void main_menu(const menu* menu_items, const std::size_t number_of_menu_items)
    {
    int old = -1;
    int cur = -1;
    int barlen = menu_width(menu_items, number_of_menu_items);
    repaintmainmenu(barlen, menu_items, number_of_menu_items);

    while (!quit)
      {
      if (cur < 0)
        {
        main_help_1();
        }
      else if (cur >= 0 && cur != old)
        {
        if (old != -1)
          {
          std::string text = pre_pad(pad_string(menu_items[old].name, barlen - 1), 1);
          mvwaddstr(win_main, 0, old * barlen, text.c_str());
          status_message(menu_items[cur].description);
          }
        else
          main_help_2();

        setcolor(win_main, MAINMENUREVCOLOR);

        std::string text = pre_pad(pad_string(menu_items[cur].name, barlen - 1), 1);
        mvwaddstr(win_main, 0, cur * barlen, text.c_str());
        setcolor(win_main, MAINMENUCOLOR);
        old = cur;
        wrefresh(win_main);
        }

      switch (int c = (key != ERR ? key : waitforkey()))
        {
        case KEY_DOWN:
        case '\n':
          if (cur >= 0)
            {
            touchwin(win_body);
            wrefresh(win_body);
            rmerror();
            setmenupos(title_height + main_menu_height, cur * barlen);
            curs_set(1);
            (menu_items[cur].fun)();   /* perform function */
            curs_set(0);

            switch (key)
              {
              case KEY_LEFT:
                cur = (cur + number_of_menu_items - 1) % number_of_menu_items;
                key = '\n';
                break;

              case KEY_RIGHT:
                cur = (cur + 1) % number_of_menu_items;
                key = '\n';
                break;

              default:
                key = ERR;
                old = -1;
                cur = -1;
              }

            repaintmainmenu(barlen, menu_items, number_of_menu_items);
            old = -1;
            }
          break;

        case KEY_LEFT:
          if (cur >= 0)
            {
            cur = (cur + number_of_menu_items - 1) % number_of_menu_items;
            }
          break;

        case KEY_RIGHT:
          if (cur >= 0)
            {
            cur = (cur + 1) % number_of_menu_items;
            }
          break;

        case KEY_ALT_L:
        case KEY_ALT_R:
          if (cur < 0)
            {
            cur = 0;
            old = -1;
            }
          break;

        case KEY_ESC:
          cur = -1;
          repaintmainmenu(barlen, menu_items, number_of_menu_items);
          break;
        }

      touchwin(win_body);
      wrefresh(win_body);
      }
    }

  void cleanup()
    {
    delwin(win_title);
    delwin(win_main);
    delwin(win_body);
    delwin(win_status);
    curs_set(1);
    endwin();
    }

  void run(const menu* menu_items, const std::size_t number_of_menu_items, const std::string& title)
    {
    initscr();
    initcolor();

    win_title = subwin(stdscr, title_height, body_width, 0, 0);
    win_main = subwin(stdscr, main_menu_height, body_width, title_height, 0);
    win_body = subwin(stdscr, body_height, body_width, title_height + main_menu_height, 0);
    win_status = subwin(stdscr, status_window_height, body_width, title_height + main_menu_height + body_height, 0);

    colorbox(win_title, TITLECOLOR, 0);
    colorbox(win_main, MAINMENUCOLOR, 0);
    colorbox(win_body, BODYCOLOR, 0);
    colorbox(win_status, STATUSCOLOR, 0);

    title_message(pad_string(title, body_width - 3));

    cbreak();
    noecho();
    PDC_return_key_modifiers(true);
    curs_set(0);
    nodelay(win_body, true);
    halfdelay(10);
    keypad(win_body, true);
    scrollok(win_body, true);

    leaveok(stdscr, true);
    leaveok(win_title, true);
    leaveok(win_main, true);
    leaveok(win_status, true);

    main_menu(menu_items, number_of_menu_items);

    cleanup();
    }

  void do_exit()
    {
    quit = true;
    }
  }