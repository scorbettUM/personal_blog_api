#include "CorsOption.h"
#include "../../../utilities/string/string_utils.h"
#include <vector>
#include <iterator>
#include <sstream>
#include <string>
#include <ranges>



std::string CORSOption::toValues() const {

    return join(values, delimiter);
}

