import subprocess
import os
import glob

default_args = [ "cl", "/Fo:build/", "/EHsc" ]

class Target:
    def __init__(self, files):
        self.args = default_args.copy()
        self.args.append('/Fe:build/')
        for f in files:
            self.args.append(f)

    def __call__(self):
        subprocess.run(self.args)

class SingleTarget(Target):
    def __init__(self, file):
        super().__init__([file])

    def __call__(self):
        super().__call__()

def make_build_dir():
    if os.path.exists("build") and not os.path.isdir("build"):
        os.remove("build")
    if not os.path.exists("build"):
        os.mkdir("build")

def build_examples():
    args = default_args.copy()
    files = glob.glob("examples/**.cpp")
    for f in files:
        SingleTarget(f)()

if __name__ == "__main__":
    make_build_dir()
    build_examples()