#include <variant>
#include <type_traits>
#include <string>
#include <metadata.hpp>
#include <dir.hpp>

namespace handler {

    std::string HandleErrorMessage(std::variant<mdata::RETURN, dir::RETURN> error) {
        return std::visit([](auto e) -> std::string {
            using T = decltype(e);

            if constexpr (std::is_same_v<T, mdata::RETURN>) {
                auto it = statics::mdata_error_map.find(e);

                if (it != statics::mdata_error_map.end())
                    return it->second;

                return "unknown metadata error.\n";
            }
            else if constexpr (std::is_same_v<T, dir::RETURN>) {
                auto it = statics::dir_error_map.find(e);

                if (it != statics::dir_error_map.end())
                    return it->second;

                return "unknown directory error.\n";
            }
        }, error);
    }
}