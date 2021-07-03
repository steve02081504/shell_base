#include <windows.h>
#include <string>

using namespace std;

void setClipboard(const wstring& str) {
	HGLOBAL hdst;
	LPWSTR dst;
	hdst = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, (str.size()+1)*sizeof(WCHAR));
	if(!hdst)
		return;
	dst = (LPWSTR)GlobalLock(hdst);
	if(!dst){
		GlobalFree(hdst);
		return;
	}
	memcpy(dst, str.c_str(), str.size()*sizeof(WCHAR));
	dst[str.size()] = 0;
	GlobalUnlock(hdst);
	if (!OpenClipboard(NULL))
		return;
	EmptyClipboard();
	if (!SetClipboardData(CF_UNICODETEXT, hdst))
		return;
	CloseClipboard();
}

wstring getClipboard(){
	wstring aret;
	if (OpenClipboard(NULL)) {
		HANDLE hClipboardData = GetClipboardData(CF_UNICODETEXT);
		if (hClipboardData) {
			WCHAR *pchData = (WCHAR*)GlobalLock(hClipboardData);
			if (pchData) {
				aret = pchData;
				GlobalUnlock(hClipboardData);
			}
		}
		CloseClipboard();
	}
	return aret;
}
