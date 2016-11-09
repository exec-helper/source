import subprocess

def listToString(list, separator):
    return separator.join(list)

def getShellOutput(cmd, working_directory = '.'):
    print("Executing '{0}' in '{1}'".format(listToString(cmd, ' '), working_directory))
    process = subprocess.Popen(cmd, stdout=subprocess.PIPE, cwd = working_directory)
    out, err = process.communicate()
    return out,err

def executeInShell(cmd, working_directory = '.'):
    print("Executing '{0}' in '{1}'".format(listToString(cmd, ' '), working_directory))
    retValue = subprocess.call(cmd, cwd = working_directory)
    return retValue
