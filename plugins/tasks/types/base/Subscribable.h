#include "Receivable.h"
#include <utilities/storage/Dispatcher.h>


using namespace messaging::pubsub;


namespace task {
    namespace types {

        template<typename KeyType, typename ValueType>
        class Subscribable : public Receivable<KeyType, ValueType>, public Dispatcher<Receivable<KeyType, ValueType>, KeyType> {
 
            protected:
                Subscribable(
                    std::string name,
                    int pool_size=100,
                    int task_order=0,
                    std::vector<std::string> subscription_targets=std::vector<std::string>()
                ): 
                    Receivable<KeyType, ValueType>(
                        name,
                        task_order,
                        subscription_targets
                    ),
                    Dispatcher<Receivable<KeyType, ValueType>, KeyType>(
                        pool_size
                    )
                {

                }

        };

    }
}
