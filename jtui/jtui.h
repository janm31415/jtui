#pragma once

#include <string>
#include <optional>
#include <vector>

extern "C"
  {
#include "curses.h"
  }

namespace jtui
  {

  struct state;

  typedef std::optional<state>(*action)(state);

  struct menu
    {
    std::string name;
    std::string description;
    action fun;
    };

  enum class active_menu_type
    {
    none,
    main,
    submenu
    };

  struct state
    {
    WINDOW* win_title = nullptr;
    WINDOW* win_main = nullptr;
    WINDOW* win_body = nullptr;
    WINDOW* win_status = nullptr;
    WINDOW* win_menu = nullptr;
    bool quit = false;
    int key = ERR;
    int menu_y = -1;
    int menu_x = -1;
    std::vector<menu> main_menu;
    std::vector<menu> sub_menu;
    int old_main_menu = -1;
    int current_main_menu = -1;
    int old_sub_menu = -1;
    int current_sub_menu = -1;
    int main_menu_item_width = -1;
    int sub_menu_item_width = -1;
    active_menu_type active_menu = active_menu_type::none;
    };

  void run(const std::vector<menu>& main_menu, const std::string& title);

  void body_message(const state& current_state, const std::string& msg);

  }