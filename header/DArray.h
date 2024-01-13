#ifndef DARRAY_H
#define DARRAY_H

template <class type>
class DArray
{
    public:
        DArray()
        {
            data = new type[1];
            capacity = 1;
            count = 0;
        }

        ~DArray()
        {
            delete[] data;
        }

        DArray(const DArray<type> &other)
        {
            data = new type[other.capacity];
            capacity = other.capacity;
            count = other.count;
            for (int i = 0; i < count; i++)
            {
                data[i] = other.data[i];
            }
        }

        DArray &operator=(const DArray<type> &other)
        {
            if (this != &other)
            {
                delete[] data;
                data = new type[other.capacity];
                capacity = other.capacity;
                count = other.count;
                for (int i = 0; i < count; i++)
                {
                    data[i] = other.data[i];
                }
            }
            return *this;
        }

        type &operator[](int index)
        {
            return data[index];
        }

        void push_back(type value)
        {
            if (count == capacity)
            {
                resize(capacity * 2);
            }
            data[count] = value;
            count++;
        }

        void pop_back()
        {
            if (count > 0)
            {
                count--;
            }
        }

        void remove(int index){
            if(index < 0 || index >= count) return;
            for(int i = index; i < count - 1; i++){
                data[i] = data[i + 1];
            }
            count--;
        }

        int back()
        {
            return data[count - 1];
        }

        int size()
        {
            return count;
        }

        void resize(int new_size)
        {
            if(new_size <= 0){
                clear();
                return;
            }
            type *temp = new type[new_size];

            count = new_size > count ? count : new_size;
            for (int i = 0; i < count; i++)
            {
                temp[i] = data[i];
            }
            delete[] data;
            data = temp;
            capacity = new_size;
        }

        void clear()
        {
            count = 0;
        }

    private:
        type *data;
        int capacity;
        int count;
};

#endif