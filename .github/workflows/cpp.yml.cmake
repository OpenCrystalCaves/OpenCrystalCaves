name: C/C++ CI

on:
  push:
    branches: [ "master" ]
  pull_request:
    branches: [ "master" ]

env:
  VERSION: @VERSION@

jobs:
  build:

    runs-on: ${{ matrix.os }}

    strategy:
      matrix:
        os:
          - ubuntu-latest
          # Temporarily disable due to broken python 3.12 via homebrew
          #- macos-latest
          - windows-latest
        build_type:
          - debug
          - release

    steps:
    - uses: actions/checkout@v4
      with:
        submodules: true
    - name: Set up Homebrew (Linux, macOS)
      id: set-up-homebrew
      if: matrix.os == 'ubuntu-latest' || matrix.os == 'macos-latest'
      uses: Homebrew/actions/setup-homebrew@master
    - name: Install SDL via homebrew (Linux, macOS)
      # Because ubuntu 22 doesn't have the latest SDL libs
      if: matrix.os == 'ubuntu-latest' || matrix.os == 'macos-latest'
      run: brew install sdl2 sdl2_image
    - name: Install packages (macOS)
      if: matrix.os == 'macos-latest'
      run: |
        build/macosx/install-sdl2.sh
    - name: Install dependencies (Windows)
      if: matrix.os == 'windows-latest'
      run: C:\vcpkg\vcpkg.exe install --triplet x64-windows sdl2 sdl2-image --recurse
    - name: Configure CMake
      if: matrix.os != 'windows-latest'
      run: |
        mkdir ${{ matrix.build_type }}
        cmake -B ${{ matrix.build_type }} -DCMAKE_BUILD_TYPE=${{ matrix.build_type }} occ
    - name: Configure CMake (Windows)
      if: matrix.os == 'windows-latest'
      run: cmake -B ${{ matrix.build_type }} -DCMAKE_BUILD_TYPE=${{ matrix.build_type }} -DCMAKE_TOOLCHAIN_FILE=C:\vcpkg\scripts\buildsystems\vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-windows occ
    - name: Build
      run: cmake --build ${{ matrix.build_type }} --config ${{ matrix.build_type }}
    - name: Test
      run: ctest -V
    - name: Download DLLs on tags (Windows)
      if: startsWith(github.ref, 'refs/tags/') && matrix.os == 'windows-latest'
      run: |
        build/windows/get-sdl2-dlls.bat dll 64
    - name: Make package on tags
      if: startsWith(github.ref, 'refs/tags/')
      run: |
        cpack -B ${{ matrix.build_type }} -C ${{ matrix.build_type }}
        echo "${{ github.workspace }}/${{ matrix.build_type }}/"
        ls "${{ github.workspace }}/${{ matrix.build_type }}/"
        ls ${{ github.workspace }}/${{ matrix.build_type }}/OpenCrystalCaves-*-*.*
    - name: Upload a Build Artifact
      uses: softprops/action-gh-release@v2
      if: startsWith(github.ref, 'refs/tags/')
      env:
        GITHUB_TOKEN: ${{ secrets.GITHUB_TOKEN }}
      with:
        files: ${{ matrix.build_type }}/OpenCrystalCaves-*-*.*
        fail_on_unmatched_files: true
    - name: Publish to itch.io (Linux)
      if: startsWith(github.ref, 'refs/tags/') && startsWith(matrix.os, 'ubuntu')
      env:
        # Create a key at https://itch.io/user/settings/api-keys
        # And add the key as a Repository secret to https://github.com/gurka/OpenCrystalCaves/settings/secrets/actions
        BUTLER_API_KEY: ${{ secrets.BUTLER_API_KEY }}
      run: |
        curl -L -o butler.zip https://broth.itch.ovh/butler/linux-amd64/LATEST/archive/default
        unzip butler.zip
        chmod +x butler
        ./butler -V
        ./butler push ${{ matrix.build_type }}/OpenCrystalCaves-*-Linux.tar.gz congusbongus/opencrystalcaves:linux --userversion $VERSION
    - name: Publish to itch.io (macOS)
      if: startsWith(github.ref, 'refs/tags/') && matrix.os == 'macos-latest'
      env:
        BUTLER_API_KEY: ${{ secrets.BUTLER_API_KEY }}
      run: |
        curl -L -o butler.zip https://broth.itch.ovh/butler/darwin-amd64/LATEST/archive/default
        unzip butler.zip
        chmod +x butler
        ./butler -V
        ./butler push ${{ matrix.build_type }}/OpenCrystalCaves-*-OSX.dmg congusbongus/opencrystalcaves:mac --userversion $VERSION
    - name: Publish to itch.io (Windows)
      if: startsWith(github.ref, 'refs/tags/') && matrix.os == 'windows-latest'
      env:
        BUTLER_API_KEY: ${{ secrets.BUTLER_API_KEY }}
      run: |
        .\build\windows\butler.bat
