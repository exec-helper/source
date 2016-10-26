from cm.unique_ordered_list import *

class Command:
    @staticmethod
    def distclean(profileMap = '.exec-helper_profiles'):
        cmd = ['distclean']
        cmd.extend(getProfileMap())
        return cmd

    @staticmethod
    def clean(profileMap = '.exec-helper_profiles'):
        cmd = ['clean']
        cmd.extend(getProfileMap())
        return cmd

    @staticmethod
    def getBuildCommand(profileMap = '.exec-helper_profiles'):
        cmd = ['build']
        cmd.extend(Command.getProfileMapOption())
        return cmd

    @staticmethod
    def getTargetOption(targets):
        return Command.buildOption('--target', targets)

    @staticmethod
    def getRunTargetOption(runTargets):
        return Command.buildOption('--run', runTargets)

    @staticmethod
    def getModeOption(modes):
        return Command.buildOption('--mode', modes)

    @staticmethod
    def getCompilerOption(compilers):
        compilerNames = []
        for compiler in compilers:
            compilerNames.append(compiler.getName())
        return Command.buildOption('--compiler', compilerNames)

    @staticmethod
    def getBuilderOption(builderName):
        return Command.buildOption('--build-system', [builderName])

    @staticmethod
    def getRootBuildDirOption(rootBuildDir):
        return Command.buildOption('--root-build-dir', [rootBuildDir])

    @staticmethod
    def buildOption(optionCommand, options):
        optionLine = UniqueOrderedList([optionCommand])
        for option in options:
            optionLine.append(option)
        return optionLine

    @staticmethod
    def getProfileMapOption(profileMap = '.exec-helper_profiles'):
        if profileMap is None:
            return []
        else:
            return ['--profile-map', profileMap]

