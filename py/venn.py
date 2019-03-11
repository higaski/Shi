import math
from matplotlib import pyplot as plt
from matplotlib_venn import venn2, venn3
import numpy as np

# Convert number to indices into binary
# e.g. 5 -> '101' > [2, 0]
def bits2indices(b):
    l = []
    if b == 0:
        return l
    for i in reversed(range(0, int(math.log(b, 2)) + 1)):
        if b & (1 << i):
            l.append(i)
    return l

# Make dictionary containing venn id's and set intersections
# e.g. d = {'100': {'c', 'b', 'a'}, '010': {'c', 'd', 'e'}, ... }
def set2dict(s):
    d = {}
    for i in range(1, 2**len(s)):
        # Make venn id strings
        key = bin(i)[2:].zfill(len(s))
        key = key[::-1]
        ind = bits2indices(i)
        # Get the participating sets for this id
        participating_sets = [s[x] for x in ind] 
        # Get the intersections of those sets
        inter = set.intersection(*participating_sets)
        d[key] = inter
    return d

python = set(['safe', 'something', 'embeddable']) 
lua = set(['fast', 'something'])
forth = set(['small', 'embeddable', 'fast'])
s = [python, lua, forth]

# Create dictionary from sets
d = set2dict(s)

# Plot it
with plt.xkcd():
    h = venn3(s, ('A', 'B', 'C'))
    for k, v in d.items():
        l = h.get_label_by_id(k)
        if l:
            l.set_text('\n'.join(sorted(v)))
plt.show()