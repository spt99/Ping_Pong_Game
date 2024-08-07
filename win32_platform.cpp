#include "utility.cpp"
#include <windows.h>


global_variable bool running = true;

struct Render_State {
	int height, width;
	void* memory;

	BITMAPINFO bitmap_info;
};

global_variable Render_State render_state;

#include "Renderer.cpp"
#include "platform_common.cpp"
#include "game.cpp"

LRESULT CALLBACK window_callback(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	LRESULT result = 0;
	switch (uMsg)
	{
	case WM_CLOSE:
	case WM_DESTROY: {
		running = false;
	}break;
	case WM_SIZE: {
		RECT rect;
		GetClientRect(hwnd, &rect);
		render_state.width = rect.right - rect.left;
		render_state.height = rect.bottom - rect.top;
		int size = render_state.width * render_state.height * sizeof(u32);
		if (render_state.memory) {
			VirtualFree(render_state.memory, 0, MEM_RELEASE);
		}
		render_state.memory = VirtualAlloc(0,size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		render_state.bitmap_info.bmiHeader.biSize = sizeof(render_state.bitmap_info.bmiHeader);
		render_state.bitmap_info.bmiHeader.biWidth = render_state.width;
		render_state.bitmap_info.bmiHeader.biHeight = render_state.height;
		render_state.bitmap_info.bmiHeader.biPlanes = 1;
		render_state.bitmap_info.bmiHeader.biBitCount = 32;
		render_state.bitmap_info.bmiHeader.biCompression = BI_RGB;

	}break;
	default: {
		result = DefWindowProc(hwnd, uMsg, wParam, lParam);
	}

	}
	return result;
}


int WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance, LPSTR     lpCmdLine, int       nShowCmd) {

	
	// create window class
	WNDCLASS window_class = {};
	window_class.style = CS_HREDRAW | CS_VREDRAW;
	window_class.lpszClassName = L"Game window";
	window_class.lpfnWndProc = window_callback;
	// register class
	RegisterClass(&window_class);
	// create window
	HWND window = CreateWindow(window_class.lpszClassName, L"My_First_Game", WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, 1280, 720, 0, 0, hInstance, 0);
	HDC hdc = GetDC(window);

	Input input = {};

	float delta_time = 0.000016666f;
	LARGE_INTEGER frame_begin_time;
	QueryPerformanceCounter(&frame_begin_time);

	float performance_frequency;
	{
		LARGE_INTEGER perf;
		QueryPerformanceCounter(&perf);
		performance_frequency = (float)perf.QuadPart;
	}

	while (running)
	{
		//input
		MSG message;
		for (int i = 0; i < BUTTON_COUNT; i++)
		{
			input.buttons[i].changed = false;
		}
		while (PeekMessage(&message, window, 0, 0, PM_REMOVE)) {
			switch (message.message)
			{
				case WM_KEYUP:
				case WM_KEYDOWN: {
					u32 vk_code = (u32)message.wParam;
					bool is_clicked = ((message.lParam & (1 << 31)) == 0);

#define process_button(b, vk)\
	case vk:{\
    input.buttons[b].changed = is_clicked != input.buttons[b].is_clicked;\
	input.buttons[b].is_clicked =  is_clicked;\
	}break;\

					switch (vk_code)
					{
						process_button(BUTTON_UP, VK_UP);
						process_button(BUTTON_DOWN, VK_DOWN);
						process_button(BUTTON_W, 'W');
						process_button(BUTTON_S, 'S');
						process_button(BUTTON_RIGHT, VK_RIGHT);
						process_button(BUTTON_LEFT, VK_LEFT);
						process_button(BUTTON_ENTER, VK_RETURN);
					}

				} break;

				default: {
					TranslateMessage(&message);
					DispatchMessage(&message);
				}
			}
			
		}
		//Simulate
		//*100000
		simulate_game(&input, delta_time);

		
		StretchDIBits(hdc, 0, 0, render_state.width, render_state.height, 0, 0, render_state.width, render_state.height,render_state.memory,&render_state.bitmap_info, DIB_RGB_COLORS,SRCCOPY);
		LARGE_INTEGER frame_end_time;
		QueryPerformanceCounter(&frame_end_time);
		delta_time = (float)(frame_end_time.QuadPart - frame_begin_time.QuadPart)/performance_frequency;
		frame_begin_time = frame_end_time;
	}

}