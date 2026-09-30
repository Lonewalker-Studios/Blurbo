#pragma once
#include <SDL3/SDL.h>
#include <memory>
namespace blurbo_utilities {
	struct SDL_Destroyer {
		void operator()(SDL_Window* window) const;
		void operator()(SDL_Gamepad* controller) const;
		void operator()(SDL_Cursor* cursor) const;
	};
	typedef std::shared_ptr<SDL_Gamepad> SDL_GamepadPtr;
	SDL_GamepadPtr makeSharedController(SDL_Gamepad* controller);
	typedef std::shared_ptr<SDL_Cursor> SDL_CursorPtr;
	SDL_CursorPtr makeSharedCursor(SDL_Cursor* cursor);
	typedef std::unique_ptr<SDL_Window, blurbo_utilities::SDL_Destroyer> SDL_WindowPtr;
}