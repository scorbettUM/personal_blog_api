#include <unordered_map>
#include <string>
#include <vector>
#include <atomic>



namespace utilities {
    namespace cache {

        enum StoreResult {
            CACHE_ITEM_UPDATED,
            CACHE_ITEM_CREATED
        };

        template<typename K, typename T>
        class QuickStore {
            public:
                QuickStore(
                    int size=100
                ){
                    keys.reserve(size * 2);
                    values.reserve(size * 2);
                }   

                std::vector<K> iter(){
                    return keys;
                }

                StoreResult store(K key, T value){

                    auto index_val = index(key);

                    if (index_val > -1){
                        keys[index_val] = key;
                        values[index_val] = value;

                        return StoreResult::CACHE_ITEM_UPDATED;

                    } else {

                         if (current_item_idx >= current_allocations){
                            keys.push_back(key);
                            values.push_back(value);

                            current_allocations += 1;

                        } else {
                            keys[current_item_idx] = key;
                            values[current_item_idx] = value;

                        }

                        current_item_idx = (current_item_idx + 1)%cache_size;

                        return StoreResult::CACHE_ITEM_CREATED;

                    }
                    
                }

                void setByIndex(int index, K key, T value){
                    keys[index] = key;
                    values[index] = value;
                }

                int index(K key){
                    int idx = 0;
                    for (const auto &cache_key : keys){
                        if (cache_key == key){
                            return idx;
                        }

                        idx += 1;
                    }

                    return -1;

                }

                std::optional<T> get(K key){
                    

                    int idx = 0;
                    for (const auto &cache_key : keys){
                        if (cache_key == key){
                            return std::make_optional(
                                values.at(idx)
                            );
                        }

                        idx += 1;
                    }

                     return std::nullopt;

                }

                std::pair<K, T> last(){
                    return std::pair(
                        keys.back(),
                        values.back()
                    );
                }

                std::optional<std::pair<K, T>> pop(){

                    if (current_allocations > 0){
                         return std::make_optional<std::pair<K, T>>(
                            std::pair(
                                keys.pop_back(),
                                values.pop_back()
                            )
                         );

                        current_allocations -= 1;
                    }

                    return std::nullopt;
                   
                }

                void clear(){
                    keys.erase(keys.begin(), keys.end());
                    values.erase(values.begin(), values.end());
                }

                void resize(int size){
                    clear();

                    keys.resize(size); 
                    values.resize(size);
                }
                std::vector<K> keys;
                std::vector<T> values;

            private:
                int current_item_idx = 0;
                int cache_size = 100;
                int current_allocations = 0;
                float expansion_factor = 1.68;
                
        };
    }
}