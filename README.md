slog
====

Simple C++ log file mechanisms. The "s" could also stand for "stream", "small", or "safe" as in thread-safe.  However, the main purpose of this tiny header file is to drop in a project and start writing logfile messages right away.

Example 1 - The bare minimum.

    #include "slog.h"
  
    int main() {
        slog::out() << "Hello World, the answer to life the universe and everything is " << 42;
    }
    
Example 2 - A couple convenient features.

    #include "slog.h"
    
    int main() {
        slog::init( "mylog.txt" );
        slog::set_timestamp_format( slog::format_time_only );
        slog::set_archiving();
        
        slog::info() << "Application version 1.0 initialized.";
        slog::warning() << "Memory low.";
        slog::error() << "Mistakes were made... error " << 37;
        
        slog::out( "DebugForFun" ) << "This message has a custom context tag.";
    }

Perhaps you already have a large project that has many statements that output to a common stream like std::cout, well a special "snooper" utility is available just for that case.

Example 3

    #include "slog.h"
    
    int main() {
        // this object will intercept a stream and log it to a file, but don't worry the following
        // statements will be none the wiser and the console will still see standard output
        slog::snooper snoop( std::cout, "cout.txt" );     
        
        std::cout << "Man, I really wish these " << 492 << " console statements were available "
            << 3.14159 << " to parse in a text file.";
        std::cout << "That would be great, but we were told to comb the desert so we're combing it!";
    }
    
install instructions
====================
Just download the [slog.h](https://raw.github.com/sudopunk/slog/master/include/slog.h "slog.h") file and include it in your project.

compiler requirements
=====================
C++11, tested on the following platforms:
* Windows 7 using Microsoft Visual Studio Express 2012 for Windows Desktop
* OSX 10.8.4 using XCode 4.6.3 Command-Line Tools (Apple LLVM 4.2 and clang-425.0.28)
* (broken) Ubuntu Linux 12.04.2 using GCC 4.8
    * timestamp not working because gcc has not implemented std::put_time yet, see [here](http://gcc.gnu.org/onlinedocs/libstdc++/manual/status.html#status.iso.200x "gcc status")
