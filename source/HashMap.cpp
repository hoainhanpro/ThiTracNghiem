#include "../header/HashMap.h"

HashNode::HashNode(std::string key, std::string value)
{
    this->key = key;
    this->value = value;
    this->next = nullptr;
}

HashMap::HashMap()
{
    this->size = 100;
    this->data = new HashNode *[size];
    for (int i = 0; i < size; i++)
    {
        data[i] = nullptr;
    }
}

HashMap::HashMap(int size)
{
    this->size = size;
    this->data = new HashNode *[size];
    for (int i = 0; i < size; i++)
    {
        data[i] = nullptr;
    }
}

HashMap::HashMap(std::initializer_list<std::pair<std::string, std::string>> list) : HashMap(list.size())
{
    typename std::initializer_list<std::pair<std::string, std::string>>::iterator it = list.begin();

    for( ;it != list.end(); it++)
    {
        int index = hashFunction(it->first);
        HashNode *node = new HashNode(it->first, it->second);
        node->next = data[index];
        data[index] = node;
    }
}

std::string &HashMap::operator[](std::string key)
{
    int index = hashFunction(key);
    HashNode *node = data[index];
    while (node != nullptr)
    {
        if (node->key == key)
        {
            return node->value;
        }
        node = node->next;
    }
    return data[index]->value;
}

HashMap::~HashMap()
{
    for (int i = 0; i < size; i++)
    {
        delete data[i];
    }
    delete[] data;
}

/**
 * @brief DJB2 hash function
*/
int HashMap::hashFunction(std::string key)
{
    uint32_t hash = 5381;
    char c;
    for (uint32_t i = 0; i < key.length(); i++)
    {
        c = key[i];
        hash = ((hash << 5) + hash) + c;
    }
    return hash % size;
}