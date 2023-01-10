#pragma once

#include <string>
#include <optional>
#include <vector>

typedef struct _win WINDOW;

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
    int key = -1;
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
    action on_idle = nullptr;
    void* user_data = nullptr;
    std::string title;
    };

  struct color_scheme
    {
    uint32_t menu_background = 0xffc0c000;
    uint32_t title_color = 0xff000000;
    uint32_t body_background = 0xffc00000;
    uint32_t text_color = 0xffc0c0c0;
    uint32_t text_color_bold = 0xffffffff;
    };

  color_scheme get_default_color_scheme();
  color_scheme get_blue_color_scheme();
  color_scheme get_dark_color_scheme();
  color_scheme get_darkblue_color_scheme();
  color_scheme get_acme_color_scheme();

  void get_jtui_window_size(uint32_t& rows, uint32_t& cols);

  std::optional<state> body_message(state current_state, const std::string& msg);

  std::optional<state> status_message(state current_state, const std::string& msg);

  std::optional<state> error_message(state current_state, const std::string& msg);

  std::optional<state> title_message(state current_state, const std::string& msg);

  std::optional<state> close_submenu(state current_state);

  std::optional<state> do_submenu(state current_state, const std::vector<menu>& sub_menu);

  std::optional<state> do_exit(state current_state);

  std::optional<state> do_editbox(state current_state, 
    const std::vector<std::string>& field_names, 
    const std::vector<std::string>& field_values, 
    int edit_length, 
    action on_editbox_ok, 
    action on_editbox_cancel);

  std::optional<state> on_idle(state current_state);

  struct settings
    {
    color_scheme colors = get_default_color_scheme();
    action idle_action = &on_idle;
    void* user_data = nullptr;
    };

  void run(const std::vector<menu>& main_menu, const std::string& title, settings s = settings());

  }