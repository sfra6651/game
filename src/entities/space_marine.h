#include "raylib.h"

struct SpaceMarineConfig {
    Texture2D &texture;
    Position pos = {(int)WINDOW_WIDTH/2, (int)WINDOW_HEIGHT/2};
    Size size = {64, 64};
    Velocity v = { 10, 10 };

};

Entity spaceMarineFactory(
    World &world,
    SpaceMarineConfig config)
{
    Entity spaceMarine = world.entities.create();
    world.attach(spaceMarine, world.positions, config.pos);
    world.attach(spaceMarine, world.renderables, Renderable{config.texture});
    world.attach(spaceMarine, world.sizes, config.size);
    world.attach(spaceMarine, world.velocities, config.v);
    return spaceMarine;
};
