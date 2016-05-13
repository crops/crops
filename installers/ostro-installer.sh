#!/bin/bash

if [[ "$(basename -- "$0")" == "ostro-installer.sh" ]]; then
  echo -e "$0 has to be sourced not run" >&2;
  echo -e "\nExample:\nsource $0\n"; exit 1
fi

if [ "$TAG" != "" ]; then
    TAG="v1.0.0"
fi
OSTRO_IMG=crops/ostrobuilder:${TAG}
OSTRO_CONTAINER=ostro-builder
OSTRO_CONF=$HOME/ostro-workspace/shared/conf/ostro.conf
BITBAKE_WRAPPER=$HOME/.crops/bitbake.ostro
WIN_PLATFORM="msys"
LINUX_PLATFORM="linux"
MAC_PLATFORM="darwin"
unset MY_PLATFORM

# establish platform
if [[ "echo "${OSTYPE}" | tr '[:upper:]' '[:lower:]'" = *$LINUX_PLATFORM* ]]; then
  MY_PLATFORM=$LINUX_PLATFORM
elif [[ "echo "${OSTYPE}" | tr '[:upper:]' '[:lower:]'" == *$MAC_PLATFORM* ]]; then
  MY_PLATFORM=$MAC_PLATFORM
elif [[ "echo "${OSTYPE}" | tr '[:upper:]' '[:lower:]'" == *$WIN_PLATFORM* ]]; then
  MY_PLATFORM=$WIN_PLATFORM
else
  echo "Unknown platform: $OSTYPE. Exiting"; return 1
fi
echo -e "\nPlatform is $MY_PLATFORM"

if [ "`which docker`" = "" ]; then
  echo -e "\nPlease install docker first, then run this installer"
  if [[ $MY_PLATFORM = $LINUX_PLATFORM ]]; then
    echo -e "\nFrom: https://docs.docker.com/linux/step_one/"
  else
    echo -e "\nFrom: https://www.docker.com/products/docker-toolbox"
  fi
  return 1
fi

echo -e "\n"
read -p "This will remove existing OSTRO build containers. Answering No will exit the installer. Proceed? Y/N " -r
echo
if [[ ! $REPLY =~ ^[Yy]$ ]]
then
  echo "There is nothing for me to do. Exiting"; return 1
else
  echo -e "\nRemove existing OSTRO build containers"
  docker ps -a | awk '{ print $1,$2 }' | grep $OSTRO_IMG | awk '{print $1 }' | xargs -I {} docker rm -f {}
  echo -e "Done"
fi

if [[ "$(docker images -q $OSTRO_IMG 2> /dev/null)" != "" ]]; then
  read -p "OSTRO build image found. Download latest OSTRO build image? Y/N " -r; echo
  if [[ ! $REPLY =~ ^[Yy]$ ]]; then
    if [[ "$(docker images -q $OSTRO_IMG 2> /dev/null)" == "" ]]; then
      echo "OSTRO build image not found. Exiting installer"; return 1
    fi
  else
    echo -e "\nRemove existing OSTRO build image"
    docker rmi $OSTRO_IMG 2> /dev/null
    echo -e "Done"

    echo -e "\nDownloading OSTRO build image. Please wait"
    docker pull $OSTRO_IMG
    echo -e "Done"
  fi
else
  read -p "Download OSTRO build image? Y/N " -r
  echo
  if [[ ! $REPLY =~ ^[Yy]$ ]]; then
    if [[ "$(docker images -q $OSTRO_IMG 2> /dev/null)" == "" ]]; then
      echo "OSTRO build image not found. Exiting installer"; return 1
    fi
  else
    echo -e "\nDownloading OSTRO build image. Please wait"
    docker pull $OSTRO_IMG
    echo -e "Done"
  fi
fi

if [[ "$(docker images -q $OSTRO_IMG 2> /dev/null)" == "" ]]; then
  echo "OSTRO build image not found. Exiting installer"; return 1
fi

mkdir -p $HOME/.crops

if [[ -f "$BITBAKE_WRAPPER" ]]; then
  read -p "OSTRO bitbake wrapper found. Do you want to reinstall bitbake wrapper? Y/N " -r; echo
  if [[ ! $REPLY =~ ^[Yy]$ ]]; then
    echo -e "\nOSTRO bitbake wrapper was not updated"
  else
    rm -rf $BITBAKE_WRAPPER
	  echo -e "Downloading default OSTRO bitbake wrapper"
    docker run --rm --entrypoint=wget -v $HOME/.crops/:$HOME/.crops/ ${OSTRO_IMG} -q -P $HOME/.crops/ https://raw.githubusercontent.com/crops/crops/master/scripts/bitbake.ostro
    chmod 755 $BITBAKE_WRAPPER
    echo -e "Done."
  fi
else
  echo -e "Downloading OSTRO bitbake wrapper"
  docker run --rm --entrypoint=wget -v $HOME/.crops/:$HOME/.crops/ ${OSTRO_IMG} -q -P $HOME/.crops/ https://raw.githubusercontent.com/crops/crops/master/scripts/bitbake.ostro
  chmod 755 $BITBAKE_WRAPPER
  echo -e "Done."
fi
export PATH=$PATH:$HOME/.crops

echo -e "\n\nTHE OSTRO BUILD ENVIRONMENT HAS BEEN SET UP"

echo -e "\nYou can now build ostro-os from the CLI\n"
echo -e "Example :\nbitbake.ostro ostro-image-noswupd\n"
