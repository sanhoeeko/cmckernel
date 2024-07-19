#pragma once

#include<vector>
#include<iosfwd>
#include<string>
using namespace std;

void writeCanReactMatrix(vector<char>& vb, const char* out_file_name) {
	std::ofstream out_file(out_file_name, std::ios::binary);
	if (!out_file) {
		std::cerr << "Unable to open file." << std::endl;
		return;
	}

	int bit_count = 0;
	char byte = 0;
	for (char bit : vb) {
		byte = (byte << 1) | bit;
		bit_count++;
		if (bit_count == 8) {
			out_file.write(&byte, sizeof(byte));
			bit_count = 0;
			byte = 0;
		}
	}

	// write the last byte
	if (bit_count != 0) {
		byte <<= (8 - bit_count);  // move bits to the left
		out_file.write(&byte, sizeof(byte));
	}

	out_file.close();
}

int readCanReactMatrix(const char* in_file_name, vector<int>& res) {
	/*
		return: number of lines
	*/
	std::ifstream file(in_file_name, std::ios::binary | std::ios::ate);
	if (!file){
		std::cerr << "Unable to open file." << std::endl;
		return 0;
	}
	// get the size of the binary file
	std::streamsize size = file.tellg();
	file.seekg(0, std::ios::beg);
	// create a buffer
	res.resize(size * 8);
	// read data
	char c;
	for (std::streamsize i = 0; i < size; ++i){
		// read 1 char
		file.read(&c, 1);
		for (int j = 0; j < 8; ++j) {
			res[i * 8 + j] = (c >> (7 - j)) & 1;
		}
	}
	return (1 + sqrt(1 + 64 * size)) / 2;	// a magic formula
}