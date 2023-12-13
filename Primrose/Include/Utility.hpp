#pragma once
#include <stdio.h>
#include <string_view>
#include <sstream>
#include <fstream>
#include <exception>


#define PrintMessage(message) printf("%s \n", message);
#define PrintMessages(message1, message2) printf("%s %s \n", message1, message2);
#define PrintGLError(number, file, function, line) printf("[OpenGL Error] %d\n[File] %s\n[Function] %s\n[Line] %d\n", number, file, function, line);


namespace {

	using uint8 = unsigned char;
	using uint16 = unsigned short;
	using uint32 = unsigned int;
	using uint64 = unsigned long long;

	using int8 = signed char;
	using int16 = signed short;
	using int32 = signed int;
	using int64 = signed long long;

}
