import re


def is_case_line(line):
    return re.match(r'^\s*case 0x[0-9A-Fa-f]{2}:\s*$', line)


with open("emulator.cpp", "r") as f:
    lines = f.readlines()

output = []
in_case = False

for i, line in enumerate(lines):
    stripped = line.strip()

    if is_case_line(line):
        output.append(line)
        output.append("        {\n")
        in_case = True

    elif in_case and stripped == "break;":
        output.append(line)
        output.append("        }\n")
        in_case = False
    else:
        output.append(line)

# Write to a new file to keep original intact
with open("emulator_wrapped.cpp", "w") as f:
    f.writelines(output)

print("✅ All cases wrapped. Output saved to emulator_wrapped.cpp")

