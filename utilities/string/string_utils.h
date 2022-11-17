#include <string>
#include <sstream>
#include <iterator>
#include <vector>




namespace utilities {
    namespace string {

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

        inline std::vector<std::string> split(const std::string &s, char delim) {
            std::stringstream ss(s);
            std::string item;
            std::vector<std::string> elems;
            while (std::getline(ss, item, delim)) {
                // elems.push_back(item);
                elems.push_back(std::move(item)); // if C++11 (based on comment from @mchiasson)
            }

            return elems;
        };
        
    }
}