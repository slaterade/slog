// Just some test cases for slog... probably should make a few more.

#include "slog.h"
#include <iostream>

int main( int argc, char* argv[] ) {

	slog::snooper snoop( std::cout, "cout-intercept-log.txt" );
	std::cout << "Hello from slog. Your friendly neighborhood simple logger.";

	slog::init();
	slog::out() << "Using the log_splitter object directly is probably not what you want " << 42 << ", alue";
	slog::out() << "Instead, use this handy global log function" << std::endl;

	slog::logger test_logger;
	test_logger.init( "test_logger.txt" );
	slog::out( test_logger ) << "blah";

	return 0;
}
