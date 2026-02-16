#ifndef SREECS_TILESET_HPP
#define SREECS_TILESET_HPP

#include <ECS/Component.hpp>

namespace sreECS
{
    struct Tile
    {

    };

    struct TileSet: public Component
    {
        
    private:
        std::unordered_map<sre::vec2i, Tile> m_tiles;

    protected:
        void on_render(Entity& entity) override;
    };
}

#endif