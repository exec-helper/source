import argparse

from exec_utils.util.util import *
from exec_utils.profileMap.parse import *
from exec_utils.buildSystem.scons import *
from exec_utils.buildSystem.make import *
from exec_utils.buildSystem.fakeBuilder import *

def getArgParser(options):
    commandOptions = ['init', 'build', 'clean', 'distclean', 'rebuild', 'run', 'analyze', 'profile']
    buildModeOptions = ['debug', 'release']
    compilerOptions = ['gcc', 'clang']
    analyzeOptions = getAnalyzeMethods()
    profileOptions = ['perf', 'callgrind']

    parser = argparse.ArgumentParser(description='Convenience script for executing commands')
    parser.add_argument('commands', metavar='commands', nargs='+', choices=commandOptions,
		   help="Commands to execute. Possible values: {0}.".format(commandOptions))
    parser.add_argument('-m', '--mode', nargs='+', choices=buildModeOptions, default=options.getModes(),
		   help="Build mode to use.")
    parser.add_argument('-t', '--target', nargs='+', default=options.getTargets(),
		   help="Target to build.")
    parser.add_argument('-r', '--run', nargs='+', default=options.runTargets,
		   help="Modes of the target to run.")
    parser.add_argument('-c', '--compiler', nargs='+', choices=compilerOptions, default=options.getCompilers(),
		   help="Compiler to use.")
    parser.add_argument('-d', '--show-stuff', action='store_true', help="Enable this to automatically open or show the results")
    parser.add_argument('-v', '--verbose-make', action='store_true', help="Enable make in verbose mode")
    parser.add_argument('-s', '--build-single-threaded', action='store_true', help="Build in single threaded mode")
    parser.add_argument('-a', '--analyze-method', nargs='+', choices=analyzeOptions, default=options.getAnalyzeMethods(),
		   help="Run analysis.")
    parser.add_argument('-l', '--profile-method', nargs='+', choices=profileOptions, default=options.getProfileMethods(),
		   help="Select performance tool.")
    parser.add_argument('-p', '--toolchain-path', nargs=1, default=options.getToolchainPath(),
		   help="Path to the compiler")
    parser.add_argument('-i', '--profile-map', nargs=1, default=options.getProfileMap(),
                    help="Path to profile map")
    parser.add_argument('-b', '--build-system', nargs=1, default=options.getBuildSystemName(),
                    help="Build system to use")
    parser.add_argument('-e', '--root-build-dir', nargs=1, default=[options.getRootBuildDir()],
                    help="Root build directory to build in")
    return parser

class Options:
    def __init__(self, relative_path_reference = getCurrentDir()):
        self.commands = []
        self.modes = ['release']
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
        self.profileMap = '.exec-helper_profiles'
        self.relative_path_reference = relative_path_reference
        self.buildSystemName = ['scons']
        self.buildSystem = self.getBuildSystemFromName(self.buildSystemName[0])
        self.rootBuildDir = 'build'

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
        self.profileMap = args.profile_map[0]
        self.buildSystemName = args.build_system
        self.buildSystem = self.getBuildSystemFromName(self.buildSystemName[0])
        self.rootBuildDir = args.root_build_dir[0]

    def parseProfileMap(self, profileMapFile):
        profileMapData = parseProfileMap(self.profileMap)
        self.profiles = {}
        for profile in profileMapData['profileMap']:
            self.profiles[profile['id']] = Profile(profile['id'], profile['suffix'], profile['targetDirectory'])

        self.allTargets = profileMapData['targets']

        if 'compilers' in profileMapData:
            self.compilers = profileMapData['compilers']
        if 'modes' in profileMapData:
            self.modes = profileMapData['modes']
        if 'verbosity' in profileMapData and profileMapData['verbosity'] == 'yes':
            self.verbosity = True
        if 'single-threaded' in profileMapData and profileMapData['single-threaded'] == 'yes':
            self.buildSingleThreaded = True
        if 'analyze-methods' in profileMapData:
            self.analyzeMethods = profileMapData['analyzeMethods']
        if 'toolchain-path' in profileMapData:
            self.toolchainPath = profileMapData['toolchain-path']
        if 'show-stuff' in profileMapData:
            self.showStuff = profileMapData['show-stuff']
        if 'build-system' in profileMapData:
            self.buildSystemName = [profileMapData['build-system']]
        if 'root-build-dir' in profileMapData:
            self.rootBuildDir = profileMapData['root-build-dir']

    def __repr__(self):
        return "Commands: " + str(self.commands) + " " + '\n' + \
                    "Compilers: " + str(self.compilers) + " " + '\n' + \
                    "modes: " + str(self.modes) + " " + '\n' + \
                    "targets: " + str(self.targets) + '\n' + \
                    "Run targets: " + str(self.runTargets) + '\n' + \
                    "Verbosity: " + str(self.verbosity) + '\n' + \
                    "Build single threaded: " + str(self.buildSingleThreaded) + '\n' + \
                    "Analyze methods: " + str(self.analyzeMethods) + '\n' + \
                    "Toolchain path: " + str(self.toolchainPath) + '\n' + \
                    "Profile methods: " + str(self.profileMethods) + '\n' + \
                    "Show stuff: " + str(self.showStuff) + '\n' + \
                    "Profile map: " + str(self.profileMap) + '\n' + \
                    "Root build dir: " + str(self.rootBuildDir)

    def getCommands(self):
        replaceWith(self.commands, 'rebuild', ['distclean', 'build'])
        return self.commands

    def getModes(self):
        return self.modes

    def getTargets(self):
        return self.targets

    def getRunTargets(self):
        allTarget = 'all'
        realRunTargets = getRunTargets(self.profiles, False)
        if realRunTargets:
            replaceWith(self.runTargets, allTarget, realRunTargets) 
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
        replaceWith(self.analyzeMethods, 'all', getAnalyzeMethods(False)) 
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

    def getProfiles(self, runModes):
        allProfiles = self.getAllProfiles()
        profiles = []
        for runMode in runModes:
            profiles.append(allProfiles[runMode])
        return profiles

    def getAllProfiles(self):
        return self.profiles

    def getBuildSystem(self):
        return self.buildSystem

    def getBuildSystemName(self):
        return self.buildSystemName

    def getAllTargets(self):
        return self.allTargets

    def getRootBuildDir(self):
        return self.rootBuildDir

    @staticmethod
    def getBuildSystemFromName(buildSystemName):
        if buildSystemName == 'scons':
            return Scons()
        elif buildSystemName == 'make':
            return Make()
        elif buildSystemName == 'fake':
            return FakeBuilder()
        else:
            raise ValueError('Error: invalid build system name: ' + buildSystemName)
