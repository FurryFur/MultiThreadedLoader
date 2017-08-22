#include "stamp.h"
#include "resource.h"

CStamp::CStamp() :
	m_hBitMap(0)
{
}

CStamp::CStamp(HINSTANCE _hInstance, HWND _hwnd, const std::wstring& _rkFilename, int _iStartX, int _iStartY) :
	m_hBitMap(0),
	m_iStartX(_iStartX),
	m_iStartY(_iStartY)
{
	// Load bitmap from file if one is specified
	if (!_rkFilename.empty())
	{
		HBITMAP hBitmap = static_cast<HBITMAP>(LoadImage(_hInstance, _rkFilename.c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE));
		if (!hBitmap)
		{
			MessageBoxA(_hwnd, "Error opening file", "Error", MB_ICONERROR);
		}
		else
		{
			m_hBitMap = hBitmap;
		}
	}

	BITMAP bitmap;
	GetObject(m_hBitMap, sizeof(BITMAP), &bitmap);
	m_iWidth = bitmap.bmWidth;
	m_iHeight = bitmap.bmHeight;
	m_iEndX = _iStartX + m_iWidth;
	m_iEndY = _iStartY + m_iHeight;
}

CStamp::~CStamp()
{
}

void CStamp::Draw(HDC _hdc)
{
	// Create device context to load bitmap into
	HDC hdcBitmap = CreateCompatibleDC(_hdc);

	// Select bitmap into device context
	HBITMAP hOldBitmap = static_cast<HBITMAP>(SelectObject(hdcBitmap, m_hBitMap));

	// Bit blip bitmap device context to main device context
	BitBlt(_hdc, m_iStartX, m_iStartY, m_iWidth, m_iHeight, hdcBitmap, 0, 0, SRCCOPY);

	// Cleanup
	static_cast<HBITMAP>(SelectObject(hdcBitmap, hOldBitmap));
	DeleteDC(hdcBitmap);
}

void CStamp::SetStartX(int _iStartX)
{
	m_iStartX = _iStartX;
	m_iEndX = _iStartX + m_iWidth;
}

void CStamp::SetStartY(int _iStartY)
{
	m_iStartY = _iStartY;
	m_iEndY = _iStartY + m_iWidth;
}
