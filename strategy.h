#pragma once

#include"defs.h"
#include"reaction.h"
#include"io.h"

struct Advisor{
	MatrixXi can_react_table;
	int num_of_matter;

	Advisor() {
		vector<int> temp;
		int can_react_mat_size = readCanReactMatrix("can_react.matrix", temp);
		this->can_react_table = MatrixXi(can_react_mat_size, can_react_mat_size);
		this->can_react_table.setZero();
		int length = can_react_mat_size - 1;
		int* ptr = temp.data();
		for (int i = 0; i < can_react_mat_size - 1; i++) {
			memcpy(this->can_react_table.data() + can_react_mat_size * i + (i + 1), ptr, length * sizeof(int));
			ptr += length;
			length--;
		}
		this->can_react_table += this->can_react_table.transpose().eval();
		this->num_of_matter = can_react_mat_size;
	}

	vector<Matter> getMattersCanReactWith(Matter m) {
		vector<Matter> res; res.reserve(num_of_matter);
		int* ptr = can_react_table.data() + m->id * num_of_matter;
		for (int i = 0; i < num_of_matter; i++) {
			if (ptr[i] == 1) {
				res.push_back(table[i]);
			}
		}
		return res;
	}
};

void checkCanReactMatrix(Matter a) {
	Advisor advisor;
	vector<Matter> ms = advisor.getMattersCanReactWith(a);
	for (Matter m : table) {
		cout << to_string(m);
		if (canReact(m, a)) {
			if (in(m, ms)) {
				cout << " ." << endl;	// normal
			}
			else {
				cout << " o" << endl;	// should be "can react"
			}
		}
		else {
			if (std::find(ms.begin(), ms.end(), m) != ms.end()) {
				cout << " x" << endl;	// should be "cannot react"
			}
			else{
				cout << " ." << endl;	// normal
			}
		}
	}
}