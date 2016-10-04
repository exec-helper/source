import unittest

from os import sys, path
sys.path.append(path.dirname(path.dirname(path.abspath(__file__ + '/../exec_utils'))))

from cm.cm import *

def testDistcleaningAllTargets(compiler, pythonVersion, testObject):
    # Touch files so we can check they have been removed
    compilerOutputDirectory = getTestRootDir() + '/' + compiler.getOutputDirectory()
    for target in getAllTargets():
        writeFile(getTestRootDir() + '/' + compiler.getOutputDirectory() + '/' + target, "Hello!")
        testObject.assertTrue(checkThatDirExists(compilerOutputDirectory), "'" + compilerOutputDirectory + "' is not dist cleaned'")

    # Clean targets
    outputFile = 'output/' + testObject.id() + '.output'
    errorFile = 'output/' + testObject.id() + '.error'

    cmd = [pythonVersion, 'exec']
    cmd.extend(getDistcleanCommand())
    cmd.extend(compiler.getCommandLineSpecification())
    testObject.assertTrue(execute(cmd, outputFile, errorFile), "Failed executing '" + str(cmd) + ". Check " + outputFile + " and " + errorFile)

    # Check that files exist
    testObject.assertFalse(checkThatDirExists(compilerOutputDirectory), "'" + compilerOutputDirectory + "' is not dist cleaned'")

class TestDistcleanTargets(unittest.TestCase):
    def test_buildAllGccPython2(self):
        compiler = Gcc('release')
        testDistcleaningAllTargets(compiler, PYTHON2, self)

    def test_buildAllGccPython3(self):
        compiler = Gcc('release')
        testDistcleaningAllTargets(compiler, PYTHON3, self)

if __name__ == '__main__':
    suite = unittest.TestLoader().loadTestsFromTestCase(TestDistcleanTargets)
    unittest.TextTestRunner(verbosity=2).run(suite)
