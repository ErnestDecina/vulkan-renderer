#include <iostream>
#include "./application/hello_triangle_application/hello_triangle_application.h"

int main() 
{
    HelloTriangleApplication app = HelloTriangleApplication();

    try
    {
        app.run();
    } // End try
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    } // End catch

    std::cout << "Cleaning project" << std::endl;
    app.~HelloTriangleApplication();

    return EXIT_SUCCESS;
}