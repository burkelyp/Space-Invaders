# Creates a test file that will test each opcode written in the disassembler
# Burkely Pettijohn
# 04/14/2025

values = [decimal.to_bytes(length=1) for decimal in range(256)]
two_byte_codes_hex = ["6", "e", "16", "1e", "26", "2e", "36", "3e", "c6", "ce",
                      "d3", "d6", "db", "de", "e6", "ee", "f6", "fe"]
two_byte_codes = [int(hex_string, 16) for hex_string in two_byte_codes_hex]
three_byte_codes_hex = ["1", "11", "21", "22", "2a", "31", "32", "3a", "c2",
                        "c3", "c4", "ca", "cb", "cc", "cd", "d2", "d4", "da",
                        "dc", "dd", "e2", "e4", "ea", "ec", "ed", "f2", "f4",
                        "fa", "fc", "fd"]
three_byte_codes = [int(hex_string, 16) for hex_string in three_byte_codes_hex]

with open('disassembler_test_file.bin', 'wb') as file:

    for index, value in enumerate(values):
        file.write(value)
        if (index in two_byte_codes) or (index in three_byte_codes):
            file.write(int("b2", 16).to_bytes(length=1))
        if index in three_byte_codes:
            file.write(int("b3", 16).to_bytes(length=1))
