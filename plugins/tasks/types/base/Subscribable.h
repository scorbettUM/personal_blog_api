#include "SubscribableTask.h"
#include <utilities/storage/Dispatcher.h>


using namespace messaging::pubsub;


namespace task {
    namespace types {

        template<typename KeyType, typename ValueType>
        class Subscribable : public SubscribableTask<KeyType, ValueType>, public Dispatcher<SubscribableTask<KeyType, ValueType>, KeyType> {
 
            protected:
                Subscribable(
                    std::string name
                ): SubscribableTask<KeyType, ValueType>(name)
                {

                }

        };

    }
}
