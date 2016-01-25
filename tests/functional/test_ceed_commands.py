''' Test Basic ceed/codi api'''

import unittest
import re
import ceedutil
import subprocess


# This test set ASSUMES the initial scripts have been run and therefore we have
# 1) 1 codi container named codi-test
# 2) 4 toolchain containers named test-toolchain-test[0-3]




class CeedCommandsTests(unittest.TestCase):
    ''' Base class for testing ceed '''

    def setUp(self):
        ''' Define some unique data for validation '''
        self.dockerAddress = ceedutil.getDockerAddress().strip()
        self.tbase=ceedutil.ToolchainNameBase
        self.tnum=ceedutil.ToolchainNumber

    def tearDown(self):
        ''' Destroy unique data '''
        self.dockerAddress = None

    def test_command(self):
        ''' Get Output from simple command'''
        SUBSTRING="/bin/bash"
        try:
            p = subprocess.Popen(["ceed/ceed","-i",self.dockerAddress,"-d",self.tbase+"0", "-g","which bash"],stdout=subprocess.PIPE)
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

    def test_command_all(self):
        ''' Get Output from simple command for all containers'''
        SUBSTRING="/bin/bash"
        successAll=True
        for i in range(self.tnum):
            try:
                p = subprocess.Popen(["ceed/ceed","-i",self.dockerAddress,"-d",self.tbase+str(i), "-g","which bash"],stdout=subprocess.PIPE)
            except subprocess.CalledProcessError as e:
                print e.output
                self.assertTrue(False)

            success=False
            output=p.communicate()[0]

            for line in output.split('\n'):
                if line.find(SUBSTRING) >= 0:
                    success=True
                    break
            successAll&=success
        self.assertTrue(successAll)


    def test_command_args_switch(self):
        ''' Get Output from simple command using argument switch'''
        SUBSTRING="/bin/bash"
        try:
            p = subprocess.Popen(["ceed/ceed","-i",self.dockerAddress,"-d",self.tbase+"0", "-r","bash","-g","which"],stdout=subprocess.PIPE)
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


    def test_command_env_inline(self):
        ''' Pass env as Q=R cmd'''
        SUBSTRING="FROGS=FRIENDS"
        try:
            p = subprocess.Popen(["ceed/ceed","-i",self.dockerAddress,"-d",self.tbase+"0", "-g","FROGS=FRIENDS printenv"],stdout=subprocess.PIPE)
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

    # This will be added when the -e flag is fixed
    # def test_command_env_as_argument(self):
    #     ''' Pass env using the -e switch to cmd'''
    #     SUBSTRING="FROGS=FRIENDS"
    #     try:
    #         p = subprocess.Popen(["ceed/ceed","-i",self.dockerAddress,"-d",self.tbase+"0", "-e","FROGS=FRIENDS","-g","printenv"],stdout=subprocess.PIPE)
    #     except subprocess.CalledProcessError as e:
    #         print e.output
    #         self.assertTrue(False)

    #     success=False
    #     output=p.communicate()[0]

    #     for line in output.split('\n'):
    #         if line.find(SUBSTRING) >= 0:
    #             success=True
    #             break
    #     self.assertTrue(success)
