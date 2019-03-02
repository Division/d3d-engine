#pragma once

#include <stdint.h>
#include <Windows.h>

class Input;

class Window {
public:
	Window(Input *input) : _input(input) {};
	~Window() = default;

	HWND initWindow(uint32_t width, uint32_t height, HINSTANCE hInstance);
	void processMessages();

	uint32_t width() const { return _width;  }
	uint32_t height() const { return _height; }

	bool quitTriggered() const { return _quitTriggered; }

private:
	Input *_input;
	bool _quitTriggered = false;
	uint32_t _width;
	uint32_t _height;
};