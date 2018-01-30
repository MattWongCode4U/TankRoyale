#pragma once
#include <string>
#include <sstream>
#include <vector>
#include <iterator>
#include<iostream>
#include<fstream>
#include <chrono>
#include <map>
#include <algorithm>
#include <filesystem>

enum class ASSET_TYPE {
	TEXTURE, MODEL, DATA, MUSIC, SOUND, SHADER
};

inline std::string getAssetPrefixPath(ASSET_TYPE assetType) {

	switch (assetType) {
	case ASSET_TYPE::TEXTURE:
		return std::string("assets/textures/");
	case ASSET_TYPE::MODEL:
		return std::string("assets/models/");
	case ASSET_TYPE::DATA:
		return std::string("assets/data/");
	case ASSET_TYPE::SOUND:
		return std::string("assets/sfx/");
	case ASSET_TYPE::MUSIC:
		return std::string("assets/music/");
	case ASSET_TYPE::SHADER:
		return std::string("assets/shaders/");
	}

	return std::string("assets/");
}

template<typename Out>
inline void split(const std::string &s, char delim, Out result) {
	std::stringstream ss;
	ss.str(s);
	std::string item;
	while (std::getline(ss, item, delim)) {
		*(result++) = item;
	}
}

inline std::vector<std::string> split(const std::string &s, char delim) {
	std::vector<std::string> elems;
	split(s, delim, std::back_inserter(elems));
	return elems;
}

inline std::string openFileRemoveSpaces(std::string fileName) {
	//open file and read it into a string
	std::ifstream t(fileName);
	std::string output((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
	t.close();

	output.erase(std::remove_if(output.begin(), output.end(), isspace), output.end());//remove spaces from data

	return output;
}

inline std::string openFile(std::string fileName) {
	//open file and read it into a string
	std::ifstream t(fileName);
	std::string output((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
	t.close();
	return output;
}

inline std::string openFileFromAsset(std::string assetName, ASSET_TYPE assetType, bool removeSpaces) {
	std::string fullPath = getAssetPrefixPath(assetType) + assetName;

	if (removeSpaces) {
		return openFileRemoveSpaces(fullPath);
	}
	else {
		return openFile(fullPath);
	}
}

inline void writeToFile(std::string fileName, std::string data) {
	//open file and write string to it 
	std::ofstream myfile;
	myfile.open(fileName);
	myfile << data;
	myfile.close();
}

inline std::vector<std::string> listFilesInPath(std::string path) {
	namespace fs = std::experimental::filesystem;
	std::vector<std::string> filesInPath;

	for (auto &p : fs::directory_iterator(path))
	{
		filesInPath.push_back(p.path().stem().string());
	}

	//will return filenames WITHOUT extension
	return filesInPath;
}
