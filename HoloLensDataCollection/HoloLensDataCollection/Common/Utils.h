#pragma once

namespace HoloLensDataCollection {
	template<typename ... Args>
	inline std::string string_format(const std::string& format, Args ... args)
	{
		size_t size = std::snprintf(nullptr, 0, format.c_str(), args ...) + 1; // Extra space for '\0'
		std::unique_ptr<char[]> buf(new char[size]);
		std::snprintf(buf.get(), size, format.c_str(), args ...);
		return std::string(buf.get(), buf.get() + size - 1); // We don't want the '\0' inside
	}

	// Writes to the data.txt file in LocalState folder, this is not thread safe, something quick and dirty to get working.
	inline void writeToTheFile(const std::string&  stringToWrite) {
		auto storageFolder = ApplicationData::Current->LocalFolder;
		auto wideString = std::wstring(stringToWrite.begin(), stringToWrite.end());
		const auto* wideStringChar = wideString.c_str();
		auto platformString = ref new Platform::String(wideStringChar);
		auto directoryPath = storageFolder->Path;
		auto filePath = directoryPath + L"\\data.txt";
		std::wstring filePathWideString(filePath->Begin());
		std::fstream outputFile;
		outputFile.open(std::string(filePathWideString.begin(), filePathWideString.end()), std::ios::app | std::ios::out);
		outputFile << stringToWrite;
		outputFile.close();
	}
}