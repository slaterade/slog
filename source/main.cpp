// Just some test cases for slog... probably should make a few more.

#include "slog.h"
#include <iostream>

int main( int argc, char* argv[] ) {

	slog::log_splitter splitter( std::cout, "cout-intercept-log.txt" );
	slog::out( "global_out_log.txt", true );

	std::cout << "Hello from slog. Your friendly neighborhood simple logger.";

	slog::out() << "Using the log_splitter object directly is probably not what you want";
	slog::out() << "Instead, use this handy thread-safe global log function";

	return 0;
}