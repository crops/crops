#!/bin/bash

if [[ "$(basename -- "$0")" == "zephyr-installer.sh" ]]; then
  echo -e "$0 has to be sourced not run" >&2;
  echo -e "\nExample:\nsource $0\n"; exit 1
fi

CODI_IMG=crops/codi:latest
CODI_CONTAINER=codi
ZEPHYR_IMG=crops/zephyr:latest
ZEPHYR_CONTAINER=crops-zephyr-0-7-5-src
VM_NAME=default
CEED_EXE=$HOME/.crops/ceed/ceed
MAKE_PATH=$HOME/.crops
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
read -p "This will remove existing CROPS containers. Answering No will exit the installer. Proceed? Y/N " -r
echo
if [[ ! $REPLY =~ ^[Yy]$ ]]
then
  echo "There is nothing for me to do. Exiting"; return 1
else
  echo -e "\nRemove existing CODI containers"
  docker ps -a | awk '{ print $1,$2 }' | grep $CODI_IMG | awk '{print $1 }' | xargs -I {} docker rm -f {}
  echo -e "Done"

  echo -e "\nRemove existing Zephyr container"
  docker ps -a | awk '{ print $1,$2 }' | grep $ZEPHYR_IMG | awk '{print $1 }' | xargs -I {} docker rm -f {}
  echo -e "Done"
fi

if [[ "$(docker images -q $CODI_IMG 2> /dev/null)" != "" ]]; then
  read -p "CODI image found. Download latest CODI image? Y/N " -r; echo
  if [[ ! $REPLY =~ ^[Yy]$ ]]; then
    if [[ "$(docker images -q $CODI_IMG 2> /dev/null)" == "" ]]; then
      echo "CODI image not found. Exiting installer"; return 1
    fi
  else
    echo -e "\nRemove existing CODI image"
    docker rmi $CODI_IMG 2> /dev/null
    echo -e "Done"

    echo -e "\nDownloading CODI image. Please wait"
    docker pull $CODI_IMG
    echo -e "Done"
  fi
else
  read -p "Download CODI image? Y/N " -r
  echo
  if [[ ! $REPLY =~ ^[Yy]$ ]]; then
    if [[ "$(docker images -q $CODI_IMG 2> /dev/null)" == "" ]]; then
      echo "CODI image not found. Exiting installer"; return 1
    fi
  else
    echo -e "\nDownloading CODI image. Please wait"
    docker pull $CODI_IMG
    echo -e "Done"
  fi
fi

if [[ "$(docker images -q $ZEPHYR_IMG 2> /dev/null)" != "" ]]; then
  read -p "Zephyr toolchain found. Download latest Zephyr toolchain? Y/N " -r; echo
  if [[ ! $REPLY =~ ^[Yy]$ ]]; then
    if [[ "$(docker images -q $ZEPHYR_IMG 2> /dev/null)" == "" ]]; then
      echo "Zephyr toolchain not found. Exiting installer"; return 1
    fi
  else
    echo -e "\nRemove existing Zephyr toolchain"
    docker rmi $ZEPHYR_IMG 2> /dev/null
    echo -e "Done"

    echo -e "\nDownloading Zephyr toolchain. Please wait"
    docker pull $ZEPHYR_IMG
    echo -e "Done"
  fi
else
  read -p "Download Zephyr toolchain? Y/N " -r; echo
  if [[ ! $REPLY =~ ^[Yy]$ ]]; then
    if [[ "$(docker images -q $ZEPHYR_IMG 2> /dev/null)" == "" ]]; then
      echo "Zephyr toolchain not found. Exiting installer"; return 1
    fi
  else
    echo -e "\nDownloading Zephyr toolchain. Please wait"
    docker pull $ZEPHYR_IMG
    echo -e "Done"
  fi
fi

