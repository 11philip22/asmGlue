import argparse
import os
import sys

StartMarker = 'MARKER:S'
EndMarker  = 'MARKER:E'

NativeTemplate = """
    LPSTR shellCodeA32 = const_cast<LPSTR>("{}");
    LPSTR shellCodeA64 = const_cast<LPSTR>("{}");
    LPSTR shellCodeB32 = const_cast<LPSTR>("{}");
    LPSTR shellCodeB64 = const_cast<LPSTR>("{}");
    DWORD shellCodeA32Length = {}, shellCodeA64Length = {};
    DWORD shellCodeB32Length = {}, shellCodeB64Length = {};
    """


def main():
    parser = argparse.ArgumentParser(description='sRDI Blob Encoder', conflict_handler='resolve')
    parser.add_argument('solution_dir', help='Solution Directory')
    arguments = parser.parse_args()

    binFileA32 = os.path.join(arguments.solution_dir, 'bin', 'ShellcodeA_x86.bin')
    binFileA64 = os.path.join(arguments.solution_dir, 'bin', 'ShellcodeA_x64.bin')
    binFileB32 = os.path.join(arguments.solution_dir, 'bin', 'ShellcodeB_x86.bin')
    binFileB64 = os.path.join(arguments.solution_dir, 'bin', 'ShellcodeB_x64.bin')

    native_file = os.path.join(arguments.solution_dir, 'ShellShock/ShellShock.cpp')

    if not os.path.isfile(binFileA32) or not os.path.isfile(binFileA64) \
    or not os.path.isfile(binFileB32) or not os.path.isfile(binFileB64):
        print("[!] ShellcodeRDI_x86.bin and ShellcodeRDI_x64.bin files weren't in the bin directory")
        return

    binDataA32 = open(binFileA32, 'rb').read()
    binDataA64 = open(binFileA64, 'rb').read()
    binDataB32 = open(binFileB32, 'rb').read()
    binDataB64 = open(binFileB64, 'rb').read()

    # Patch the native loader

    native_insert = NativeTemplate.format(
        ''.join('\\x{:02X}'.format(b) for b in binDataA32),
        ''.join('\\x{:02X}'.format(b) for b in binDataA64),
        ''.join('\\x{:02X}'.format(b) for b in binDataB32),
        ''.join('\\x{:02X}'.format(b) for b in binDataB64),
        len(binDataA32), len(binDataA64),
        len(binDataB32), len(binDataB64)
    )

    code = open(native_file, 'r').read()
    start = code.find(StartMarker) + len(StartMarker)
    end = code.find(EndMarker) - 2 # for the //
    code = code[:start] + native_insert + code[end:] 
    open(native_file, 'w').write(code)

    print('[+] Updated {}'.format(native_file))

    print("")

if __name__ == '__main__':
    main()
