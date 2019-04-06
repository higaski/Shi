import os
import re
import urllib.request


def escape(str):
    str = str.replace("/", "&frasl;")
    str = str.replace('"', "&quot;")
    str = str.replace("@", "@@")
    str = str.replace("[", "\[")
    str = str.replace("]", "\]")
    return str


# Get list of words from forth-standard.org
fp = urllib.request.urlopen("https://forth-standard.org/standard/core")
forth_standard = fp.readlines()
l_url = []
for line in forth_standard:
    line = line.decode("utf8")
    if '<li role="menuitem"><a href="core/' in line:
        word = re.findall(r">([^<]*)<", line)[1].lower()
        word = escape(word)

        # For some fucked up reason markdown with [\] doesn't work
        # Fall back to html for this one
        if word == "\\":
            l_url.append(
                '<a href="https://forth-standard.org/standard/core/bs"> \ </a>'
            )
        else:
            url = (
                "https://forth-standard.org/standard/"
                + re.findall(r"\"(.+?)\"", line)[1]
            )
            l_url.append("[" + word + "]" + "(" + url + ")")
fp.close()

# Get list of words from .asm files
file_path = os.path.dirname(os.path.realpath(__file__))
dir_path = file_path + "/../src/lib/core"
d_name = {}
d_label = {}
for file in os.listdir(dir_path):
    filename = os.fsdecode(file)
    if filename.endswith(".asm"):
        with open(os.path.join(dir_path, filename), "r") as f:
            l_name = []
            l_label = []
            for line in f:
                if "WORD" in line and "FLAG" in line:
                    str = re.findall(r"\"(.+?)\"", line)[0]
                    str = escape(str)
                    l_name.append(str)
                    str = line.split(",")[-1].upper()
                    str = str.replace('"', "").lstrip().rstrip("\n")
                    l_label.append(str)
            if len(l_name):
                d_name[os.path.splitext(filename)[0]] = l_name
            if len(l_label):
                d_label[os.path.splitext(filename)[0]] = l_label


def print_words_as_markdown_table(l):
    """
    Print markdown table based on words from forth-standard.org
    """
    longest = len(max(l, key=len))
    for str in l:
        string_length = len(str) + (longest - len(str))
        str = str.ljust(string_length)
        print("/// | " + str + " | unimplemented |")


def print_words(d):
    """
    Just print the words from .asm files
    """
    for k, v in sorted(d.items()):
        print("\n>>> " + k + " <<<")
        for str in v:
            print(str)


def len_longest_key(d):
    l = []
    for k, v in sorted(d.items()):
        l.append(len(max(v, key=len)))
    return max(l)


def print_shi_words_c_macros(d):
    longest = len_longest_key(d)
    for k, v in sorted(d.items()):
        print("\n>>> " + k + " <<<")
        for str in v:
            string_length = len(str) + (longest - len(str))
            str = str.ljust(string_length)
            print("#define SHI_ENABLE_" + str + " 1")


def print_shi_words_asm_equs(d):
    for k, v in sorted(d.items()):
        print("\n>>> " + k + " <<<")
        for str in v:
            print(".equ ENABLE_" + str + ", SHI_ENABLE_" + str)


# print_words(d_name)
# print_words_as_markdown_table(l_url)
# print_shi_words_c_macros(d_label)
print_shi_words_asm_equs(d_label)
