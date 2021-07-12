#include <string>

extern void before_login();
extern void terminal_login();
extern std::wstring terminal_tab_press(const std::wstring&command,size_t tab_num);
extern void terminal_run(const std::wstring&command);
extern void terminal_exit();
