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

def main():
    data_dir = 'data'
    out_dir = 'source'

    extensions = ('.png', '.gmd', '.ttf', '.jpg')

    for root, dirs, files in os.walk(data_dir):
        for file in files:
            if file.lower().endswith(extensions):
                full_path = os.path.join(root, file)
                print(f"Processing {full_path}...")
                bin_to_header(full_path, out_dir)

if __name__ == "__main__":
    main()
