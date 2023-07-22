import sys
import gc

def append(fl, sl):
    if fl == None and sl == None:
        return None
    else:
        if fl == None and sl != None:
            return (sl[0], append(fl, sl[1]))
        elif fl != None and sl == None:
            return (fl[0], append(fl[1], sl))
        else:
            return (fl[0], append(fl[1], sl))

def permBase(l):
    return ((l[0], (l[1][0], None)), ((l[1][0], (l[0], None)), None))

def getByN (l, n):
    if n == 0:
        return l[0]
    else:
        return getByN(l[1], n-1)

def delByN (l, n):
    if n == 0:
        return l[1]
    else:
        return (l[0], delByN(l[1], n-1))


def appendToAll (l, n):
    if l == None:
        return None
    else:
        return ((n, l[0]), appendToAll(l[1], n))

def count (l):
    n = 0
    while l != None:
        n+=1
        l = l[1]
    return n

def permut(l, n):
    if count(l) == 2:
        return permBase(l)
    elif n == 0:
        return None
    else:
        return append(appendToAll(permut(delByN(l, n-1), count(delByN(l, n-1))), getByN(l, n-1)), permut(l, n-1))

def genPerms(l):
    return permut(l, count(l))



sys.setrecursionlimit(100000)
gc.enable()

#genPerms((1, (2, None)))
#genPerms((1, (2, (3, None))))
# print(count(genPerms((1, (2, (3, (4, None)))))))
#print(count(genPerms((1, (2, (3, (4, (5, None))))))))
#print(count(genPerms((1, (2, (3, (4, (5, (6, None)))))))))
print(count(genPerms((1, (2, (3, (4, (5, (6, (7, None))))))))))
# genPerms((1, (2, (3, (4, (5, (6, (7, (8, None)))))))))
gc.collect()
print("done")
