#define SDL_MAIN_HANDLED
#include "internal.h"

#include <stdlib.h>
#include <signal.h>

struct _engine_data __engine_data;

static void sig_handler(int signal) {}

int main()
{
	signal(SIGSEGV, sig_handler);
	atexit(__end_engine);

	__initialize_engine();
	__run_engine();
}

#ifdef _WIN32
#include <Windows.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	return main();
}
#endif