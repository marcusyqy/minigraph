import subprocess
import os
import sys
import glob
import shutil

# TODO: check if cl exists
compiler = "cl" 
# "/Fe:build/", "/Fo:build/",  moved into body
flags = [ "/std:c++17", "/Zc:__cplusplus", "/wd4530" ]

def which(program):
    def is_exe(fpath):
        return os.path.isfile(fpath) and os.access(fpath, os.X_OK)

    fpath, fname = os.path.split(program)
    if fpath:
        if is_exe(program):
            return program
    else:
        for path in os.environ.get("PATH", "").split(os.pathsep):
            exe_file = os.path.join(path, program)
            if is_exe(exe_file):
                return exe_file

    return None

def create_dir_if_dir_doesnt_exists(name):
    if os.path.exists(name) and not os.path.isdir(name):
        os.remove(name)
    if not os.path.exists(name):
        os.mkdir(name)

class Target:
    def __init__(self, target, files):
        self.args = [compiler]
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

    def format(self, target):
        cmd = ['clang-format', '-i', '--verbose']
        for f in glob.glob(f"{target}/**.hpp"):
            cmd.append(f)
        for f in glob.glob(f"{target}/**.cpp"):
            cmd.append(f)
        subprocess.run(cmd)

# clean garbage up
def process_args(argv):
    should_build_test = False
    should_build_examples = False
    should_format_repo = False
    #all flags present if no args are being passed.
    if len(argv) == 0:
        should_build_examples = True
        should_build_test = True

    for f in argv:
        if f == "examples":
            should_build_examples = True
        if f == "test":
            should_build_test = True
        if f == "format":
            should_format_repo = True

    return should_build_test, should_build_examples, should_format_repo

if __name__ == "__main__":
    should_build_tests, should_build_examples, should_format_repo = process_args(sys.argv[1:])

    # check if compiler exists
    compiler_file_path = shutil.which(compiler)
    if (should_build_tests or should_build_examples) and compiler_file_path == None:
        print(f"compiler `{compiler}` does not exist. Did you forget to source it?")
        exit(1)

    solution = Solution()
    if should_build_examples:
        print("BUILDING EXAMPLES")
        solution.build("examples")

    if should_build_tests:
        print("BUILDING TESTS")
        solution.build("test")
        print("RUNNING TESTS")
        solution.run("test")

    if should_format_repo:
        print("FORMATTING")
        solution.format("minigraph")
        solution.format("examples")
        solution.format("test")

