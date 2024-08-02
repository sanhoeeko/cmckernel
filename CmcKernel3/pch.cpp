// pch.cpp: 与预编译标头对应的源文件

#include "pch.h"
#include "result.h"
#include "kernel.h"

// 当使用预编译的头时，需要使用此源文件，编译才能成功。

extern CMCKernel* kernel;

VResult possibleMatters(int* input, int length)
{
    static vector<int> res;
    res = kernel->getPossibleMatters(from_cardIds(vector<int>(input, input + length)));
    return { res.data(), (int)res.size() };
}
