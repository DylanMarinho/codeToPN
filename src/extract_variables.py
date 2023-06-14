import argparse
import os.path
import re


def transform_number(entry):
    """
    From a number of rodata line, get an integer
    :param entry: Piece of data of rodata file
    :return: Binary integer
    """
    ret = ""
    # entry is in little endian
    for i in range(len(entry) // 2):
        ret += entry[len(entry) - 2 * i - 2]
        ret += entry[len(entry) - 2 * i - 1]
    # Add "0x"
    ret = "0x" + ret
    return ret


def parse_line(line):
    """
    Parse a line containing variables of a rodata file
    :param line: Line containing varibles: [addr] [data]
    :return: List [memory adress, data1, data2, ...]
    """
    line = re.sub(r'\s+', ' ', line)  # remove multiple spaces
    vars = line.split(" ")
    vars.pop(0)  # Remove first entry: it is empty
    ret = [int("0x" + vars[0], 0)]  # vars[0] is addr
    for k in vars[1:-2]:  # 0 is included, -1 is comment, -2 is empty
        if k != "":
            ret.append(transform_number(k))
    return ret


def parsed_line_to_entries(parsed_line):
    """
    From a parsed line, return a list of memory entries
    :param parsed_line: Line of rodata parsed [addr] [data]
    :return: Table of adresses and memory value
    """
    ret_table = []
    addr = parsed_line[0]
    values = parsed_line[1::]
    for i in range(len(values)):
        ret_table.append([addr + i * 4, int(values[i], 0)])  # 4 = sizeof(int)
    return ret_table


def parse_rodata(file_content):
    """
    Parse the content of a rodata file and generate a table of each line of memory
    :param file_content: Content of a rodatafile (list of lines)
    :return: Table of memory entries
    """
    ret_table = []

    line_of_variable = False
    begin_line = "Contents of section .rodata:"
    for line in file_content:
        # If we did not begin to read variables
        if (not line_of_variable):
            if begin_line in line:
                line_of_variable = True  # From begin_line, we begin to read variables
            continue

        # Extract variables
        parsed_line = parse_line(line)
        # Add memory entries in ret table
        entries = parsed_line_to_entries(parsed_line)
        for k in entries:
            ret_table.append(k)
    return ret_table


def write_in_file(ufile, memory_entries, output_name):
    """
    Update the declaration file with the content of the memory
    :param file: input declaration file
    :param memory_entries: table of memory entries
    :param output_name: Name of the output file
    :return: None (write a file, tmp/[declaration file]_[model].c)
    """
    try:
        input = open(ufile, "r")
        content = input.readlines()
        new_content = []

        for line in content:
            # Update datastart
            if "const int dataStart" in line:
                datastart = memory_entries[0][0]
                new_content.append("const int dataStart = {};\n".format(datastart))
            elif "void initConsts" in line:
                # Write function update memory
                new_content.append("void initConsts(mem_t &mem) {\n")
                for k in memory_entries:
                    new_content.append("\tmemWrite(mem, {},{});\n".format(k[0], k[1]))
                new_content.append("}\n")
            else:
                new_content.append(line)

        # Write file
        output_path = os.path.join("tmp", output_name)
        output = open(output_path, "w")
        output.writelines(new_content)

    except FileNotFoundError:
        print("File {} not found".format(ufile))


def get_filename(file):
    return os.path.basename(os.path.splitext(file)[0])


if __name__ == "__main__":
    # Parser
    parser = argparse.ArgumentParser(
        prog='extract_variables',
        description='Extract variables from a .rodata file and generate Romeo function')
    parser.add_argument('file',
                        help='path to the rodata file')
    parser.add_argument('--output',
                        help='output filename (default: [input filename].var)')
    parser.add_argument('-u',
                        default=False, action='store_true',
                        help='Update declaration file with data')
    parser.add_argument('-ufile',
                        default="examples/declarations2.c",
                        help="if -u is set, set the input declaration file (default: examples/declarations2.c)'")
    args = parser.parse_args()

    # Parsing function
    file = open(args.file, "r")
    content = file.readlines()
    file.close()
    memory_entries = parse_rodata(content)

    # TODO: temp code, need to be placed somewhere in the workflow
    datastart = memory_entries[0][0]
    print("Datastart: {}".format(datastart))
    for k in memory_entries:
        print("memWrite(mem, {},{});".format(k[0], k[1]))

    if args.u:
        output_name = "{}_{}.c".format(get_filename(args.ufile), get_filename(args.file))
        write_in_file(args.ufile, memory_entries, output_name)
