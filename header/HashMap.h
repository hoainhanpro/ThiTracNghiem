#ifndef HASHMAP_H
#define HASHMAP_H

#include <initializer_list>
#include <utility>
#include <string>

/**
 * @brief HashMap but only for string key and string value
 */
class HashMap
{
    public:
        struct HashNode
        {
            std::string key;
            std::string value;
            HashNode *next;

            HashNode(std::string key, std::string value);
        };

        HashMap();
        HashMap(int size);
        HashMap(std::initializer_list<std::pair<std::string, std::string>> list);

        std::string &operator[](std::string key);

        ~HashMap();
        
    private:
        HashNode **data;
        int size;

        int hashFunction(std::string key);
        
};

typedef HashMap::HashNode HashNode;

#endif