#!/usr/bin/env python3
import os, subprocess
import sys
from pathlib import Path

def main():
    passed = failed = 0
    for input_file in sorted((Path(__file__).parent.parent / "tests/data_input").glob("*.txt")):
        print(f"{input_file}")
        expected_file = Path(__file__).parent.parent / ("tests/data_output/expected" + input_file.stem[5:] + ".txt")
        result = subprocess.run([Path(__file__).parent.parent / f"build/club_app{'.exe' if sys.platform == 'win32' else ''}", input_file], capture_output=True, text=True, check=False)
        
        actual = result.stdout.strip()
        expected = expected_file.read_text().strip()

        if actual == expected:
            print("PASSED")
            passed += 1
        else:
            print("FAILED")
            print(f"Expected: {expected}")
            print(f"Actual: {actual}")
            if result.stderr: print(f"Stderr: {result.stderr.strip()}")
            failed += 1
    
    print(os.linesep + f"Summary: {passed} passed, {failed} failed.")

main()