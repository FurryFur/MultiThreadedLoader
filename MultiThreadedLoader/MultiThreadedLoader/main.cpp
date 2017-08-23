
#include <Windows.h>
#include <vector>
#include <array>
#include <string>
#include <thread>
#include <algorithm>
#include <functional>
#include <memory>
#include <iostream>
#include <chrono>
#include "resource.h"
#include "util.h"
#include "stamp.h"
#include "backBuffer.h"

typedef std::chrono::high_resolution_clock Clock;

#define WINDOW_CLASS_NAME L"MultiThreaded Loader Tool"
const unsigned int _kuiWINDOWWIDTH = 3000;
const unsigned int _kuiWINDOWHEIGHT = 900;
#define MAX_FILES_TO_OPEN 100
#define MAX_CHARACTERS_IN_FILENAME 100
const size_t g_kNumThreads = 4;

//Global Variables
std::vector<std::wstring> g_vecImageFileNames;
std::vector<std::wstring> g_vecSoundFileNames;
std::vector<std::unique_ptr<CStamp>> g_vecpStamps;
HINSTANCE g_hInstance;
bool g_bIsFileLoaded = false;

bool ChooseImageFilesToLoad(HWND _hwnd)
{
	OPENFILENAME ofn;
	SecureZeroMemory(&ofn, sizeof(OPENFILENAME)); // Better to use than ZeroMemory
	wchar_t wsFileNames[MAX_FILES_TO_OPEN * MAX_CHARACTERS_IN_FILENAME + MAX_PATH]; //The string to store all the filenames selected in one buffer togther with the complete path name.
	wchar_t _wsPathName[MAX_PATH + 1];
	wchar_t _wstempFile[MAX_PATH + MAX_CHARACTERS_IN_FILENAME]; //Assuming that the filename is not more than 20 characters
	wchar_t _wsFileToOpen[MAX_PATH + MAX_CHARACTERS_IN_FILENAME];
	ZeroMemory(wsFileNames, sizeof(wsFileNames));
	ZeroMemory(_wsPathName, sizeof(_wsPathName));
	ZeroMemory(_wstempFile, sizeof(_wstempFile));

	//Fill out the fields of the structure
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = _hwnd;
	ofn.lpstrFile = wsFileNames;
	ofn.nMaxFile = MAX_FILES_TO_OPEN * MAX_CHARACTERS_IN_FILENAME + MAX_PATH;  //The size, in charactesr of the buffer pointed to by lpstrFile. The buffer must be atleast 256(MAX_PATH) characters long; otherwise GetOpenFileName and 
													   //GetSaveFileName functions return False
													   // Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
													   // use the contents of wsFileNames to initialize itself.
	ofn.lpstrFile[0] = '\0';
	ofn.lpstrFilter = L"Bitmap Images(.bmp)\0*.bmp\0"; //Filter for bitmap images
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT;

	//If the user makes a selection from the  open dialog box, the API call returns a non-zero value
	if (GetOpenFileName(&ofn) != 0) //user made a selection and pressed the OK button
	{
		//Extract the path name from the wide string -  two ways of doing it
		//First way: just work with wide char arrays
		wcsncpy_s(_wsPathName, wsFileNames, ofn.nFileOffset);
		int i = ofn.nFileOffset;
		int j = 0;

		while (true)
		{
			if (*(wsFileNames + i) == '\0')
			{
				_wstempFile[j] = *(wsFileNames + i);
				wcscpy_s(_wsFileToOpen, _wsPathName);
				wcscat_s(_wsFileToOpen, L"\\");
				wcscat_s(_wsFileToOpen, _wstempFile);
				g_vecImageFileNames.push_back(_wsFileToOpen);
				j = 0;
			}
			else
			{
				_wstempFile[j] = *(wsFileNames + i);
				j++;
			}
			if (*(wsFileNames + i) == '\0' && *(wsFileNames + i + 1) == '\0')
			{
				break;
			}
			else
			{
				i++;
			}

		}

		g_bIsFileLoaded = true;
		return true;
	}
	else // user pressed the cancel button or closed the dialog box or an error occured
	{
		return false;
	}

}

