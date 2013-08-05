// Copyright (c) 2013, Richard Slater
// http://github.com/sudopunk/slog

#ifndef SLOG_H_INCLUDED
#define SLOG_H_INCLUDED

#include <string>
#include <sstream>
#include <fstream>
#include <mutex>

namespace slog {

class log_splitter : public std::streambuf {
public:

	/// Constructor
	/// @param[in] destination the stream which to monitor
	/// @param[in] log_file_name the full file name of the desired log file
	log_splitter( std::ostream& destination, const std::string& log_file_name )
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

	/// Destructor
	~log_splitter() {
		if ( m_owner != nullptr ) {
			m_owner->rdbuf( m_dest );
		}
	}

	/// TODO:
	void init_new_file( const std::string& log_file_name ) {

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
	std::mutex m_stream_mutex;
};

std::string create_timestamp() {
	// TODO:
	return "[timestamp]";
}

std::ostream& out( const std::string& log_file_name = "", bool enable_timestamp = false ) {
	static std::ostringstream oss_out;
	static log_splitter singleton( oss_out, log_file_name );
	static bool is_timestamp_enabled = enable_timestamp;
	static std::mutex mut;

	std::lock_guard<std::mutex> lock( mut );
	if ( !log_file_name.empty() ) {
		singleton.init_new_file( log_file_name );

	} else {

		// append newline
		oss_out << "\n";

		// optional timestamp
		if ( is_timestamp_enabled ) {
			oss_out << create_timestamp() << " ";
		}
	}
	return oss_out;
}

} // end of namespace slog

#endif SLOG_H_INCLUDED