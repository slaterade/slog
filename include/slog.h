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

class message : public std::ostringstream {
public:
	message( logger* owner )
		: m_owner( owner ) {
	}

	~message() {
		m_owner->capture( *this );
	}
	
private:
	logger* m_owner;
};

class global_logger {
public:
	static logger& get() {
		static logger singleton;
		return singleton;
	}
};

message out( logger& the_logger = global_logger::get() ) {
	return message( &the_logger );
}

void init( const std::string& log_file_name = "slog.txt", logger& the_logger = global_logger::get() ) {
	the_logger.init( log_file_name );
}

class snooper : public std::streambuf {
public:
	snooper( std::ostream& destination, const std::string& log_file_name )
		: m_dest( destination.rdbuf() )
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
			m_owner->rdbuf( m_dest );
		}
	}

protected:
	int overflow( int ch ) {
		if ( m_is_good ) {
			m_log_file.rdbuf()->sputc( ch );
		}
		return m_dest->sputc( ch );
	}

private:
	std::streambuf* m_dest;
	std::ofstream m_log_file;
	std::ostream* m_owner;
	bool m_is_good;
};

} // end of namespace slog

#endif SLOG_H_INCLUDED