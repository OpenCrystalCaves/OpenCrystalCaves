#include "level_loader.h"

#include <cstdio>
#include <fstream>
#include <unordered_set>
#include <utility>

#include "game.h"
#include "level.h"
#include "logger.h"

static const std::unordered_set<LevelId> completedLevels{LevelId::LEVEL_4};

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
  24,
  24,
  24,
  24,
  24,
  24,
  23,
  23,
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
  {Sprite::SPRITE_HEX_ROCKS_1, {4, 2}},
  {Sprite::SPRITE_VERTICAL_WALL_1, {2, 2}},
  {Sprite::SPRITE_COBBLE_1, {2, 2}},
  {Sprite::SPRITE_RED_PANEL_1, {2, 2}},
  {Sprite::SPRITE_PEBBLE_WALL_1, {2, 2}},
  {Sprite::SPRITE_DARK_STONE_1, {3, 2}},
  {Sprite::SPRITE_DIAMOND_WALL_1, {2, 2}},
  {Sprite::SPRITE_COLUMN_AND_KNOB_1, {2, 2}},
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
  std::vector<int> tile_ids;
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
      tile_ids.push_back(static_cast<int>(*ptr));
    }
  }
  level->level_id = level_id;
  level->height = levelRows[static_cast<int>(level_id)];
  const auto background = levelBGs[static_cast<int>(level_id)];
  const auto block_sprite = blockColors[static_cast<int>(level_id)];

  level->has_earth = false;
  level->has_moon = false;
  bool is_stars_row = false;
  bool is_horizon_row = false;
  auto mode = TileMode::NONE;
  int volcano_sprite = -1;
  int entrance_level = static_cast<int>(LevelId::LEVEL_1);
  for (int i = 0; i < static_cast<int>(tile_ids.size()); i++)
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
    const auto tile_id = tile_ids[i];
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
    auto item = Item::INVALID;
    switch (mode)
    {
      case TileMode::BUMPABLE_PLATFORM:
        switch (tile_id)
        {
          case 'd':
          case -103:
            sprite = static_cast<int>(Sprite::SPRITE_BUMP_PLATFORM_RED_MID);
            flags |= TILE_SOLID;
            if (tile_ids[i] == -103)
            {
              // TODO: add hidden crystal
            }
            break;
          case 'n':
          case -102:
            sprite = static_cast<int>(Sprite::SPRITE_BUMP_PLATFORM_RED_MID);
            flags |= TILE_SOLID;
            if (tile_ids[i] == -102)
            {
              // TODO: add hidden crystal
            }
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
        if (tile_ids[i + 1] != 'n')
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
          case '$':
            // Air tank (top)
            level->hazards.emplace_back(new AirTank(geometry::Position{x * 16, y * 16}, true));
            break;
            // Crystals
          case '+':
            item = Item(Sprite::SPRITE_CRYSTAL_1_Y, ItemType::ITEM_TYPE_CRYSTAL, 0);
            break;
          case 'b':
            item = Item(Sprite::SPRITE_CRYSTAL_1_G, ItemType::ITEM_TYPE_CRYSTAL, 0);
            break;
          case 'R':
            item = Item(Sprite::SPRITE_CRYSTAL_1_R, ItemType::ITEM_TYPE_CRYSTAL, 0);
            break;
          case 'c':
            item = Item(Sprite::SPRITE_CRYSTAL_1_B, ItemType::ITEM_TYPE_CRYSTAL, 0);
            break;
            // Ammo
          case 'G':
            item = Item(Sprite::SPRITE_PISTOL, ItemType::ITEM_TYPE_AMMO, AMMO_AMOUNT);
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
          case 'f':
            sprite = static_cast<int>(block_sprite) + 4;  // SW
            flags |= TILE_SOLID;
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
            sprite = static_cast<int>(Sprite::SPRITE_BUMP_PLATFORM_RED_L);
            flags |= TILE_SOLID;
            if (tile_id == -104)
            {
              // TODO: add hidden crystal
            }
            mode = TileMode::BUMPABLE_PLATFORM;
            break;
          case 'd':
            sprite = static_cast<int>(Sprite::SPRITE_BUMP_PLATFORM_RED_MID);
            flags |= TILE_SOLID;
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
            switch (tile_ids[i - level->width])
            {
              case '[':
                switch (tile_ids[i - level->width + 1])
                {
                  case '#':
                    // Bottom left of grille
                    sprite = static_cast<int>(Sprite::SPRITE_GRILLE_3);
                    break;
                  case '*':
                    // Bottom left of gear
                    sprite = static_cast<int>(Sprite::SPRITE_GEAR_3);
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
              case 'X':
                // Bottom-left of exit
                // Ignore - we've already added an exit
                break;
              case -91:
                // Bottom of blue door; skip as we should have added it using the top
                break;
              case -92:
                // Bottom of green door; skip as we should have added it using the top
                break;
              default:
                // Check tile above-left
                switch (tile_ids[i - level->width - 1])
                {
                  case 'X':
                    // Bottom-right of exit
                    sprite = static_cast<int>(Sprite::SPRITE_EXIT_BOTTOM_RIGHT_1);
                    flags |= TILE_ANIMATED;
                    sprite_count = 4;
                    break;
                  default:
                    break;
                }
                break;
            }
            break;
          case 'N':
            level->has_moon = true;
            break;
          case 'S':
            // Snake
            level->enemies.emplace_back(new Snake(geometry::Position{x * 16, y * 16}));
            break;
          case 'u':
            // TODO: volcano spawn point?
            sprite = static_cast<int>(Sprite::SPRITE_VOLCANO_EJECTA_R_1);
            sprite_count = 4;
            flags |= TILE_ANIMATED;
            mode = TileMode::EJECTA;
            break;
          case 'v':
            // Horizontal toggle switch
            level->actors.emplace_back(new Switch(geometry::Position{x * 16, y * 16}, Sprite::SPRITE_SWITCH_OFF));
            break;
          case 'V':
            level->moving_platforms.push_back({geometry::Position{x * 16, y * 16}, false, false});
            break;
          case 'w':
            level->hazards.emplace_back(new Laser(geometry::Position{x * 16, y * 16}, false));
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
            if (is_horizon_row || (x == 0 && tile_ids[i + 1] == 'Z'))
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
            switch (tile_ids[i + 1])
            {
                // [4n = winners drugs sign
              case '4':
                sprite = static_cast<int>(Sprite::SPRITE_WINNERS_1);
                flags |= TILE_SOLID_TOP;
                mode = TileMode::SIGN;
                break;
                // [m = mine sign
              case 'm':
                sprite = static_cast<int>(Sprite::SPRITE_MINE_SIGN_1);
                flags |= TILE_RENDER_IN_FRONT;
                mode = TileMode::SIGN;
                break;
                // [d = danger sign
              case 'd':
                sprite = static_cast<int>(Sprite::SPRITE_DANGER_1);
                flags |= TILE_SOLID_TOP;
                mode = TileMode::SIGN;
                break;
              case 'r':
                // [r = red crate
                sprite = static_cast<int>(Sprite::SPRITE_RED_CRATE_1);
                flags |= TILE_SOLID_TOP;
                mode = TileMode::SIGN;
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
                break;
            }
            break;
          case '/':
            level->enemies.emplace_back(new Hopper(geometry::Position{x * 16, y * 16}));
            break;
          case '_':
            sprite = static_cast<int>(Sprite::SPRITE_PLATFORM_BLUE);
            flags |= TILE_SOLID_TOP;
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
          case -11:
            // Shovel
            item = Item(Sprite::SPRITE_SHOVEL, ItemType::ITEM_TYPE_SCORE, 800);
            break;
          case -12:
            // Pickaxe
            item = Item(Sprite::SPRITE_PICKAXE, ItemType::ITEM_TYPE_SCORE, 5000);
            break;
          case -14:
            // Tall Green Monster
            level->enemies.emplace_back(new Bigfoot(geometry::Position{x * 16, y * 16}));
            break;
          case -16:
            if (tile_ids[i + 1] == 'n')
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
          case -41:
            // Stopped vertical moving platform
            level->moving_platforms.push_back({geometry::Position{x * 16, y * 16}, false, true});
            break;
          case -43:
            sprite = static_cast<int>(Sprite::SPRITE_TORCH_1);
            sprite_count = 4;
            flags |= TILE_ANIMATED;
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
          case -77:
            if (tile_ids[i + 1] == 'n')
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
          case -86:
            // Blue mushroom
            item = Item(Sprite::SPRITE_MUSHROOM_BLUE, ItemType::ITEM_TYPE_SCORE, 1000);
            break;
          case -91:
            // Top of blue door
            level->actors.emplace_back(new Door(geometry::Position{x * 16, y * 16}, LeverColor::LEVER_COLOR_B));
            break;
          case -92:
            // Top of green door
            level->actors.emplace_back(new Door(geometry::Position{x * 16, y * 16}, LeverColor::LEVER_COLOR_G));
            break;
          case -94:
            // Blue lever
            level->actors.emplace_back(new Lever(geometry::Position{x * 16, y * 16}, LeverColor::LEVER_COLOR_B));
            break;
          case -95:
            // Green lever
            level->actors.emplace_back(new Lever(geometry::Position{x * 16, y * 16}, LeverColor::LEVER_COLOR_G));
            break;
          case -113:
            if (tile_ids[i + 1] == 'n')
            {
              // -113 nnn = bottom of volcano
              sprite = static_cast<int>(Sprite::SPRITE_VOLCANO_BOTTOM_1);
              mode = TileMode::VOLCANO;
              volcano_sprite = sprite + 1;
            }
            break;
          case -114:
            if (tile_ids[i + 1] == 'n')
            {
              // -114 n = top of volcano
              sprite = static_cast<int>(Sprite::SPRITE_VOLCANO_TOP_1);
              mode = TileMode::VOLCANO;
              volcano_sprite = sprite + 1;
            }
            break;
          case -117:
            // Candle
            item = Item(Sprite::SPRITE_CANDLE, ItemType::ITEM_TYPE_SCORE, 1000);
            break;
          default:
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
    level->items.push_back(item);
  }

  return level;
}

}
