import argparse

from os import sys, path
sys.path.append(path.dirname(path.dirname(path.abspath(__file__ + '/../exec_utils'))))
from cm.cm import *

def parseArgs():
    pythonOptions = [PYTHON2, PYTHON3]
    compilerOptions = [GCC, CLANG]

    parser = argparse.ArgumentParser(description='Integration test parameters')
    parser.add_argument('-c', '--compiler', nargs='+', choices=compilerOptions, default=[GCC],
		   help="Compiler to test.")
    parser.add_argument('-p', '--python-version', nargs=1, choices=pythonOptions, default=[PYTHON3],
		   help="Python version to test.")
  
    args = parser.parse_args()
    UnittestOptions.setPythonVersion(args.python_version[0])

    for compiler in args.compiler:
        if compiler == GCC:
            UnittestOptions.addCompiler(Gcc(COMPILER_RELEASE_MODE, '/random/compiler/path'))
        elif compiler == CLANG:
            UnittestOptions.addCompiler(Clang(COMPILER_RELEASE_MODE, '/random/compiler/path'))
        else:
            print("Error: unkown compiler")

    print("Python version = " + UnittestOptions.getPythonVersion())

    compilerNames = []
    for compiler in UnittestOptions.getCompilers():
        compilerNames.append(compiler.getName())

    print("Compilers =  " + str(compilerNames))

    return args


