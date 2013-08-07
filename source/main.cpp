// Just some test cases for slog... probably should make a few more.

#include "slog.h"
#include <iostream>
#include <vector>
#include <thread>

int main( int argc, char* argv[] ) {

	// capture sneaky cout stuff
	slog::snooper snoop( std::cout, "cout-intercept-log.txt" );
	std::cout << "Hello from slog. Your friendly neighborhood simple logger." << std::endl;

	// basic interface
	slog::set_timestamp_format();
	slog::set_archiving();
	slog::out() << "Using the log_splitter object directly is probably not what you want " << 42 << ", alue";
	slog::out() << "Instead, use this handy global log function" << std::endl;

	// go crazy with some thread action
	std::vector<std::thread> threads;
	for ( auto i = 0; i < 50; ++i ) {
		std::thread t(
			[=] () {
				for ( int j = 0; j < 1000; ++j ) {
					slog::out() << "Hello from thread #" << i << " on loop #" << j;
					std::this_thread::sleep_for( std::chrono::milliseconds( 1 ) );
				}
			}
		);
		threads.push_back( std::move( t ) );
	}
	for ( auto& thread : threads ) {
		thread.join();
	}

	// test a non singleton logger here
	slog::logger test_logger;
	test_logger.init( "test_logger.txt" );
	test_logger.set_timestamp_format( slog::format_date_plus_time );
	slog::out( test_logger ) << "blah";

	return 0;
}