bool ChooseSoundFilesToLoad(HWND _hwnd)
{
	OPENFILENAME ofn;
	SecureZeroMemory(&ofn, sizeof(OPENFILENAME)); // Better to use than ZeroMemory
	wchar_t wsFileNames[MAX_FILES_TO_OPEN * MAX_CHARACTERS_IN_FILENAME + MAX_PATH]; //The string to store all the filenames selected in one buffer togther with the complete path name.
	wchar_t _wsPathName[MAX_PATH + 1];
	wchar_t _wstempFile[MAX_PATH + MAX_CHARACTERS_IN_FILENAME]; //Assuming that the filename is not more than 20 characters
	wchar_t _wsFileToOpen[MAX_PATH + MAX_CHARACTERS_IN_FILENAME];
	ZeroMemory(wsFileNames, sizeof(wsFileNames));
	ZeroMemory(_wsPathName, sizeof(_wsPathName));
	ZeroMemory(_wstempFile, sizeof(_wstempFile));

	//Fill out the fields of the structure
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = _hwnd;
	ofn.lpstrFile = wsFileNames;
	ofn.nMaxFile = MAX_FILES_TO_OPEN * 20 + MAX_PATH;  //The size, in charactesr of the buffer pointed to by lpstrFile. The buffer must be atleast 256(MAX_PATH) characters long; otherwise GetOpenFileName and 
													   //GetSaveFileName functions return False
													   // Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
													   // use the contents of wsFileNames to initialize itself.
	ofn.lpstrFile[0] = '\0';
	ofn.lpstrFilter = L"Wave Files (*.wav)\0*.wav\0All Files (*.*)\0*.*\0"; //Filter for wav files
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT;

	//If the user makes a selection from the  open dialog box, the API call returns a non-zero value
	if (GetOpenFileName(&ofn) != 0) //user made a selection and pressed the OK button
	{
		//Extract the path name from the wide string -  two ways of doing it
		//Second way: work with wide strings and a char pointer 

		std::wstring _wstrPathName = ofn.lpstrFile;

		_wstrPathName.resize(ofn.nFileOffset, '\\');

		wchar_t *_pwcharNextFile = &ofn.lpstrFile[ofn.nFileOffset];

		while (*_pwcharNextFile)
		{
			std::wstring _wstrFileName = _wstrPathName + _pwcharNextFile;

			g_vecSoundFileNames.push_back(_wstrFileName);

			_pwcharNextFile += lstrlenW(_pwcharNextFile) + 1;
		}

		g_bIsFileLoaded = true;
		return true;
	}
	else // user pressed the cancel button or closed the dialog box or an error occured
	{
		return false;
	}

}

LRESULT CALLBACK WindowProc(HWND _hwnd, UINT _uiMsg, WPARAM _wparam, LPARAM _lparam)
{
	RECT rc;
	static CBackBuffer s_backbuffer;
	std::array<std::thread, g_kNumThreads> threads;

	switch (_uiMsg)
	{
	case WM_CREATE:
	{
		GetClientRect(_hwnd, &rc);

		s_backbuffer.Initialise(_hwnd, rc.right - rc.left, rc.bottom - rc.top);
	}
	case WM_KEYDOWN:
	{
		switch (_wparam)
		{
		case VK_ESCAPE:
		{
			SendMessage(_hwnd, WM_CLOSE, 0, 0);
			return(0);
		}
		break;
		default:
			break;
		}
	}
	break;
	case WM_PAINT:
	{
		auto t1 = Clock::now();

		// Clear background
		s_backbuffer.Clear();

		// Create drawing threads
		size_t stride = (g_vecpStamps.size() < g_kNumThreads) ? 1 : (g_vecpStamps.size() / g_kNumThreads);
		for (size_t i = 0; i < g_vecpStamps.size(); i += stride)
		{
			// Check if this is the last thread to be dispatched
			size_t threadIdx = i / stride;
			bool lastThread = (threadIdx == (g_kNumThreads - 1)) ? true : false;

			// Create thread / delegate work
			threads.at(threadIdx) = std::thread{ [i, stride, lastThread]()
			{
				for (size_t j = i; (j < (i + stride) || lastThread) && j < g_vecpStamps.size(); ++j)
				{
					g_vecpStamps[j]->Draw(s_backbuffer.GetBFDC());
				}
			} };

			// Exit loop if last thread has been dispatched
			if (lastThread)
			{
				break;
			}
		}

		// Wait for draw threads to complete
		for_each(threads.begin(), threads.end(), [](std::thread& thread) { if (thread.joinable()) thread.join(); });

		// Draw to window
		s_backbuffer.Present();

		auto t2 = Clock::now();
		auto dt = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
		OutputDebugString((L"Image Draw Time: " + ToWString(dt) + L"ms\n").c_str());
		
		return (0);
	}
	break;
	case WM_COMMAND:
	{
		switch (LOWORD(_wparam))
		{


		case ID_FILE_LOADIMAGE:
		{
			if (ChooseImageFilesToLoad(_hwnd))
			{
				auto t1 = Clock::now();

				g_vecpStamps.clear();
				if (g_vecpStamps.capacity() < g_vecImageFileNames.size())
				{
					g_vecpStamps.reserve(g_vecImageFileNames.size());
				}

				std::mutex mutex;
				size_t stride = (g_vecImageFileNames.size() < g_kNumThreads) ? 1 : (g_vecImageFileNames.size() / g_kNumThreads);
				for (int i = 0; i < g_vecImageFileNames.size(); i += stride)
				{
					// Check if this is the last thread to be dispatched
					size_t threadIdx = i / stride;
					bool lastThread = (threadIdx == (g_kNumThreads - 1)) ? true : false;

					// Create thread / delegate work
					threads.at(threadIdx) = std::thread([_hwnd, i, stride, lastThread, &mutex]() {
						for (size_t j = i; (j < (i + stride) || lastThread) && j < g_vecImageFileNames.size(); ++j)
						{
							auto pImage = std::make_unique<CStamp>(g_hInstance, g_vecImageFileNames[j], j * 100, 0);

							std::lock_guard<std::mutex> lock{ mutex };
							g_vecpStamps.push_back(std::move(pImage));
						}
					});
					
					// Exit loop if last thread has been dispatched
					if (lastThread)
					{
						break;
					}
				}
				for_each(threads.begin(), threads.end(), [](std::thread& thread) { if (thread.joinable()) thread.join(); });

				g_vecImageFileNames.clear();

				InvalidateRect(_hwnd, NULL, FALSE);

				auto t2 = Clock::now();
				auto dt = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
				OutputDebugString((L"Image Load Time: " + ToWString(dt) + L"ms\n").c_str());
			}
			else
			{
				MessageBox(_hwnd, L"No Image File selected", L"Error Message", MB_ICONWARNING);
			}

			return (0);
		}
		break;
		case ID_FILE_LOADSOUND:
		{
			if (ChooseSoundFilesToLoad(_hwnd))
			{
				std::vector<std::thread> soundThreads;
				for (size_t i = 0; i < g_vecSoundFileNames.size(); ++i)
				{
					soundThreads.emplace_back( [i]() {
						mciSendString((L"open " + g_vecSoundFileNames[i] + L" type waveaudio").c_str(), NULL, 0, 0);
						mciSendString((L"play " + g_vecSoundFileNames[i] + L" wait").c_str(), NULL, 0, 0);
						mciSendString((L"close " + g_vecSoundFileNames[i]).c_str(), NULL, 0, 0);
					} );
				}
				std::for_each(soundThreads.begin(), soundThreads.end(), std::mem_fn(&std::thread::join));
				g_vecSoundFileNames.clear();
			}
			else
			{
				MessageBox(_hwnd, L"No Sound File selected", L"Error Message", MB_ICONWARNING);
			}
			return (0);
		}
		break;
		case ID_EXIT:
		{
			SendMessage(_hwnd, WM_CLOSE, 0, 0);
			return (0);
		}
		break;
		default:
			break;
		}
	}
	break;
	case WM_CLOSE:
	{
		PostQuitMessage(0);
	}
	break;
	default:
		break;
	}
	return (DefWindowProc(_hwnd, _uiMsg, _wparam, _lparam));
}


