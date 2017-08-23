#ifndef __STAMP_H__
#define __STAMP_H__

#include <Windows.h>
#include <string>

class CStamp
{
public:
	CStamp(HINSTANCE hInstance, const std::wstring& _rkFilename, int _iStartX, int _iStartY, int _iWidth, int _iHeight);
	~CStamp();

	// Make non-copiable (can't shallow copy bitmap handler)
	CStamp(const CStamp&) = delete;
	CStamp& operator=(const CStamp&) = delete;

	void Draw(HDC _hdc);
	void SetStartX(int _iStartX);
	void SetStartY(int _iStartY);
	void SetWidth(int _iWidth);
	void SetHeight(int _iHeight);

private:
	HBITMAP m_hBitMap;
	int m_iWidth;
	int m_iHeight;
	int m_iStartX;
	int m_iStartY;
	int m_iEndX;
	int m_iEndY;
};

#endif