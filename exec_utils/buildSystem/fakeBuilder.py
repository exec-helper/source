import os
import shutil
import stat

from ..profileMap.profile import *

class FakeBuilder:
    def __init__(self):
        pass

    @staticmethod
    def getTargetBuildDirectory(target):
        rootBuildDir = target.getRootBuildDir()
        profile = target.getProfile()
        return rootBuildDir + '/' + profile.targetDirectory

    @staticmethod
    def getTargetBuildFile(target):
        profile = target.getProfile()
        targetName = target.getTargetName()
        return FakeBuilder.getTargetBuildDirectory(target) + '/' + targetName + profile.suffix

    @staticmethod
    def getExecutableContent():
        return \
                '#!/usr/bin/python\n' \
                '# File created by the fakeBuilder module\n' \
                '\n' \
                'import os\n' \
                '\n' \
                'fileName = __file__ + ".output"\n' \
                '\n' \
                'numberOfExecutions = 0\n' \
                'if os.path.isfile(fileName):\n' \
                '    with open(fileName, "r") as f:\n' \
                '        numberOfExecutions = int(f.read())\n' \
                '\n' \
                'numberOfExecutions = numberOfExecutions + 1\n' \
                'print("Number of executions = " + str(numberOfExecutions))\n' \
                '\n' \
                'with open(fileName, "w") as f:\n' \
                '    f.write(str(numberOfExecutions))\n'

    def init(self, workingDir, mode):
        return True

    def build(self, target, verbose, options, prependCommand = None):
        targetBuildDirectory = self.getTargetBuildDirectory(target)
        try:
            # Create target build directory if it does not exist
            if not os.path.isdir(targetBuildDirectory):
                os.makedirs(targetBuildDirectory)
        except:
            print("Error: could not create directory '" + targetBuildDirectory + "'")
            return False

        targetBuildFile = self.getTargetBuildFile(target)

        # Write the file
        try:
            with open(targetBuildFile, 'w') as f:
                f.write(self.getExecutableContent())
        except:
            print("Error: could not create file '" + targetBuildFile + "'")
            return False

        # Make it executable
        fileStatus = os.stat(targetBuildFile)
        os.chmod(targetBuildFile, fileStatus.st_mode | stat.S_IEXEC)

        return True

    def clean(self, options, target, verbose):
        targetBuildFile = self.getTargetBuildFile(target)
        try:
            os.remove(targetBuildFile)
        except:
            print("Error: could not remove file '" + targetBuildFile + "'")
            return False
        return True

    def distclean(self, options, target):
        rootBuildDir = target.getRootBuildDir()
        try:
            if os.path.isdir(rootBuildDir):
                print("Removing '" + rootBuildDir + "'")
                shutil.rmtree(rootBuildDir)
        except:
            print("Error: could not remove directory '" + rootBuildDir + "'")
            return False
        return True
