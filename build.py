import subprocess
import os
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
        #print(f"running {self.args}")
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

if __name__ == "__main__":
    solution = Solution()
    # print("BUILDING EXAMPLES")
    # solution.build("examples")
    print("BUILDING TESTS")
    solution.build("test")
    print("RUNNING TESTS")
    solution.run("test")
