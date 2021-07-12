#include <windows.h>

static const auto hOut=GetStdHandle(STD_OUTPUT_HANDLE);

static auto GetConsoleCursorInfo() {
	CONSOLE_CURSOR_INFO CurSorInfo;
	PCONSOLE_CURSOR_INFO aret=&CurSorInfo;
	GetConsoleCursorInfo(hOut, aret);
	return*aret;
}

static CONSOLE_CURSOR_INFO CurSorInfo=GetConsoleCursorInfo();

void hideCursor(){
	CurSorInfo.bVisible=false;
	SetConsoleCursorInfo(hOut,&CurSorInfo);
}
void showCursor(){
	CurSorInfo.bVisible=true;
	SetConsoleCursorInfo(hOut,&CurSorInfo);
}
static COORD CursorPosition;

void saveCursorPos(){
	CONSOLE_SCREEN_BUFFER_INFO bInfo;
	GetConsoleScreenBufferInfo(hOut, &bInfo);
	CursorPosition=bInfo.dwCursorPosition;
}
void resetCursorPos(){
	SetConsoleCursorPosition(hOut,CursorPosition);
}
