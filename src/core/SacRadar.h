//
// Created by tjdic on 6/9/2024.
//

#ifndef SACRADAR_H
#define SACRADAR_H


#include "tjd_share.h"


class SacRadar {
public:
    void init(void* window);
    void update_viewport(s32 width, s32 height);
    void update(s64 delta);
};



#endif //SACRADAR_H
