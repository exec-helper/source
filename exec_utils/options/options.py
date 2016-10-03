from exec_utils.util.util import *

class Options:
    def __init__(self, relative_path_reference = getCurrentDir()):
        self.commands = []
        self.modes = 'release'
        self.targets= ['all']
        self.runTargets= ['all']
        self.compilers = ['gcc']
        self.showStuff = False
        self.verbosity = False
        self.buildSingleThreaded = False
        self.analyzeMethods = ['all']
        self.profileMethods = ['perf']
        self.toolchainPath = ['']
        self.currentDir = getCurrentDir()
        self.profileMap = 'exec-helper_profiles'
        self.relative_path_reference = relative_path_reference
        pass 

    def parse(self, args):
        """ Parse the given arguments. Calling the respective getters before this function is called, results in the default values being returned. """
        self.commands = args.commands
        self.modes = args.mode
        self.targets = args.target
        self.runTargets = args.run
        self.compilers = args.compiler
        self.verbosity = args.verbose_make
        self.buildSingleThreaded = args.build_single_threaded
        self.analyzeMethods = args.analyze_method
        self.toolchainPath = args.toolchain_path[0]
        self.profileMethods = args.profile_method 
        self.showStuff = args.show_stuff
        if args.profile_map:
            relative_path = os.path.relpath(args.profile_map[0], self.relative_path_reference)
            filename,unused_ = os.path.splitext(relative_path)
            self.profileMap = filename.replace("/", ".")

    @staticmethod
    def replaceWith(hayStack, needle, replacementNeedle):
        while hayStack.count(needle) > 0:
            index = hayStack.index(needle)
            hayStack.pop(index)
            insertIndex = index
            for replacement in replacementNeedle:
                hayStack.insert(insertIndex, replacement) 
                insertIndex = insertIndex + 1

    def getCommands(self):
        self.replaceWith(self.commands, 'rebuild', ['distclean', 'build'])
        return self.commands

    def getModes(self):
        return self.modes

    def getTargets(self):
        realTargets = getTargets(self.currentDir, False)
        if realTargets:
            self.replaceWith(self.targets, 'all', realTargets) 
        return self.targets

    def getRunTargets(self):
        allTarget = 'all'
        realRunTargets = getRunTargets(self.getProfileMap(), False)
        if realRunTargets:
            self.replaceWith(self.runTargets, allTarget, realRunTargets) 
        else:
            while(self.runTargets.count(allTarget) > 0):
                self.runTargets.remove(allTarget)
        return self.runTargets

    def getCompilers(self):
        return self.compilers

    def getVerbosity(self):
        return self.verbosity

    def getBuildSingleThreaded(self):
        return self.buildSingleThreaded

    def getAnalyzeMethods(self):
        self.replaceWith(self.analyzeMethods, 'all', getAnalyzeMethods(False)) 
        return self.analyzeMethods

    def getToolchainPath(self):
        return self.toolchainPath

    def getProfileMethods(self):
        return self.profileMethods

    def getShowStuff(self):
        return self.showStuff

    def getCurrentDir(self):
        return self.currentDir

    def getProfileMap(self):
        return self.profileMap
