// The MIT License (MIT)

// Copyright (c) 2013 Richard Slater

// Permission is hereby granted, free of charge, to any person obtaining a copy of
// this software and associated documentation files (the "Software"), to deal in
// the Software without restriction, including without limitation the rights to
// use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
// the Software, and to permit persons to whom the Software is furnished to do so,
// subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
// FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
// IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#ifndef SLOG_H_INCLUDED
#define SLOG_H_INCLUDED

#include <string>
#include <sstream>
#include <fstream>
#include <mutex>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>

namespace slog {

static const std::string format_date_plus_time = "%Y-%m-%d_%H-%M-%S";
static const std::string format_time_only = "[%H:%M:%S] ";

/// uses the C library function "strftime" standard format
/// @param[in] format string specifiers to format the timestamp
std::string create_timestamp( const std::string& format ) {
	if ( format.empty() ) {
		return "";
	}
	std::time_t now_c = std::chrono::system_clock::to_time_t( std::chrono::system_clock::now() );
	std::stringstream ss;
	ss << std::put_time( std::localtime( &now_c ), format.c_str() );
	return ss.str();
}

/// splits a file name into its base and extention
/// @param[in] file_name_in the original file name
/// @param[out] stripped_file_name_out the file name minus its extention
/// @param[out] extention_out the file extention
void remove_extention(
	const std::string& file_name_in,
	std::string& stripped_file_name_out,
	std::string& extention_out ) {

	size_t period_pos = file_name_in.find_last_of( "." );
	if ( period_pos != std::string::npos ) {
		extention_out = file_name_in.substr( period_pos );
	}
	stripped_file_name_out = file_name_in.substr( 0, period_pos );
}

/// make a binary copy of one file to a new filename
/// @param[in] source_file_name the source file name
/// @param[in] destination_file_name the destination file name
void copy_file( const std::string& source_file_name, const std::string& destination_file_name ) {
	std::ifstream source( source_file_name.c_str(), std::ios::binary );
	std::ofstream destination( destination_file_name.c_str(), std::ios::binary );
	destination << source.rdbuf();
}

/// provides synchronization, file access, and timestamp mechanisms for the message class
class logger {
public:

	/// constructor
	/// @param[in] log_file_name the log file name
	logger( const std::string& log_file_name = "slog.txt" )
		: m_is_initialized( false )
		, m_is_archiving_enabled( false )
		, m_timestamp_format( "" )
		, m_log_file_name( log_file_name ) {
	}

	/// destructor
	~logger() {
		if ( m_is_archiving_enabled && m_log_file.is_open() ) {
			m_log_file.close();
			std::string archive_file_name;
			std::string extention;
			remove_extention( m_log_file_name, archive_file_name, extention );
			archive_file_name += "_" + create_timestamp( format_date_plus_time ) + extention;
			copy_file( m_log_file_name, archive_file_name );
		}
	}

	/// open a log file for output
	/// @param[in] log_file_name a full file path (e.g. "c:/test/superlog.txt" or "funlog.txt" )
	void init( const std::string& log_file_name ) {
		m_log_file_name = log_file_name;
		m_log_file.open( m_log_file_name.c_str(), std::ios::trunc | std::ios::out );
		m_is_initialized = true;
	}

	/// @param[in] stream a stringstream containing the message to log
	void capture( std::ostringstream& stream ) {
		std::string timestamp = create_timestamp( m_timestamp_format );
		std::lock_guard<std::mutex> lock( m_log_mutex );
		if ( !m_is_initialized ) {
			init( m_log_file_name );
		}
		if ( m_log_file.good() ) {
			m_log_file << timestamp << stream.str() << "\n";
		}
	}
	
	void set_timestamp_format( const std::string& timestamp_format ) {
		m_timestamp_format = timestamp_format;
	}

