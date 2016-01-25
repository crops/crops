''' Test Basic ceed/codi api'''

import unittest
import re
import ceedutil
import subprocess


# This test set ASSUMES the initial scripts have been run and therefore we have
# 1) 1 codi container named codi-test
# 2) 4 toolchain containers named test-toolchain-test[0-3]




class CeedBasicTests(unittest.TestCase):
    ''' Base class for testing ceed '''

    def setUp(self):
        ''' Define some unique data for validation '''
        self.dockerAddress = ceedutil.getDockerAddress().strip()
        self.tbase=ceedutil.ToolchainNameBase
        self.tnum=ceedutil.ToolchainNumber
        self.cPort=ceedutil.CodiPort


    def tearDown(self):
        ''' Destroy unique data '''
        self.dockerAddress = None

    def test_connect(self):
        ''' Connect to codi'''
        SUBSTRING="Connected to CODI on"
        try:
            p = subprocess.Popen(["ceed/ceed","-i",self.dockerAddress,"-l"],stdout=subprocess.PIPE)
        except subprocess.CalledProcessError as e:
            print e.output
            self.assertTrue(False)

        success=False
        output=p.communicate()[0]

        for line in output.split('\n'):
            if line.find(SUBSTRING) >= 0:
                success=True
                break
        self.assertTrue(success)

    def test_connect_with_port(self):
        ''' Connect to codi'''
        SUBSTRING="Connected to CODI on"
        try:
            p = subprocess.Popen(["ceed/ceed","-i",self.dockerAddress,"-s",str(self.cPort),"-l"],stdout=subprocess.PIPE)
        except subprocess.CalledProcessError as e:
            print e.output
            self.assertTrue(False)

        success=False
        output=p.communicate()[0]

        for line in output.split('\n'):
            if line.find(SUBSTRING) >= 0:
                success=True
                break
        self.assertTrue(success)

    def test_connect_with_badport_shouldfail(self):
        ''' Connect to codi'''
        SUBSTRING="Connected to CODI on"
        try:
            p = subprocess.Popen(["ceed/ceed","-i",self.dockerAddress,"-s",str(self.cPort+1),"-l"],stdout=subprocess.PIPE)
        except subprocess.CalledProcessError as e:
            print e.output
            self.assertTrue(False)

        success=False
        output=p.communicate()[0]

        for line in output.split('\n'):
            if line.find(SUBSTRING) >= 0:
                success=True
                break
        self.assertFalse(success)


    def test_listContainers(self):
        ''' List all the toolchain Containers'''
        SUBSTRING1="TURFF"
        SUBSTRING2=self.tbase
        EXPECTED_COUNT=4
        try:
            p = subprocess.Popen(["ceed/ceed","-i",self.dockerAddress,"-l"],stdout=subprocess.PIPE)
        except subprocess.CalledProcessError as e:
            print e.output
            self.assertTrue(False)


        count=0
        output=p.communicate()[0]

        for line in output.split('\n'):
            if line.find(SUBSTRING1) >= 0:
                if line.find(SUBSTRING2) >= 0:
                    count+=1
        self.assertEqual(count,EXPECTED_COUNT)
