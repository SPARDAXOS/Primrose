#pragma once
#include <string_view>
#include <sstream>
#include <fstream>
#include "Utility.hpp"
#include <iostream>



struct JFIFHeader {

	int16 SOI;
	int16 APP0;
	int16 Length;
	char Identifiers[5];
	int16 Version;
	char Units;
	int16 XDensity;
	int16 YDensity;
	char XThumbnail;
	char YThumbnail;
};

namespace FileManagement {
	inline std::string Read(const std::string_view& filePath) {

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
	[[nodiscard]] inline bool CRead(const std::string_view& filePath, std::string& buffer) {
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
			return false;
		}

		fseek(TargetFile, 0, SEEK_END);
		const std::size_t FileSize = ftell(TargetFile);
		fseek(TargetFile, 0, SEEK_SET);

		TargetFileData.resize(FileSize + 1); // +1 byte as padding for safety

		fread_s(TargetFileData.data(), FileSize, 1, FileSize, TargetFile);
		fclose(TargetFile);
		TargetFile = nullptr;


		buffer = TargetFileData;
		return true;
	}
}

namespace ImageLoader {

	inline void ParseJPG(const std::string_view& filePath, std::string& buffer) {

		std::string TargetFileData;

		FILE* TargetFile;
		fopen_s(&TargetFile, filePath.data(), "r");
		if (TargetFile == nullptr) {
			PrintMessages("File failed to open! ", filePath.data());
			return;
		}

		fseek(TargetFile, 0, SEEK_END);
		const std::size_t FileSize = ftell(TargetFile);
		fseek(TargetFile, 0, SEEK_SET);

		TargetFileData.resize(FileSize + 1); // +1 byte as padding for safety

		fread_s(TargetFileData.data(), FileSize, 1, FileSize, TargetFile);
		

		std::istringstream stream(TargetFileData);
		std::string line;
		uint8 TwoBytes[2];
		char* ptr;
		//unsigned int counter = 0;

		for (unsigned int i = 0; i < TargetFileData.size(); i++) {
			TwoBytes[0] = TargetFileData.data()[i];
			TwoBytes[1] = TargetFileData.data()[i + 1];
			if (TwoBytes[0] == 0xff && TwoBytes[1] == 0xE1) {
				ptr = &TargetFileData.data()[i];
				std::cout << "it found the byte at " << i << " index and it is " << *TwoBytes;
			}
		}


		while (std::getline(stream, line)) {
			std::cout << line.data() << std::endl;
		}
		
	}

}