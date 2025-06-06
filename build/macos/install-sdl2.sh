#!/bin/sh
set -e

OSX_LIB_PATH="/Library/Frameworks"

OSX_SDL2_PATH_FULL="$OSX_LIB_PATH/SDL2.framework"
OSX_SDL2_MIXER_PATH_FULL="$OSX_LIB_PATH/SDL2_mixer.framework"

SDL2_VERSION=2.26.4
SDL2_MIXER_VERSION=2.6.3
SDL2_URL=https://www.libsdl.org/release/SDL2-$SDL2_VERSION.dmg
SDL2_MIXER_URL=https://www.libsdl.org/projects/SDL_mixer/release/SDL2_mixer-$SDL2_MIXER_VERSION.dmg

function installDMG {
  URL=$1

  ORIGINAL_PATH=`pwd`
  DMGFILE=$(basename $URL)

  echo "Starting downloading "$DMGFILE" ..."
  if [ ! -f "$DMGFILE" ]; then
    curl -O $URL
  fi

  DMGDISK=$(hdiutil attach $DMGFILE|awk '{print $1}'|grep -E '/dev/disk\ds\d')

  cd /Volumes/${DMGFILE%-*}
  sudo cp -av ${DMGFILE%-*}.framework $OSX_LIB_PATH/
  cd $ORIGINAL_PATH
  hdiutil detach $DMGDISK
}

function getSdl2 {
  installDMG $SDL2_URL
}

function getSdl2_mixer {
  installDMG $SDL2_MIXER_URL
}

if [[ "$1" == "--force" ]]
then
echo "Force build requested by --force."
getSdl2
getSdl2_mixer
exit
fi

# check to see if folder is empty
if [[ -d "$OSX_SDL2_PATH_FULL"  ]]; then
  if [[ -n "$(ls -A $OSX_SDL2_PATH_FULL)" ]]
  then
    echo "SDL2 exists. Skip building..."
  else
    echo "SDL2 dir exists but it is empty..."
    getSdl2
  fi
else
   echo "SDL2 dir missing..."
   getSdl2
fi

if [[ -d "$OSX_SDL2_MIXER_PATH_FULL"  ]]; then
  if [[ -n "$(ls -A $OSX_SDL2_MIXER_PATH_FULL)" ]]
  then
    echo "SDL2_mixer exists. Skip building..."
  else
    echo "SDL2_mixer dir exists buit it is empty..."
    getSdl2_mixer
  fi
else
   echo "SDL2_mixer dir missing..."
   getSdl2_mixer
fi