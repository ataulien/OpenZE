#pragma once

namespace Engine
{
    enum EComponents
    {
        C_NONE = 0,
        C_COLLISION = 1 << 0,       //0b0001
        C_VISUAL = 1 << 1,          //0b0010
        C_ATTRIBUTES = 1 << 2,      //0b0100
        C_AI = 1 << 3,              //0b1000
    };

    namespace Components
    {
        struct Attributes
        {
        };

        struct AI
        {
        };
    }
}
