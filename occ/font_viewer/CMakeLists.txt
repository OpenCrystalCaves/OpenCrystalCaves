add_executable(font_viewer
  "font_viewer.cc"
  "../occ/src/spritemgr.cc"
  "../occ/src/spritemgr.h"
)
target_link_libraries(font_viewer
  "utils"
  "sdl_wrapper"
  SDL2::SDL2
)
if(APPLE)
	set_target_properties(font_viewer PROPERTIES
		MACOSX_RPATH 1
		BUILD_WITH_INSTALL_RPATH 1
		INSTALL_RPATH "@loader_path/../Frameworks;/Library/Frameworks")
endif()

