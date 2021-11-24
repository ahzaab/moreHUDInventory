#pragma once

namespace SKSE
{
    namespace HashUtil
    {
        // Calc CRC32 of null terminated string
        uint32_t CRC32(const char* str, uint32_t start = 0);
    }
}

