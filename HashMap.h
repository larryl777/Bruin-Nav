//
//  HashMap.hpp
//  project4
//
//  Created by Larry Lim on 3/6/24.
//

#ifndef HashMap_h
#define HashMap_h

#include <string>
#include <iostream>
#include <vector>
#include <list>

template <typename T>
class HashMap
{
public:
    HashMap(double max_load = 0.75); // constructor
    ~HashMap(); // destructor; deletes all of the items in the hashmap
    int size() const; // return the number of associations in the hashmap
    // The insert method associates one item (key) with another (value).
    // If no association currently exists with that key, this method inserts
    // a new association into the hashmap with that key/value pair. If there is
    // already an association with that key in the hashmap, then the item
    // associated with that key is replaced by the second parameter (value).
    // Thus, the hashmap must contain no duplicate keys.
    void insert(const std::string& key, const T& value);
    // If no association exists with the given key, return nullptr; otherwise,
    // return a pointer to the value associated with that key. This pointer can be
    // used to examine that value or modify it directly within the map.
    const T* find(const std::string& key) const;
    // If no association exists with the given key, return nullptr; otherwise,
    // return a pointer to the value associated with that key. This pointer can be
    // used to examine that value or modify it directly within the map.
    T* find(const std::string& key) {
        const auto& hm = *this;
        return const_cast<T*>(hm.find(key));
    }
    
    // Defines the bracket operator for HashMap, so you can use your map like this:
    // your_map["david"] = 2.99;
    // If the key does not exist in the hashmap, this will create a new entry in
    // the hashmap and map it to the default value of type T. Then it will return a
    // reference to the newly created value in the map.
    T& operator[](const std::string& key);
    
    HashMap(const HashMap&) = delete;
    HashMap& operator=(const HashMap&) = delete;
    
private:
    int m_buckets;
    int m_items;
    double m_max;
    struct pair{
        std::string key;
        T value;
        
    };
    std::list<pair>* m_map;
    
    void exceedmaxload();       //step 7, reallocate new hash map
};

template <typename T>
HashMap<T>::HashMap(double max_load){
    m_buckets = 10;
    m_items = 0;
    m_max  = max_load;
    
    m_map = new std::list<pair>[m_buckets];     //allocate array of linked lists
}
template <typename T>
HashMap<T>::~HashMap(){
    for(int i = 0; i < m_buckets; i++){
        m_map[i].clear();
    }
    delete [] m_map;
}


template <typename T>
int HashMap<T>::size() const{
    return m_items;
}

template <typename T>
void HashMap<T>::insert(const std::string& key, const T& value){
    T* exists = find(key);              //check if key exists in container
    if(exists != nullptr){
        *exists = value;                //repalce old valuew with new value
        return;
    }
    std::size_t hashval = std::hash<std::string>()(key);        //hashing
    
    size_t bucketnum = hashval % m_buckets;
    m_map[bucketnum].push_back({key, value});               //adding pair to list
    m_items++;
    
    if(m_items/ static_cast<double>(m_buckets) > m_max){        //check if load is exceeded
        exceedmaxload();
        
    }
}

template <typename T>
const T* HashMap<T>::find(const std::string& key) const{
    std::size_t hashval = std::hash<std::string>()(key);        //hashing
    
    size_t bucketnum = hashval % m_buckets;
    
    for(auto it = m_map[bucketnum].begin(); it != m_map[bucketnum].end(); it++){        //loop through container to find key
        if(it->key == key){
            return &it->value;
        }
    }
    return nullptr;
}

template <typename T>
T& HashMap<T>::operator[](const std::string& key){
    T* exists = find(key);          //check if it exists
    if(exists != nullptr){          //return it if it dose
        return *exists;
    }
    
    std::size_t hashval = std::hash<std::string>()(key);  //hashing
    
    size_t bucketnum = hashval % m_buckets;
    
    m_map[bucketnum].push_back({key, T()});             //adding pair
    m_items++;
    
    if(m_items/ static_cast<double>(m_buckets) > m_max){        //check if exceeds load
        exceedmaxload();
    }
     return m_map[bucketnum].back().value;
}
template <typename T>
void HashMap<T>::exceedmaxload(){
    int x = m_buckets;
    m_buckets *=2;                          //double size
    std::list<pair>* newmap = new std::list<pair>[m_buckets];        //new array that is oduble size
    for (int i = 0; i < x; i++) {                                       //loop through current array
        for (auto it = m_map[i].begin(); it != m_map[i].end();) {
            auto current = it++;
            std::size_t hashval = std::hash<std::string>()(current->key);       //hashing
            size_t bucketnum = hashval % m_buckets;
            newmap[bucketnum].splice(newmap[bucketnum].begin(), m_map[i], current);  //splice to prevent duplicates
        }
        
    }
    delete[] m_map;
    m_map = newmap;                             //transfer over
}
#endif /* HashMap_hpp */
