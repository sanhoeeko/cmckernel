#pragma once

#include<vector>
#include<iosfwd>
#include<string>
using namespace std;

inline char bool4toChar(char* src) {
	// data in [src] is either 0 or 1
	return 'A' + (src[0] << 3) + (src[1] << 2) + (src[2] << 1) + src[3];
}
inline int* chartoBool4(int* ptr, char c) {
	int x = c - 'A';
	ptr[3] = x % 2;
	ptr[2] = (x >> 1) % 2;
	ptr[1] = (x >> 2) % 2;
	ptr[0] = (x >> 3);
	return ptr + 4;
}

void writeCanReactMatrix(vector<char>& vb, int n_lines, const char* out_file_name) {
	int n = vb.size();
	int n_4 = (n - 1) / 4 + 1;				// pad to a multiple of 4
	char* buffer = new char[n_4 * 4];
	char* output = new char[n_4 + 1];		// the last char is '\0'
	memset(buffer, 0, n_4 * 4 * sizeof(char));
	memset(output, 0, (n_4 + 1) * sizeof(char));
	memcpy(buffer, &vb[0], n * sizeof(char));
	for (int i = 0; i < n_4; i++) {
		output[i] = bool4toChar(buffer + i * 4);
	}
	ofstream fout(out_file_name, ios::out | ios::trunc);
	fout << n_lines << endl;
	fout << output << endl;
	fout.close();
}

int readCanReactMatrix(const char* in_file_name, vector<int>& res) {
	// return: number of lines
	ifstream fin(in_file_name);
	string n_str;
	std::getline(fin, n_str);
	int n_lines = stoi(n_str);
	int m = (n_lines * (n_lines - 1)) / 2;
	int m4 = (m - 1) / 4 + 1;
	string buffer;
	std::getline(fin, buffer);
	res.resize(m4 * 4);
	int* ptr = res.data();
	for (int i = 0; i < m4; i++) {
		ptr = chartoBool4(ptr, buffer[i]);
	}
	res.resize(m);
	return n_lines;
}

void GenerateCanReactMatrix(const char* out_file_name) {
	int n = table.size();
	int num_of_pairs = ((n - 1) * (n - 2)) / 2;
	vector<char> arr; arr.reserve(num_of_pairs);
	Matter* ptr = table_start;
	for (int i = 0; i < n; i++) {
		for (int j = i + 1; j < n; j++) {
			if (canReact(table[i], table[j])) {
				arr.push_back(1);
			}
			else {
				arr.push_back(0);
			}
		}
		cout << "Calculated line " << i << endl;
	}
	writeCanReactMatrix(arr, n, out_file_name);
}