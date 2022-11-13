#include <vector>


namespace messaging {
    namespace pubsub {

        template<typename Subscriber, typename KeyType>
        class Dispatcher {

            public:
                Dispatcher(
                    int size=100
                ){
                    subscribers.reserve(size * 2);
                }

                std::vector<std::shared_ptr<Subscriber>> subscribed(){
                    return subscribers;
                }

                template<typename T>void subscribe(T subscriber){
                    subscribers.push_back(
                        std::move(
                            std::make_shared<T>(subscriber)
                        )
                    );
                }

                template<typename ValueType> void send(KeyType key, ValueType value){

                    for (std::shared_ptr<Subscriber> &subscriber: subscribers){
                        subscriber->receive(key, value);
                    }
                    
                }
                void resize(int size){
                    subscribers.resize(size); 
                }

                void clear(){
                    subscribers.erase(subscribers.begin(), subscribers.end());
                }

                size_t count(){
                    return subscribers.size();
                }

            private:
                std::vector<std::shared_ptr<Subscriber>> subscribers;
        };

    }
}

