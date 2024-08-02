import ctypes as ct


class Result(ct.Structure):
    _fields_ = [("array", ct.POINTER(ct.c_int)),
                ("length", ct.c_int)]


class Kernel:
    def __init__(self):
        self.dll = ct.CDLL('CmcKernel3.dll', winmode=0)
        self.dll.Init()
        self.dll.free_result.argtypes = [Result]
        self.dll.possibleMatters.argtypes = [ct.POINTER(ct.c_int), ct.c_int]
        self.dll.possibleMatters.restype = Result

    def free(self, result):
        return self.dll.free_result(result)

    def vv_decorator(func):
        def wrapper_func(self, input_array):
            length = len(input_array)
            c_input_array = (ct.c_int * length)(*input_array)
            result = getattr(self.dll, func.__name__)(c_input_array, length)  # 调用C++包装函数
            output_array = [result.array[i] for i in range(result.length)]  # 将C类型数组转换回Python列表
            self.free(result)  # 释放C++中分配的内存
            return output_array

        return wrapper_func

    @vv_decorator
    def possibleMatters(self): pass
