f = open("../inc/shi.hpp", "r")
shi_hpp = f.read().splitlines()

out = []
for l in shi_hpp:
    if "#" in l:
        if "if" in l or "error" in l or "ifdef" in l or "ifndef" in l or "else" in l or "endif" in l:
            continue
    out.append(l)

f = open("../doc/shi.hpp", "w")
for l in out:
    f.write(l + "\n")
