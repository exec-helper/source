import unittest
import options

from os import sys, path
sys.path.append(path.dirname(path.dirname(path.abspath(__file__ + '/../exec_utils'))))

from cm.cm import *

def testDistCleaning(target, pythonVersion, testObject):
    # Touch files so we can check they have been removed
    for distribution in target.getDistributions():
        for architecture in target.getArchitectures():
            for compiler in target.getCompilers():
                for binary in target.getAllBinaries():
                    fileToDistClean = getTestRootDir() + '/' + target.getRootBuildDir(compiler, distribution, architecture) + '/' + binary
                    writeFile(fileToDistClean, "Hello!")
                    testObject.assertTrue(checkThatFileExists(fileToDistClean), "'" + fileToDistClean + "' was not created. Failing test.'")

    outputFile = 'output/' + testObject.id() + '.output'
    errorFile = 'output/' + testObject.id() + '.error'

    testObject.assertTrue(executeTarget([DISTCLEAN_COMMAND], target, pythonVersion, outputFile, errorFile))

    # Check that the files were cleaned
    for distribution in target.getDistributions():
        for architecture in target.getArchitectures():
            for compiler in target.getCompilers():
                for binary in target.getAllBinaries():
                    targetFile = getTestRootDir() + '/' + target.getRootBuildDir(compiler, distribution, architecture) + '/' + binary
                    testObject.assertFalse(checkThatFileExists(targetFile), "'" + targetFile + "' was not cleaned.")

class TestDistcleanTargets(unittest.TestCase):
    @classmethod
    def setUp(self):
        self.compilers = UnittestOptions.getCompilers()
        self.pythonVersion = UnittestOptions.getPythonVersion()

    def test_distcleanAll(self):
        target = Target(['all'], ['all'], self.compilers)
        testDistCleaning(target, self.pythonVersion, self)

def main():
    options.parseArgs()
    suite = unittest.TestLoader().loadTestsFromTestCase(TestDistcleanTargets)
    testResult = unittest.TextTestRunner(verbosity=2).run(suite)
    if not testResult.wasSuccessful():
        sys.exit(1)
    sys.exit(0)

if __name__ == '__main__':
    main()
