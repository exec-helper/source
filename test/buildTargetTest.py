import unittest

from os import sys, path
sys.path.append(path.dirname(path.dirname(path.abspath(__file__ + '/../exec_utils'))))

from cm.cm import *

def testBuildingAllExamples(compiler, pythonVersion, testObject):
        # Build
        cmd = [pythonVersion, 'exec']
        cmd.extend(getBuildCommand())
        cmd.extend(compiler.getBuildCommand())
        testObject.assertTrue(execute(cmd, 'output/' + testObject.id() + '.output', 'output/' + testObject.id() + '.error'))

        # Check that files exist
        for target in getAllTargets():
            testObject.assertTrue(checkThatFileIsExists(getTestRootDir() + '/' + compiler.getOutputDirectory() + '/' + target))

class TestBuildTargets(unittest.TestCase):
    @classmethod
    def setUp(self):
        execute([PYTHON3, 'exec', 'distclean', 'clean'])

    def test_buildAllGccPython2(self):
        compiler = Gcc('release')
        testBuildingAllExamples(compiler, PYTHON2, self)

    def test_buildAllGccPython3(self):
        compiler = Gcc('release')
        testBuildingAllExamples(compiler, PYTHON3, self)

    def test_buildAllClangPython2(self):
        compiler = Clang('release')
        testBuildingAllExamples(compiler, PYTHON2, self)


    def test_buildAllClangPython3(self):
        compiler = Clang('release')
        testBuildingAllExamples(compiler, PYTHON3, self)

if __name__ == '__main__':
    suite = unittest.TestLoader().loadTestsFromTestCase(TestBuildTargets)
    unittest.TextTestRunner(verbosity=2).run(suite)
