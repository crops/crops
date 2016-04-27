#!/usr/bin/python

# runbitbake.py
#
# Copyright (C) 2016 Intel Corporation
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License version 2 as
# published by the Free Software Foundation.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

import argparse
import subprocess
import os
import tempfile
import shutil
import sys
import signal

bitbake_process = None

old_handler = {}
old_handler[str(signal.SIGINT)] = signal.getsignal(signal.SIGINT)
old_handler[str(signal.SIGTERM)] = signal.getsignal(signal.SIGTERM)

def addExtra(tempdir,builddir,name,extraList):
    myf = "{}/conf/{}".format(builddir,name)
    myf_orig = "{}/{}.orig".format(tempdir,name)
    tmpfile = "{}/{}.orig.tmp".format(tempdir,name)

    # copy isn't atomic so make sure that orig is created atomically so that
    # file.orig is always correct even if file gets hosed. So that
    # means if a user ever sees file.orig, they can be assured that it
    # is the same as the original file with no corruption.
    shutil.copyfile(myf, tmpfile)
    with open(tmpfile, "r") as f:
        fd = f.fileno()
        os.fdatasync(fd)

    # Remember first sync the file AND directory to make sure data
    # is written out
    fd = os.open(os.path.dirname(tmpfile), os.O_RDONLY)
    os.fsync(fd)
    os.close(fd)

    # Rename should be atomic with respect to disk, yes all of this assumes
    # linux and possibly non-network filesystems.
    os.rename(tmpfile, myf_orig)

    with open(myf, "a") as f:
        if extraList:
            for conf in extraList:
                with open(conf) as f2:
                    content = f2.readlines()
                for l in content:
                    f.write("%s\n"%format(l.strip()))

def restore_files(tempdir, builddir, conffiles):
    for f in conffiles:
        dest = os.path.join(builddir, "conf", f)
        src = os.path.join(tempdir, f + ".orig")

        if os.path.exists(src):
            os.rename(src, dest)

# If bitbake is around let it do all the signal handling
def handler(signum, frame):
    if bitbake_process:
        # SIGINT is special if there is a tty. Because with a tty SIGINT will
        # automatically get sent to all processes in the process group. So we
        # don't need to send it ourselves.
        if signum == signal.SIGINT and sys.stdin.isatty():
            pass
        else:
            # If there is a bitbake process we want to let it tear down all
            # its children itself so send the signal to bitbake.
            bitbake_process.send_signal(signum)
    else:
        old_handler[str(signum)](signum, frame)

signal.signal(signal.SIGINT, handler)
signal.signal(signal.SIGTERM, handler)

parser = argparse.ArgumentParser()

parser.add_argument("--extraconf", action='append', help="File containing"
                    "extra configuration")
parser.add_argument("--extralayers", action='append', help="File containing"
                    "extra bblayers")

parser.add_argument("--pokydir", default="/home/yoctouser/poky",
                    required=True, help="Directory containing poky")
parser.add_argument("--target", "-t", required=True,
                    help="What bitbake should build")
parser.add_argument("--builddir", "-b", required=True,
                    help="Directory to build in")

args = parser.parse_args()

builddir = args.builddir

if not os.path.isdir(builddir):
    os.makedirs(builddir)

# tempdir is a subdirectory of builddir in case builddir and local.conf
# already existed. Then if something goes wrong with local.conf the user can
# restore it by using builddir/tempdir/local.conf.orig
tempdir = tempfile.mkdtemp(prefix="runbitbake-tmpdir", dir=builddir)

# Have to use bash since the default on ubuntu is dash which is garbage
try:
    cmd = 'bash -c ". {}/oe-init-build-env {}"'.format(args.pokydir, builddir)
    subprocess.check_call(cmd, stdout=sys.stdout, stderr=sys.stderr,
                          shell=True)

    try:
        addExtra(tempdir,builddir,"local.conf",args.extraconf)
        addExtra(tempdir,builddir,"bblayers.conf",args.extralayers)

        cmd = '. {}/oe-init-build-env {} && '.format(args.pokydir,
                                                     builddir)
        cmd += 'exec bitbake {}'.format(args.target)
        bitbake_process = subprocess.Popen(['/bin/bash', '-c', cmd],
                                           stdout=sys.stdout,
                                           stderr=sys.stderr, shell=False)
        bitbake_process.wait()
    finally:
        restore_files(tempdir, builddir, ["local.conf", "bblayers.conf"])

except subprocess.CalledProcessError as e:
    print e

finally:
    shutil.rmtree(tempdir, ignore_errors=True)
