#include "jtui/jtui.h"
#include <vector>

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
  v.push_back({ std::string("suB message 2"), std::string("Prints another message"), &message2 });
  return do_submenu(st, v);
  }

std::optional<jtui::state> on_address_ok(jtui::state st)
  {
  std::vector<std::string> field_names;
  field_names.push_back("Name");
  field_names.push_back("Street");
  field_names.push_back("City");
  field_names.push_back("Country");
  for (int i = 0; i < (int)field_names.size(); ++i)
    {
    std::string line = field_names[i] + std::string(": ") + st.editbox_field_values[i] + std::string("\n");
    st = *jtui::body_message(st, line);
    }
  return st;
  }

std::optional<jtui::state> on_address_cancel(jtui::state st)
  {
  return jtui::body_message(st, "Address was cancelled\n");
  }

std::optional<jtui::state> command(jtui::state st)
  {
  std::vector<std::string> field_names;
  field_names.push_back("Name");
  field_names.push_back("Street");
  field_names.push_back("City");
  field_names.push_back("Country");
  std::vector<std::string> field_values;
  field_values.push_back("John Doe");
  field_values.push_back("some street");
  return jtui::do_editbox(st, field_names, field_values, 50, &on_address_ok, &on_address_cancel);
  }

std::optional<jtui::state> file_menu(jtui::state st)
  {
  std::vector<jtui::menu> v;
  v.push_back({ std::string("Command"), std::string("Enter a command"), &command});
  v.push_back({ std::string("eXit"), std::string("Quit the application"), &jtui::do_exit });
  return jtui::do_submenu(st, v);
  }

std::optional<jtui::state> action_menu(jtui::state st)
  {
  std::vector<jtui::menu> v;
  v.push_back({ std::string("Message 1"), std::string("Prints a message"), &message1 });
  v.push_back({ std::string("mEssage 2"), std::string("Prints another message"), &message2 });
  v.push_back({ std::string("Sub sub"), std::string("Make a sub sub menu"), &subsub });
  return jtui::do_submenu(st, v);
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
  #ifdef _WIN32
  std::locale::global(std::locale(""));
  ::setlocale(LC_ALL, "");
  #endif
  std::vector<jtui::menu> main_menu = build_menu();
  jtui::run(main_menu, std::string("jtui demo"));
  return 0;
  }
