#include <iostream>
#include "./window/glfw_window.h"

int main() {
    Window window = Window(500, 500, "Hello World");
    window.run();
    window.~Window();

    return 0;
}