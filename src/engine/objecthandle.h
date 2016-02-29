#pragma once

#define OBJECT_HANDLE_COUNT_BITS 15
#define OBJECT_HANDLE_HANDLE_BITS (32 - OBJECT_HANDLE_COUNT_BITS)

namespace Engine
{
    struct ObjectHandle
    {
        union
        {
            struct
            {
                uint32_t count: OBJECT_HANDLE_COUNT_BITS;
                uint32_t offset: OBJECT_HANDLE_HANDLE_BITS;
            };
            uint32_t handle;
        };

		/**
		 * @brief <-operator for std maps and set
		 */
		bool operator<(const ObjectHandle &rhs) const 
		{
			return handle < rhs.handle;
		}
    };
}

