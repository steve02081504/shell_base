#include <string>
#include <vector>

#include "my-gists/cpp/wcwidth.hpp"//wcwidth
#include "my-gists/windows/clipboard.hpp"//setClipboard、getClipboard
#include "my-gists/windows/Cursor.hpp"//hideCursor、showCursor、saveCursorPos、resetCursorPos
#include "shell_base.hpp"
#define NOMINMAX
#include <windows.h>

#ifdef _DEBUG
#include <iostream>
#endif



using namespace std;

#define floop while(1)

size_t GetStrWide(const wstring&str,size_t begin=0,size_t end=wstring::npos){
	size_t aret=0;
	auto i=str.begin()+begin;
	auto e=str.begin()+(end==wstring::npos?str.size():end);
	while(i!=e)
		aret+=max(wcwidth(*(i++)),0);
	return aret;
}

void putstr(const wstring&str){
	for(auto&c:str)
		_putwch(c);
}
void putchar_x_time(wchar_t the_char,size_t time){
	while(time--)
		_putwch(the_char);
}

void reflash_command(size_t&insert_index,wstring&command,const wstring&new_command){
	auto size_all=GetStrWide(command);
	resetCursorPos();
	putchar_x_time(' ',size_all);
	#ifdef _DEBUG
	wcout<<"\n\n\ncommand_size: "<<new_command.size()<<"   ";
	#endif
	resetCursorPos();
	putstr(new_command);
	resetCursorPos();
	putstr(new_command.substr(0,insert_index));
	command=new_command;
}
void move_insert_index(size_t&insert_index,size_t new_insert_index,const wstring&command){
	#ifdef _DEBUG
	wcout<<"\n\ninsert_index: "<<new_insert_index<<"   ";
	resetCursorPos();
	putstr(command.substr(0,insert_index));
	#endif
	if(new_insert_index >= insert_index)
		putstr(command.substr(insert_index,new_insert_index-insert_index));
	else{
		resetCursorPos();
		putstr(command.substr(0,new_insert_index));
	}
	if(insert_index>command.size())
		insert_index=command.size();

	insert_index=new_insert_index;
}

/*
#include <iostream>
int main(){
	floop{
		auto tmp=_getwch();
		wcout << tmp;
		if(tmp==0||tmp==0xE0)
			wcout << "    " << _getwch();
		wcout << endl;
	}
}

/*/
BOOL WINAPI ConsoleHandler(DWORD CEvent)
{
	switch(CEvent)
	{
		case CTRL_CLOSE_EVENT:
			terminal_exit();
			break;
	}
	return TRUE;
}
int wmain(size_t argc,wchar_t**argv){
	SetConsoleCtrlHandler((PHANDLER_ROUTINE)ConsoleHandler,TRUE);
	before_login();
	vector<wstring>command_history;
	{
		vector<wstring> argv_t(argc);
		auto tmp = argc;
		while (tmp--)
			argv_t[tmp] = argv[tmp];
		terminal_args(argc, argv_t);
	}
	terminal_login();
	floop{
	command_in:
		putstr(L">> ");
		saveCursorPos();
		wstring command,tab_head,tab_end;
		size_t tab_num=0,insert_index=0,edit_history_index=0;
		size_t history_index=command_history.size();
		vector<wstring>edit_history_command{L""};
		vector<size_t>edit_history_insert_index{0};
		command_history.push_back(L"");
		floop{
			showCursor();
			auto c=_getwch();
			hideCursor();
			switch(c){
			case 27://esc
				return 0;
			case 13://enter
				_putwch('\n');
				goto run_command;
			case WEOF:
				if(command.empty())
					return 0;
				else
					goto run_command;
			case 24://ctrl-x
				setClipboard(command);
				command.clear();
				break;
			case 26://ctrl-z Undo
				if(edit_history_index)
					edit_history_index--;
				goto load_history;
			case 25://ctrl-y redo
				edit_history_index++;
				goto load_history;
			load_history:
				if(edit_history_index>edit_history_command.size()-1)
					edit_history_index=edit_history_command.size()-1;
				reflash_command(insert_index,command,edit_history_command[edit_history_index]);
				move_insert_index(insert_index,edit_history_insert_index[edit_history_index],command);
				break;
			case 3://ctrl-c
				setClipboard(command);
				break;
			case 22:{//ctrl-v
				auto insert_text=getClipboard();
				auto old_command=command;
				command.insert(insert_index,insert_text);
				reflash_command(insert_index,old_command,command);
				move_insert_index(insert_index,insert_index+insert_text.size(),command);
				break;
			}
			case 9:{//tab
				if(tab_num==0){
					tab_head=command.substr(0,insert_index);
					tab_end=command.substr(insert_index);
				}
				auto new_command_head=terminal_tab_press(tab_head,tab_num);
				auto new_command=new_command_head+tab_end;
				reflash_command(insert_index,command,new_command);
				move_insert_index(insert_index,new_command_head.size(),new_command);
				break;
			}
			case 0xE0:{//方向字符先导字符
				switch(_getwch()){
				case 72://up
					if(history_index){
						command_history[history_index]=command;
						history_index--;
						reflash_command(insert_index,command,command_history[history_index]);
						move_insert_index(insert_index,command.size(),command);
					}
					break;
				case 80://down
					if(command_history.size()>history_index+1){
						command_history[history_index]=command;
						history_index++;
						reflash_command(insert_index,command,command_history[history_index]);
						move_insert_index(insert_index,command.size(),command);
					}
					break;
				case 75://left
					if(insert_index){
						move_insert_index(insert_index,insert_index-1,command);
					}
					break;
				case 77://right
					if(insert_index<command.size()){
						_putwch(command[insert_index++]);
					}
					break;
				case 83://delete
					if(insert_index<command.size()){
						auto old_command=command;
						command.erase(insert_index,1);
						reflash_command(insert_index,old_command,command);
					}
					break;
				}
				break;
			}
			case 8://backspace
				if(insert_index){
					auto old_command=command;
					command.erase(insert_index-1,1);
					reflash_command(insert_index,old_command,command);
					move_insert_index(insert_index,insert_index-1,command);
				}
				break;
			default:
				if(wcwidth(c)<1)
					break;
				auto old_command=command;
				command.insert(insert_index,1,c);
				reflash_command(insert_index,old_command,command);
				move_insert_index(insert_index,insert_index+1,command);
				break;
			}
			if(c==9)
				tab_num++;
			else
				tab_num=0;
			if(c!=25&&c!=26)
				if(command!=edit_history_command.back()){
					edit_history_command.resize(insert_index+1);
					edit_history_insert_index.resize(insert_index+1);
					edit_history_command.push_back(command);
					edit_history_insert_index.push_back(insert_index);
					edit_history_index++;
				}
		}
	run_command:
		command_history[command_history.size()-1]=command;
		if (command != L"exit")
			terminal_run(command);
		else
			break;
	}
	terminal_exit();
	return 0;
}
//*/
