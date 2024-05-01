import ctypes as ct
import random as rd

import utils as ut


class CmcKernel:
    def __init__(self):
        # Load the shared library
        self.lib = ct.CDLL('CmcKernelDll/x64/Release/CmcKernelDll.dll')
        str_function_list = [
            'CardNamesToIds',
            'CardIdsToNames',
            'QueryLegalStrategies',
            'QueryPossibleMatters',
            'QueryReactions',
        ]

        # Specify the return type and argument type for the C++ function
        for func_name in str_function_list:
            getattr(self.lib, func_name).restype = ct.c_char_p
            getattr(self.lib, func_name).argtypes = [ct.c_char_p]

        # Attach cpp functions to this object
        for func_name in str_function_list:
            # 'func_name=func_name' is similar to c++ capturing
            setattr(self, func_name, lambda x, func_name=func_name: 
                    self.call_cpp_function(func_name, x))

        # initialize the CMC table
        self.lib.Init()

    def call_cpp_function(self, func_name, input_string):
        # Create a C-style string buffer from the Python string
        inp = ct.create_string_buffer(input_string.encode('utf-8'))

        # Call the C++ function
        result = getattr(self.lib, func_name)(inp)

        # Convert the C-style string back to a Python string
        return result.decode('utf-8')
    
    def CardNamesToIds(self):
        """
        Format: ',' split
        """
        pass
    
    def CardIdsToNames(self):
        """
        Format: ',' split
        """
        pass

    def QueryLegalStrategies(self):
        """
        Input format: {Matter to React};{card1},{card2},...
		Output format: {status};{Matter1}:[[strategy1],[...],] \n {Matter2}:[[...],],...
        """
        pass

    def QueryPossibleMatters(self):
        """
        Input format: card names split by space
		Output format: [...] list of matters
        """
        pass

    def QueryReactions(self):
        """
        Input format: {Matter a},{Matter b}
		Output format: [...] list of reactions
        """
        pass


cmc_kernel = CmcKernel()

elements = ["H", "Li", "Be", "B", "C", "N", "O", "F",
            "Na", "Mg", "Al", "Si", "P", "S", "Cl", "K", "Ca",
            "Sc", "Ti", "V", "Cr", "Mn", "Fe", "Co", "Ni", "Cu", "Zn",
            "Ga", "Ge", "As", "Se", "Br", "Rb", "Sr", "Y", "Zr",
            "Nb", "Mo", "Tc", "Ru", "Rh", "Pd", "Ag", "Cd", "In", "Sn",
            "Sb", "Te", "I", "Cs", "Ba"]
numbers = ["2", "3", "4"]

deck1 = ["H", "O", "2", "3"]
deck2 = ["N", "Na", "Mg", "Al", "Si", "P", "S", "Cl", "Fe", "Cu", "Br", "I", "4"]
deck3 = ut.removeSubsequence(ut.removeSubsequence(elements + numbers, deck1), deck2)


def randCard(n: int):
    total_deck = deck1 * 8 + deck2 * 2 + deck3
    return rd.sample(total_deck, n)
