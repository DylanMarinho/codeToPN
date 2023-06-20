import argparse
import os

output_dir = "tmp/"


def extract_adress(line):
    """
    From a line of a rodata file, extract the adress
    :param line: Line of rodata
    :return: Adress
    """
    address = line.split("\t")[0]
    address = address.replace(" ", "")
    address = address[:-1]
    return address


def get_last_instruction(compiled_file, file_name):
    """
    From a compiled file, try to return the last instruction
    :param compiled_file: Path to a, ARM compiled file
    :return: Last instruction
    """
    # We return the last adress which is not a nop or a word

    objdump_output = os.path.join(output_dir, file_name + ".objdump")

    # objdump
    os.system("arm-none-eabi-objdump -d {} > {}".format(compiled_file, objdump_output))

    # read extracted file
    f = open(objdump_output, "r")
    content = f.readlines()
    last_address = 0
    keyword_to_exclude = [".word", "nop"]
    for line in content:
        last_instruction = True
        for keyword in keyword_to_exclude:
            if keyword in line:
                last_instruction = False
        if last_instruction:
            last_address = extract_adress(line)
    print("The last instruction found is '{}'".format(last_address))
    return last_address


def run(file_name, file_path=""):
    """
    From a file_name, generate the PN
    :param file_name:
    :return: PN file
    """

    input_file = os.path.join(file_path, file_name) + ".c"
    compiled_file = os.path.join(output_dir, file_name)
    bin_file = compiled_file + ".bin"
    rowdata_file = compiled_file + ".rowdata"
    output_xml_file = rowdata_file = compiled_file + ".xml"
    instructions_file = os.path.join(output_dir, "instructions_{}".format(file_name) + ".c")

    # compile
    os.system(
        "arm-none-eabi-gcc {} -o {} -mcpu=cortex-m4 -mthumb -mfloat-abi=soft -mfpu=fpv4-sp-d16 -nostartfiles -fno-builtin --specs=nosys.specs -nostdlib".format(
            input_file, compiled_file
        ))

    # extract
    os.system("arm-none-eabi-objdump -d {} | awk -f src/extract.awk > {}".format(compiled_file, bin_file))

    # Extract last instruction
    last_instruction = get_last_instruction(compiled_file, file_name)

    # extract rowdata
    os.system("arm-none-eabi-objdump -s -j .rodata {} > {}".format(compiled_file, rowdata_file))
    os.system("python3 src/extract_variables.py {} -u".format(rowdata_file))

    # Extract instructions
    os.system("cat {} | src/extract {} > {}".format(bin_file, last_instruction, instructions_file))

    # Rename output
    os.system("mv {} {}".format("program.xml", output_xml_file))


if __name__ == "__main__":
    # Parser
    parser = argparse.ArgumentParser(
        prog='main',
        description='From a c programm, generate a Petri net')
    parser.add_argument('file',
                        help='path to the c file')
    args = parser.parse_args()

    file_name = os.path.basename(os.path.splitext(args.file)[0])
    file_path = os.path.dirname(args.file)
    run(file_name, file_path)