HWND CreateAndRegisterWindow(HINSTANCE _hInstance)
{
	WNDCLASSEX winclass; // This will hold the class we create.
	HWND hwnd;           // Generic window handle.

						 // First fill in the window class structure.
	winclass.cbSize = sizeof(WNDCLASSEX);
	winclass.style = CS_DBLCLKS | CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	winclass.lpfnWndProc = WindowProc;
	winclass.cbClsExtra = 0;
	winclass.cbWndExtra = 0;
	winclass.hInstance = _hInstance;
	winclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	winclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	winclass.hbrBackground =
		static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
	winclass.lpszMenuName = NULL;
	winclass.lpszClassName = WINDOW_CLASS_NAME;
	winclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	// register the window class
	if (!RegisterClassEx(&winclass))
	{
		return (0);
	}

	HMENU _hMenu = LoadMenu(_hInstance, MAKEINTRESOURCE(IDR_MENU1));

	// create the window
	hwnd = CreateWindowEx(NULL, // Extended style.
		WINDOW_CLASS_NAME,      // Class.
		L"MultiThreaded Loader Tool",   // Title.
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		10, 10,                    // Initial x,y.
		_kuiWINDOWWIDTH, _kuiWINDOWHEIGHT,                // Initial width, height.
		NULL,                   // Handle to parent.
		_hMenu,                   // Handle to menu.
		_hInstance,             // Instance of this application.
		NULL);                  // Extra creation parameters.

	return hwnd;
}



int WINAPI WinMain(HINSTANCE _hInstance,
	HINSTANCE _hPrevInstance,
	LPSTR _lpCmdLine,
	int _nCmdShow)
{
	MSG msg;  //Generic Message

	HWND _hwnd = CreateAndRegisterWindow(_hInstance);

	if (!(_hwnd))
	{
		return (0);
	}


	// Enter main event loop
	while (true)
	{
		// Test if there is a message in queue, if so get it.
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			// Test if this is a quit.
			if (msg.message == WM_QUIT)
			{
				break;
			}

			// Translate any accelerator keys.
			TranslateMessage(&msg);
			// Send the message to the window proc.
			DispatchMessage(&msg);
		}

	}

	// Return to Windows like this...
	return (static_cast<int>(msg.wParam));
}