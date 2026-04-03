import os

def generate_all_assets_header(data_dir, out_file):
    assets = []
    # Only include data assets, exclude audio
    extensions = ('.png', '.gmd', '.ttf', '.jpg')

    for root, dirs, files in os.walk(data_dir):
        for file in files:
            if file.lower().endswith(extensions):
                safe_name = file.replace('.', '_')
                assets.append(safe_name + '.h')

    assets = sorted(list(set(assets)))

    with open(out_file, 'w') as f:
        f.write('#ifndef ALL_ASSETS_H\n')
        f.write('#define ALL_ASSETS_H\n\n')
        for asset in assets:
            f.write(f'#include "{asset}"\n')
        f.write('\n#endif // ALL_ASSETS_H\n')

if __name__ == "__main__":
    generate_all_assets_header('data', 'source/all_assets.h')
