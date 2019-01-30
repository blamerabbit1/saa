#include "Console.h"


// Constructor
Console::Console() {
	// Get console handlers
	hIn = GetStdHandle(STD_INPUT_HANDLE);
	hOut = GetStdHandle(STD_OUTPUT_HANDLE);

	// Get information about console(flags)
	GetConsoleMode(hIn, &dwConsoleMode);

	// Little annoying stuff => disable QuickEdit
	// it's blocks mouse event in Win10
	dwConsoleMode |= ENABLE_MOUSE_INPUT;
	dwConsoleMode &= ~ENABLE_QUICK_EDIT_MODE;
	// Save changes 
	SetConsoleMode(hIn, dwConsoleMode | ENABLE_EXTENDED_FLAGS);

	// Disable cursor, we don't need it
	// looks better without it
	ChangeCursorVisibility();

	UserMouse = Mouse();
	LastMouse = Mouse();
}

// Enable/Disable cursor visibility in CMD
void Console::ChangeCursorVisibility(bool disable)
{
	GetConsoleCursorInfo(hOut, &hConsoleCursorInfo);

	hConsoleCursorInfo.bVisible = hConsoleCursorInfo.bVisible & !disable;
	SetConsoleCursorInfo(hOut, &hConsoleCursorInfo);

}

// Waiting for user move
void Console::WaitForClick() {
	DWORD NumRead{};

	while (true){
		ReadConsoleInput(hIn,& input, 1, &NumRead);

		switch (input.EventType){
		case MOUSE_EVENT:
			// Set value only when left/right button was clicked
			if (input.Event.MouseEvent.dwButtonState & (FROM_LEFT_1ST_BUTTON_PRESSED | RIGHTMOST_BUTTON_PRESSED)) {
				if (LastMouse.dwButtonState != input.Event.MouseEvent.dwButtonState &&
					(LastMouse.crdPosition.X != input.Event.MouseEvent.dwMousePosition.X ||
						LastMouse.crdPosition.Y != input.Event.MouseEvent.dwMousePosition.Y)) {
					// Position
					UserMouse.crdPosition = input.Event.MouseEvent.dwMousePosition;
					// Buttons clicked
					UserMouse.dwButtonState = input.Event.MouseEvent.dwButtonState;

					LastMouse = UserMouse;
					// Set cursor position at same as mouse
					SetConsoleCursorPosition(hOut, UserMouse.crdPosition);
					return;
				}
			}
		}
	}
}