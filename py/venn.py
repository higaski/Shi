import math
import itertools
from matplotlib import pyplot as plt
from matplotlib_venn import venn2, venn3
import numpy as np


def gen_index(n):
    """
     Generate list index for itertools combinations
    """
    x = -1
    while True:
        while True:
            x = x + 1
            if bin(x).count("1") == n:
                break
        yield x


def make_intersections(sets):
    """
    Generate all combinations of intersections
    """
    l = [None] * 2 ** len(sets)
    for i in range(1, len(sets) + 1):
        ind = gen_index(i)
        for subset in itertools.combinations(sets, i):
            inter = set.intersection(*subset)
            l[next(ind)] = inter
    return l


def number2venn_id(x, n_fill):
    """
    Get weird reversed binary string id for venn
    """
    id = bin(x)[2:].zfill(n_fill)
    id = id[::-1]
    return id


def sets2dict(sets):
    """
    Iterate over all combinations and remove duplicates from intersections with more sets
    """
    l = make_intersections(sets)
    d = {}
    for i in range(1, len(l)):
        d[number2venn_id(i, len(sets))] = l[i]
        for j in range(1, len(l)):
            if bin(j).count("1") < bin(i).count("1"):
                l[j] = l[j] - l[i]
                d[number2venn_id(j, len(sets))] = l[j] - l[i]
    return d


python = set(["easy", "embeddable", "modern", "safe"])
lua = set(["embeddable", "fast", "modern"])
forth = set(["blazingly fast", "embeddable", "small"])
sets = [python, lua, forth]

d = sets2dict(sets)

# Plot it
with plt.xkcd():
    h = venn3(sets, ("Python", "Lua", "Forth"))
    for k, v in d.items():
        l = h.get_label_by_id(k)
        if l:
            l.set_fontsize(12)
            l.set_text("\n".join(sorted(v)))

plt.show()
