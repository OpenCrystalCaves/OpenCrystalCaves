add_executable(image_viewer
  "image_viewer.cc"
  "../occ/src/imagemgr.cc"
  "../occ/src/imagemgr.h"
)
target_link_libraries(image_viewer
  "utils"
  "sdl_wrapper"
  SDL2::SDL2
  SDL2_image::SDL2_image
)
if(APPLE)
	set_target_properties(image_viewer PROPERTIES
		MACOSX_RPATH 1
		BUILD_WITH_INSTALL_RPATH 1
		INSTALL_RPATH "@loader_path/../Frameworks;/Library/Frameworks")
endif()

