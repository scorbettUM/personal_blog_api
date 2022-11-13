#include <string>
#include <vector>


namespace messaging {
    namespace pubsub {

        template<typename KeyType, typename ValueType>
        class Receiver {
            public:
                virtual void receive(KeyType key, ValueType value) = 0;
        };

    }
}