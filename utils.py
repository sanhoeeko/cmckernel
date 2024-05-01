def toListOfStr(s: str):
    lst = s[1:-1].split(',')
    if lst[-1] == '':
        return lst[:-1]
    else:
        return lst


def isSubsequence(sub: list, main: list):
    sub_len, main_len = len(sub), len(main)
    i = j = 0
    while i < sub_len and j < main_len:
        if sub[i] == main[j]:
            i += 1
        j += 1
    return i == sub_len


def removeSubsequence(main, sub):
    result = []
    for item in main:
        if item not in sub:
            result.append(item)
    return result
