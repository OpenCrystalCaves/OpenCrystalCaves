cmake_minimum_required(VERSION 3.15)
cmake_policy(SET CMP0067 NEW)

project(occ)

set(VERSION_MAJOR "0")
set(VERSION_MINOR "5")
set(VERSION_PATCH "0")
set(VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")

# Optionally configure CI files since they are excluded in source archives
if(EXISTS ${CMAKE_SOURCE_DIR}/../.github/workflows/cpp.yml.cmake)
	configure_file(
		${CMAKE_SOURCE_DIR}/../.github/workflows/cpp.yml.cmake
		${CMAKE_SOURCE_DIR}/../.github/workflows/cpp.yml
		@ONLY
	)
endif()

# TODO: Use CMakePresets.json
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(UNIX AND NOT APPLE)
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fuse-ld=gold")
endif()

# occ is both the binary and the directory, so they can't be in the same folder...
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/build)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/build)

find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2_mixer CONFIG REQUIRED)

if(MSVC)
  set(COMPILE_OPTIONS -MP -W4 -WX -wd\"4090\" -wd\"4996\" -wd\"4204\" /D_SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING /Zc:__cplusplus)
else()
  set(COMPILE_OPTIONS -Wall -Wextra -Wpedantic)
endif()

# sdl_wrapper
add_subdirectory("sdl_wrapper")
target_compile_options(sdl_wrapper PRIVATE ${COMPILE_OPTIONS})
target_include_directories(sdl_wrapper PUBLIC
  "utils/export"
)

# game
add_subdirectory("game")
target_compile_options(game PRIVATE ${COMPILE_OPTIONS})
target_include_directories(game PUBLIC
  "utils/export"
)

# unlzexe
add_subdirectory(external/unlzexe)

# utils
add_subdirectory("utils")
target_compile_options(utils PRIVATE ${COMPILE_OPTIONS})

# occ
add_subdirectory("occ")
target_compile_options(occ PRIVATE ${COMPILE_OPTIONS})
target_include_directories(occ PUBLIC
  "external/cfgpath"
  "sdl_wrapper/export"
  "game/export"
  "utils/export"
)

add_subdirectory("image_viewer")
target_compile_options(image_viewer PRIVATE ${COMPILE_OPTIONS})
target_include_directories(image_viewer SYSTEM PUBLIC
  "occ/src"
  "sdl_wrapper/export"
  "utils/export"
)

add_subdirectory("sound_player")
target_compile_options(sound_player PRIVATE ${COMPILE_OPTIONS})
target_include_directories(sound_player SYSTEM PUBLIC
  "external/find_steam_game"
  "occ/src"
  "sdl_wrapper/export"
  "utils/export"
)

add_subdirectory("char_viewer")
target_compile_options(char_viewer PRIVATE ${COMPILE_OPTIONS})
target_include_directories(char_viewer SYSTEM PUBLIC
  "game/export"
  "occ/src"
  "sdl_wrapper/export"
  "utils/export"
)

add_subdirectory("font_viewer")
target_compile_options(font_viewer PRIVATE ${COMPILE_OPTIONS})
target_include_directories(font_viewer SYSTEM PUBLIC
  "game/export"
  "occ/src"
  "sdl_wrapper/export"
  "utils/export"
)

add_subdirectory("level_viewer")
target_compile_options(level_viewer PRIVATE ${COMPILE_OPTIONS})
target_include_directories(level_viewer SYSTEM PUBLIC
  "game/export"
  "occ/src"
  "sdl_wrapper/export"
  "utils/export"
)

add_subdirectory("panel_test")
target_compile_options(panel_test PRIVATE ${COMPILE_OPTIONS})
target_include_directories(panel_test SYSTEM PUBLIC
  "occ/src"
  "sdl_wrapper/export"
  "utils/export"
)

add_subdirectory("sprite_viewer")
target_compile_options(sprite_viewer PRIVATE ${COMPILE_OPTIONS})
target_include_directories(sprite_viewer SYSTEM PUBLIC
  "game/export"
  "occ/src"
  "sdl_wrapper/export"
  "utils/export"
)

add_subdirectory("sprite_to_emoji")
target_compile_options(sprite_to_emoji PRIVATE ${COMPILE_OPTIONS})
target_include_directories(sprite_to_emoji SYSTEM PUBLIC
  "occ/src"
  "sdl_wrapper/export"
  "utils/export"
)

# tests
enable_testing()
add_subdirectory(external/googletest)
if(MSVC)
  target_compile_options(gmock PRIVATE /D_SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING)
  target_compile_options(gmock_main PRIVATE /D_SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING)
  target_compile_options(gtest PRIVATE /D_SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING)
  target_compile_options(gtest_main PRIVATE /D_SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING)
endif()
# Match CRT used by googletest
if(MSVC)
  set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()
add_test(game game/game_test)
target_compile_options(game PRIVATE ${COMPILE_OPTIONS})
add_test(sdl_wrapper sdl_wrapper/sdl_wrapper_test)
target_compile_options(sdl_wrapper PRIVATE ${COMPILE_OPTIONS})
add_test(utils utils/utils_test)
target_compile_options(utils PRIVATE ${COMPILE_OPTIONS})

# install/package

set(INSTALL_PREFIX .)
set(EXE_EXTENSION "")
if(WIN32)
	set(EXE_EXTENSION ".exe")
endif()
install(
  PROGRAMS
    ${CMAKE_BINARY_DIR}/build/occ${EXE_EXTENSION}
    DESTINATION ${INSTALL_PREFIX}
    COMPONENT occ)
install(DIRECTORY
	${CMAKE_SOURCE_DIR}/../media
	DESTINATION ${INSTALL_PREFIX}
  COMPONENT occ)
install(FILES
	${CMAKE_SOURCE_DIR}/../README.md
	DESTINATION ${INSTALL_PREFIX}
  COMPONENT occ)

set(CPACK_PACKAGE_NAME "OpenCrystalCaves")
set(CPACK_PACKAGE_VENDOR "OpenCrystalCaves Team")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "OpenCrystalCaves: Retro Platform Game")
set(CPACK_PACKAGE_VERSION ${VERSION})
set(CPACK_PACKAGE_VERSION_MAJOR ${VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${VERSION_PATCH})
set(CPACK_PACKAGE_EXECUTABLES "occ" "OpenCrystalCaves")
# Only install/package OCC stuff, ignore googletest
set(CPACK_ARCHIVE_COMPONENT_INSTALL ON)
set(CPACK_COMPONENTS_ALL occ)
if(WIN32)
	set(CPACK_GENERATOR ZIP)
	set(CPACK_PACKAGE_INSTALL_DIRECTORY "OpenCrystalCaves")
	# Copy DLLs
	file(GLOB DLLS "${CMAKE_SOURCE_DIR}/../dll/*.dll")
	foreach(DLL ${DLLS})
		install(
      FILES "${DLL}" DESTINATION ${INSTALL_PREFIX}
      COMPONENT occ)
	endforeach()
elseif(APPLE)
	set(CPACK_GENERATOR "DragNDrop")
	set(CPACK_DMG_FORMAT "UDBZ")
	set(CPACK_DMG_VOLUME_NAME "OpenCrystalCaves")
	set(CPACK_SYSTEM_NAME "OSX")
  # TODO: icon

	# copy SDL frameworks
	set(LIBS SDL2 SDL2_mixer)
  foreach(LIB ${LIBS})
    install(DIRECTORY /Library/Frameworks/${LIB}.framework
      DESTINATION ${INSTALL_PREFIX}/Frameworks
      COMPONENT occ)
  endforeach()
else()
	# probably linux

	# Pack only tar.gz since deb and rpm packages may differ in distros
	set(CPACK_GENERATOR TGZ)

	# debian specific
	set(CPACK_DEBIAN_PACKAGE_MAINTAINER "OpenCrystalCaves Team")
	set(CPACK_DEBIAN_PACKAGE_SECTION Games)
	set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6, libsdl2-mixer")

	# fedora/suse specific
	set(CPACK_RPM_PACKAGE_LICENSE "MIT")
	set(CPACK_RPM_PACKAGE_REQUIRES "sdl_mixer >= 2")

endif()
include(CPack)
