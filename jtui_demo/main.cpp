#include <iostream>
#include "jtui/jtui.h"
#include <vector>

std::optional<jtui::state> quit(jtui::state st)
  {
  return std::optional<jtui::state>();
  }

std::optional<jtui::state> message1(jtui::state st)
  {
  jtui::body_message(st, "Hello world!");
  return st;
  }

std::optional<jtui::state> message2(jtui::state st)
  {
  jtui::body_message(st, "This is a message ending with newline\n");
  return st;
  }

std::optional<jtui::state> file_menu(jtui::state st)
  {
  std::vector<jtui::menu> v;
  v.push_back({ std::string("Exit"), std::string("Quit the application"), &quit });
  st.sub_menu = v;
  st.active_menu = jtui::active_menu_type::submenu;
  return st;
  }

std::optional<jtui::state> action_menu(jtui::state st)
  {
  std::vector<jtui::menu> v;
  v.push_back({ std::string("Message 1"), std::string("Prints a message"), &message1 });
  v.push_back({ std::string("Message 2"), std::string("Prints another message"), &message2 });
  st.sub_menu = v;
  st.active_menu = jtui::active_menu_type::submenu;
  return st;
  }

std::vector<jtui::menu> build_menu()
  {
  std::vector<jtui::menu> v;
  v.push_back({std::string("File"), std::string("Go inside the file menu"), &file_menu});
  v.push_back({std::string("Actions"), std::string("Go inside the action menu"), &action_menu});
  return v;
  }

int main(int argc, char** argv)
  {
  std::vector<jtui::menu> main_menu = build_menu();
  jtui::run(main_menu, std::string("jtui demo"));
  return 0;
  }