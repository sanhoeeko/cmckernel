#pragma once

#include"defs.h"
#include<vector>

VectorXi integerVec(VectorXr& v);

void writeCanReactMatrix(std::vector<char>& vb, const char* out_file_name);
int readCanReactMatrix(const char* in_file_name, std::vector<int>& res);
void writeJson(json& js, const char* file_name);
json readJson(const char* file_name);

#include"itertools.h"