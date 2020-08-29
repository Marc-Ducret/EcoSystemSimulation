#include <iostream>
#include <unistd.h>
#include <cmath>
#include <SFML/Graphics.hpp>

template<typename T>
T sign(T val) {
    return (T(0) < val) - (val < T(0));
}

typedef unsigned char tile_kind;

const tile_kind EMPTY = 0;
const tile_kind WATER = 1;
const tile_kind PLANT = 2;
const tile_kind PREY = 3;
const tile_kind PREDATOR = 4;
const tile_kind VOID = 0xFF;

unsigned int render_tile_kind(tile_kind kind) {
    switch (kind) {
        case EMPTY:
            return 0x204020;
        case WATER:
            return 0x2020A0;
        case PLANT:
            return 0x20A020;
        case PREY:
            return 0xA04080;
        case PREDATOR:
            return 0xA02020;
        case VOID:
            return 0x808080;
        default:
            return 0xFF00FF;
    }
}

struct char2 {
    char x, y;

    char2 sign() const {
        return char2{
                .x = ::sign(x),
                .y = ::sign(y)
        };
    }

    char2 x2() const {
        return char2{
                .x = x,
                .y = 0
        };
    }

    char2 y2() const {
        return char2{
                .x = 0,
                .y = y
        };
    }
};

struct int2 {
    int x, y;

    int2 operator+(int2 that) const {
        return int2{
                .x = x + that.x,
                .y = y + that.y
        };
    }

    int2 operator+(char2 that) const {
        return int2{
                .x = x + that.x,
                .y = y + that.y
        };
    }
};

struct tile {
    tile_kind kind;
    unsigned short age;
    unsigned char thinkOffset;
    char2 target;
    float variance;
};

typedef unsigned char action_kind;

const action_kind MOVE = 0;

struct action {
    action_kind kind;
    int2 from, to;
};

const int MAP_SIZE = 64;

struct map {
    tile tiles[MAP_SIZE][MAP_SIZE];

    void render(unsigned char raster[], int offsetX, int offsetY, int width, int height, int scale) {
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                unsigned int color = render_tile_kind(
                        kind_at(int2{.x=(x - offsetX) / scale, .y=(y - offsetY) / scale}));
                raster[(x + width * y) * 4 + 0] = color >> 16;
                raster[(x + width * y) * 4 + 1] = color >> 8;
                raster[(x + width * y) * 4 + 2] = color >> 0;
                raster[(x + width * y) * 4 + 3] = 0xFF;
            }
        }
    }

    std::vector<action> actions{};

    void step() {
        actions.clear();
        for (int y = 0; y < MAP_SIZE; ++y) {
            for (int x = 0; x < MAP_SIZE; ++x) {
                int2 position{.x = x, .y = y};
                switch (kind_at(position)) {
                    case PREY:
                        tile *current = at(position);
                        if (current->target.x != 0 || current->target.y != 0) {
                            char2 dir = current->target.sign();
                            if (abs(current->target.x) > abs(current->target.y) &&
                                kind_at(position + dir.x2()) == EMPTY) {
                                actions.push_back(action{
                                        .kind = MOVE,
                                        .from = position,
                                        .to = position + dir.x2()
                                });
                            } else if (kind_at(position + dir.y2()) == EMPTY) {
                                actions.push_back(action{
                                        .kind = MOVE,
                                        .from = position,
                                        .to = position + dir.y2()
                                });
                            }
                        }
                        break;
//                    case PREDATOR:
//                        break;
                }
            }
        }
        for (auto action : actions) {
            switch (action.kind) {
                case MOVE:
                    if (kind_at(action.to) == EMPTY)
                        move(action.from, action.to);
                    break;
            }
        }
    }

    tile_kind kind_at(int2 position) {
        if (position.x < 0 || position.x >= MAP_SIZE || position.y < 0 || position.y >= MAP_SIZE)
            return VOID;
        return tiles[position.x][position.y].kind;
    }

    tile *at(int2 position) {
        if (position.x < 0 || position.x >= MAP_SIZE || position.y < 0 || position.y >= MAP_SIZE)
            return nullptr;
        return &tiles[position.x][position.y];
    }

    void move(int2 from, int2 to) {
        tile *tileFrom = at(from);
        tile *tileTo = at(to);
        *tileTo = *tileFrom;
        tileFrom->kind = EMPTY;
    }
};

const unsigned int WINDOW_WIDTH = 512, WINDOW_HEIGHT = 512;

int main() {
    map map{};
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Eco System");
    map.tiles[1][1].kind = PLANT;
    map.tiles[1][2].kind = PREY;
    map.tiles[1][2].target = char2{.x = 10};
    map.tiles[2][1].kind = PREDATOR;
    window.setFramerateLimit(60);
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
