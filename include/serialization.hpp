#ifndef HPP_SERIALIZATION
#define HPP_SERIALIZATION

#include <string>
#include <fstream>
#include <cassert>


class Serializer{
    public:
    
    Serializer(std::string path);
    ~Serializer();

    template<typename T>
    void Serialize(T x){
        assert(bad == false);
        output << x << '\n';
    }

    private:
        std::fstream output;
        bool bad;

};

class Deserializer{
    public:
    
    Deserializer(std::string path);
    ~Deserializer();

    template<typename T>
    T Deserialize(){
        assert(bad == false);
        T x;
        input >> x;
        return x;
    }

    private:
        std::fstream input;
        bool bad;

};

#endif