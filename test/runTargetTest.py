import unittest
import options
import glob

from os import sys, path
sys.path.append(path.dirname(path.dirname(path.abspath(__file__ + '/../exec_utils'))))

from cm.cm import *

def testRunning(target, pythonVersion, testObject):
    for compiler in target.getCompilers():
        for binary in target.getAllBinaries():
            targetFile = getTestRootDir() + '/' + compiler.getOutputDirectory() + '/' + binary
    
            # Check that the targets to run exist
            testObject.assertTrue(checkThatFileExists(targetFile), "'" + targetFile + "' does not exist.")

            # Check that it has not been run yet
            outputFile = getTestRootDir() + '/' + os.path.basename(binary) + '.cpp.output'
            testObject.assertFalse(checkThatFileExists(outputFile), "'" + outputFile + "' already exists.")

    # Build
    outputFile = 'output/' + testObject.id() + '.output'
    errorFile = 'output/' + testObject.id() + '.error'
    testObject.assertTrue(executeTarget([RUN_COMMAND], target, pythonVersion, outputFile, errorFile))

    # Check that the right targets were run
    for compiler in target.getCompilers():
        for binary in target.getAllBinaries():
            outputFile = getTestRootDir() + '/' + os.path.basename(binary) + '.cpp.output'
            testObject.assertTrue(checkThatFileExists(outputFile), "Target '" + binary + "' did not run.")

class TestRunTargets(unittest.TestCase):
    @classmethod
    def setUp(self):
        self.compilers = UnittestOptions.getCompilers()
        self.pythonVersion = UnittestOptions.getPythonVersion()

        target = Target(['all'], ['all'], Compiler.getAllCompilers())
        executeTarget([DISTCLEAN_COMMAND, BUILD_COMMAND], target, self.pythonVersion)

    @classmethod
    def tearDown(self):
        # Clean output files
        for f in glob.glob(getTestRootDir() + "/*.output"):
            os.remove(f)

    def test_runAllCompilers(self):
        target = Target(['all'], ['all'], Compiler.getAllCompilers())
        testRunning(target, PYTHON2, self)

    def test_runAll(self):
        target = Target(['all'], ['all'], self.compilers)
        testRunning(target, self.pythonVersion, self)

    def test_runSpecificTargetAllRunTargets(self):
        for targetName in Target.getAllTargetNames():
            target = Target([targetName], ['all'], self.compilers)
            testRunning(target, self.pythonVersion, self)

    def test_runSpecificTargetSpecificRunTarget(self):
        for targetName in Target.getAllTargetNames():
            for runTarget in Target.getAllRunTargets():
                target = Target([targetName], [runTarget], self.compilers)
                testRunning(target, self.pythonVersion, self)

def main():
    options.parseArgs()
    suite = unittest.TestLoader().loadTestsFromTestCase(TestRunTargets)
    testResult = unittest.TextTestRunner(verbosity=2).run(suite)
    if not testResult.wasSuccessful():
        sys.exit(1)
    sys.exit(0)

if __name__ == '__main__':
    main()
