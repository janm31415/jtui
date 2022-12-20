#include <iostream>
#include "jtui/jtui.h"
#include <vector>

void file_menu();
void action_menu();
void exit();
void message();

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
  }

int main(int argc, char** argv)
  {
  //setlocale(LC_ALL, "");
  std::vector<jtui::menu> m = jtui::build_menu();
  jtui::run(m.data(), m.size(), "jtui demo");
  return 0;
  }