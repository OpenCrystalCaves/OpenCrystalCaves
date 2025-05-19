#include "level_loader.h"

#include <cstdio>
#include <fstream>
#include <unordered_set>
#include <utility>

#include "game.h"
#include "level.h"
#include "logger.h"

static const std::unordered_set<LevelId> completedLevels{LevelId::LEVEL_1, LevelId::LEVEL_2, LevelId::LEVEL_4};

namespace LevelLoader
{

// https://moddingwiki.shikadi.net/wiki/Crystal_Caves_Map_Format

constexpr int levelLoc = 0x8CE0;
constexpr int levelRows[] = {
  // intro
  5,
  // finale
  6,
  // main
  25,
  // 1-8
  24,
  24,
  24,
  24,
  24,
  24,
  23,
  23,
  // 9-16
  24,
  24,
  24,
  24,
  24,
  23,
  24,
  24,
};
const std::pair<Sprite, geometry::Size> levelBGs[] = {
  // intro
  {Sprite::SPRITE_STARS_1, {6, 1}},
  // finale
  {Sprite::SPRITE_STARS_1, {6, 1}},
  // main
  {Sprite::SPRITE_ROCKS_1, {2, 2}},
  // 1-8
  {Sprite::SPRITE_HEX_ROCKS_1, {4, 2}},
  {Sprite::SPRITE_VERTICAL_WALL_1, {2, 2}},
  {Sprite::SPRITE_COBBLE_1, {2, 2}},
  {Sprite::SPRITE_RED_PANEL_1, {2, 2}},
  {Sprite::SPRITE_PEBBLE_WALL_1, {2, 2}},
  {Sprite::SPRITE_DARK_STONE_1, {3, 2}},
  {Sprite::SPRITE_DIAMOND_WALL_1, {2, 2}},
  {Sprite::SPRITE_COLUMN_AND_KNOB_1, {2, 2}},
  // 9-16
  {Sprite::SPRITE_GREEN_SCAFFOLD_1, {3, 2}},
  {Sprite::SPRITE_WOOD_WALL_1, {4, 2}},
  {Sprite::SPRITE_GREY_STONE_1, {2, 2}},
  {Sprite::SPRITE_RED_RECT_1, {4, 2}},
  {Sprite::SPRITE_BRICK_1, {2, 2}},
  {Sprite::SPRITE_RED_SCAFFOLD_1, {4, 2}},
  {Sprite::SPRITE_METAL_BARS_1, {2, 2}},
  {Sprite::SPRITE_BLUE_DIAMOND_1, {2, 2}},
};
// Different levels use different block colours
const Sprite blockColors[] = {
  // Intro 1-2
  Sprite::SPRITE_BLOCK_PEBBLE_NW,
  Sprite::SPRITE_BLOCK_PEBBLE_NW,
  // Main
  Sprite::SPRITE_BLOCK_PEBBLE_NW,
  // 1-8
  Sprite::SPRITE_BLOCK_BROWN_NW,
  Sprite::SPRITE_BLOCK_CYAN_NW,
  Sprite::SPRITE_BLOCK_PEBBLE_NW,
  Sprite::SPRITE_BLOCK_CYAN_NW,
  Sprite::SPRITE_BLOCK_CYAN_NW,
  Sprite::SPRITE_BLOCK_PEBBLE_NW,
  Sprite::SPRITE_BLOCK_PEBBLE_NW,
  Sprite::SPRITE_BLOCK_BLUE_NW,
  // 9-16
  Sprite::SPRITE_BLOCK_PEBBLE_NW,
  Sprite::SPRITE_BLOCK_PEBBLE_NW,
  Sprite::SPRITE_BLOCK_BLUE_NW,
  Sprite::SPRITE_BLOCK_BLUE_NW,
  Sprite::SPRITE_BLOCK_GREEN_NW,
  Sprite::SPRITE_BLOCK_PEBBLE_NW,
  Sprite::SPRITE_BLOCK_METAL_NW,
  Sprite::SPRITE_BLOCK_PEBBLE_NW,
};
const Sprite bump_platforms[] = {
  // Intro 1-2
  Sprite::SPRITE_BUMP_PLATFORM_BLUE_L,
  Sprite::SPRITE_BUMP_PLATFORM_BLUE_L,
  // Main
  Sprite::SPRITE_BUMP_PLATFORM_BLUE_L,
  // 1-8
  Sprite::SPRITE_BUMP_PLATFORM_RED_L,
  Sprite::SPRITE_BUMP_PLATFORM_BLUE_L,
  Sprite::SPRITE_BUMP_PLATFORM_BLUE_L,
  Sprite::SPRITE_BUMP_PLATFORM_BLUE_L,
  Sprite::SPRITE_BUMP_PLATFORM_RED_L,
  Sprite::SPRITE_BUMP_PLATFORM_BLUE_L,
  Sprite::SPRITE_BUMP_PLATFORM_BLUE_L,
  Sprite::SPRITE_BUMP_PLATFORM_BLUE_L,
  // 9-16
  Sprite::SPRITE_BUMP_PLATFORM_BLUE_L,
  Sprite::SPRITE_BUMP_PLATFORM_BLUE_L,
  Sprite::SPRITE_BUMP_PLATFORM_BLUE_L,
  Sprite::SPRITE_BUMP_PLATFORM_GREEN_L,
  Sprite::SPRITE_BUMP_PLATFORM_BLUE_L,
  Sprite::SPRITE_BUMP_PLATFORM_BLUE_L,
  Sprite::SPRITE_BUMP_PLATFORM_BLUE_L,
  Sprite::SPRITE_BUMP_PLATFORM_BLUE_L,
};
std::vector<Sprite> STARS{
  // The sprite with the bright star (3) seems to be less common...
  Sprite::SPRITE_STARS_1, Sprite::SPRITE_STARS_1, Sprite::SPRITE_STARS_1, Sprite::SPRITE_STARS_1, Sprite::SPRITE_STARS_2,
  Sprite::SPRITE_STARS_2, Sprite::SPRITE_STARS_2, Sprite::SPRITE_STARS_2, Sprite::SPRITE_STARS_3, Sprite::SPRITE_STARS_4,
  Sprite::SPRITE_STARS_4, Sprite::SPRITE_STARS_4, Sprite::SPRITE_STARS_4, Sprite::SPRITE_STARS_5, Sprite::SPRITE_STARS_5,
  Sprite::SPRITE_STARS_5, Sprite::SPRITE_STARS_5, Sprite::SPRITE_STARS_6, Sprite::SPRITE_STARS_6, Sprite::SPRITE_STARS_6,
  Sprite::SPRITE_STARS_6,
};
std::vector<Sprite> HORIZON{
  Sprite::SPRITE_HORIZON_LAMP,
  Sprite::SPRITE_HORIZON_1,
  Sprite::SPRITE_HORIZON_2,
  Sprite::SPRITE_HORIZON_3,
  Sprite::SPRITE_HORIZON_4,
};

enum class TileMode
{
  NONE,
  BUMPABLE_PLATFORM,
  SIGN,
  WOOD_STRUT,
  WOOD_PILLAR,
  VOLCANO,
  EJECTA,
  EXIT,
  COLUMN,
  CRATE,
  HAMMER_RAIL,
  HAMMER,
  GLASS_BALL,
  CATERPILLAR,
  TRICERATOPS,
  AIR_PIPE,
};

std::unique_ptr<Level> load(const ExeData& exe_data, const LevelId level_id)
{
  LOG_INFO("Loading level %d", static_cast<int>(level_id));
  // Find the location in exe data of the level
  const char* ptr = exe_data.data.c_str() + levelLoc;
  int l;
  for (l = static_cast<int>(LevelId::INTRO); l <= static_cast<int>(LevelId::LEVEL_16); l++)
  {
    if (l == static_cast<int>(level_id))
    {
      break;
    }
    // Skip this level's rows
    for (int row = 0; row < levelRows[l]; row++)
    {
      const size_t len = *ptr;
      ptr++;
      ptr += len;
    }
  }

  auto level = std::make_unique<Level>();

  // Read the tile ids of the level
  level->width = 0;
  for (int row = 0; row < levelRows[l]; row++)
  {
    const int len = *ptr;
    if (level->width == 0)
    {
      level->width = len;
    }
    ptr++;
    const auto row_str = std::string(ptr).substr(0, len);
    LOG_DEBUG("%s", row_str.c_str());
    for (int i = 0; i < len; i++, ptr++)
    {
      level->tile_ids.push_back(static_cast<int>(*ptr));
      level->tile_unknown.push_back(false);
    }
  }
  level->level_id = level_id;
  level->height = levelRows[static_cast<int>(level_id)];
  const auto background = levelBGs[static_cast<int>(level_id)];
  const auto block_sprite = blockColors[static_cast<int>(level_id)];
  const int bump_sprite = static_cast<int>(bump_platforms[static_cast<int>(level_id)]);

  level->has_earth = false;
  level->has_moon = false;
  bool is_stars_row = false;
  bool is_horizon_row = false;
  auto mode = TileMode::NONE;
  int volcano_sprite = -1;
  int entrance_level = static_cast<int>(LevelId::LEVEL_1);
  Caterpillar* caterpillar = nullptr;
  for (int i = 0; i < static_cast<int>(level->tile_ids.size()); i++)
  {
    const int x = i % level->width;
    if (x == 0)
    {
      is_stars_row = false;
      is_horizon_row = false;
      mode = TileMode::NONE;
      volcano_sprite = -1;
    }
    const int y = i / level->width;
    const auto tile_id = level->tile_ids[i];
    Tile tile;
    int bg = static_cast<int>(background.first);
    if (is_stars_row)
    {
      bg = static_cast<int>(STARS[rand() % STARS.size()]);
    }
    else if (is_horizon_row)
    {
      bg = static_cast<int>(HORIZON[rand() % HORIZON.size()]);
    }
    else if (background.first != Sprite::SPRITE_NONE)
    {
      // Normal background
      bg = static_cast<int>(background.first) + (((y + 1) % background.second.y()) * 4) + (x % background.second.x());
    }
    // Decode tile ids from exe data
    int sprite = -1;
    int sprite_count = 1;
    int flags = 0;
    switch (mode)
    {
      case TileMode::BUMPABLE_PLATFORM:
        switch (tile_id)
        {
          case 'd':
          case -103:
            level->actors.emplace_back(
              new BumpPlatform(geometry::Position{x * 16, y * 16}, static_cast<Sprite>(bump_sprite + 1), tile_id == -103));
            flags |= TILE_SOLID;
            break;
          case 'n':
          case -102:
            level->actors.emplace_back(
              new BumpPlatform(geometry::Position{x * 16, y * 16}, static_cast<Sprite>(bump_sprite + 2), tile_id == -102));
            flags |= TILE_SOLID;
            mode = TileMode::NONE;
            break;
          default:
            break;
        }
        break;
      case TileMode::SIGN:
        switch (tile_id)
        {
          case '4':
            // [4n = winners drugs sign
            sprite = static_cast<int>(Sprite::SPRITE_WINNERS_2);
            flags |= TILE_SOLID_TOP;
            break;
          case 'n':
            sprite = static_cast<int>(Sprite::SPRITE_WINNERS_3);
            flags |= TILE_SOLID_TOP;
            mode = TileMode::NONE;
            break;
          case 'm':
            // [m = mine sign
            sprite = static_cast<int>(Sprite::SPRITE_MINE_SIGN_2);
            flags |= TILE_RENDER_IN_FRONT;
            mode = TileMode::NONE;
            break;
          case 'd':
            // [d = danger sign
            sprite = static_cast<int>(Sprite::SPRITE_DANGER_2);
            flags |= TILE_SOLID_TOP;
            mode = TileMode::NONE;
            break;
          case 'r':
            // [r = red crate
            sprite = static_cast<int>(Sprite::SPRITE_RED_CRATE_2);
            flags |= TILE_SOLID_TOP;
            mode = TileMode::NONE;
            break;
          case '#':  // [# = 2x2 grille
            sprite = static_cast<int>(Sprite::SPRITE_GRILLE_2);
            mode = TileMode::NONE;
            break;
          case '*':  // [* = 2x2 gear
            sprite = static_cast<int>(Sprite::SPRITE_GEAR_2);
            mode = TileMode::NONE;
            break;
          default:
            break;
        }
        break;
      case TileMode::WOOD_STRUT:
        switch (tile_id)
        {
          case 'n':
            sprite = static_cast<int>(Sprite::SPRITE_WOOD_STRUT_2);
            mode = TileMode::NONE;
            break;
          default:
            break;
        }
        break;
      case TileMode::WOOD_PILLAR:
        switch (tile_id)
        {
          case 'n':
            sprite = static_cast<int>(Sprite::SPRITE_WOOD_PILLAR_2);
            mode = TileMode::NONE;
            break;
          default:
            break;
        }
        break;
      case TileMode::VOLCANO:
        sprite = volcano_sprite;
        volcano_sprite++;
        if (level->tile_ids[i + 1] != 'n')
        {
          mode = TileMode::NONE;
          volcano_sprite = -1;
        }
        break;
      case TileMode::EJECTA:
        sprite = static_cast<int>(Sprite::SPRITE_VOLCANO_EJECTA_L_1);
        sprite_count = 4;
        flags |= TILE_ANIMATED;
        mode = TileMode::NONE;
        break;
      case TileMode::EXIT:
        sprite = static_cast<int>(Sprite::SPRITE_EXIT_TOP_RIGHT_1);
        flags |= TILE_SOLID;
        mode = TileMode::NONE;
        break;
      case TileMode::COLUMN:
        flags |= TILE_SOLID_TOP;
        if (tile_id == 'n')
        {
          sprite = static_cast<int>(Sprite::SPRITE_COLUMN_HEAD_R);
          mode = TileMode::NONE;
        }
        else
        {
          sprite = static_cast<int>(Sprite::SPRITE_COLUMN_HEAD);
        }
        break;
      case TileMode::CRATE:
        flags |= TILE_SOLID_TOP;
        switch (tile_id)
        {
          case 'b':
            sprite = static_cast<int>(Sprite::SPRITE_CRATE_U1);
            break;
          case 'n':
            if (level->tile_ids[i + 1] != 'n')
            {
              sprite = static_cast<int>(Sprite::SPRITE_CRATE_UR);
              mode = TileMode::NONE;
            }
            else
            {
              sprite = static_cast<int>(Sprite::SPRITE_CRATE_U2);
            }
            break;
          case 'y':
            sprite = static_cast<int>(Sprite::SPRITE_CRATE_UR);
            mode = TileMode::NONE;
            break;
          default:
            break;
        }
        break;
      case TileMode::HAMMER_RAIL:
        switch (tile_id)
        {
          case 'n':
            if (level->tile_ids[i + 1] != 'n')
            {
              mode = TileMode::NONE;
            }
            sprite = static_cast<int>(Sprite::SPRITE_HAMMER_RAIL_2);
            break;
          default:
            break;
        }
        break;
      case TileMode::HAMMER:
        switch (tile_id)
        {
          case 'n':
            if (level->tile_ids[i + 1] != 'n')
            {
              mode = TileMode::NONE;
            }
            sprite = static_cast<int>(Sprite::SPRITE_HAMMER_RAIL_2);
            break;
          default:
            break;
        }
        break;
      case TileMode::GLASS_BALL:
        switch (tile_id)
        {
          case 'n':
            if (level->tile_ids[i + 1] != 'n')
            {
              mode = TileMode::NONE;
            }
            sprite = static_cast<int>(level->tile_ids[i - 1] == 'n' ? Sprite::SPRITE_GLASS_BALL_4 : Sprite::SPRITE_GLASS_BALL_2);
            flags |= TILE_RENDER_IN_FRONT;
            break;
          default:
            break;
        }
        break;
      case TileMode::CATERPILLAR:
        switch (tile_id)
        {
          case 'P':  // fallthrough
          case 'n':
          {
            auto new_caterpillar = new Caterpillar(geometry::Position{x * 16, y * 16});
            if (caterpillar)
            {
              caterpillar->set_child(*new_caterpillar);
            }
            caterpillar = new_caterpillar;
            level->enemies.emplace_back(caterpillar);
            if (level->tile_ids[i + 1] != 'n')
            {
              mode = TileMode::NONE;
              caterpillar = nullptr;
            }
          }
          break;
          default:
            break;
        }
        break;
      case TileMode::TRICERATOPS:
        if (level->tile_ids[i + 1] != 'n')
        {
          mode = TileMode::NONE;
        }
        break;
      case TileMode::AIR_PIPE:
        // Skip this tile
        mode = TileMode::NONE;
        break;
      default:
        switch (tile_id)
        {
          case ' ':
            break;
          case '!':
            // Faucet
            level->hazards.emplace_back(new Faucet(geometry::Position{x * 16, y * 16}));
            break;
          case '#':
            // Spider
            level->enemies.emplace_back(new Spider(geometry::Position{x * 16, y * 16}));
            break;
          case '*':
            // Rockman
            level->enemies.emplace_back(new Rockman(geometry::Position{x * 16, y * 16}));
            break;
          case '$':
            // Air tank (top)
            level->hazards.emplace_back(new AirTank(geometry::Position{x * 16, y * 16}, true));
            break;
            // Crystals
          case '+':
            level->actors.emplace_back(new Crystal(geometry::Position{x * 16, y * 16}, Sprite::SPRITE_CRYSTAL_1_Y));
            level->crystals++;
            level->has_crystals = true;
            break;
          case 'b':
            level->actors.emplace_back(new Crystal(geometry::Position{x * 16, y * 16}, Sprite::SPRITE_CRYSTAL_1_G));
            level->crystals++;
            level->has_crystals = true;
            break;
          case 'R':
            level->actors.emplace_back(new Crystal(geometry::Position{x * 16, y * 16}, Sprite::SPRITE_CRYSTAL_1_R));
            level->crystals++;
            level->has_crystals = true;
            break;
          case 'c':
            level->actors.emplace_back(new Crystal(geometry::Position{x * 16, y * 16}, Sprite::SPRITE_CRYSTAL_1_B));
            level->crystals++;
            level->has_crystals = true;
            break;
            // Ammo
          case 'G':
            level->actors.emplace_back(new Ammo(geometry::Position{x * 16, y * 16}));
            break;
            // Blocks
          case 'r':
            sprite = static_cast<int>(block_sprite);  // NW
            flags |= TILE_SOLID;
            break;
          case 't':
            sprite = static_cast<int>(block_sprite) + 1;  // N
            flags |= TILE_SOLID;
            break;
          case 'y':
            sprite = static_cast<int>(block_sprite) + 2;  // NE
            flags |= TILE_SOLID;
            break;
          case '4':
            sprite = static_cast<int>(block_sprite) + 8;  // W
            flags |= TILE_SOLID;
            break;
          case '5':
            sprite = static_cast<int>(block_sprite) + 9;  // MID
            flags |= TILE_SOLID;
            break;
          case '6':
            sprite = static_cast<int>(block_sprite) + 10;  // E
            flags |= TILE_SOLID;
            break;
          case '9':
            // Mine cart
            level->enemies.emplace_back(new MineCart(geometry::Position{x * 16, y * 16}));
            break;
          case 'B':
            // Clear block
            level->actors.emplace_back(new ClearBlock(geometry::Position{x * 16, y * 16}));
            break;
          case 'f':
            sprite = static_cast<int>(block_sprite) + 4;  // SW
            flags |= TILE_SOLID;
            break;
          case 'F':
            level->hazards.emplace_back(new Flame(geometry::Position{x * 16, y * 16}));
            break;
          case 'g':
            sprite = static_cast<int>(block_sprite) + 5;  // S
            flags |= TILE_SOLID;
            break;
          case 'h':
            sprite = static_cast<int>(block_sprite) + 6;  // SE
            flags |= TILE_SOLID;
            break;
            // Bumpable platforms
          case 'D':
          case -104:
            // Keep adding bumpable platforms until we get an 'n'
            level->actors.emplace_back(
              new BumpPlatform(geometry::Position{x * 16, y * 16}, static_cast<Sprite>(bump_sprite), tile_id == -104));
            flags |= TILE_SOLID;
            mode = TileMode::BUMPABLE_PLATFORM;
            break;
          case 'd':
          case -103:
            level->actors.emplace_back(
              new BumpPlatform(geometry::Position{x * 16, y * 16}, static_cast<Sprite>(bump_sprite + 1), tile_id == -103));
            flags |= TILE_SOLID;
            mode = TileMode::BUMPABLE_PLATFORM;
            break;
          case 'A':
            // Green slime
            level->enemies.emplace_back(new Slime(geometry::Position{x * 16, y * 16}));
            break;
          case 'H':
            level->moving_platforms.push_back({geometry::Position{x * 16, y * 16}, true, false});
            break;
          case 'I':
            // Thorn
            level->hazards.emplace_back(new Thorn(geometry::Position{x * 16, y * 16}));
            break;
          case 'J':
            // Flame spout
            sprite = static_cast<int>(Sprite::SPRITE_FLAME_SPOUT);
            flags |= TILE_SOLID;
            break;
          case 'k':
            sprite = static_cast<int>(Sprite::SPRITE_CONCRETE_V);
            flags |= TILE_SOLID;
            break;
          case 'K':
            sprite = static_cast<int>(Sprite::SPRITE_CONCRETE);
            flags |= TILE_SOLID;
            break;
          case 'l':
            sprite = static_cast<int>(Sprite::SPRITE_CONCRETE_X);
            flags |= TILE_SOLID;
            break;
          case 'L':
            sprite = static_cast<int>(Sprite::SPRITE_CONCRETE_H);
            flags |= TILE_SOLID;
            break;
          case 'm':
            level->has_earth = true;
            break;
          case 'n':
            // Check tile above for continuation tile
            if (i < level->width)
            {
              break;
            }
            switch (level->tile_ids[i - level->width])
            {
              case '[':
                switch (level->tile_ids[i - level->width + 1])
                {
                  case '#':
                    // Bottom left of grille
                    sprite = static_cast<int>(Sprite::SPRITE_GRILLE_3);
                    break;
                  case '*':
                    // Bottom left of gear
                    sprite = static_cast<int>(Sprite::SPRITE_GEAR_3);
                    break;
                  case 'b':
                    // Bottom left of crate
                    sprite = static_cast<int>(Sprite::SPRITE_CRATE_DL);
                    break;
                  case 'y':
                    // Bottom left of crate
                    sprite = static_cast<int>(Sprite::SPRITE_CRATE_DL);
                    break;
                  default:
                    break;
                }
                break;
              case '#':
                // Bottom right of grille
                sprite = static_cast<int>(Sprite::SPRITE_GRILLE_4);
                break;
              case '*':
                // Bottom right of gear
                sprite = static_cast<int>(Sprite::SPRITE_GEAR_4);
                break;
              case '$':
                // Air tank (bottom)
                level->hazards.emplace_back(new AirTank(geometry::Position{x * 16, y * 16}, false));
                break;
              case 'b':
                // Bottom of crate
                sprite = static_cast<int>(Sprite::SPRITE_CRATE_D1);
                break;
              case 'n':
                // Keep looking left to see what we're continuing from
                for (int ci = i - level->width - 1; ci >= 0; ci--)
                {
                  switch (level->tile_ids[ci])
                  {
                    case 'b':
                      if (level->tile_ids[i + 1] == 'n')
                      {
                        // Bottom of crate
                        sprite = static_cast<int>(Sprite::SPRITE_CRATE_D2);
                      }
                      else
                      {
                        // Bottom right of crate
                        sprite = static_cast<int>(Sprite::SPRITE_CRATE_DR);
                      }
                      break;
                    case 'X':
                      // Bottom-right of exit
                      sprite = static_cast<int>(Sprite::SPRITE_EXIT_BOTTOM_RIGHT_1);
                      flags |= TILE_ANIMATED;
                      sprite_count = 4;
                      break;
                    default:
                      break;
                  }
                  if (level->tile_ids[ci] != 'n')
                  {
                    break;
                  }
                }
                break;
              case 'T':
                // Column below head
                sprite = static_cast<int>(Sprite::SPRITE_COLUMN);
                break;
              case 'U':
                // Hammer
                sprite = static_cast<int>(Sprite::SPRITE_HAMMER_RAIL_1);
                mode = TileMode::HAMMER;
                break;
              case 'X':
                // Bottom-left of exit
                // Ignore - we've already added an exit
                break;
              case 'y':
                // Bottom right of crate
                sprite = static_cast<int>(Sprite::SPRITE_CRATE_DR);
                break;
              case -8:
                // Glass ball thing
                sprite = static_cast<int>(Sprite::SPRITE_GLASS_BALL_3);
                mode = TileMode::GLASS_BALL;
                flags |= TILE_RENDER_IN_FRONT;
                break;
              case -91:
                // Bottom of blue door; skip as we should have added it using the top
                break;
              case -92:
                // Bottom of green door; skip as we should have added it using the top
                break;
              case -93:
                // Bottom of red door; skip as we should have added it using the top
                break;
              default:
                LOG_INFO("Unknown tile on level %d (%d,%d) tile_id=%d (%c)",
                         static_cast<int>(level_id),
                         x,
                         y,
                         tile_id,
                         static_cast<char>(tile_id));
                level->tile_unknown[i] = true;
                break;
            }
            break;
          case 'N':
            level->has_moon = true;
            break;
          case 's':
            // Moving laser
            level->hazards.emplace_back(new Laser(geometry::Position{x * 16, y * 16}, false, true));
            break;
          case 'S':
            // Snake
            level->enemies.emplace_back(new Snake(geometry::Position{x * 16, y * 16}));
            break;
          case 'T':
            // Hammer rail
            sprite = static_cast<int>(Sprite::SPRITE_HAMMER_RAIL_1);
            mode = TileMode::HAMMER_RAIL;
            break;
          case 'U':
            // Hammer
            sprite = static_cast<int>(Sprite::SPRITE_HAMMER_RAIL_1);
            mode = TileMode::HAMMER;
            level->hazards.emplace_back(new Hammer(geometry::Position{x * 16, y * 16}));
            break;
          case 'u':
            // TODO: volcano spawn point?
            sprite = static_cast<int>(Sprite::SPRITE_VOLCANO_EJECTA_R_1);
            sprite_count = 4;
            flags |= TILE_ANIMATED;
            mode = TileMode::EJECTA;
            LOG_INFO(
              "Unknown tile on level %d (%d,%d) tile_id=%d (%c)", static_cast<int>(level_id), x, y, tile_id, static_cast<char>(tile_id));
            level->tile_unknown[i] = true;
            break;
          case 'v':
            // Horizontal toggle switch
            level->actors.emplace_back(
              new Switch(geometry::Position{x * 16, y * 16}, Sprite::SPRITE_SWITCH_OFF, SWITCH_FLAG_MOVING_PLATFORMS));
            break;
          case 'V':
            level->moving_platforms.push_back({geometry::Position{x * 16, y * 16}, false, false});
            break;
          case 'w':
            level->hazards.emplace_back(new Laser(geometry::Position{x * 16, y * 16}, false));
            break;
          case 'W':
            // Air Pipe
            // WL = left facing, WR = right facing
            level->hazards.emplace_back(new AirPipe(geometry::Position{x * 16, y * 16}, level->tile_ids[i + 1] == 'L'));
            mode = TileMode::AIR_PIPE;
            break;
          case 'x':
            // TODO: remember completion state
            // Show levels under construction with cones
            if (!completedLevels.contains(static_cast<LevelId>(entrance_level)))
            {
              sprite = static_cast<int>(Sprite::SPRITE_CONES);
              flags |= TILE_RENDER_IN_FRONT;
            }
            level->entrances.push_back({geometry::Position{x * 16, y * 16}, entrance_level, EntranceState::CLOSED});
            entrance_level++;
            break;
          case 'X':
            // Xn = exit
            level->exit = std::make_unique<Exit>(geometry::Position{x * 16, y * 16});
            mode = TileMode::EXIT;
            break;
          case 'Y':
            // Player spawn
            level->player_spawn = geometry::Position(x * 16, y * 16);
            break;
          case 'z':
            if (is_horizon_row || (x == 0 && level->tile_ids[i + 1] == 'Z'))
            {
              // Random horizon tile
              bg = static_cast<int>(HORIZON[rand() % HORIZON.size()]);
              is_horizon_row = true;
            }
            else
            {
              // Random star tile
              bg = static_cast<int>(STARS[rand() % STARS.size()]);
              is_stars_row = true;
            }
            break;
          case 'Z':
            break;
          case '[':
            switch (level->tile_ids[i + 1])
            {
              case '4':
                // [4n = winners drugs sign
                sprite = static_cast<int>(Sprite::SPRITE_WINNERS_1);
                flags |= TILE_SOLID_TOP;
                mode = TileMode::SIGN;
                break;
              case '=':
                // [=n - triceratops
                level->enemies.emplace_back(new Triceratops(geometry::Position{x * 16, y * 16}));
                mode = TileMode::TRICERATOPS;
                break;
              case 'b':
                // [bnn = Yellow/green crate, 4x2
                sprite = static_cast<int>(Sprite::SPRITE_CRATE_UL);
                flags |= TILE_SOLID_TOP;
                mode = TileMode::CRATE;
                break;
              case 'd':
                // [d = danger sign
                sprite = static_cast<int>(Sprite::SPRITE_DANGER_1);
                flags |= TILE_SOLID_TOP;
                mode = TileMode::SIGN;
                break;
              case 'm':
                // [m = mine sign
                sprite = static_cast<int>(Sprite::SPRITE_MINE_SIGN_1);
                flags |= TILE_RENDER_IN_FRONT;
                mode = TileMode::SIGN;
                break;
              case 'P':
                // [P = caterpillar
                {
                  auto new_caterpillar = new Caterpillar(geometry::Position{x * 16, y * 16});
                  if (caterpillar)
                  {
                    caterpillar->set_child(*new_caterpillar);
                  }
                  caterpillar = new_caterpillar;
                  level->enemies.emplace_back(caterpillar);
                  mode = TileMode::CATERPILLAR;
                }
                break;
              case 'r':
                // [r = red crate
                sprite = static_cast<int>(Sprite::SPRITE_RED_CRATE_1);
                flags |= TILE_SOLID_TOP;
                mode = TileMode::SIGN;
                break;
              case 'T':
                // [Tn = Head of column
                sprite = static_cast<int>(Sprite::SPRITE_COLUMN_HEAD_L);
                flags |= TILE_SOLID_TOP;
                mode = TileMode::COLUMN;
                break;
              case 'y':
                // [y = Yellow/green crate, 2x2
                sprite = static_cast<int>(Sprite::SPRITE_CRATE_UL);
                flags |= TILE_SOLID_TOP;
                mode = TileMode::CRATE;
                break;
              case '#':
                // [# = 2x2 grille
                sprite = static_cast<int>(Sprite::SPRITE_GRILLE_1);
                mode = TileMode::SIGN;
                break;
              case '*':
                // [* = 2x2 gear
                sprite = static_cast<int>(Sprite::SPRITE_GEAR_1);
                mode = TileMode::SIGN;
                break;
              default:
                level->tile_unknown[i] = true;
                break;
            }
            break;
          case ']':
            // Power
            level->actors.emplace_back(new Power(geometry::Position{x * 16, y * 16}));
            break;
          case '/':
            level->enemies.emplace_back(new Hopper(geometry::Position{x * 16, y * 16}));
            break;
          case '_':
            sprite = static_cast<int>(Sprite::SPRITE_PLATFORM_BLUE);
            flags |= TILE_SOLID_TOP;
            break;
          case '|':
            // Stalactite
            level->hazards.emplace_back(new Stalactite(geometry::Position{x * 16, y * 16}));
            break;
          case '~':
            // Bat
            level->enemies.emplace_back(new Bat(geometry::Position{x * 16, y * 16}));
            break;
          case -5:
            sprite = static_cast<int>(Sprite::SPRITE_BARREL_BROKEN);
            flags |= TILE_SOLID_TOP;
            break;
          case -6:
            sprite = static_cast<int>(Sprite::SPRITE_BARREL_CRACKED);
            flags |= TILE_SOLID_TOP;
            break;
          case -7:
            sprite = static_cast<int>(Sprite::SPRITE_BARREL);
            flags |= TILE_SOLID_TOP;
            break;
          case -8:
            // Glass ball thing
            sprite = static_cast<int>(Sprite::SPRITE_GLASS_BALL_1);
            mode = TileMode::GLASS_BALL;
            flags |= TILE_RENDER_IN_FRONT;
            break;
          case -11:
            // Shovel
            level->actors.emplace_back(
              new ScoreItem(geometry::Position{x * 16, y * 16}, Sprite::SPRITE_SHOVEL, SoundType::SOUND_PICKUP_GUN, 800));
            break;
          case -12:
            // Pickaxe
            level->actors.emplace_back(
              new ScoreItem(geometry::Position{x * 16, y * 16}, Sprite::SPRITE_PICKAXE, SoundType::SOUND_PICKUP_GUN, 5000));
            break;
          case -13:
            // Snoozer
            level->enemies.emplace_back(new Snoozer(geometry::Position{x * 16, y * 16}));
            break;
          case -14:
            // Tall Green Monster
            level->enemies.emplace_back(new Bigfoot(geometry::Position{x * 16, y * 16}));
            break;
          case -16:
            if (level->tile_ids[i + 1] == 'n')
            {
              // Wood struts
              sprite = static_cast<int>(Sprite::SPRITE_WOOD_STRUT_1);
              mode = TileMode::WOOD_STRUT;
            }
            break;
          case -19:
            sprite = static_cast<int>(Sprite::SPRITE_PIPE_DR);
            break;
          case -20:
            sprite = static_cast<int>(Sprite::SPRITE_PIPE_DL);
            break;
          case -21:
            sprite = static_cast<int>(Sprite::SPRITE_PIPE_UL);
            break;
          case -22:
            sprite = static_cast<int>(Sprite::SPRITE_PIPE_UR);
            break;
          case -23:
            sprite = static_cast<int>(Sprite::SPRITE_PIPE_H);
            break;
          case -24:
            sprite = static_cast<int>(Sprite::SPRITE_PIPE_V);
            break;
          case -40:
            // Switch for turning off laser
            level->actors.emplace_back(new Switch(geometry::Position{x * 16, y * 16}, Sprite::SPRITE_SWITCH_OFF, SWITCH_FLAG_LASERS));
            break;
          case -41:
            // Stopped vertical moving platform
            level->moving_platforms.push_back({geometry::Position{x * 16, y * 16}, false, true});
            break;
          case -43:
            sprite = static_cast<int>(Sprite::SPRITE_TORCH_1);
            sprite_count = 4;
            flags |= TILE_ANIMATED;
            break;
          case -53:
            sprite = static_cast<int>(Sprite::SPRITE_PILLAR_1);
            flags |= TILE_RENDER_IN_FRONT;
            break;
          case -54:
            sprite = static_cast<int>(Sprite::SPRITE_PILLAR_3);
            flags |= TILE_RENDER_IN_FRONT;
            break;
          case -56:
            // Slime barrier
            flags |= TILE_BLOCKS_SLIME;
            break;
          case -57:
            sprite = static_cast<int>(Sprite::SPRITE_SIGN_UP);
            break;
          case -58:
            sprite = static_cast<int>(Sprite::SPRITE_SIGN_DOWN);
            break;
          case -67:
            sprite = static_cast<int>(Sprite::SPRITE_LEDGE_R);
            flags |= TILE_SOLID_TOP;
            break;
          case -69:
            sprite = static_cast<int>(Sprite::SPRITE_PURPLE_MUSHROOM);
            flags |= TILE_RENDER_IN_FRONT;
            break;
          case -70:
            sprite = static_cast<int>(Sprite::SPRITE_PILLAR_2);
            flags |= TILE_RENDER_IN_FRONT;
            break;
          case -77:
            if (level->tile_ids[i + 1] == 'n')
            {
              // Wood pillar
              sprite = static_cast<int>(Sprite::SPRITE_WOOD_PILLAR_1);
              mode = TileMode::WOOD_PILLAR;
            }
            break;
          case -78:
            sprite = static_cast<int>(Sprite::SPRITE_WOOD_V);
            flags |= TILE_SOLID;
            break;
          case -79:
            sprite = static_cast<int>(Sprite::SPRITE_WOOD_H);
            flags |= TILE_SOLID;
            break;
          case -80:
            // Hidden block
            level->actors.emplace_back(new HiddenBlock(geometry::Position{x * 16, y * 16}));
            break;
          case -84:
            // Green mushroom
            level->actors.emplace_back(new GreenMushroom(geometry::Position{x * 16, y * 16}));
            break;
          case -85:
            // Red mushroom
            level->actors.emplace_back(new RedMushroom(geometry::Position{x * 16, y * 16}));
            break;
          case -86:
            // Blue mushroom
            level->actors.emplace_back(
              new ScoreItem(geometry::Position{x * 16, y * 16}, Sprite::SPRITE_MUSHROOM_BLUE, SoundType::SOUND_BLUE_MUSHROOM, 1000));
            break;
          case -87:
            // Egg
            // TODO: egg
            level->actors.emplace_back(
              new ScoreItem(geometry::Position{x * 16, y * 16}, Sprite::SPRITE_EGG, SoundType::SOUND_CRYSTAL, 1000));
            break;
          case -88:
            // Key
            level->actors.emplace_back(new Key(geometry::Position{x * 16, y * 16}));
            break;
          case -89:
            // Chest
            level->actors.emplace_back(new Chest(geometry::Position{x * 16, y * 16}));
            break;
          case -90:
            // Light switch
            level->actors.emplace_back(new Switch(geometry::Position{x * 16, y * 16}, Sprite::SPRITE_LIGHT_SWITCH_OFF, SWITCH_FLAG_LIGHTS));
            // Light switch implies level is dark
            level->switch_flags &= ~SWITCH_FLAG_LIGHTS;
            break;
          case -91:
            // Top of blue door
            level->actors.emplace_back(new Door(geometry::Position{x * 16, y * 16}, LeverColor::LEVER_COLOR_B));
            break;
          case -92:
            // Top of green door
            level->actors.emplace_back(new Door(geometry::Position{x * 16, y * 16}, LeverColor::LEVER_COLOR_G));
            break;
          case -93:
            // Top of red door
            level->actors.emplace_back(new Door(geometry::Position{x * 16, y * 16}, LeverColor::LEVER_COLOR_R));
            break;
          case -94:
            // Blue lever
            level->actors.emplace_back(new Lever(geometry::Position{x * 16, y * 16}, LeverColor::LEVER_COLOR_B));
            break;
          case -95:
            // Green lever
            level->actors.emplace_back(new Lever(geometry::Position{x * 16, y * 16}, LeverColor::LEVER_COLOR_G));
            break;
          case -96:
            // Red lever
            level->actors.emplace_back(new Lever(geometry::Position{x * 16, y * 16}, LeverColor::LEVER_COLOR_R));
            break;
          case -112:
            sprite = static_cast<int>(Sprite::SPRITE_COLUMN);
            break;
          case -113:
            if (level->tile_ids[i + 1] == 'n')
            {
              // -113 nnn = bottom of volcano
              sprite = static_cast<int>(Sprite::SPRITE_VOLCANO_BOTTOM_1);
              mode = TileMode::VOLCANO;
              volcano_sprite = sprite + 1;
            }
            break;
          case -114:
            if (level->tile_ids[i + 1] == 'n')
            {
              // -114 n = top of volcano
              sprite = static_cast<int>(Sprite::SPRITE_VOLCANO_TOP_1);
              mode = TileMode::VOLCANO;
              volcano_sprite = sprite + 1;
            }
            break;
          case -116:
            // Gravity
            level->actors.emplace_back(new Gravity(geometry::Position{x * 16, y * 16}));
            break;
          case -117:
            // Candle
            level->actors.emplace_back(
              new ScoreItem(geometry::Position{x * 16, y * 16}, Sprite::SPRITE_CANDLE, SoundType::SOUND_PICKUP_GUN, 1000));
            break;
          case -119:
            // Pipe in hole (H)
            sprite = static_cast<int>(Sprite::SPRITE_HOLE_PIPE_H);
            break;
          case -118:
            // Pipe in hole (V)
            sprite = static_cast<int>(Sprite::SPRITE_HOLE_PIPE_V);
            break;
          case -126:
            level->hazards.emplace_back(new Laser(geometry::Position{x * 16, y * 16}, true));
            break;
          default:
            LOG_INFO(
              "Unknown tile on level %d (%d,%d) tile_id=%d (%c)", static_cast<int>(level_id), x, y, tile_id, static_cast<char>(tile_id));
            level->tile_unknown[i] = true;
            break;
        }
        break;
    }
    if (sprite == -1 && flags == 0)
    {
      tile = Tile::INVALID;
    }
    else
    {
      tile = Tile(sprite, sprite_count, flags);
    }
    level->tiles.push_back(tile);
    level->bgs.push_back(bg);
  }

  return level;
}

}
