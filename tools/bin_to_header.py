import os
import sys

def bin_to_header(bin_file, out_dir):
    filename = os.path.basename(bin_file)
    safe_name = filename.replace('.', '_')
    header_file = os.path.join(out_dir, safe_name + '.h')
    c_file = os.path.join(out_dir, safe_name + '.c')

    with open(bin_file, 'rb') as f:
        data = f.read()

    size = len(data)

    with open(header_file, 'w') as f:
        f.write(f'#ifndef {safe_name.upper()}_H\n')
        f.write(f'#define {safe_name.upper()}_H\n\n')
        f.write(f'#include <stddef.h>\n')
        f.write(f'#ifdef __cplusplus\nextern "C" {{\n#endif\n')
        f.write(f'extern const unsigned char {safe_name}[];\n')
        f.write(f'extern const size_t {safe_name}_size;\n')
        f.write(f'#ifdef __cplusplus\n}}\n#endif\n\n')
        f.write(f'#endif // {safe_name.upper()}_H\n')

    with open(c_file, 'w') as f:
        f.write(f'#include "{safe_name}.h"\n\n')
        f.write(f'const unsigned char {safe_name}[] = {{\n')
        for i in range(0, size, 12):
            chunk = data[i:i+12]
            hex_chunk = ', '.join([f'0x{b:02x}' for b in chunk])
            f.write(f'    {hex_chunk},\n')
        f.write(f'}};\n\n')
        f.write(f'const size_t {safe_name}_size = {size};\n')

if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("Usage: python bin_to_header.py <input_bin> <output_dir>")
        sys.exit(1)
    bin_to_header(sys.argv[1], sys.argv[2])