	void set_archiving( bool is_archiving_enabled = true ) {
		m_is_archiving_enabled = is_archiving_enabled;
	}

private:
	bool m_is_initialized;
	bool m_is_archiving_enabled;
	std::ofstream m_log_file;
	std::mutex m_log_mutex;
	std::string m_timestamp_format;
	std::string m_log_file_name;
};

/// acts as a stream to capture a log message
class message {
public:

	/// constructor
	/// @param[in] owner the logger object which will handle the file i/o
	message( logger* owner )
		: m_owner( owner ) {
	}

	/// move constructor
	/// @param[in] other the right-hand-side message object to cannibalize
	message( message&& other )
		: m_owner( other.m_owner )
		, m_stream( other.m_stream.str() ) {
	}

	/// destructor which handles the stream capture
	~message() {
		m_owner->capture( m_stream );
	}

	/// @param[in] value the value to copy into this message
	/// @returns a stream object necessary for statement chains
	template <typename T>
	std::ostringstream& operator<<( const T& value ) {
		m_stream << value;
		return m_stream;
	}
	
private:
	logger* m_owner;
	std::ostringstream m_stream;
};

/// exists only to encapsulate a nasty global
class global_logger {
public:

	/// singleton accessor
	/// @returns a reference to the one-and-only global logger instance
	static logger& get() {
		static logger singleton;
		return singleton;
	}
};

/// primary function which slog is built around, use this to log your messages
/// @param[in] the_logger logger object to bind this single function call
/// @returns a message object to capture a stream log message
message out( logger& the_logger = global_logger::get() ) {
	return message( &the_logger );
}

/// initialize the logging system by setting a output file path
/// @param[in] log_file_name a full file path (e.g. "c:/test/superlog.txt" or "funlog.txt" )
/// @param[in] the_logger logger object to initialize
void init( const std::string& log_file_name, logger& the_logger = global_logger::get() ) {
	the_logger.init( log_file_name );
}

/// uses the C library function "strftime" standard format
/// @param[in] format string specifiers to format the timestamp
/// @param[in[ the_logger the logger object which to set
void set_timestamp_format( const std::string& format = format_time_only, logger& the_logger = global_logger::get() ) {
	the_logger.set_timestamp_format( format );
}

/// if enabled, each time the application closes the log file will be saved with a timestamp
/// @param[in] is_archiving_enabled set to true to enable archiving
/// @param[in[ the_logger the logger object which to set
void set_archiving( bool is_archiving_enabled = true, logger& the_logger = global_logger::get() ) {
	the_logger.set_archiving( is_archiving_enabled );
}

/// ever wanted to redirect std::cout to a log file? well this is the class for you.
class snooper : public std::streambuf {
public:

	/// constructor
	/// @param[in] stream_to_watch the stream object to capture, for example std::cout
	/// @param[in] log_file_name a full file path, e.g. "cout.txt" or "sneaky.log"
	snooper( std::ostream& stream_to_watch, const std::string& log_file_name )
		: m_stream_to_watch( stream_to_watch.rdbuf() )
		, m_log_file( log_file_name.c_str(), std::ios_base::out | std::ios_base::trunc )
		, m_owner( nullptr )
		, m_is_good( false ) {

		if ( m_log_file.is_open() ) {
			m_is_good = true;
			stream_to_watch.rdbuf( this );
			m_owner = &stream_to_watch;
		}
	}
	
	/// destructor
	~snooper() {
		if ( m_owner != nullptr ) {
			m_owner->rdbuf( m_stream_to_watch );
		}
	}

protected:

	/// override of base class virtual function, this is where the stream is watched
	/// @param[in] character a single character "to be put"
	/// @returns the input character if successful
	int overflow( int character ) {
		if ( m_is_good ) {
			m_log_file.rdbuf()->sputc( character );
		}
		return m_stream_to_watch->sputc( character );
	}

private:
	std::streambuf* m_stream_to_watch;
	std::ofstream m_log_file;
	std::ostream* m_owner;
	bool m_is_good;
};

} // end of namespace slog

#endif // SLOG_H_INCLUDED
