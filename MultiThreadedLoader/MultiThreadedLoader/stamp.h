//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2017 Media Design School
//
// File Name	: stamp.h
// Description	: Header file for CStamp class
// Author		: Lance Chaney
// Mail			: lance.cha7337@mediadesign.school.nz
//

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

	// Draws this stamp
	void Draw(HDC _hdc);

	// Sets the x position of the left hand side of the stamp
	void SetStartX(int _iStartX);

	// Sets the y position of the top side of the stamp
	void SetStartY(int _iStartY);

	// Sets the width of the stamp
	void SetWidth(int _iWidth);

	// Sets the height of the stamp
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