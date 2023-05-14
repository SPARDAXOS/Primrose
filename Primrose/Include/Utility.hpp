#pragma once
#include <stdio.h>
#include <string_view>
#include <sstream>
#include <fstream>


#define PrintMessage(message) printf("%s \n", message);
#define PrintMessages(message1, message2) printf("%s %s \n", message1, message2);


using uint8 = unsigned char;
using uint16 = unsigned short;
using uint32 = unsigned int;
using uint64 = unsigned long long;

using int8 = signed char;
using int16 = signed short;
using int32 = signed int;
using int64 = signed long long;


namespace FileManagement {
	inline std::string Read(const std::string_view& filePath) {
		std::stringstream TargetFileData;
		std::ifstream TargetFile;
		TargetFile.open(filePath.data(), std::ios::in);
		if (!TargetFile.is_open()) {
			PrintMessages("File failed to open! ", filePath.data());
			return TargetFileData.str();
		}

		std::string Line;
		while (getline(TargetFile, Line))
		{
			TargetFileData << Line << "\n";
		}

		TargetFile.close();
		return TargetFileData.str();
	}
}