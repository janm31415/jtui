#include <iostream>
#include "jtui/jtui.h"
#include <vector>

void file_menu();
void action_menu();
void exit();
void message();
void message2();

namespace jtui
  {

  std::vector<menu> build_menu()
    {
    std::vector<menu> v;
    v.push_back(make_menu("File", "Go inside the file menu", &file_menu));
    v.push_back(make_menu("Actions", "Go inside the action menu", &action_menu));
    return v;
    }

  std::vector<menu> build_file_menu()
    {
    std::vector<menu> v;
    v.push_back(make_menu("Exit", "Quit the application", &exit));    
    return v;
    }

  std::vector<menu> build_actions_menu()
    {
    std::vector<menu> v;
    v.push_back(make_menu("Message", "Shows a message", &message));
    v.push_back(make_menu("Message 2", "Shows another message", &message2));
    return v;
    }

  }


void file_menu()
  {
  static std::vector<jtui::menu> m = jtui::build_file_menu();
  do_menu(m.data(), m.size());
  }

void action_menu()
  {
  static std::vector<jtui::menu> m = jtui::build_actions_menu();
  do_menu(m.data(), m.size());
  }

void exit()
  {
  jtui::do_exit();
  }

void message()
  {
  jtui::body_message("Hello world!");
  }

void message2()
  {
  jtui::body_message("Hello other world!\n");
  }

int main(int argc, char** argv)
  {
  std::locale::global(std::locale(""));
  ::setlocale(LC_ALL, "");
  std::vector<jtui::menu> m = jtui::build_menu();
  jtui::run(m.data(), m.size(), "jtui demo");
  return 0;
  }