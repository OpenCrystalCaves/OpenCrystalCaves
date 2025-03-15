add_executable(level_viewer
  "level_viewer.cc"
  "../occ/src/spritemgr.cc"
  "../occ/src/spritemgr.h"
  "../occ/src/utils.cc"
  "../occ/src/utils.h"
)
target_link_libraries(level_viewer
  "game"
  "utils"
  "sdl_wrapper"
  "unlzexe"
  $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
  $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
)
if(APPLE)
	set_target_properties(level_viewer PROPERTIES
		MACOSX_RPATH 1
		BUILD_WITH_INSTALL_RPATH 1
		INSTALL_RPATH "@loader_path/../Frameworks;/Library/Frameworks")
endif()
