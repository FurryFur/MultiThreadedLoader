//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2017 Media Design School
//
// File Name	: backbuffer.h
// Description	: Header file for CBackBuffer class
// Author		: Lance Chaney
// Mail			: lance.cha7337@mediadesign.school.nz
//

#pragma once

#ifndef BACKBUFFER_H
#define BACKBUFFER_H

//Library Includes
#include <Windows.h>

class CBackBuffer
{
public:
	CBackBuffer();
	~CBackBuffer();
	CBackBuffer(const CBackBuffer&) = delete;
	CBackBuffer& operator = (const CBackBuffer&) = delete;

	// Initialize the backbuffer to the specified width and height
	bool Initialise(HWND _hWnd, int _iWidth, int _iHeight);

	// Gets the handle to the in memory device context associated with this backbuffer
	HDC GetBFDC() const;

	// Returns the height of the backbuffer in pixels
	int GetHeight() const;

	// Returns the width of the backbuffer in pixels
	int GetWidth() const;

	// Clears the backbuffer
	void Clear();

	// Presents the backbuffer to the display window
	void Present();

private:
	HWND m_hWnd;
	HDC m_BFDC;  //Handle to the in-memory DC for holding the backbuffer 
	HBITMAP m_hBFBitmap; //handle to the bitmap that represents the backbuffer
	HBITMAP m_hOldBitmap; //Used in select object to save the old bitmap.
	int m_iWidth;
	int m_iHeight;
};

#endif // BACKBUFFER_H