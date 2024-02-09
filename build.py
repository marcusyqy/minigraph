import subprocess
import os
import sys
import glob

# TODO: check if cl exists
compiler = [ "cl"] 
# "/Fe:build/", "/Fo:build/",  moved into body
flags = [ "/EHsc", "/std:c++17", "/Zc:__cplusplus" ]

def create_dir_if_dir_doesnt_exists(name):
    if os.path.exists(name) and not os.path.isdir(name):
        os.remove(name)
    if not os.path.exists(name):
        os.mkdir(name)

class Target:
    def __init__(self, target, files):
        self.args = compiler.copy()
        for f in files:
            self.args.append(f)
        for f in flags:
            self.args.append(f)

        self.args.append(f"/Fo:build/{target}/")
        self.args.append(f"/Fe:build/{target}/")

    def __call__(self):
        subprocess.run(self.args)

class SingleTarget(Target):
    def __init__(self, target, file):
        super().__init__(target, [file])

    def __call__(self):
        super().__call__()

class Solution: 
    def __init__(self):
        create_dir_if_dir_doesnt_exists("build")

    def build(self, target):
        create_dir_if_dir_doesnt_exists(f"build/{target}")
        files = glob.glob(f"{target}/**.cpp")
        for f in files:
            SingleTarget(target, f)()

    def run(self, target):
        create_dir_if_dir_doesnt_exists(f"build/{target}")
        files = glob.glob(f"build/{target}/**.exe")
        for f in files:
            subprocess.run([f])

# clean garbage up
def process_args(argv):
    should_build_test = False
    should_build_examples = False
    #all flags present if no args are being passed.
    if len(argv) == 0:
        should_build_examples = True
        should_build_test = True
    for f in argv:
        if f == "examples":
            should_build_examples = True
        if f == "test":
            should_build_test = True
    return should_build_test, should_build_examples

if __name__ == "__main__":
    should_build_tests, should_build_examples = process_args(sys.argv[1:])

    solution = Solution()
    if should_build_examples:
        print("BUILDING EXAMPLES")
        solution.build("examples")

    if should_build_tests:
        print("BUILDING TESTS")
        solution.build("test")
        print("RUNNING TESTS")
        solution.run("test")

