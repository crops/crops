''' CROPS test runner '''

import unittest
import sys

def run_unittests():
    ''' Execute Unit Tests '''
    tests = unittest.TestLoader().discover('tests/unit')
    result = unittest.TextTestRunner(verbosity=2).run(tests)
    return result.wasSuccessful()

def run_functional_tests():
    ''' Execute Functional Tests '''
    tests = unittest.TestLoader().discover('tests/functional')
    result = unittest.TextTestRunner(verbosity=2).run(tests)
    return result.wasSuccessful()

if __name__ == '__main__':
    print "#" * 70
    print "Test Runner: Unit tests"
    print "#" * 70
    unit_results = run_unittests()

    print "#" * 70
    print "Test Runner: Functional tests"
    print "#" * 70
    functional_results = run_functional_tests()

    if unit_results and functional_results:
        sys.exit(0)
    else:
        sys.exit(1)
