#include "Receivable.h"
#include <utilities/storage/Dispatcher.h>


using namespace messaging::pubsub;


namespace task {
    namespace types {

        template<typename KeyType, typename ValueType>
        class Subscribable : public Receivable<KeyType, ValueType>, public Dispatcher<Receivable<KeyType, ValueType>, KeyType> {
 
            protected:
                Subscribable(
                    std::string name
                ): Receivable<KeyType, ValueType>(name)
                {

                }

        };

    }
}
