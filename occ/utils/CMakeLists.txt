project(utils)

add_library(utils
  "export/exe_data.h"
  "export/geometry.h"
  "export/logger.h"
  "export/occ_math.h"
  "export/misc.h"
  "export/path.h"
  "export/sound.h"
  "export/sprite.h"
  "export/vector.h"
  "src/exe_data.cc"
  "src/geometry.cc"
  "src/logger.cc"
  "src/misc.cc"
  "src/path.cc"
)
target_include_directories(utils PUBLIC
  "export"
  "../external/find_steam_game"
  "../external/unlzexe"
)
target_link_libraries(utils PUBLIC
  "unlzexe"
)

add_executable(utils_test
  "test/src/geometry_test.cc"
  "test/src/misc_test.cc"
  "test/src/occ_math_test.cc"
  "test/src/vector_test.cc"
)
target_include_directories(utils_test PUBLIC
  "export"
)
target_link_libraries(utils_test
  gtest_main
  gmock_main
  utils
)

add_library(utils_stubs
	"test/stubs/logger_stub.cc"
)
target_include_directories(utils_stubs PUBLIC
  "export"
)
