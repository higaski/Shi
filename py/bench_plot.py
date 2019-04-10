import matplotlib.pyplot as plt
import numpy as np
from operator import truediv
from resize_canvas import resize_canvas

# Acker, count primes by trial division, lerp
c = [43, 131935, 619]
lua = [3087, 8279811, 19961]
mp = [6851, 8302808, 136294]
shi = [172, 828119, 3108]

# Relative overhead over C
lua_rel = list(map(truediv, lua, c))
mp_rel = list(map(truediv, mp, c))
shi_rel = list(map(truediv, shi, c))

with plt.xkcd():
    cases = len(c)
    ind = np.arange(cases)  # the x locations for the groups
    width = 0.2  # the width of the bars

    fig, ax = plt.subplots()
    ax.bar(ind - width, mp_rel, width, color="#5B5BFF", label="MicroPython")
    ax.bar(ind, lua_rel, width, color="#5BAD5B", label="Lua")
    ax.bar(ind + width, shi_rel, width, color="#DD143E", label="Forth (Shi)")

    ax.set_ylabel("Times slower than C")
    ax.set_xticks(ind)
    ax.set_xticklabels(("Ackermann", "Count primes\nby trial division", "Lerp"))
    ax.legend()

plt.savefig("bench.png", format="png", dpi=120)
resize_canvas("bench.png", "bench.png", canvas_width=900)
