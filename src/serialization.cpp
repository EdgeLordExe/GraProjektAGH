#include <iostream>

#include "serialization.hpp"


Serializer::Serializer(std::string path){
    output.open(path,std::ios::out);

    if(output.bad()){
        std::cout << "Couldn't open " << path <<" for serialization!";
        bad = true;
        return;
    }
    bad = false;
}

Serializer::~Serializer(){
    output.close();
}

Deserializer::Deserializer(std::string path){
    input.open(path,std::ios::in);

    if(input.bad()){
        std::cout << "Couldn't open " << path <<" for deserialization!";
        bad = true;
        return;
    }
    bad = false;
}

Deserializer::~Deserializer(){
    input.close();
}
