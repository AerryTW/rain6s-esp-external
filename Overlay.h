#pragma once
#include <dinput.h>
#include <tchar.h>
#include <Windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <Dwmapi.h> 
#include <TlHelp32.h>
#include <d3d9.h>
#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")

//dx11 render
#include "GUIRD/imgui.h"
#include "GUIRD/imgui_impl_win32.h"
#include "GUIRD/imgui_impl_dx11.h"
#include <d3d11.h>
#include <dwmapi.h>

#pragma comment(lib,"d3d11.lib")
#pragma comment(lib, "dwmapi.lib")



// Data
static ID3D11Device* g_pd3dDevice = NULL;
static ID3D11DeviceContext* g_pd3dDeviceContext = NULL;
static IDXGISwapChain* g_pSwapChain = NULL;
static ID3D11RenderTargetView* g_mainRenderTargetView = NULL;
ImVec4 clear_color = ImVec4(0.0f, 0.0f, 0.f, 0.f);
// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();

//aim information


class LH_Rd {

public:
	//Constructor
	LH_Rd();

	//Hard code cause we're lazy xDDDD
	int ScreenWidth = 1920;

	int ScreenHeight = 1080;

	LPDIRECT3D9 d3;
	LPDIRECT3DDEVICE9 d3dev;

	LPD3DXFONT d3Font;

	HWND ExEWindow;
	HWND GWindow;

	MARGINS margins = { 0, 0, ScreenWidth, ScreenHeight };

	ID3DXLine* d3Line;

	//Initializes our direct3Dx9 instance
	void InitD3D(HWND hWindow);

	//Renders our drawings to the screen
	void Render();

	//End
	void EndRender();
	//Sets up the overlay window
	void CreateExEWindow();

	//Main loop that calls the esp then renders it
	WPARAM Loop();

	//WPM
	void insane(ULONG64 fLocalPlayer);

	//Esp stuff
	void ESP();
	
	//bot
	void ShootTarget();
	//void set_viewAngle(ULONG64 entity, ULONG64 entity2, Vector3 angles, bool open);
	//aim math
	//Vector3 calculate_angle(Vector3 translation, Vector3 position);
	//Vector3 calculate_euler(Vector4 quaternion);
	//Vector3 CalcAngle(Vector3 enemypos, Vector3 camerapos);

	void DrawCircle(int X, int Y, int radius, int numSides, D3DCOLOR Color);

	void FillArea(float x, float y, float width, float height, D3DCOLOR color);

	void FillRectangle(double x, double y, double w, double h, D3DCOLOR color);

	void DrawBox(float x, float y, float width, float height, D3DCOLOR color);

	void DrawLine(float x1, float y1, float x2, float y2, D3DCOLOR color);

	void DrawString(int x, int y, D3DCOLOR color, LPD3DXFONT g_pFont, const char* fmt, ...);

};