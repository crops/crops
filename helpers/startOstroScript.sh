#!/bin/bash
POKY_DIR=/ostro/ostro-os/
EXTRA_CONF=/ostro/ostro-shared/conf/ostro.conf
BUILD_DIR=/ostro/build/
EXTRA_LAYERS=/ostro/ostro-shared/conf/ostro.layers

H_UID=$(ls -al /ostro/ostro-shared/ | egrep "\."|egrep -v "\.\." | awk '{print $3}')
H_USER=$(getent passwd "$H_UID" | cut -d: -f1)
if [ "$H_USER" == "" ]; then
    H_USER="yoctobuilder"
    /usr/sbin/useradd -m -u ${H_UID} ${H_USER}
fi

# provide the ostro.conf if there isn't one yet
if [ ! -f ${EXTRA_CONF} ]; then
    mkdir -p /ostro/ostro-shared/conf
    cp /ostro/sample-conf/ostro.conf ${EXTRA_CONF}
fi

# provide ostro.layers if there isn't one yet
if [ ! -f ${EXTRA_LAYERS} ]; then
    mkdir -p /ostro/ostro-shared/conf
    cp /ostro/sample-conf/ostro.layers ${EXTRA_LAYERS}
fi

mkdir -p /ostro/ostro-shared/images/
chmod a+rwx /ostro/ostro-shared/images/
chmod a+rwx /ostro/ostro-shared/conf
chmod a+rw  ${EXTRA_CONF}


# switch into user to build
sudo  --user ${H_USER} /ostro/bin/runbitbake.py --pokydir $POKY_DIR --extraconf $EXTRA_CONF --extralayers $EXTRA_LAYERS -b $BUILD_DIR -t $*

# we need to source the bitbake env in order to use the bitbake script to find the images
cd ${BUILD_DIR}
source ${POKY_DIR}/oe-init-build-env ${BUILD_DIR} >> /dev/null
CON_DIR=`bitbake -e | egrep "DEPLOY_DIR_IMAGE\="|tr "\=" " " | tr -d "\""| awk '{print $2}'`
echo "copying images to shared folder from ${CON_DIR}"
rsync -a ${CON_DIR}/ /ostro/ostro-shared/images/ > /dev/null 2>&1


