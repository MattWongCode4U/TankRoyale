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

inline void writeToFile(std::string fileName, std::string data) {
	//open file and write string to it 
	std::ofstream myfile;
	myfile.open(fileName);
	myfile << data;
	myfile.close();
}
