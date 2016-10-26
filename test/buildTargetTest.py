import unittest
import options

from os import sys, path
sys.path.append(path.dirname(path.dirname(path.abspath(__file__ + '/../exec_utils'))))

from cm.cm import *

def testBuilding(target, pythonVersion, testObject):
    # Check that the environment is currently clean
    for compiler in target.getCompilers():
        for binary in target.getAllBinaries():
            targetFile = getTestRootDir() + '/' + target.getRootBuildDir(compiler) + '/' + binary
            testObject.assertFalse(checkThatFileExists(targetFile), "'" + targetFile + "' already exists.")

    # Build
    outputFile = 'output/' + testObject.id() + '.output'
    errorFile = 'output/' + testObject.id() + '.error'
    testObject.assertTrue(executeTarget([BUILD_COMMAND], target, pythonVersion, outputFile, errorFile))

    # Check that the right files were built
    for compiler in target.getCompilers():
        for binary in target.getAllBinaries():
            targetFile = getTestRootDir() + '/' + target.getRootBuildDir(compiler) + '/' + binary
            testObject.assertTrue(checkThatFileExists(targetFile), "'" + targetFile + "' does not exist.")

class TestBuildTargets(unittest.TestCase):
    @classmethod
    def setUp(self):
        self.compilers = UnittestOptions.getCompilers()
        self.pythonVersion = UnittestOptions.getPythonVersion()
        target = Target(['all'], ['all'], Compiler.getAllCompilers())
        executeTarget([DISTCLEAN_COMMAND], target, self.pythonVersion)

    def test_buildAllCompilers(self):
        target = Target(['all'], ['all'], Compiler.getAllCompilers())
        testBuilding(target, PYTHON2, self)

    def test_buildAll(self):
        target = Target(['all'], ['all'], self.compilers)
        testBuilding(target, self.pythonVersion, self)

    def test_buildSpecificTargetAllRunTargets(self):
        for targetName in Target.getAllTargetNames():
            target = Target([targetName], ['all'], self.compilers)
            testBuilding(target, self.pythonVersion, self)

    def test_buildSpecificTargetSpecificRunTarget(self):
        for targetName in Target.getAllTargetNames():
            for runTarget in Target.getAllRunTargets():
                target = Target([targetName], [runTarget], self.compilers)
                testBuilding(target, self.pythonVersion, self)

def main():
    options.parseArgs()
    suite = unittest.TestLoader().loadTestsFromTestCase(TestBuildTargets)
    testResult = unittest.TextTestRunner(verbosity=2).run(suite)
    if not testResult.wasSuccessful():
        sys.exit(1)
    sys.exit(0)

if __name__ == '__main__':
    main()
