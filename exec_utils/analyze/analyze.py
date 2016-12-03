#!/usr/bin/python

import os
import distutils.spawn

from ..util.util import *
from ..buildSystem.buildSystem import *
from ..runner.runner import runner
from ..filter.filterchain import FilterChain
from ..filter.valgrindMemcheck import ValgrindMemcheck
from ..target.compiler import ClangStaticAnalyzer

def analyzeClang(target, verbose, options):
    clangStaticAnalyzerCommand = []
    clangStaticAnalyzerCommand.append('scan-build')
    prefix = ''
    if target.getToolchainPath() is not None and target.getToolchainPath() != '':
        prefix = target.getToolchainPath() + '/' 
    else:
        prefix = os.path.dirname(distutils.spawn.find_executable(target.getCompiler().getCCompiler(), path=None)) + '/'
    if target.getCompiler().getCCompiler() == 'clang':
        clangStaticAnalyzerCommand.append('--use-analyzer=' + prefix + target.getCompiler().getCCompiler())
    clangStaticAnalyzerCommand.append('--use-cc=' + prefix + target.getCompiler().getCCompiler())
    clangStaticAnalyzerCommand.append('--use-c++=' + prefix + target.getCompiler().getCxxCompiler())
    return buildBuildSystem(target, options.getVerbosity(), options, prependCommand = clangStaticAnalyzerCommand)

def analyzeCppcheck(target, verbose):
    binary_name = 'cppcheck'
    if not isInstalled(binary_name):
        print('Please install ' + binary_name + ' or add it to your path.')

    includes = getAllDirsThatContainPattern(getSrcDir(), '*.h')

    targetDir = getSrcDir(target)

    cmd = [binary_name]
    cmd.append('--enable=all')
    cmd.append('--std=c++11')
    cmd.append('--force')
    cmd.append('--error-exitcode=1')
    if verbose:
        cmd.append('--verbose')
    else:
        cmd.append('-q')

    for include in includes:
        cmd.append('--include=' + include)

    cmd.append(targetDir)
#    cmd.append('--check-config')
    retValue = isSuccess(executeInShell(cmd))
    if(retValue):
        print('Cppcheck did not find major issues.')
    else:
        print('Cppcheck found major issues.')
    return retValue

def analyzeCpplint(target, verbose):
    binary_name = 'cpplint'
    if not isInstalled(binary_name):
        print('Please install ' + binary_name + ' or add it to your path.')

    cmd = [binary_name]
    cmd.append('--filter=-legal/copyright,-whitespace/parens,-build/include,-whitespace/line_length,-runtime/references')
    cmd.append('src/simd-and/src/benchmark-simd-and-vector.cpp')
    return isSuccess(executeInShell(cmd))

def analyzeSimian(target, verbose):
    binary_name = 'simian'
    if not isInstalled(binary_name):
        print('Please install ' + binary_name + ' or add it to your path.')

    cmd = [binary_name]
    if target == '' or target is None or target == 'all':
        # Since subprocess expands wildcards in path itself rather than passing it to the shell, we let python create an exhaustive list of files to parse
        files = findAllFilesPattern(getSrcDir(), '*.cpp')
    else:
        files = findAllFilesPattern(getSrcDir() + '/' + target, '*.cpp')

    if not files:
        print('Warning: did not find any files to check.')
        return True

    cmd.append(' '.join(files))
    return isSuccess(executeInShell(cmd))

def analyzeCpd(target, verbose):
    binary_name = '3rdparty/pmd/pmd.sh'
    cmd = [binary_name, 'cpd']
    cmd.extend(['--minimum-tokens', '50'])
    cmd.extend(['--language', 'cpp'])

    if target == '' or target is None or target == 'all':
        # Since subprocess expands wildcards in path itself rather than passing it to the shell, we let python create an exhaustive list of files to parse
        files = findAllFilesPattern(getSrcDir(), '*.cpp')
        includes = findAllFilesPattern(getSrcDir(), '*.h')
    else:
        files = findAllFilesPattern(getSrcDir() + '/' + target, '*.cpp')
        includes = findAllFilesPattern(getSrcDir() + '/' + target, '*.h')

    if not files:
        print('Warning: did not find any files to check.')
        return True

    cmd.append('--files')
    cmd.append(' '.join(files))
    cmd.append(' '.join(includes))
    return isSuccess(executeInShell(cmd))

def analyzeBuildSystem(target, verbose, showStuff, options):
    mode = target.getMode()
    targetName = target.getTargetName()
    profile = target.getProfile()
    method = target.getAnalyzeMethod()
    
    if method == 'clang':
        return analyzeClang(target, verbose, options)
    elif method == 'cppcheck':
        return analyzeCppcheck(targetName, verbose)
    elif method == 'cpplint':
        return analyzeCpplint(targetName, verbose)
    elif method == 'simian':
        return analyzeSimian(targetName, verbose)
    elif method == 'cpd':
        return analyzeCpd(targetName, verbose)
    elif method == 'valgrind':
        filterchain = FilterChain()
        filterchain.addFilter(ValgrindMemcheck())
        if not runner(target, showStuff, options, filterchain):
            return False
        return True
    else:
        print('Error: unknown method to analyze project: ' + method)
    return False
