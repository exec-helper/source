import copy
from exec_utils.util.util import *
from .compiler import *

ALL_TARGET = 'all'

class Target:
    def __init__(self, options):
        self.modes = options.getModes()
        self.targets= options.getTargets()
        self.allTargets = options.getAllTargets()
        self.toolchainPath = options.getToolchainPath()
        self.compilers = self.names2Compilers(options.getCompilers(), self.toolchainPath)
        self.buildSingleThreaded = options.getBuildSingleThreaded()
        self.analyzeMethods = options.getAnalyzeMethods()
        self.profileMethods = options.getProfileMethods()
        self.rootBuildDir = options.getRootBuildDir()
        self.architectures = options.getArchitectures()
        self.distributions = options.getDistributions()
        self.profiles = []
        allProfiles = options.getAllProfiles()
        for runTarget in options.getRunTargets():
            self.profiles.append(allProfiles[runTarget])

    def __repr__(self):
        return \
                    "Modes: " + str(self.modes) + " " + '\n' + \
                    "Targets: " + str(self.targets) + '\n' + \
                    "All targets: " + str(self.allTargets) + '\n' + \
                    "Profiles: " + str(self.profiles) + '\n' + \
                    "Compilers: " + str(self.compilers) + " " + '\n' + \
                    "Build single threaded: " + str(self.buildSingleThreaded) + '\n' + \
                    "Analyze methods: " + str(self.analyzeMethods) + '\n' + \
                    "Profile methos: " + str(self.profileMethods) + '\n' + \
                    "Toolchain path: " + str(self.toolchainPath) + '\n' + \
                    "Architectures: " + str(self.architectures) + '\n' + \
                    "Distributions: " + str(self.distributions) + '\n'

    def getTargetName(self):
        return self.targets[0]

    def getMode(self):
        return self.modes[0]

    def getProfile(self):
        return self.profiles[0]

    def getCompiler(self):
        return self.compilers[0]

    def getToolchainPath(self):
        return self.toolchainPath

    def getBuildSingleThreaded(self):
        return self.buildSingleThreaded

    def getAnalyzeMethod(self):
        return self.analyzeMethods[0]

    def getProfileMethod(self):
        return self.profileMethods[0]

    def getArchitecture(self):
        return self.architectures[0]

    def getDistribution(self):
        return self.distributions[0]

    def getRootBuildDir(self):
        rootDir = self.rootBuildDir
        rootDir = rootDir.replace('{DISTRIBUTION}', self.getDistribution())
        rootDir = rootDir.replace('{ARCHITECTURE}', self.getArchitecture())
        rootDir = rootDir.replace('{COMPILER}', self.getCompiler().getCCompiler())
        rootDir = rootDir.replace('{MODE}', self.getMode())
        return rootDir

    def getTargets(self, replaceAllTarget = True):
        targetNames = self.targets

        if replaceAllTarget and self.allTargets:
            replaceWith(targetNames, ALL_TARGET, self.allTargets) 

        targets = []
        for targetName in targetNames:
            newTarget = copy.copy(self)
            newTarget.targets = [targetName]
            targets.append(newTarget)
        return targets

    def getRunTargets(self):
        runTargetNames = self.profiles

        runTargets = []
        for profile in runTargetNames:
            newTargets = copy.copy(self)
            newTargets.profiles = [profile]
            runTargets.append(newTargets)
        return runTargets

    def getModes(self):
        modeNames = self.modes

        modes = []
        for modeName in modeNames:
            newTargets = copy.copy(self)
            newTargets.modes = [modeName]
            modes.append(newTargets)
        return modes

    def getCompilers(self):
        compilerNames = self.compilers

        compilers = []
        for compilerName in compilerNames:
            newTargets = copy.copy(self)
            newTargets.compilers = [compilerName]
            compilers.append(newTargets)
        return compilers

    def getProfilers(self):
        profileNames = self.profileMethods

        profileMethods = []
        for profileName in profileNames:
            newTargets = copy.copy(self)
            newTargets.profileMethods = [profileName]
            profileMethods.append(newTargets)
        return profileMethods

    def getAnalyzeMethods(self):
        analyzeNames = self.analyzeMethods

        analyzeMethods = []
        for analyzeName in analyzeNames:
            newTargets = copy.copy(self)
            newTargets.analyzeMethods = [analyzeName]
            analyzeMethods.append(newTargets)
        return analyzeMethods

    def getProfileMethods(self):
        profileNames = self.profileMethods

        profileMethods = []
        for profileName in profileNames:
            newTargets = copy.copy(self)
            newTargets.profileMethods = [profileName]
            profileMethods.append(newTargets)
        return profileMethods

    def getArchitectures(self):
        architectureNames = self.architectures

        architectures = []
        for architectureName in architectureNames:
            newTargets = copy.copy(self)
            newTargets.architectures = [architectureName]
            architectures.append(newTargets)
        return architectures

    def getDistributions(self):
        distributionNames = self.distributions

        distributions = []
        for distributionName in distributionNames:
            newTargets = copy.copy(self)
            newTargets.distributions = [distributionName]
            distributions.append(newTargets)
        return distributions

    def iterateTargets(self, *parameters):
        targetNames = [self]
        for parameter in parameters:
            newTargetNames = []
            if parameter == 'mode':
                for targetName in targetNames: 
                    newTargetNames.extend(targetName.getModes())
            elif parameter == 'target':
                for targetName in targetNames:
                    newTargetNames.extend(targetName.getTargets())
            elif parameter == 'compiler': 
                for targetName in targetNames:
                    newTargetNames.extend(targetName.getCompilers())
            elif parameter == 'runTarget':
                for targetName in targetNames:
                    newTargetNames.extend(targetName.getRunTargets())
            elif parameter == 'analyzeMethod':
                for targetName in targetNames:
                    newTargetNames.extend(targetName.getAnalyzeMethods())
            elif parameter == 'profileMethod':
                for targetName in targetNames:
                    newTargetNames.extend(targetName.getProfileMethods())
            elif parameter == 'architecture':
                for targetName in targetNames:
                    newTargetNames.extend(targetName.getArchitectures())
            elif parameter == 'distribution':
                for targetName in targetNames:
                    newTargetNames.extend(targetName.getDistributions())
            else:
                newTargetNames = targetNames
            targetNames = newTargetNames
        return targetNames

    @staticmethod
    def names2Compilers(names, toolchainPath):
        compilers = []

        for name in names:
            if name == 'gcc':
                compilers.append(Gcc(toolchainPath))
            elif name == 'clang':
                compilers.append(Clang(toolchainPath))
            else:
                raise ValueError('Unknown compiler: ' + name)

        return compilers
