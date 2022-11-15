#include <vector>


namespace messaging {
    namespace pubsub {

        template<typename Subscriber, typename KeyType>
        class Dispatcher {

            public:
                Dispatcher(
                    int initial_list_size=100
                ){
                    subscribers.reserve(initial_list_size * 2);
                }

                std::vector<std::shared_ptr<Subscriber>> subscribed(){
                    return subscribers;
                }

                template<typename T>void subscribe(T subscriber){
                    subscribers.push_back(subscriber);
                }

                template<typename ValueType, typename... Args> void send(
                    KeyType key, 
                    ValueType value
                ){

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

