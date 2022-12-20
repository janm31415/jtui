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

  void rmline(WINDOW* win, int nr)
    {
    std::string text = pad_string(" ", body_width - 2);
    mvwaddstr(win, nr, 1, text.c_str());
    wrefresh(win);
    }

  void title_message(const state& current_state, const std::string& msg)
    {
    mvwaddstr(current_state.win_title, 0, 2, msg.c_str());
    wrefresh(current_state.win_title);
    }

  std::optional<state> body_message(state current_state, const std::string& msg)
    {
    waddstr(current_state.win_body, msg.c_str());
    wrefresh(current_state.win_body);
    return current_state;
    }

  void status_message(const state& current_state, const std::string& msg)
    {
    std::string text = pad_string(msg, body_width - 3);
    mvwaddstr(current_state.win_status, 1, 2, text.c_str());
    wrefresh(current_state.win_status);
    }

  int menu_width(const std::vector<menu>& menu_items)
    {
    int menu_width = 0;
    for (const auto& m : menu_items)
      {
      if (m.name.length() > menu_width)
        menu_width = (int)m.name.length();
      }
    return menu_width + 2;
    }

  void cleanup(state current_state)
    {
    delwin(current_state.win_title);
    delwin(current_state.win_main);
    delwin(current_state.win_body);
    delwin(current_state.win_status);
    curs_set(1);
    endwin();
    }

  state idle(state current_state)
    {
    char buf[256];
    time_t t;
    struct tm* tp;

    if (time(&t) == -1)
      return current_state;  /* time not available */

    tp = localtime(&t);
    sprintf(buf, " %.4d-%.2d-%.2d  %.2d:%.2d:%.2d",
      tp->tm_year + 1900, tp->tm_mon + 1, tp->tm_mday,
      tp->tm_hour, tp->tm_min, tp->tm_sec);

    mvwaddstr(current_state.win_title, 0, body_width - (int)strlen(buf) - 2, buf);
    wrefresh(current_state.win_title);
    return current_state;
    }

  state draw_main_menu(state current_state)
    {
    int i = 0;
    for (const auto& m : current_state.main_menu)
      {
      std::string text = pre_pad(pad_string(m.name, current_state.main_menu_item_width - 1), 1);
      mvwaddstr(current_state.win_main, 0, i * current_state.main_menu_item_width, text.c_str());
      ++i;
      }

    touchwin(current_state.win_main);
    wrefresh(current_state.win_main);

    if (current_state.current_main_menu < 0)
      {
      status_message(current_state, "Use ALT to enter the menu.");
      }
    else if (current_state.current_main_menu >= 0 && current_state.current_main_menu != current_state.old_main_menu)
      {
      if (current_state.old_main_menu != -1)
        {
        std::string text = pre_pad(pad_string(current_state.main_menu[current_state.old_main_menu].name, current_state.main_menu_item_width - 1), 1);
        mvwaddstr(current_state.win_main, 0, current_state.old_main_menu * current_state.main_menu_item_width, text.c_str());
        status_message(current_state, current_state.main_menu[current_state.current_main_menu].description);
        }
      else
        {
        status_message(current_state, "Use arrow keys to navigate the menu.");
        }
      setcolor(current_state.win_main, MAINMENUREVCOLOR);

      std::string text = pre_pad(pad_string(current_state.main_menu[current_state.current_main_menu].name, current_state.main_menu_item_width - 1), 1);
      mvwaddstr(current_state.win_main, 0, current_state.current_main_menu * current_state.main_menu_item_width, text.c_str());
      setcolor(current_state.win_main, MAINMENUCOLOR);
      current_state.old_main_menu = current_state.current_main_menu;
      wrefresh(current_state.win_main);
      }
    return current_state;
    }

  state draw_sub_menu(state current_state)
    {
    if (current_state.win_menu == nullptr)
      {
      current_state.sub_menu_item_width = menu_width(current_state.sub_menu);
      int mheight = (int)current_state.sub_menu.size() + 2;
      int mwidth = current_state.sub_menu_item_width + 2;
      current_state.win_menu = newwin(mheight, mwidth, current_state.menu_y, current_state.menu_x);
      colorbox(current_state.win_menu, SUBMENUCOLOR, 1);
      current_state.old_sub_menu = -1;
      current_state.current_sub_menu = 0;
      }
    int i = 0;
    for (const auto& m : current_state.sub_menu)
      {
      mvwaddstr(current_state.win_menu, i + 1, 2, m.name.c_str());
      ++i;
      }
    touchwin(current_state.win_menu);
    wrefresh(current_state.win_menu);

    if (current_state.current_sub_menu != current_state.old_sub_menu)
      {
      if (current_state.old_sub_menu != -1)
        {
        std::string text = pre_pad(pad_string(current_state.sub_menu[current_state.old_sub_menu].name, current_state.sub_menu_item_width - 1), 1);
        mvwaddstr(current_state.win_menu, current_state.old_sub_menu + 1, 1, text.c_str());
        }

      setcolor(current_state.win_menu, SUBMENUREVCOLOR);

      std::string text = pre_pad(pad_string(current_state.sub_menu[current_state.current_sub_menu].name, current_state.sub_menu_item_width - 1), 1);
      mvwaddstr(current_state.win_menu, current_state.current_sub_menu + 1, 1, text.c_str());
      setcolor(current_state.win_menu, SUBMENUCOLOR);
      status_message(current_state, current_state.sub_menu[current_state.current_sub_menu].description);
      current_state.old_sub_menu = current_state.current_sub_menu;
      wrefresh(current_state.win_menu);
      }

    return current_state;
    }

  state draw(state current_state)
    {
    state new_state = idle(current_state);
    if (new_state.active_menu == active_menu_type::submenu)
      new_state = draw_sub_menu(new_state);
    else
      new_state = draw_main_menu(new_state);
    return new_state;
    }

  std::optional<state> do_exit()
    {
    return std::optional<state>();
    }

  std::optional<state> do_submenu(state current_state, const std::vector<menu>& sub_menu)
    {
    current_state.sub_menu = sub_menu;
    current_state.active_menu = jtui::active_menu_type::submenu;
    if (current_state.win_menu != nullptr)
      {
      rmline(current_state.win_status, 0);
      delwin(current_state.win_menu);
      current_state.win_menu = nullptr;
      touchwin(current_state.win_body);
      wrefresh(current_state.win_body);
      ++current_state.menu_x; // this is a sub sub menu
      ++current_state.menu_y;
      }
    current_state.current_sub_menu = 0;
    current_state.old_sub_menu = -1;
    return current_state;
    }

  std::optional<state> enter_submenu(state current_state)
    {
    touchwin(current_state.win_body);
    wrefresh(current_state.win_body);
    rmline(current_state.win_status, 0);
    current_state.menu_x = current_state.current_main_menu * current_state.main_menu_item_width;
    current_state.menu_y = title_height + main_menu_height;
    curs_set(1);
    std::optional<state> new_state = (current_state.main_menu[current_state.current_main_menu].fun)(current_state);   /* perform function */
    curs_set(0);
    return new_state;
    }

  std::optional<state> process_input(state current_state)
    {
    for (;;)
      {
      int c = (current_state.key != ERR) ? current_state.key : wgetch(current_state.win_body);
      if (c != ERR)
        {
        current_state.key = ERR;
        switch (c)
          {
          case KEY_UP:
          {
          if (current_state.active_menu == active_menu_type::submenu)
            {
            current_state.current_sub_menu = (current_state.current_sub_menu + (int)current_state.sub_menu.size() - 1) % (int)current_state.sub_menu.size();
            if (current_state.sub_menu.size() == 1)
              current_state.old_sub_menu = -1;
            return current_state;
            }
          break;
          }
          case KEY_DOWN:
          {
          if (current_state.active_menu == active_menu_type::submenu)
            {
            current_state.current_sub_menu = (current_state.current_sub_menu + 1) % (int)current_state.sub_menu.size();
            if (current_state.sub_menu.size() == 1)
              current_state.old_sub_menu = -1;
            return current_state;
            }
          if (current_state.active_menu == active_menu_type::main && current_state.current_main_menu >= 0)
            {
            return enter_submenu(current_state);
            }
          break;
          }
          case '\n':
          {
          if (current_state.active_menu == active_menu_type::main && current_state.current_main_menu >= 0)
            {
            return enter_submenu(current_state);
            }
          if (current_state.active_menu == active_menu_type::submenu)
            {
            touchwin(current_state.win_body);
            wrefresh(current_state.win_body);
            rmline(current_state.win_status, 0);
            current_state.old_sub_menu = -1;
            curs_set(1);
            std::optional<state> new_state = (current_state.sub_menu[current_state.current_sub_menu].fun)(current_state);   /* perform function */
            curs_set(0);
            return new_state;
            }
          break;
          }
          case KEY_LEFT:
          {
          if (current_state.active_menu == active_menu_type::main && current_state.current_main_menu >= 0)
            {
            current_state.current_main_menu = (current_state.current_main_menu + (int)current_state.main_menu.size() - 1) % (int)current_state.main_menu.size();
            return current_state;
            }
          if (current_state.active_menu == active_menu_type::submenu)
            {
            rmline(current_state.win_status, 0);
            delwin(current_state.win_menu);
            current_state.win_menu = nullptr;
            touchwin(current_state.win_body);
            wrefresh(current_state.win_body);
            current_state.active_menu = active_menu_type::main;
            current_state.old_main_menu = -1;
            current_state.current_main_menu = (current_state.current_main_menu + (int)current_state.main_menu.size() - 1) % (int)current_state.main_menu.size();
            current_state.key = '\n';
            return current_state;
            }
          break;
          }
          case KEY_RIGHT:
          {
          if (current_state.active_menu == active_menu_type::main && current_state.current_main_menu >= 0)
            {
            current_state.current_main_menu = (current_state.current_main_menu + 1) % (int)current_state.main_menu.size();
            return current_state;
            }
          if (current_state.active_menu == active_menu_type::submenu)
            {
            rmline(current_state.win_status, 0);
            delwin(current_state.win_menu);
            current_state.win_menu = nullptr;
            touchwin(current_state.win_body);
            wrefresh(current_state.win_body);
            current_state.active_menu = active_menu_type::main;
            current_state.old_main_menu = -1;
            current_state.current_main_menu = (current_state.current_main_menu + 1) % (int)current_state.main_menu.size();
            current_state.key = '\n';
            return current_state;
            }
          break;
          }
          case KEY_ALT_L:
          case KEY_ALT_R:
          {
          if (current_state.active_menu != active_menu_type::submenu && current_state.current_main_menu < 0)
            {
            current_state.current_main_menu = 0;
            current_state.old_main_menu = -1;
            current_state.active_menu = active_menu_type::main;
            return current_state;
            }
          break;
          }
          case KEY_ESC:
          {
          if (current_state.active_menu == active_menu_type::submenu)
            {
            rmline(current_state.win_status, 0);
            delwin(current_state.win_menu);
            current_state.win_menu = nullptr;
            touchwin(current_state.win_body);
            wrefresh(current_state.win_body);
            current_state.active_menu = active_menu_type::main;
            current_state.old_main_menu = -1;
            return current_state;
            }
          else if (current_state.active_menu == active_menu_type::main && current_state.current_main_menu >= 0)
            {
            current_state.current_main_menu = -1;
            current_state.active_menu = active_menu_type::none;
            return current_state;
            }
          break;
          }
          default:
            break;
          }
        }
      else
        {
        current_state = idle(current_state);
        }
      }
    }

  void run(const std::vector<menu>& main_menu, const std::string& title)
    {
    state current_state;
    current_state.main_menu = main_menu;
    current_state.main_menu_item_width = menu_width(current_state.main_menu);
    initscr();
    initcolor();

    current_state.win_title = subwin(stdscr, title_height, body_width, 0, 0);
    current_state.win_main = subwin(stdscr, main_menu_height, body_width, title_height, 0);
    current_state.win_body = subwin(stdscr, body_height, body_width, title_height + main_menu_height, 0);
    current_state.win_status = subwin(stdscr, status_window_height, body_width, title_height + main_menu_height + body_height, 0);

    colorbox(current_state.win_title, TITLECOLOR, 0);
    colorbox(current_state.win_main, MAINMENUCOLOR, 0);
    colorbox(current_state.win_body, BODYCOLOR, 0);
    colorbox(current_state.win_status, STATUSCOLOR, 0);

    title_message(current_state, pad_string(title, body_width - 3));

    cbreak();
    noecho();
    PDC_return_key_modifiers(true);
    curs_set(0);
    nodelay(current_state.win_body, true);
    halfdelay(10);
    keypad(current_state.win_body, true);
    scrollok(current_state.win_body, true);

    leaveok(stdscr, true);
    leaveok(current_state.win_title, true);
    leaveok(current_state.win_main, true);
    leaveok(current_state.win_status, true);

    current_state = draw(current_state);

    while (std::optional<state> new_state = process_input(current_state))
      {
      current_state = *new_state;
      current_state = draw(current_state);
      }

    cleanup(current_state);
    }



  } // namespace jtui