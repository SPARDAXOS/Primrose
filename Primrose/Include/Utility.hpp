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

		//std::ifstream t("file.txt");
		//t.seekg(0, std::ios::end);
		//size_t size = t.tellg();
		//std::string buffer(size, ' ');
		//t.seekg(0);
		//t.read(&buffer[0], size);



		//std::size_t size = 0;
		//FILE* fin = nullptr;

		//fopen_s(&fin, vertex_filename, "r");
		//if (fin == nullptr) {
		//	return false;
		//}

		//fseek(fin, 0, SEEK_END);
		//size = ftell(fin);
		//fseek(fin, 0, SEEK_SET);
		//std::string vertex_source;
		//vertex_source.resize(size + 1);
		//fread(vertex_source.data(), 1, size, fin);
		//fclose(fin);
		//fin = nullptr;



		std::stringstream TargetFileData;
		std::ifstream TargetFile(filePath.data(), std::ios::in);
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
	inline std::string CRead(const std::string_view& filePath) {
		//std::ifstream t("file.txt");
		//t.seekg(0, std::ios::end);
		//size_t size = t.tellg();
		//std::string buffer(size, ' ');
		//t.seekg(0);
		//t.read(&buffer[0], size);

		std::string TargetFileData;

		FILE* TargetFile;
		fopen_s(&TargetFile, filePath.data(), "r");
		if (TargetFile == nullptr) {
			PrintMessages("File failed to open! ", filePath.data());
			return TargetFileData;
		}

		fseek(TargetFile, 0, SEEK_END);
		const std::size_t FileSize = ftell(TargetFile);
		fseek(TargetFile, 0, SEEK_SET);

		TargetFileData.resize(FileSize + 1); // +1 byte as padding for safety

		fread_s(TargetFileData.data(), FileSize, 1, FileSize, TargetFile);
		fclose(TargetFile);
		TargetFile = nullptr;

		return TargetFileData;
	}
}