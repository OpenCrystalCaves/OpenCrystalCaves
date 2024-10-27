add_executable(sound_player
  "sound_player.cc"
)
target_link_libraries(sound_player
  "utils"
  "sdl_wrapper"
  SDL2::SDL2
  SDL2_mixer::SDL2_mixer
)
if(APPLE)
	set_target_properties(sound_player PROPERTIES
		MACOSX_RPATH 1
		BUILD_WITH_INSTALL_RPATH 1
		INSTALL_RPATH "@loader_path/../Frameworks;/Library/Frameworks")
endif()
