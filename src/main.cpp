// Include SuperBLT
#include <superblt_flat.h>

// Include windows.h for whnd
#include <Windows.h>
#include <string>
#include <fstream>

using namespace std;

static int timer = 0;		// Delay the initialization by a little bit.
static int timerTarget = 10; // 10 frames.
static bool attemptTransform = false;

enum class TransformErrorCode
{
	SUCCESS,
	ERR_APP_FULLSCREEN,
	ERR_UNKNOWN
};

//==============================================
//	Returns true if the hwnd is fullscreen
//	https://stackoverflow.com/questions/7009080/detecting-full-screen-mode-in-windows
//==============================================
bool IsFullscreen(HWND& window)
{
	RECT a, b;
	GetWindowRect(window, &a);
	GetWindowRect(GetDesktopWindow(), &b);

	return (a.left == b.left &&
		a.top == b.top &&
		a.right == b.right &&
		a.bottom == b.bottom);
}

TransformErrorCode TransformWindow()
{
	// Get the window handle
	wstring windowName = L"PAYDAY 2";
	HWND wHandle = FindWindowW(NULL, windowName.c_str());

	// at this point we have both the hwnd and pid
	// see if that shit is not fullscreen, to only mess with the hwnd in that specific case
	if (!IsFullscreen(wHandle))
	{
		RECT desktop;

		const HWND hDesktop = GetDesktopWindow();
		GetWindowRect(hDesktop, &desktop);

		int WINDOW_WIDTH = desktop.right;
		int WINDOW_HEIGHT = desktop.bottom;

		string log = "Attempting to transform into a window of size: " + to_string(WINDOW_WIDTH) + "x" + to_string(WINDOW_HEIGHT);
		PD2HOOK_LOG_LOG(log.c_str());

		LONG lStyle = GetWindowLong(wHandle, GWL_STYLE);
		lStyle &= ~(WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU);
		PD2HOOK_LOG_LOG("SetWindowLong pass 1");
		SetWindowLong(wHandle, GWL_STYLE, lStyle);

		LONG lExStyle = GetWindowLong(wHandle, GWL_EXSTYLE);
		lExStyle &= ~(WS_EX_DLGMODALFRAME | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE);
		PD2HOOK_LOG_LOG("SetWindowLong pass 2");
		SetWindowLong(wHandle, GWL_EXSTYLE, lExStyle);

		PD2HOOK_LOG_LOG("Show window");

		ShowWindow(wHandle, SW_SHOW);
		PD2HOOK_LOG_LOG("Set window pos and size.");
		SetWindowPos(wHandle, NULL, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER);

		return TransformErrorCode::SUCCESS;
	}
	else
	{
		return TransformErrorCode::ERR_APP_FULLSCREEN;
	}

	return TransformErrorCode::ERR_UNKNOWN;
}

void Plugin_Init()
{
	PD2HOOK_LOG_LOG("Initializing ...");
}

void Plugin_Update()
{
	// Put your per-frame update code here

	if (timer > timerTarget && !attemptTransform)
	{
		auto t = TransformWindow();
		if (t != TransformErrorCode::SUCCESS)
		{
			// Failed? I need a output
			ofstream f;
			f.open("main_opt.log", ios::out);
			f << "Process failed with error code: " << static_cast<int>(t) << endl;
			f.close();

			PD2HOOK_LOG_LOG("Process failed with error code: " + static_cast<int>(t));
		}
		else
		{
			ofstream f;
			f.open("main_opt.log", ios::out);
			f << "Process succeeded!" << endl;
			f.close();
			PD2HOOK_LOG_LOG("Transform into borderless succeeded.");
		}

		attemptTransform = true;
	}

	if (!attemptTransform)
	{
		timer++;
	}
}

void Plugin_Setup_Lua(lua_State *L)
{
	// Deprecated, see this function's documentation (in superblt_flat.h) for more detail
}

int Plugin_PushLua(lua_State *L)
{
	return 0;
}