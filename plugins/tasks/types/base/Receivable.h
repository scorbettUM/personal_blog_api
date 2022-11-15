#include <utilities/storage/Receiver.h>
#include "BaseTask.h"

using namespace messaging::pubsub;

namespace task {
    namespace types {
        template<typename KeyType, typename ValueType>
        class Receivable: public BaseTask, public Receiver<KeyType, ValueType>{

            protected:
                Receivable(
                    std::string name,
                    int order=0,
                    std::vector<std::string> task_dependencies=std::vector<std::string>()
                )
                : BaseTask(
                    name,
                    0,
                    order,
                    task_dependencies
                )
                {}

        };
    }
}