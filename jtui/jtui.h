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

  enum class activity_type
    {
    none,
    main,
    submenu,
    editbox
    };

  struct state
    {
    WINDOW* win_title = nullptr;
    WINDOW* win_main = nullptr;
    WINDOW* win_body = nullptr;
    WINDOW* win_status = nullptr;
    WINDOW* win_menu = nullptr;
    WINDOW* win_editbox = nullptr;
    WINDOW* win_inputline = nullptr;
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
    int editbox_field_width = -1;
    int editbox_active_line = -1;
    int editbox_cursor_pos = -1;
    bool editbox_insert_mode = false;
    std::vector<std::string> editbox_field_values;
    activity_type activity = activity_type::none;
    action on_editbox_ok = nullptr;
    action on_editbox_cancel = nullptr;
    void* user_data = nullptr;
    };

  void run(const std::vector<menu>& main_menu, const std::string& title);

  std::optional<state> body_message(state current_state, const std::string& msg);

  std::optional<state> status_message(state current_state, const std::string& msg);

  std::optional<state> error_message(state current_state, const std::string& msg);

  std::optional<state> title_message(state current_state, const std::string& msg);

  std::optional<state> do_submenu(state current_state, const std::vector<menu>& sub_menu);

  std::optional<state> do_exit(state current_state);

  std::optional<state> do_editbox(state current_state, 
    const std::vector<std::string>& field_names, 
    const std::vector<std::string>& field_values, 
    int edit_length, 
    action on_editbox_ok, 
    action on_editbox_cancel);
  }