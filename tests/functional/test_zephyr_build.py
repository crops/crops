''' Test Zephyr Build'''
import unittest
import re
import subprocess
import os.path
import utils.ceedutil as ceedutil


# This test set ASSUMES the initial scripts have been run and therefore we have
# 1) 1 codi container named codi-test
# 2) 1 zephyr toolchain container named zephyr-test


class ZephyrBuildTest(unittest.TestCase):
    ''' Base class for testing Zephyr builds '''

    def setUp(self):
        ''' Define some unique data for validation '''
        self.dockerAddress = ceedutil.getDockerAddress().strip()
        self.codiPort=ceedutil.CodiPort
        self.zephyrName="zephyr-test"
        self.zephyrHostPath=os.environ['HOME']+"/crops-test-workspace/"
        self.zephyrContainerPath="/crops/"
        self.zephyrPrjPath="zephyr-project/samples/nanokernel/apps/hello_world/"
        self.zephyrBin="outdir/zephyr.bin"
        self.devnull=open(os.devnull, 'w')

    def tearDown(self):
        ''' Destroy unique data '''
        self.dockerAddress = None

    def test_a_git_clone(self):
        ''' Checkout Zephyr source'''
        SUBSTRING="Note: checking out"
        try:
            subprocess.call(["rm","-rf",os.environ['HOME']+"/crops-test-workspace/zephyr-project"],stdout=self.devnull)
            subprocess.call(["ceed/ceed","-i",self.dockerAddress,"-d",self.zephyrName, "-s",str(self.codiPort),
                "-g","git clone --branch v1.0.0 /zephyr-src /crops/zephyr-project"],stdout=self.devnull)
        except subprocess.CalledProcessError as e:
            print e.output
            self.assertTrue(False)

        success=False

        if os.path.isdir(self.zephyrHostPath+self.zephyrPrjPath):
            success=True

        self.assertTrue(success)

    def test_x86_build(self):
        ''' Build X86 hello world application\n'''
        BOARD="arduino_101"
        try:
            subprocess.call(["scripts/make.zephyr","pristine","BOARD="+BOARD,"-C",self.zephyrContainerPath+self.zephyrPrjPath],stdout=self.devnull)
            subprocess.call(["scripts/make.zephyr","BOARD="+BOARD,"-C",self.zephyrContainerPath+self.zephyrPrjPath],stdout=self.devnull)
        except subprocess.CalledProcessError as e:
            print e.output
            self.assertTrue(False)

        success=False

        if os.path.isfile(self.zephyrHostPath+self.zephyrPrjPath + self.zephyrBin):
            success=True

        self.assertTrue(success)


    def test_arm_build(self):
        ''' Build ARM hello world application\n'''
        BOARD="arduino_due"
        try:
            subprocess.call(["scripts/make.zephyr","pristine","BOARD="+BOARD,"-C",self.zephyrContainerPath+self.zephyrPrjPath],stdout=self.devnull)
            subprocess.call(["scripts/make.zephyr","BOARD="+BOARD,"-C",self.zephyrContainerPath+self.zephyrPrjPath],stdout=self.devnull)
        except subprocess.CalledProcessError as e:
            print e.output
            self.assertTrue(False)

        success=False

        if os.path.isfile(self.zephyrHostPath+self.zephyrPrjPath + self.zephyrBin):
            success=True

        self.assertTrue(success)


    def test_arc_build(self):
        ''' Build ARC hello world application\n'''
        BOARD="arduino_101_sss"
        try:
            subprocess.call(["scripts/make.zephyr","pristine","BOARD="+BOARD,"-C",self.zephyrContainerPath+self.zephyrPrjPath],stdout=self.devnull)
            subprocess.call(["scripts/make.zephyr","BOARD="+BOARD,"-C",self.zephyrContainerPath+self.zephyrPrjPath],stdout=self.devnull)
        except subprocess.CalledProcessError as e:
            print e.output
            self.assertTrue(False)

        success=False

        if os.path.isfile(self.zephyrHostPath+self.zephyrPrjPath + self.zephyrBin):
            success=True

        self.assertTrue(success)
