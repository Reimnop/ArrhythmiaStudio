#include "MainWindow.h"

#include "logger.h"

int main()
{
	Logger::initLog();

#if NDEBUG
	try 
	{
#endif
		MainWindow* window = new MainWindow();
		window->run();

		delete window;
#if NDEBUG
	}
	catch (const std::exception& e)
	{
		Logger::error("An error has occurred!");
		Logger::error(e.what());
	}
#endif
}
