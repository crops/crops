# some shared utility functions
import subprocess

ToolchainNameBase="crops-toolchain-test"
ToolchainNumber=4
CodiPort=10000
def cmd_exists(cmd):
    return subprocess.call ("command -v "+cmd,shell=True,
                            stdout=subprocess.PIPE,stderr=subprocess.PIPE)==0

def getDockerMachineAddress():
    machine_name=subprocess.check_output(['docker-machine','active'])
    ADDRESS=subprocess.check_output(['docker-machine','ip',machine_name.strip()])
    return ADDRESS

def getDockerAddress():
    ADDRESS="127.0.0.1"
    if cmd_exists('docker-machine'):
        ADDRESS=getDockerMachineAddress()
    return ADDRESS
