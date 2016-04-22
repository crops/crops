#!/bin/bash
OSTRO_CONF="ostro.conf"
POKY_DIR=/ostro/ostro-os/
EXTRA_CONF=/ostro/ostro-shared/conf/ostro.conf
BUILD_DIR=/ostro/build/

H_UID=$(ls -al /ostro/ostro-shared/ | egrep "\."|egrep -v "\.\." | awk '{print $3}')
H_USER=$(getent passwd "$H_UID" | cut -d: -f1)
if [ "$H_USER" == "" ]; then
    H_USER="yoctobuilder"
    /usr/sbin/useradd -m -u ${H_UID} ${H_USER}
fi

# provide the ostro.conf if there isn't one yet
if [ ! -f /ostro/ostro-shared/conf/${OSTRO_CONF} ]; then
    mkdir -p /ostro/ostro-shared/conf
    cp /ostro/sample-conf/ostro.conf /ostro/ostro-shared/conf/${OSTRO_CONF}
fi
chmod a+rwx /ostro/ostro-shared/conf
chmod a+rw  /ostro/ostro-shared/conf/${OSTRO_CONF}


# switch into user to build
sudo  --user ${H_USER} /ostro/bin/runbitbake.py --pokydir $POKY_DIR --extraconf $EXTRA_CONF -b $BUILD_DIR -t $*
echo "copying images to shared folder"
CON_DIR=`bitbake -e | egrep "DEPLOY_DIR_IMAGE\="|tr "\=" " " | tr -d "\""| awk '{print $2}'`
rsync -a ${CON_DIR} /ostro/ostro-shared/images/

