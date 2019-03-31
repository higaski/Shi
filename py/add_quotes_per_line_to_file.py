with open("file.txt", "r") as f:
    for line in f:
        line = line.rstrip()
        print('"' + line + '"')
