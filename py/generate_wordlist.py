import os

file_path = os.path.dirname(os.path.realpath(__file__))
dir_path = file_path + '/../src/lib/core'

d = {}
for file in os.listdir(dir_path):
    filename = os.fsdecode(file)
    if filename.endswith('.asm'): 
        with open(os.path.join(dir_path, filename), 'r') as f:
            l = []
            for line in f:
                if 'WORD' in line and 'FLAG' in line:
                    str = line.split(',')[-1].upper()
                    str = str.replace('"', '').lstrip().rstrip('\n')
                    l.append(str)
            if len(l):
                d[os.path.splitext(filename)[0]] = l
            
for k, v in sorted(d.items()):
    longest = max(v, key=len)           

def print_for_c():            
    for k, v in sorted(d.items()):
        print('\n' + k)
        for str in v:
            string_length = len(str) + (len(longest) - len(str))
            str = str.ljust(string_length)
            print('#define SHI_ENABLE_' + str + ' 1')

def print_for_asm():
    for k, v in sorted(d.items()):
        print('\n' + k)
        for str in v:
            print('.equ ENABLE_' + str + ', SHI_ENABLE_' + str)    

# Print c header or assembly list
print_for_c()
print_for_asm()