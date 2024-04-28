#pragma once

#include<vector>
#include<iosfwd>
using namespace std;

inline char bool4toChar(char* src) {
	// data in [src] is either 0 or 1
	return 'A' + (src[0] << 3) + (src[1] << 2) + (src[2] << 1) + src[3];
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

void GenerateCanReactMatrix(const char* out_file_name) {
	int n = table.size();
	int num_of_pairs = ((n - 1) * (n - 2)) / 2;
	vector<char> arr; arr.reserve(num_of_pairs);
	Matter* ptr = table_start;
	//n = 10;
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
	/*
	srand(time(0));
	for (int i = 0; i < n; i++) {
		for (int j = i + 1; j < n; j++) {
			if ((double)rand() / RAND_MAX > 0.4) {
				arr.push_back(1);
			}
			else {
				arr.push_back(0);
			}
		}
	}*/
	writeCanReactMatrix(arr, n, out_file_name);
}