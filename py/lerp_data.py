import numpy as np
from numpy.core._multiarray_umath import dtype

n = 2000

x1 = np.random.randint(low=0, high=2 ** 10, size=n, dtype=np.int32)
x2 = np.random.randint(low=2 ** 11, high=2 ** 15, size=n, dtype=np.int32)
y1 = np.random.randint(low=0, high=2 ** 10, size=n, dtype=np.int32)
y2 = np.random.randint(low=2 ** 11, high=2 ** 15, size=n, dtype=np.int32)
x = np.zeros((n), dtype=np.int32)
y = np.zeros((n), dtype=np.int32)
for i in range(n):
    x[i] = np.random.randint(low=x1[i], high=x2[i])
    y[i] = y1[i] + ((y2[i] - y1[i]) * (x[i] - x1[i])) / (x2[i] - x1[i])


f = open("lerp_data.txt", "w+")

f.write(
    "typedef struct {\n"
    "  int32_t x[" + str(n) + "];\n"
    "  int32_t x1[" + str(n) + "];\n"
    "  int32_t x2[" + str(n) + "];\n"
    "  int32_t y1[" + str(n) + "];\n"
    "  int32_t y2[" + str(n) + "];\n"
    "  int32_t y[" + str(n) + "];\n"
    "} lerp_data_t;\n\n"
)

f.write("const lerp_data_t lerp_data = {")


def write_np_array(np_array):
    f.write("{")
    for i in range(n - 1):
        f.write(str(np_array[i]) + ", ")
    f.write(str(np_array[n - 1]) + "},\n")


write_np_array(x)
write_np_array(x1)
write_np_array(x2)
write_np_array(y1)
write_np_array(y2)
write_np_array(y)

f.write("};")

f.close()
