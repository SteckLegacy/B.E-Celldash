import os
import subprocess
import sys

def main():
    tools_dir = os.path.dirname(os.path.abspath(__file__))
    bin_to_header_py = os.path.join(tools_dir, 'bin_to_header.py')
    generate_all_assets_py = os.path.join(tools_dir, 'generate_all_assets_h.py')

    data_dir = os.path.join(os.path.dirname(tools_dir), 'data')
    source_dir = os.path.join(os.path.dirname(tools_dir), 'source')

    print("--- Procesando recursos para Visual Studio ---")

    # 1. Ejecutar bin_to_header para cada archivo en data
    for root, dirs, files in os.walk(data_dir):
        for file in files:
            if file.endswith(('.png', '.gmd', '.mp3', '.ogg', '.ttf', '.jpg')):
                full_path = os.path.join(root, file)
                print(f"Procesando: {file}")
                subprocess.run([sys.executable, bin_to_header_py, full_path, source_dir], check=True)

    # 2. Generar all_assets.h
    print("Generando all_assets.h...")
    subprocess.run([sys.executable, generate_all_assets_py], check=True)

    print("--- Proceso completado con éxito ---")

if __name__ == "__main__":
    main()
