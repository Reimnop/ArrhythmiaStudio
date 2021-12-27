#include "Arguments.h"
#include "helper.h"
#include "MainWindow.h"

#include "log4cxx/propertyconfigurator.h"

int main(int argc, char** argv)
{
	Arguments::parseArguments(argc, argv);

	log4cxx::PropertyConfigurator::configure("log4cxx.cfg");

	log4cxx::LoggerPtr logger = log4cxx::Logger::getLogger("Main");

#if NDEBUG
	try 
	{
#endif
		LOG4CXX_INFO(logger, STRINGIFY(Starting  PROJECT_NAME));

		MainWindow* window = new MainWindow();
		window->run();

		delete window;
#if NDEBUG
	}
	catch (const std::exception& e)
	{
		LOG4CXX_ERROR(logger, "An error has occured!");
		LOG4CXX_ERROR(logger, e.what());
	}
#endif
}
