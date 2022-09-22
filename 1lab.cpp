#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <tchar.h>
#include <windowsx.h>
#include <gdiplus.h>
#include <wchar.h>
#include <stdio.h>

#define DRAWRECT 0
#define DRAWBIT 1

using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")


HBITMAP PngToBitmap(WCHAR* imagePath);

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

void drawImage(HDC hdc, int x, int y, HBITMAP hBitmap);


const int WINDOWHEIGHT = 600;
const int WINDOWWIDTH = 800;

static int bitWidth;
static int bitHeight;

WCHAR imagePath[] = L"bober.png";
//WCHAR imagePath[] = L"flag.png";

int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance, LPSTR IpCmdLine, int nCmdShow)
{
	WNDCLASSEX wcex; HWND hWnd; MSG msg;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_DBLCLKS;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = L"HelloWorldClass";
	wcex.hIconSm = wcex.hIcon;

	RegisterClassEx(&wcex);
	hWnd = CreateWindow(L"HelloWorldClass", L"Hello, World!",
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0,
		WINDOWWIDTH, WINDOWHEIGHT, NULL, NULL, hInstance, NULL);
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int)msg.wParam;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message,
	WPARAM wParam, LPARAM lParam)
{	
	PAINTSTRUCT ps;
	HDC hdc;
	static RECT userInnerWindow;
	static int id = DRAWBIT;
	//Color gdipColor(255, 0, 0, 255);
	//SetBkMode(hdc, TRANSPARENT);
	static const HBRUSH BACKGROUND_BRUSH = CreateSolidBrush(RGB(0,0,0));
	static const HBRUSH RECT_BRUSH = CreateSolidBrush(RGB(229, 255, 204));
	static const HPEN RECT_PEN = CreatePen(PS_SOLID, 1, RGB(0, 153, 0));

	

	static int x = 200;
	static int y = 200;
	//static int size = 10;
	static int speed = 10;

	static bool autoMoving = false;
	static int autoSpeedX = 10;
	static int autoSpeedY = 10;

	//static int bitWidth = 100;
	//static int bitHeight = 100;

	static HBRUSH hbrush;
	static HPEN hpen;
	static HBITMAP spriteBitmap;



	switch (message)
	{
	case WM_CREATE:
	{
		hbrush = RECT_BRUSH;
		hpen = RECT_PEN;
		spriteBitmap = PngToBitmap(imagePath);		 
		break;
	}
	case WM_PAINT:
	{
		hdc = BeginPaint(hWnd, &ps);
		drawImage(hdc, x, y, spriteBitmap);
		// draw rectangle 
		if (id == DRAWRECT) 
		{
			SelectObject(hdc, hbrush);
			SelectObject(hdc, hpen);
			Rectangle(hdc, x, y, x + bitWidth, y + bitHeight);
		}
		// draw image
		else {
			drawImage(hdc, x, y, spriteBitmap);
		}
		EndPaint(hWnd, &ps);
		//InvalidateRect(hWnd, NULL, TRUE);
		break;

	}
	case WM_TIMER:
	{
		if (autoMoving)
		{
			GetClientRect(hWnd, &userInnerWindow);

			if (x + autoSpeedX + bitWidth < userInnerWindow.right)
			{
				if (x + autoSpeedX > 0)
				{
					x = x + autoSpeedX;
				}
				else
				{
					x = x - autoSpeedX;
					autoSpeedX *= -1;
				}
			}
			else
			{
				x = x - autoSpeedX;
				autoSpeedX *= -1;
			}

			if (y + autoSpeedY + bitHeight < userInnerWindow.bottom)
			{
				if (y + autoSpeedY > 0)
				{
					y = y + autoSpeedY;
				}
				else
				{
					y = y - autoSpeedY;
					autoSpeedY *= -1;
				}
			}
			else
			{
				y = y - autoSpeedY;
				autoSpeedY *= -1;
			}
		}
		InvalidateRect(hWnd, NULL, TRUE);
		break;
	}
	case WM_MOUSEMOVE:
	{
		if (autoMoving)
			break;
		GetClientRect(hWnd, &userInnerWindow);
		int newX = LOWORD(lParam);
		int newY = HIWORD(lParam);
		

		//fprintf(myfile, "%d\t%d\r\n", newX, newY);

		if (newY <= 0) 
		{
			y = 0;
		}
		else
		{
			y = newY + bitHeight <= userInnerWindow.bottom ? newY : userInnerWindow.bottom -bitHeight;
		}
		if (newX <= 0)
		{
			x = 0;
		}
		else
		{
			x = newX + bitWidth <= userInnerWindow.right ? newX : userInnerWindow.right - bitWidth;
		}
		InvalidateRect(hWnd, NULL, TRUE);
		break;
	}
	case WM_MOUSEWHEEL:
	{
		int mouseDelta = GET_WHEEL_DELTA_WPARAM(wParam);
		if (LOWORD(wParam) == MK_SHIFT)
		{
			if (mouseDelta > 0)
			{
				x = x - speed >= 0 ? x - speed : 0;
			}
			else
			{
				x = x + bitWidth + speed <= userInnerWindow.right ? x + speed : userInnerWindow.right - bitWidth;
			}
		}
		else
		{
			if (mouseDelta > 0)
			{
				y = y - speed >= 0 ? y - speed : 0;
			}
			else
			{
				y = y + bitHeight + speed <= userInnerWindow.bottom ? y + speed : userInnerWindow.bottom - bitHeight;
			}
		}
		InvalidateRect(hWnd, NULL, TRUE);
		break;
	}
	case WM_CHAR:
	{
		autoMoving = false;
		KillTimer(hWnd, 1);
		GetClientRect(hWnd, &userInnerWindow);
		switch (wParam)
		{
			case 'W': {
				y = y- speed >=0 ? y- speed : 0;
				break;
			}
			case 'w': {
				y = y - speed >= 0 ? y - speed : 0;
				break;
			}
			case 'A': {
				x = x - speed >= 0 ? x - speed : 0;
				break;
			}
			case 'a': {
				x = x - speed >= 0 ? x - speed : 0;
				break;
			}
			case 'S': {
				y = y + bitHeight + speed <= userInnerWindow.bottom ? y + speed : userInnerWindow.bottom -bitHeight;
				break;
			}
			case 's': {
				y = y + bitHeight + speed <= userInnerWindow.bottom ? y + speed : userInnerWindow.bottom - bitHeight;
				break;
			}
			case 'D': {
				x = x + bitWidth + speed <= userInnerWindow.right ? x + speed : userInnerWindow.right - bitWidth;
				break;
			}
			case 'd': {
				x = x + bitWidth + speed <= userInnerWindow.right ? x + speed : userInnerWindow.right - bitWidth;
				break;
			}
			default:
				break;
		}
		InvalidateRect(hWnd, NULL, TRUE);
		break;
	}
	case WM_KEYDOWN:
	{
		autoMoving = false;
		KillTimer(hWnd, 1);
		GetClientRect(hWnd, &userInnerWindow);
		switch (wParam)
		{
			case VK_UP:
			{
				y = y - speed >= 0 ? y - speed : 0;
				break;
			}
			case VK_DOWN:
			{
				y = y + bitHeight + speed <= userInnerWindow.bottom ? y + speed : userInnerWindow.bottom - bitHeight;
				break;
			}
			case VK_LEFT:
			{
				x = x - speed >= 0 ? x - speed : 0;
				break;
			}
			case VK_RIGHT:
			{
				x = x + bitWidth + speed <= userInnerWindow.right ? x + speed : userInnerWindow.right - bitWidth;
				break;
			}
			default:
				break;
		}
		InvalidateRect(hWnd, NULL, TRUE);
		//InvalidateRect(hWnd, NULL, TRUE);
		break;
	}
	
	case WM_LBUTTONDOWN:
	{
		if (!autoMoving)
		{
			autoMoving = true;
			SetTimer(hWnd, 1, 25, NULL);
		}
		else
		{
			autoMoving = false;
			KillTimer(hWnd, 1);
		}	

		break;
	}

		break;
	case WM_DESTROY:
		if (autoMoving) 
		{
			KillTimer(hWnd, 1);
		}
		DeleteObject(hbrush);
		DeleteObject(hpen);
		DeleteObject(spriteBitmap);
		PostQuitMessage(0);
		break;	
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	
	return 0;
}


void drawImage(HDC hdc, int x, int y, HBITMAP hBitmap) {
	HBITMAP hNewBmp;
	HDC compDc;
	BITMAP bmp;
	int bmpWidth, bmpHeight;

	compDc = CreateCompatibleDC(hdc);
	hNewBmp = (HBITMAP)SelectObject(compDc, hBitmap);

	if (hNewBmp) {
		SetMapMode(compDc, GetMapMode(hdc));
		GetObject(hBitmap, sizeof(BITMAP), (LPSTR)&bmp);

		bmpWidth = bmp.bmWidth;
		bmpHeight = bmp.bmHeight;
		POINT bmpSize;
		bmpSize.x = bmpWidth;
		bmpSize.y = bmpHeight;

		DPtoLP(hdc, &bmpSize, 1);

		POINT point;
		point.x = 0;
		point.y = 0;

		DPtoLP(compDc, &point, 1);
		BitBlt(hdc, x, y, bmpWidth, bmpHeight, compDc, point.x, point.y, SRCCOPY);
		SelectObject(compDc, hNewBmp);
	}

	DeleteDC(compDc);
}

HBITMAP PngToBitmap(WCHAR* imagePath) {
	GdiplusStartupInput test;
	ULONG_PTR token;
	GdiplusStartup(&token, &test, NULL);
	Color backgroundImage = Color(Color::MakeARGB(0, 255,255,255));

	HBITMAP convertedBitmap = NULL;
	Bitmap* Bitmap = Bitmap::FromFile(imagePath, false);
	
	if (Bitmap) {
		bitWidth = Bitmap->GetWidth();
		bitHeight = Bitmap->GetHeight();
		Bitmap->GetHBITMAP(backgroundImage, &convertedBitmap);
		delete Bitmap;
	}
	GdiplusShutdown(token);
	return convertedBitmap;
}