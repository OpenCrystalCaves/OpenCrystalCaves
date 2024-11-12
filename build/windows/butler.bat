@echo off
setlocal enabledelayedexpansion

if "!BUTLER_API_KEY!" == "" (
  echo Unable to deploy - No BUTLER_API_KEY environment variable specified
  exit /b 1
)

set PROJECT="congusbongus/opencrystalcaves"

for %%f in (OpenCrystalCaves*.zip) do (
  butler push --userversion !VERSION! %%f !PROJECT!:win
)