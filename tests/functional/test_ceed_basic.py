''' Crawl the running Docker site and verify all links give a 200 OK '''

import unittest
import re
import requests
import subprocess
import select

# This test set ASSUMES the initial scripts have been run and therefore we have
# 1) 1 codi container named codi-test
# 2) 4 toolchain containers named test-toolchain-test[0-3]

def cmd_exists(cmd):
    return subprocess.call ("command -v "+cmd,shell=True,
                            stdout=subprocess.PIPE,stderr=subprocess.PIPE)==0

def getDockerMachineAddress():
    machine_name=subprocess.check_output(['docker-machine','active'])
    ADDRESS=subprocess.check_output(['docker-machine','ip',machine_name])
    return ADDRESS

def getDockerAddress():
    ADDRESS="127.0.0.1"
    if cmd_exists('docker-machine'):
        ADDRESS=getDockerMachineAddress()
    return ADDRESS


class CeedBasicTests(unittest.TestCase):
    ''' Base class for testing ceed '''

    def setUp(self):
        ''' Define some unique data for validation '''
        self.dockerAddress = getDockerAddress()


    def tearDown(self):
        ''' Destroy unique data '''
        self.dockerAddress = None

    def test_connect(self):
        ''' Connect to codi'''
        SUBSTRING="Connected to CODI"
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


    def test_listContainers(self):
        ''' List all the toolchain Containers'''
        SUBSTRING1="TURFF"
        SUBSTRING2="crops/toolchain:test"
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
