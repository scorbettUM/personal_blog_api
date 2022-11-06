#include <string>
#include <sstream>
#include <iterator>

template <typename Range, typename Value = typename Range::value_type>
    std::string join(
        Range const& elements, 
        const char *const delimiter
    ) {

    std::ostringstream joined;

    auto start = std::begin(elements);
    auto end = std::end(elements);

    if (start != end) {
        std::copy(
            start, 
            std::prev(end), 
            std::ostream_iterator<Value>(joined, delimiter)
        );

        start = std::prev(end);
    }

    if (start != end) {
        joined << *start;
    }

    return joined.str();
};