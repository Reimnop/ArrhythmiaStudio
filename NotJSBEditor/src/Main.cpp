#include "MainWindow.h"

int main()
{
    MainWindow* window = new MainWindow();
    window->run();

    delete window;
}