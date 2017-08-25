//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2017 Media Design School
//
// File Name	: backbuffer.cpp
// Description	: Implementation file for CBackBuffer class
// Author		: Lance Chaney
// Mail			: lance.cha7337@mediadesign.school.nz
//

#include "backBuffer.h"

CBackBuffer::CBackBuffer()
{
}

CBackBuffer::~CBackBuffer()
{
	SelectObject(m_BFDC, m_hOldBitmap);
	DeleteDC(m_BFDC);
}

bool CBackBuffer::Initialise(HWND _hWnd, int _iWidth, int _iHeight)
{
	m_hWnd = _hWnd;
	m_iWidth = _iWidth;
	m_iHeight = _iHeight;

	HDC hdc = GetDC(_hWnd);

	// Create back buffer device context
	m_BFDC = CreateCompatibleDC(hdc);

	// Create back buffer bitmap
	m_hBFBitmap = CreateCompatibleBitmap(hdc, _iWidth, _iHeight);

	// Select back buffer bitmap into back buffer device context
	m_hOldBitmap = static_cast<HBITMAP>(SelectObject(m_BFDC, m_hBFBitmap));

	ReleaseDC(_hWnd, hdc);

	return true;
}

HDC CBackBuffer::GetBFDC() const
{
	return m_BFDC;
}

int CBackBuffer::GetHeight() const
{
	return m_iHeight;
}

int CBackBuffer::GetWidth() const
{
	return m_iWidth;
}

HBITMAP CBackBuffer::GetHBitmap() const
{
	return m_hBFBitmap;
}

void CBackBuffer::Clear()
{
	RECT rect;
	rect.top = 0;
	rect.left = 0;
	rect.right = m_iWidth;
	rect.bottom = m_iHeight;

	HBRUSH hBrush = CreateSolidBrush(RGB(255,255,255));
	HBRUSH hOldBrush = static_cast<HBRUSH>(SelectObject(m_BFDC, hBrush));

	FillRect(m_BFDC, &rect, hBrush);

	SelectObject(m_BFDC, hOldBrush);
	DeleteObject(hBrush);
}

void CBackBuffer::Present()
{
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(m_hWnd, &ps);

	BitBlt(hdc, 0, 0, m_iWidth, m_iHeight, m_BFDC, 0, 0, SRCCOPY);

	EndPaint(m_hWnd, &ps);
}
