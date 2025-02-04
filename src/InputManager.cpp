/// InputManager was taken from https://github.com/alexdantas/sdl2-platformer
/// This file is under GNU General Public License (see here https://github.com/alexdantas/sdl2-platformer#license)

#include "InputManager.h"

core::InputManager* core::InputManager::instance = nullptr;

core::InputManager::InputManager() :
	keyboard(nullptr),
	mouse(0),
	mouseX(0),
	mouseY(0),
	will_quit(false),
	isLocked(false)
{ }
core::InputManager* core::InputManager::getInstance()
{
	if (!instance)
		instance = new core::InputManager();

	return instance;
}
void core::InputManager::update(int cameraX, int cameraY)
{
	int i;
	for (i = 0; i < KEYBOARD_SIZE; i++)
	{
		this->keyDown[i] = false;
		this->keyUp[i] = false;
	}
	for (i = 0; i < MOUSE_MAX; i++)
	{
		this->mouseDown[i] = false;
		this->mouseUp[i] = false;
	}
	curPrintableKey = 0;

	// Get key events from the OS
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			this->will_quit = true;
			break;

			// SDL2's new way of handling input
		case SDL_TEXTINPUT:
			// WHAT
			break;

		case SDL_KEYDOWN:
		{
			this->keyboard = SDL_GetKeyboardState(nullptr);

			int index = event.key.keysym.scancode;

			this->keyDown[index] = true;

			if (core::InputManager::isPrintable(event.key.keysym.sym))
				this->curPrintableKey = event.key.keysym.sym;
		}
		break;

		case SDL_KEYUP:
		{
			this->keyboard = SDL_GetKeyboardState(nullptr);

			int index = event.key.keysym.scancode;
			this->keyUp[index] = true;
		}
		break;

		case SDL_MOUSEMOTION:
			this->mouseX = event.motion.x + cameraX;
			this->mouseY = event.motion.y + cameraY;
			break;

		case SDL_MOUSEBUTTONDOWN:
			this->mouse = SDL_GetMouseState(&(this->mouseX),
				&(this->mouseY));

			if (event.button.button == SDL_BUTTON_LEFT)
				this->mouseDown[MOUSE_LEFT] = true;

			else if (event.button.button == SDL_BUTTON_RIGHT)
				this->mouseDown[MOUSE_RIGHT] = true;
			break;

		case SDL_MOUSEBUTTONUP:
			this->mouse = SDL_GetMouseState(&(this->mouseX),
				&(this->mouseY));

			if (event.button.button == SDL_BUTTON_LEFT)
				this->mouseUp[MOUSE_LEFT] = true;

			else if (event.button.button == SDL_BUTTON_RIGHT)
				this->mouseUp[MOUSE_RIGHT] = true;
			break;

			// Brand new SDL2 event.
		case SDL_MOUSEWHEEL:
			// event.x; // Ammount scrolled horizontally
			// // If negative, scrolled to the right
			// // If positive, scrolled to the left

			// event.y; // Ammount scrolled vertically
			// // If negative, scrolled down
			// // If positive, scrolled up
			break;

		default:
			break;
		}
	}
}
bool core::InputManager::isKeyDown(int key)
{
	if (this->isLocked) return false;

	if (key < 0 || key >= KEYBOARD_SIZE)
		return false;

	return (this->keyDown[key]);
}
bool core::InputManager::isKeyUp(int key)
{
	if (this->isLocked) return false;

	if (key < 0 || key >= KEYBOARD_SIZE)
		return false;

	return (this->keyUp[key]);
}
bool core::InputManager::isKeyPressed(KeyboardKey key)
{
	if (this->isLocked) return false;

	if (!(this->keyboard))
		return false;

	int sdl_key = static_cast<int>(key);

	if (this->keyboard[sdl_key])
		return true;

	return false;
}
bool core::InputManager::shift()
{
	return (instance->isKeyPressed(KEY_LEFT_SHIFT) ||
		instance->isKeyPressed(KEY_RIGHT_SHIFT));
}
bool core::InputManager::ctrl()
{
	return (instance->isKeyPressed(KEY_LEFT_CTRL) ||
		instance->isKeyPressed(KEY_RIGHT_CTRL));
}
bool core::InputManager::alt()
{
	return (instance->isKeyPressed(KEY_LEFT_ALT) ||
		instance->isKeyPressed(KEY_RIGHT_ALT));
}
bool core::InputManager::isMouseDown(MouseButton button)
{
	if (this->isLocked) return false;

	if (button == MOUSE_MAX)
		return false;

	return this->mouseDown[button];
}
bool core::InputManager::isMouseUp(MouseButton button)
{
	if (this->isLocked) return false;

	if (button == MOUSE_MAX)
		return false;

	return this->mouseUp[button];
}
bool core::InputManager::isMousePressed(MouseButton button)
{
	if (this->isLocked) return false;

	switch (button)
	{
	case MOUSE_LEFT:
		if (this->mouse & SDL_BUTTON(1))
			return true;
		break;

	case MOUSE_RIGHT:
		if (this->mouse & SDL_BUTTON(3))
			return true;
		break;

	default:
		break;
	}

	return false;
}
int core::InputManager::getMouseX()
{
	return this->mouseX;
}
int core::InputManager::getMouseY()
{
	return this->mouseY;
}
bool core::InputManager::quitRequested()
{
	return (this->will_quit);
}
bool core::InputManager::isMouseInside(SDL_Rect rectangle)
{
	return (this->mouseX >= rectangle.x) && (this->mouseX <= rectangle.x + rectangle.w) && (this->mouseY >= rectangle.y)
		&& (this->mouseY <= rectangle.y + rectangle.h);
}
bool core::InputManager::isPrintable(SDL_Keycode key)
{
	// We test `key` on SDL's internal values for keys
	//
	// To get a list of them, go here:
	// http://wiki.libsdl.org/SDLKeycodeLookup

	return ((key > SDLK_SPACE) && (key < SDLK_z));
}
bool core::InputManager::isPrintableKeyDown()
{
	return (core::InputManager::isPrintable(this->curPrintableKey));
}
std::string core::InputManager::getCurPrintableKey()
{
	char c = (char)(this->curPrintableKey);
	const char* a = &c;
	return (std::string(a));
}
void core::InputManager::lock()
{
	this->isLocked = true;
}
void core::InputManager::unlock()
{
	this->isLocked = false;
}
