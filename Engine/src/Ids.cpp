#include "Ids.h"

#include <chrono>
#include <random>

namespace buki
{
    namespace
    {
        std::mt19937_64& GetRandomGenerator()
        {
            static std::mt19937_64 generator(
                std::random_device{}() ^
                static_cast<std::uint64_t>(
                    std::chrono::steady_clock::now()
                    .time_since_epoch()
                    .count()));

            return generator;
        }
    }

    EditorId GenerateEditorId()
    {
        EditorId id = GetRandomGenerator()();

        if (id == InvalidEditorId)
            id = 1;

        return id;
    }
}