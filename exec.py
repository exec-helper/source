#!/usr/bin/python

import inspect

from exec_utils.util.util import *
from exec_utils.cmd.cmd import *
from exec_utils.options.options import *
from exec_utils.target.target import *
import sys

EXIT_SUCCESS = 0
EXIT_ERROR = 1

def execute(options):
    target = Target(options)
    for command in options.getCommands():
        if(command == 'init'):
            if not init(options, target):
                return False
        elif(command == 'build'):
            if not build(options, target):
                print("Build failed")
                return False
        elif(command == 'clean'):
            if not clean(options, target):
                print("Clean failed")
                return False
        elif(command == 'distclean'):
                if not distclean(options, target):
                    return False
        elif(command == 'run'):
            if not run(options, target):
                return False
        elif(command == 'analyze'):
            if not analyze(options, target):
                return False
        elif(command == 'profile'):
            if not profile(options, target):
                return False
        else:
            print("Error: invalid command")
            return False

    return True


def main():
    real_path_to_this_file = os.path.dirname(os.path.realpath(__file__))        # The real path is the dirname of the completely resolved path (no symlinks)
    args = Options(real_path_to_this_file)        # Until Options.parse() is called, the getters will return the default values
    firstPhaseParser = getArgParser(args)

    # First parse the argument to find the defaults in settings file
    firstPhaseArguments = firstPhaseParser.parse_args()
    print('Profile map file = ' + firstPhaseArguments.profile_map[0])
    args.parseProfileMap(firstPhaseArguments.profile_map[0])

    # Find the options overridden on the command line
    secondPhaseParser = getArgParser(args)
    args.parse(secondPhaseParser.parse_args())

    print("Configuration: ")
    print(str(args))

    retCode = execute(args)

    if retCode:
        sys.exit(EXIT_SUCCESS)
    else:
        sys.exit(EXIT_ERROR)

if __name__ == "__main__":
    main()
