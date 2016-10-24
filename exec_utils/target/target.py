import copy
from exec_utils.util.util import *

ALL_TARGET = 'all'

class Target:
    def __init__(self, options):
        self.modes = options.getModes()
        self.targets= options.getTargets()
        self.allTargets = options.getAllTargets()
        self.compilers = options.getCompilers()
        self.buildSingleThreaded = options.getBuildSingleThreaded()
        self.analyzeMethods = options.getAnalyzeMethods()
        self.profileMethods = options.getProfileMethods()
        self.toolchainPath = options.getToolchainPath()
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
                    "Toolchain path: " + str(self.toolchainPath)

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

    def getTargets(self, replaceAllTarget = True):
        targetNames = self.targets

        if replaceAllTarget and self.allTargets:
            replaceWith(targetNames, ALL_TARGET, self.allTargets) 
        print("Target names = " + str(targetNames))

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
        print("Modes = " + str(modes))
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
            else:
                newTargetNames = targetNames
            targetNames = newTargetNames
        return targetNames
