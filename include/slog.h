// Copyright (c) 2013, Richard Slater
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//     * Neither the name of the <organization> nor the
//       names of its contributors may be used to endorse or promote products
//       derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#ifndef SLOG_H_INCLUDED
#define SLOG_H_INCLUDED

#include <string>
#include <sstream>
#include <fstream>
#include <mutex>

namespace slog {

/// provides synchronization, file access, and timestamp mechanisms for the message class
class logger {
public:

	void init( const std::string& log_file_name ) {
		m_log_file.open( log_file_name.c_str(), std::ios::trunc | std::ios::out );
	}

	void capture( std::ostringstream& buf ) {
		std::string timestamp = "";
		if ( m_is_timestamp_enabled ) {
			timestamp = create_timestamp();
		}

		std::lock_guard<std::mutex> lock( m_log_mutex );
		if ( m_log_file.good() ) {
			m_log_file << timestamp << buf.str() << "\n";
		}
	}

	void enable_timestamp( bool is_timestamp_enabled = true ) {
		m_is_timestamp_enabled = is_timestamp_enabled;
	}

private:
	std::string create_timestamp() {
		return "[timestamp] ";
	}

	bool m_is_timestamp_enabled;
	std::ofstream m_log_file;
	std::mutex m_log_mutex;
};

class message {
public:
	message( logger* owner )
		: m_owner( owner ) {
	}

	message( message&& other )
		: m_owner( other.m_owner )
		, m_stream( std::move( other.m_stream ) ) {
	}

	~message() {
		m_owner->capture( m_stream );
	}

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

/// @param[in] the_logger logger object to bind this single function call
/// @returns a message object to capture a stream log message
message out( logger& the_logger = global_logger::get() ) {
	return message( &the_logger );
}

/// @param[in] log_file_name simple enough, a full file path (e.g. "c:/test/superlog.txt" or "funlog.txt" )
/// @param[in] the_logger logger object to initialize
void init( const std::string& log_file_name = "slog.txt", logger& the_logger = global_logger::get() ) {
	the_logger.init( log_file_name );
}

/// ever wanted to redirect std::cout to a log file? well this is the class for you.
class snooper : public std::streambuf {
public:
	snooper( std::ostream& destination, const std::string& log_file_name )
		: m_destination( destination.rdbuf() )
		, m_log_file( log_file_name.c_str(), std::ios_base::out | std::ios_base::trunc )
		, m_owner( nullptr )
		, m_is_good( false ) {

		if ( m_log_file.is_open() ) {
			m_is_good = true;
			destination.rdbuf( this );
			m_owner = &destination;
		}
	}

	~snooper() {
		if ( m_owner != nullptr ) {
			m_owner->rdbuf( m_destination );
		}
	}

protected:
	int overflow( int ch ) {
		if ( m_is_good ) {
			m_log_file.rdbuf()->sputc( ch );
		}
		return m_destination->sputc( ch );
	}

private:
	std::streambuf* m_destination;
	std::ofstream m_log_file;
	std::ostream* m_owner;
	bool m_is_good;
};

} // end of namespace slog

#endif SLOG_H_INCLUDED
