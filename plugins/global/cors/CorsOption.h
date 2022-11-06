
#include <vector>
#include <string>


class CORSOption {
    public:
        CORSOption(
            std::string option_name,
            std::vector<std::string> option_values
        ){
            name = option_name;
            values = option_values;

        }
        std::string name;
        std::vector<std::string> values;

        std::string toValues() const;

    private:
        const char* delimiter = ",";
};