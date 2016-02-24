#pragma once

namespace Engine
{
    struct Entity
    {
        Entity(uint32_t offset = 0) :
        mask(C_NONE)
    {
        handle.count = 0;
        handle.offset = offset;
    }

        ObjectHandle handle;
        EComponents mask;
    };
}

