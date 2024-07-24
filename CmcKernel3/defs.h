#pragma once

#include<Eigen/Dense>
using namespace Eigen;

#include<nlohmann/json.hpp>
using json = nlohmann::json;

#define EIGEN_NO_DEBUG

typedef double Real;
typedef Matrix<Real, -1, -1> MatrixXr;
typedef Matrix<Real, -1, 1> VectorXr;
