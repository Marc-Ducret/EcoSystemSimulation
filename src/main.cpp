#include <iostream>
#include <cmath>
#include <SFML/Graphics.hpp>

#include "math_utils.h"
#include "tile.h"
#include "action.h"

const int MAP_SIZE = 64;
const int THINK_PERIOD = 8;

struct map {
    tile::tile tiles[MAP_SIZE][MAP_SIZE];
    unsigned long clock;

    void render(unsigned char raster[], int offsetX, int offsetY, int width, int height, int scale) {
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                unsigned int color = tile::Color(
                        kind_at(int2{.x=(x - offsetX) / scale, .y=(y - offsetY) / scale}));
                raster[(x + width * y) * 4 + 0] = color >> 16;
                raster[(x + width * y) * 4 + 1] = color >> 8;
                raster[(x + width * y) * 4 + 2] = color >> 0;
                raster[(x + width * y) * 4 + 3] = 0xFF;
            }
        }
    }

    std::vector<action::action> actions{};

    bool find_closest(tile::Kind kind, int2 position, int distance, int2 *result) {
        int dist = 1;
        int turns = 0;
        int2 dir{.x = 1, .y = 1};
        int2 offset{.x = -1, .y = 0};
        while (dist < distance) {
            if (kind_at(position + offset) == kind) {
                *result = position + offset;
                return true;
            }
            offset = offset + dir;
            if (offset.x == 0) {
                dir.y = -dir.y;
                turns++;
            }
            if (offset.y == 0) {
                dir.x = -dir.x;
                turns++;
            }
            if (turns == 3) {
                turns = 0;
                offset = offset + offset.sign();
                dist++;
            }
        }
        return false;
    }

    void think(int2 position, tile::tile *tile) {
        switch (kind_at(position)) {
            case tile::PREY:
                break;
            case tile::PREDATOR:
                find_target(position, tile::PREY, 16);
                break;
        }
    }

    void adjust(int2 position, tile::tile *tile) {
        switch (tile->type) {
            case tile::PREDATOR:
                find_target(position + tile->target, tile::PREY, 1);
                break;
        }

        switch (tile->type) {
            case tile::PREY:
            case tile::PREDATOR:
                if (tile->target.x != 0 || tile->target.y != 0) {
                    char2 dir = tile->target.sign();
                    if ((abs(tile->target.x) * (kind_at(position + dir.x2()) == tile::EMPTY)) >
                        (abs(tile->target.y) * (kind_at(position + dir.y2()) == tile::EMPTY))) {
                        actions.push_back(action::action{
                                .kind = action::MOVE,
                                .from = position,
                                .to = position + dir.x2()
                        });
                    } else {
                        actions.push_back(action::action{
                                .kind = action::MOVE,
                                .from = position,
                                .to = position + dir.y2()
                        });
                    }
                }
                break;
        }
    }

    void do_action(action::action action) {
        switch (action.kind) {
            case action::MOVE:
                if (kind_at(action.to) == tile::EMPTY) {
                    move(action.from, action.to);
                    tile::tile *tile = at(action.to);
                    tile->target = char2{
                            .x = static_cast<char>(tile->target.x - (action.to.x - action.from.x)),
                            .y = static_cast<char>(tile->target.y - (action.to.y - action.from.y)),
                    };
                }
                break;
        }
    }

    void step() {
        actions.clear();
        for (int y = 0; y < MAP_SIZE; ++y) {
            for (int x = 0; x < MAP_SIZE; ++x) {
                int2 position{.x = x, .y = y};
                tile::tile *tile = at(position);
                tile->age++;
                if ((clock + tile->think_offset) % THINK_PERIOD == 0) {
                    think(position, tile);
                }
                adjust(position, tile);
            }
        }
        for (auto action : actions) {
            do_action(action);
        }
        clock++;
    }

    tile::Kind kind_at(int2 position) {
        if (position.x < 0 || position.x >= MAP_SIZE || position.y < 0 || position.y >= MAP_SIZE)
            return tile::VOID;
        return tiles[position.x][position.y].type;
    }

    tile::tile *at(int2 position) {
        if (position.x < 0 || position.x >= MAP_SIZE || position.y < 0 || position.y >= MAP_SIZE)
            return nullptr;
        return &tiles[position.x][position.y];
    }

    void move(int2 from, int2 to) {
        auto *tile_from = at(from);
        auto *tile_to = at(to);
        *tile_to = *tile_from;
        tile_from->type = tile::EMPTY;
    }

    void set_target(int2 position, int2 target) {
        at(position)->target = char2{
                .x = static_cast<char>(target.x - position.x),
                .y = static_cast<char>(target.y - position.y),
        };
    }

    void find_target(int2 position, tile::Kind kind, int sight) {
        int2 target{};
        if (find_closest(kind, position, sight, &target)) {
            set_target(position, target);
        }
    }
};

const unsigned int WINDOW_WIDTH = 512, WINDOW_HEIGHT = 512;

int main() {
    map map{};
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Eco System");
    map.tiles[1][1].type = tile::PLANT;
    map.tiles[1][3].type = tile::PREY;
    map.tiles[1][3].target = char2{.x = 60, .y = 60};
    map.tiles[2][1].type = tile::PREDATOR;
    window.setFramerateLimit(10);
    window.setVerticalSyncEnabled(true);
    sf::Texture texture;
    texture.create(WINDOW_WIDTH, WINDOW_HEIGHT);
    auto *raster = new unsigned char[WINDOW_WIDTH * WINDOW_HEIGHT * 4];
    while (window.isOpen()) {
        sf::Event event{};
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        map.step();
        map.render(raster, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 8);
        texture.update(raster);
        sf::Sprite sprite(texture);
        window.draw(sprite);
        window.display();
    }
    delete[] raster;
    return 0;
}
