//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2017 Media Design School
//
// File Name	: stamp.cpp
// Description	: Implementation file for CStamp class
// Author		: Lance Chaney
// Mail			: lance.cha7337@mediadesign.school.nz
//

#include "stamp.h"
#include "resource.h"
#include "util.h"

CStamp::CStamp(HINSTANCE _hInstance, const std::wstring& _rkFilename, int _iStartX, int _iStartY, int _iWidth, int _iHeight) :
	m_hBitMap(0),
	m_iStartX(_iStartX),
	m_iStartY(_iStartY)
{
	// Load bitmap from file if one is specified
	if (!_rkFilename.empty())
	{
		HBITMAP hBitmap = static_cast<HBITMAP>(LoadImage(NULL, _rkFilename.c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE));
		if (!hBitmap)
		{
			throw ResourceLoadException();
		}
		else
		{
			m_hBitMap = hBitmap;
		}
	}

	m_iWidth = _iWidth;
	m_iHeight = _iHeight;
	m_iEndX = _iStartX + m_iWidth;
	m_iEndY = _iStartY + m_iHeight;
}

CStamp::~CStamp()
{
	DeleteObject(m_hBitMap);
}

void CStamp::Draw(HDC _hdc)
{
	BITMAP bitmap;
	GetObject(m_hBitMap, sizeof(BITMAP), &bitmap);

	// Create device context to load bitmap into
	HDC hdcBitmap = CreateCompatibleDC(_hdc);

	// Select bitmap into device context
	HBITMAP hOldBitmap = static_cast<HBITMAP>(SelectObject(hdcBitmap, m_hBitMap));

	// Bit blip bitmap device context to main device context
	SetStretchBltMode(_hdc, HALFTONE);
	StretchBlt(_hdc, m_iStartX, m_iStartY, m_iWidth, m_iHeight, hdcBitmap, 0, 0, bitmap.bmWidth, bitmap.bmHeight, SRCCOPY);

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

void CStamp::SetWidth(int _iWidth)
{
	m_iWidth = _iWidth;
	m_iEndX = m_iStartX + _iWidth;
}

void CStamp::SetHeight(int _iHeight)
{
	m_iHeight = _iHeight;
	m_iEndY = m_iStartY + _iHeight;
}

