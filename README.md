slog
====

Simple C++ log file mechanisms. The "s" could also stand for "stream", "small", or "safe" as in thread-safe.  However, the main purpose of this tiny header file is to drop in a project and start writing logfile messages right away.  Here's an example:

    #include "slog.h"
  
    int main() {
      slog::init( "log.txt" );
      slog::out() << "Hello World, the answer to life the universe and everything is " << 42;
    }
    
Perhaps you already have a large project that has many statements that output to a common stream like std::cout, well a special "snooper" utility is available just for that case.  Here's another small example:

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
* Ubuntu Linux 12.04.2 using GCC 4.8
