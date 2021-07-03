#include <string>
#include <vector>

using namespace std;

void before_login();
void terminal_login();
wstring terminal_tab_press(const wstring&command,size_t tab_num);
void terminal_run(const wstring&command);
void terminal_exit();

void setClipboard(const wstring& str);
wstring getClipboard();

#define floop while(1)
extern "C" int wcwidth(wchar_t);

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
	auto size_defore_index=GetStrWide(command,0,insert_index);
	auto size_after_index=GetStrWide(command,insert_index);
	auto size_all=size_defore_index+size_after_index;
	putchar_x_time(' ',size_after_index);
	putchar_x_time('\b',size_all);
	putchar_x_time(' ',size_defore_index);
	putchar_x_time('\b',size_defore_index);
	putstr(new_command);
	auto size_of_new_command=GetStrWide(new_command);
	if(size_of_new_command>size_defore_index)
		putchar_x_time('\b',size_of_new_command-size_defore_index);
	else
		insert_index=new_command.size();

	command=new_command;
}
void move_insert_index(size_t&insert_index,size_t new_insert_index,const wstring&command){
	if(new_insert_index > insert_index)
		putstr(command.substr(insert_index,new_insert_index-insert_index));
	else
		putchar_x_time('\b',GetStrWide(command,new_insert_index,insert_index));

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

int main(){
	before_login();
	vector<wstring>command_history;
	terminal_login();
	floop{
	command_in:
		putstr(L">> ");
		wstring command,tab_head,tab_end;
		size_t tab_num=0,insert_index=0,edit_history_index=0;
		size_t history_index=command_history.size();
		vector<wstring>edit_history_command{L""};
		vector<size_t>edit_history_insert_index{0};
		command_history.push_back(L"");
		floop{
			auto c=_getwch();
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
				if(edit_history_index<edit_history_command.size()-1)
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
				}
				break;
			}
			case 8://backspace
				if(insert_index){
					auto old_command=command;
					move_insert_index(insert_index,insert_index-1,command);
					command.erase(insert_index,1);
					reflash_command(insert_index,old_command,command);
				}
				break;
			default:
				if(wcwidth(c)<1)
					break;
				auto old_command=command;
				command.insert(insert_index,1,c);
				reflash_command(insert_index,old_command,command);
				_putwch(command[insert_index++]);
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
