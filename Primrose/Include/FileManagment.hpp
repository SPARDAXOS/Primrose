#pragma once
#include <string_view>
#include <sstream>
#include <fstream>
#include "Utility.hpp"


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