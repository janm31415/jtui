#pragma once

#include <string>

extern "C"
  {
#include "curses.h"
  }

namespace jtui
  {

  typedef void (*menu_function)(void);

  struct menu
    {
    std::string name;
    std::string description;
    menu_function fun = nullptr;
    };

  menu make_menu(const std::string& name, const std::string& description, menu_function fun_ptr);

  void do_menu(const menu* menu_items, const std::size_t number_of_menu_items);

  void run(const menu* menu_items, const std::size_t number_of_menu_items, const std::string& title);

  void do_exit();
  }