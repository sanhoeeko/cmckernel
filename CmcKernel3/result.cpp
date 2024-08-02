#include "result.h"
#include "pch.h"
#include "kernel.h"

CMCKernel* kernel;

void Init()
{
    kernel = new CMCKernel(false);
    kernel->ReadCardSetJson("cardsets.json");
}

void free_result(VResult result) {
    delete[] result.data;
}

CardSet VResult::toCardSet()
{
    return from_cardIds(vector<int>(data, data + length));
}

vector<uc> VResult::toVectorUC()
{
    //
}

VResult v_wrap(const std::vector<uc>& (*func)(const std::vector<uc>&), const int* input_array, int length) {
    std::vector<uc> input; input.resize(length);
    for (int i = 0; i < length; i++) input[i] = (uc)input_array[i];
    std::vector<uc> output = func(input);

    VResult result;
    result.length = output.size();
    result.data = new int[result.length];               // allocate memory here
    for (size_t i = 0; i < output.size(); ++i) {
        result.data[i] = output[i];
    }
    return result;
}
