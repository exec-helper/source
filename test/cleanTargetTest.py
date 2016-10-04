import unittest

from os import sys, path
sys.path.append(path.dirname(path.dirname(path.abspath(__file__ + '/../exec_utils'))))

from cm.cm import *

def testCleaningAllTargets(compiler, pythonVersion, testObject):
    # Touch files so we can check they have been removed
    for target in getAllTargets():
        writeFile(getTestRootDir() + '/' + compiler.getOutputDirectory() + '/' + target, "Hello!")

    # Clean targets
    outputFile = 'output/' + testObject.id() + '.output'
    errorFile = 'output/' + testObject.id() + '.error'

    cmd = [pythonVersion, 'exec']
    cmd.extend(getCleanCommand())
    cmd.extend(compiler.getCommandLineSpecification())
    testObject.assertTrue(execute(cmd, outputFile, errorFile), "Failed executing '" + str(cmd) + ". Check " + outputFile + " and " + errorFile)

    # Check that files exist
    for target in getAllTargets():
        targetFile = getTestRootDir() + '/' + compiler.getOutputDirectory() + '/' + target
        testObject.assertFalse(checkThatFileIsExists(targetFile), "'" + targetFile + "' is not cleaned.")

class TestCleanTargets(unittest.TestCase):
    def test_buildAllGccPython2(self):
        compiler = Gcc('release')
        testCleaningAllTargets(compiler, PYTHON2, self)

    def test_buildAllGccPython3(self):
        compiler = Gcc('release')
        testCleaningAllTargets(compiler, PYTHON3, self)

if __name__ == '__main__':
    suite = unittest.TestLoader().loadTestsFromTestCase(TestCleanTargets)
    unittest.TextTestRunner(verbosity=2).run(suite)
