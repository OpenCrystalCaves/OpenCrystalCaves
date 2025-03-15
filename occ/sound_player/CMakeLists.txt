add_executable(sound_player
  "sound_player.cc"
  "../occ/src/soundmgr.cc"
  "../occ/src/soundmgr.h"
)
target_link_libraries(sound_player
  "utils"
  "sdl_wrapper"
  $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
  $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
  $<IF:$<TARGET_EXISTS:SDL2_mixer::SDL2_mixer>,SDL2_mixer::SDL2_mixer,SDL2_mixer::SDL2_mixer-static>
)
if(APPLE)
	set_target_properties(sound_player PROPERTIES
		MACOSX_RPATH 1
		BUILD_WITH_INSTALL_RPATH 1
		INSTALL_RPATH "@loader_path/../Frameworks;/Library/Frameworks")
endif()
