#include <iostream>
#include "jtui/jtui.h"
#include <vector>

std::optional<jtui::state> quit(jtui::state st)
  {
  return jtui::do_exit();
  }

std::optional<jtui::state> message1(jtui::state st)
  {
  return jtui::body_message(st, "Hello world!");
  }

std::optional<jtui::state> message2(jtui::state st)
  {
  return jtui::body_message(st, "This is a message ending with newline\n");  
  }

std::optional<jtui::state> subsub(jtui::state st)
  {
  std::vector<jtui::menu> v;
  v.push_back({ std::string("Sub message 1"), std::string("Prints a message"), &message1 });
  v.push_back({ std::string("Sub message 2"), std::string("Prints another message"), &message2 });
  return do_submenu(st, v);
  }

std::optional<jtui::state> file_menu(jtui::state st)
  {
  std::vector<jtui::menu> v;
  v.push_back({ std::string("Exit"), std::string("Quit the application"), &quit });
  return do_submenu(st, v);
  }

std::optional<jtui::state> action_menu(jtui::state st)
  {
  std::vector<jtui::menu> v;
  v.push_back({ std::string("Message 1"), std::string("Prints a message"), &message1 });
  v.push_back({ std::string("Message 2"), std::string("Prints another message"), &message2 });
  v.push_back({ std::string("Sub sub"), std::string("Make a sub sub menu"), &subsub });
  return do_submenu(st, v);
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