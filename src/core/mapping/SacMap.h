//
// Created by tjdic on 6/9/2024.
//

#ifndef SACMAP_H
#define SACMAP_H

#include "rendering/gl/render_info.h"
#include "tjd_share.h"


struct SacMapLayer {
    bool       visible = true;
    RenderInfo render_info{};
    s64        vertex_count = 0;
    f32       *vertices{};
};

class SacMap {
    SacMapLayer *layers{};

public:
    void init();
    void render();
};


#endif // SACMAP_H
