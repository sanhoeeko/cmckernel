#pragma once

#include"matter.h"
#include<Eigen/Dense>
using namespace Eigen;

struct Reaction {
	vector<Matter> reactants;
	vector<Matter> resultants;
	MatrixXi coefs;
	float val;
};

typedef VectorXi* BalanceResult;

BalanceResult balanceEq(vector<Matter>& resultants, bits elements, MatrixXf& mat,
	int num_of_reactants, int num_of_resultants, int num_of_elements) {
	float* ptr = mat.data() + num_of_reactants * num_of_elements;
	for (int i = 0; i < num_of_resultants; i++) {
		resultants[i]->formula.template cast_to_elements<float>(elements, ptr + i * num_of_elements);
	}
	CompleteOrthogonalDecomposition<Eigen::MatrixXf> cod(mat);
	int dim_of_kernel = cod.dimensionOfKernel();
	if (dim_of_kernel == 1) {
		// transform the result to int
	}
	else {
		return NULL;
	}
}

MatrixXf initMat(vector<Matter>& reactants, bits elements, 
	int num_of_reactants, int num_of_resultants, int num_of_elements) {
	/*
		mat: each column is a matter
	*/
	MatrixXf mat = MatrixXf(num_of_elements, num_of_reactants + num_of_resultants);
	float* ptr = mat.data();
	int n = reactants.size();
	for (int i = 0; i < n; i++) {
		reactants[i]->formula.template cast_to_elements<float>(elements, ptr + i * num_of_elements);
	}
	return mat;
}

vector<Reaction> getPossibleReaction1(vector<Matter>& reactants) {
	bits elements = getElements(reactants);
	int num_of_elements = elements.size();
	int num_of_reactants = reactants.size();
	MatrixXf mat = initMat(reactants, elements, num_of_reactants, 1, num_of_elements);
	vector<Matter> possible_matters = filterMatterSufficient(elements);
	// iter over all possible matter sets
}