echo -e "\nLaunching CODI container"
docker run -d --name $CODI_CONTAINER -v //var//run//docker.sock://var//run//docker.sock \
  --net=host $CODI_IMG || { echo 'Couldn't start $CODI_CONTAINER\' ; return 1; }
echo -e "Done"

echo -e "\nLaunching Zephyr toolchain"
docker run -d --name $ZEPHYR_CONTAINER  -e TURFFID=$ZEPHYR_CONTAINER -v $HOME/crops-workspace:/crops \
  --net=host $ZEPHYR_IMG || { echo 'Couldn't start $ZEPHYR_CONTAINER\' ; return 1; }
echo -e "Done"

mkdir -p $HOME/crops-workspace;

if [[ $MY_PLATFORM == $MAC_PLATFORM ]]; then
  if [[ ! -f "$CEED_EXE" ]]; then
    echo -e "Downloading CEED executable for Mac..."
    mkdir -p $HOME/.crops/ceed/
    curl -s -o $CEED_EXE http://crops.minchev.co.uk/ceed/mac/ceed
    chmod 755 $CEED_EXE
    echo -e "Done."
  fi

  if [[ ! -f "$MAKE_PATH/make.zephyr" ]]; then
    echo -e "Downloading Zephyr CLI make utility..."
    curl -s -o $MAKE_PATH/make.zephyr https://raw.githubusercontent.com/crops/crops/master/scripts/make.zephyr
    chmod 755 $MAKE_PATH/make.zephyr
    echo -e "Done."
  fi
else
  if [[ -f "$CEED_EXE" ]]; then
    read -p "CEED found. Do you want to reinstall CEED? Y/N " -r; echo
    if [[ ! $REPLY =~ ^[Yy]$ ]]; then
      echo -e "\nCEED executable was not updated"
    else
      echo -e "\nRemove existing CEED executable"
      rm -rf $CEED_EXE
      echo -e "Done"

      git clone https://github.com/crops/crops.git; cd crops/dockerfiles;
      if [[ $MY_PLATFORM == $WIN_PLATFORM ]]; then
        echo -e "\nBuilding CEED executable"
        docker ps -a -q --filter "name=ceed-windows" | awk '{print $1}' | xargs -I {} docker rm -f {}
        docker images -q --filter "label=name=ceed-windows" | awk '{print $1}' | xargs -I {} docker rmi {}
        docker build -t crops/ceed:windows -f Dockerfile.ceed.win ../
        docker run --name=ceed-windows crops/ceed:windows;
        docker-machine ssh default "docker cp ceed-windows:/usr/local/crops/ceed/ceed $CEED_EXE";
        docker ps -a -q --filter "name=ceed-windows" | awk '{print $1}' | xargs -I {} docker rm -f {}
        docker images -q --filter "label=name=ceed-windows" | awk '{print $1}' | xargs -I {} docker rmi {}
        echo -e "Done"
      else
        echo -e "\nBuilding CEED executable"
        docker ps -a -q --filter "name=ceed-linux" | awk '{print $1}' | xargs -I {} docker rm -f {}
        docker images -q --filter "label=name=ceed-linux" | awk '{print $1}' | xargs -I {} docker rmi {}
        docker build -t crops/ceed:linux -f Dockerfile.ceed.linux ../
        docker run --name=ceed-linux crops/ceed:linux;
        docker cp ceed-linux:/usr/local/crops/ceed/ceed $CEED_EXE;
        docker ps -a -q --filter "name=ceed-linux" | awk '{print $1}' | xargs -I {} docker rm -f {}
        docker images -q --filter "label=name=ceed-linux" | awk '{print $1}' | xargs -I {} docker rmi {}
      fi
      cd ../../; cp -fp ./crops/scripts/make.zephyr $MAKE_PATH/
      cp -fp ./crops/scripts/gdb.zephyr $MAKE_PATH/; rm -rf ./crops
    fi
  else
    read -p "Do you want to install CEED? Y/N " -r; echo
    if [[ ! $REPLY =~ ^[Yy]$ ]]; then
      echo -e "\nExiting installer. Your install is incomplete."; return 1
    else
      mkdir -p $HOME/.crops/ceed
      git clone https://github.com/crops/crops.git; cd crops/dockerfiles;
      if [[ $MY_PLATFORM == $WIN_PLATFORM ]]; then
        echo -e "\nBuilding CEED executable"
        docker ps -a -q --filter "name=ceed-windows" | awk '{print $1}' | xargs -I {} docker rm -f {}
        docker images -q --filter "label=name=ceed-windows" | awk '{print $1}' | xargs -I {} docker rmi {}
        docker build -t crops/ceed:windows -f Dockerfile.ceed.win ../
        docker run --name=ceed-windows crops/ceed:windows;
        docker-machine ssh default "docker cp ceed-windows:/usr/local/crops/ceed/ceed $CEED_EXE";
        docker ps -a -q --filter "name=ceed-windows" | awk '{print $1}' | xargs -I {} docker rm -f {}
        docker images -q --filter "label=name=ceed-windows" | awk '{print $1}' | xargs -I {} docker rmi {}
        echo -e "Done"
      else
        echo -e "\nBuilding CEED executable"
        docker ps -a -q --filter "name=ceed-linux" | awk '{print $1}' | xargs -I {} docker rm -f {}
        docker images -q --filter "label=name=ceed-linux" | awk '{print $1}' | xargs -I {} docker rmi {}
        docker build -t crops/ceed:linux -f Dockerfile.ceed.linux ../
        docker run --name=ceed-linux crops/ceed:linux;
        docker cp ceed-linux:/usr/local/crops/ceed/ceed $CEED_EXE;
        docker ps -a -q --filter "name=ceed-linux" | awk '{print $1}' | xargs -I {} docker rm -f {}
        docker images -q --filter "label=name=ceed-linux" | awk '{print $1}' | xargs -I {} docker rmi {}
        echo -e "Done"
      fi
      cd ../../; cp -fp ./crops/scripts/make.zephyr $MAKE_PATH/;
      cp -fp ./crops/scripts/gdb.zephyr $MAKE_PATH/; rm -rf ./crops
    fi
  fi
fi

echo -e "\n\nTHE CROPS ENVIRONMENT HAS BEEN SET UP"

if [[ $MY_PLATFORM != $LINUX_PLATFORM ]]; then
  MACHINE=`docker-machine active`
  IP=`docker-machine ip $MACHINE`
  CEED_EXE="$CEED_EXE -i $IP"
fi

echo -e "\nInitialize Zephyr environment for CLI use"
echo -e "Example :\n${CEED_EXE} -d ${ZEPHYR_CONTAINER} -g \"git clone --branch v1.0.0 /zephyr-src /crops/zephyr-project/\""

echo -e "\nYou can now build Zephyr applications from the CLI"
echo -e "Example :\n$MAKE_PATH/make.zephyr BOARD=arduino_101 -C /crops/zephyr-project/samples/nanokernel/apps/hello_world/"

export CEED_EXE=$CEED_EXE
export ZEPHYR_CONTAINER=$ZEPHYR_CONTAINER
