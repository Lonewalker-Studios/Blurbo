#include "SDL_Wrappers.h"
#include <iostream>
namespace blurbo_utilities {
	void SDL_Destroyer::operator()(SDL_Window* window) const
	{
		SDL_DestroyWindow(window);
		std::cout << "window destroyed" << std::endl;
	}

	void SDL_Destroyer::operator()(SDL_Gamepad* controller) const
	{
		SDL_CloseGamepad(controller);
		std::cout << "controller closed" << std::endl;
	}

	void SDL_Destroyer::operator()(SDL_Cursor* cursor) const
	{
		SDL_DestroyCursor(cursor);
		std::cout << "cursor destroyed" << std::endl;   
	}

	SDL_GamepadPtr makeSharedController(SDL_Gamepad* controller)
	{
		return SDL_GamepadPtr(controller, SDL_Destroyer{});
	}

	SDL_CursorPtr makeSharedCursor(SDL_Cursor* cursor)
	{
		return SDL_CursorPtr(cursor, SDL_Destroyer{});
	}
}
