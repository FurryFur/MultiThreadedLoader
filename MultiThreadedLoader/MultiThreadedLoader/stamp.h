#ifndef __STAMP_H__
#define __STAMP_H__

#include <Windows.h>
#include <string>

class CStamp
{
public:
	CStamp();
	CStamp(HINSTANCE hInstance, HWND _hwnd, const std::wstring& _rkstrFilename, int, int );
	~CStamp();

	void Draw(HDC _hdc);
	void SetStartX(int _iStartX);
	void SetStartY(int _iStartY);

private:
	HBITMAP m_hBitMap;
	BITMAP m_bitmapStructure;
	int m_iWidth;
	int m_iHeight;
	int m_iStartX;
	int m_iStartY;
	int m_iEndX;
	int m_iEndY;
};

#endif