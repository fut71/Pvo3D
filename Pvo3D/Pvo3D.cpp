// Pvo3D.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "framework.h"
#include "resource.h"
#include "DxRenderer.h"

// 라이브러리 링크
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

HINSTANCE GInstance = nullptr;
HWND GWindowHandle = nullptr;

DxRenderer* GRenderer = nullptr;

// 윈도우 프로시저 선언
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

// 어플리케이션 시작점
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine,
	_In_ int nCmdShow)
{
	// 윈도우 클래스 등록
	GInstance = hInstance;

	WNDCLASSEXW wcex = {};
	wcex.cbSize = sizeof(WNDCLASSEXW);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.hInstance = hInstance;
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
 	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	wcex.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	wcex.lpszClassName = L"Pvo3DWindowClass";

	RegisterClassExW(&wcex);

	// 윈도우 생성
	GWindowHandle = CreateWindowW(
		L"Pvo3DWindowClass",
		L"Pvo3D",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		WINDOW_WIDTH, WINDOW_HEIGHT,
		nullptr, nullptr,
		hInstance, nullptr);

	if(!GWindowHandle)
	{
		return -1;
	}

	ShowWindow(GWindowHandle, nCmdShow);

	// 렌더러 생성, 초기화
	GRenderer = new DxRenderer();
	if(!GRenderer->Initialize(GWindowHandle, WINDOW_WIDTH, WINDOW_HEIGHT))
	{
		delete GRenderer;
		return -1;
	}

	// 메시지 루프
	MSG msg = {};
	while(true)
	{
		if(PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			if(msg.message == WM_QUIT)
				break;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			// 렌더러 루프
			GRenderer->BeginRender();
			GRenderer->Render();
			GRenderer->EndRender();
		}
	}

	// 렌더러 정리, 할당 해제
	GRenderer->Finalize();
	delete GRenderer;

	return static_cast<int>(msg.wParam);
}

// 윈도우 메시지 프로시저
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case WM_DESTROY:
		// 종료 메시지 처리
		PostQuitMessage(0);
		break;
	case WM_SIZE:
		// 윈도우 리사이즈
		if(GRenderer && GRenderer->GetDevice())
		{
			const uint32_t width = LOWORD(lParam);
			const uint32_t height = HIWORD(lParam);
			GRenderer->Resize(width, height);
		}
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
