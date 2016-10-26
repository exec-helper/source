from exec_utils.util.util import *
from exec_utils.options.options import *

from cm.command import *

class Target:
    def __init__(self, targets, runTargets, compilers, profileMap = '.exec-helper_profiles', rootBuildDir = 'build/{COMPILER}/{MODE}', builder = 'fake'):
        self.targets = replaceWith(targets, 'all', Target.getAllTargetNames())
        self.runTargets = replaceWith(runTargets, 'all', Target.getAllRunTargets()) 
        self.compilers = compilers
        self.profileMap = profileMap
        self.rootBuildDir = rootBuildDir
        self.builder = builder

    def getRunTargets(self):
        runTargets = []
        for runTarget in self.runTargets:
            runTargets.append(Target(self.target, runTarget, self.compilers))
        return runTargets

    def getAllBinaries(self):
        binaryTargets = []
        for target in self.targets:
            for runTarget in self.runTargets:
                if runTarget == 'bin':
                    binaryTargets.extend(['bin/' + target])
                elif runTarget == 'unittest':
                    binaryTargets.extend(['bin/test/' + runTarget + '/' + target + '-' + runTarget])
                else:
                    raise ValueError("Unknown run target: '" + runTarget + "'")
        return binaryTargets

    def getProfileMapOptions(self):
        return Command.getProfileMapOption(self.profileMap)

    def getCompilers(self):
        return self.compilers

    def getCompilerOptions(self):
        return Command.getCompilerOption(self.compilers)

    def getTargetOptions(self):
        return Command.getTargetOption(self.targets)

    def getRunTargetOptions(self):
        return Command.getRunTargetOption(self.runTargets)

    def getModeOptions(self):
        modes = UniqueOrderedList()
        for compiler in self.compilers:
            modes.append(compiler.getMode())
        return Command.getModeOption(modes)

    def getBuilderOptions(self):
        return Command.getBuilderOption(self.builder)

    def getRootBuildDirOptions(self):
        return Command.getRootBuildDirOption(self.rootBuildDir)

    def getRootBuildDir(self, compiler):
        rootBuildDir = self.rootBuildDir
        rootBuildDir = rootBuildDir.replace('{COMPILER}', compiler.getName())
        rootBuildDir = rootBuildDir.replace('{MODE}', compiler.getMode())
        return rootBuildDir

    @staticmethod
    def getAllRunTargets():
        return ['bin', 'unittest']

    @staticmethod
    def getAllTargets():
        targets = []
        for target in getAllTargetNames():
            targets.append(Target(target, 'all', getAllCompilers()))
        return targets

    @staticmethod
    def getAllTargetNames():
        return ['example']


