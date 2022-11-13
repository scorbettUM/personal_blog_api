#include <utilities/storage/Receiver.h>
#include "BaseTask.h"

using namespace messaging::pubsub;

namespace task {
    namespace types {
        template<typename KeyType, typename ValueType>
        class SubscribableTask: public BaseTask, public Receiver<KeyType, ValueType>{

            protected:
                SubscribableTask(
                    std::string name
                )
                : BaseTask(name)
                {}

        };
    }
}