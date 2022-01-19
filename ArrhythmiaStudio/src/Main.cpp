#include "Arguments.h"
#include "helper.h"
#include "MainWindow.h"
#include "log4cxx/propertyconfigurator.h"

#include <memory>

int main(int argc, char** argv, char* environment[])
{
	Arguments::parseArguments(argc, argv);

	log4cxx::PropertyConfigurator::configure("log4cxx.cfg");

	log4cxx::LoggerPtr logger = log4cxx::Logger::getLogger("Main");

#if NDEBUG
	try 
	{
#endif
		LOG4CXX_INFO(logger, STRINGIFY(Starting PROJECT_NAME));

        std::unique_ptr<MainWindow> mainWindow = std::make_unique<MainWindow>();
        mainWindow->run();
#if NDEBUG
	}
	catch (const std::exception& e)
	{
		LOG4CXX_ERROR(logger, "An error has occured!");
		LOG4CXX_ERROR(logger, e.what());
	}
#endif
}
