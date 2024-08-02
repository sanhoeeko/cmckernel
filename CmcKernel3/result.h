#pragma once

#include <vector>
#include "cardset.h"
typedef unsigned char uc;
using namespace std;

struct VResult
{
    int* data;
    int length;
    
    CardSet toCardSet();
    vector<uc> toVectorUC();
};

VResult v_wrap(const std::vector<uc>& (*func)(const std::vector<uc>&), const int* input_array, int length);