#include "texture_store.hpp"
#include <iostream>


Texture2D TextureStore::GetTexture(textureId id){
    return textures[id];
}

textureId TextureStore::LoadTextureWithPath(std::string path){
    std::cout << "Loading " << path << std::endl;
    if(mapping.count(path)){
        return mapping[path];
    }
    
    textureId id = textures.size();
    std::cout << "Texture-id :" << id << std::endl;
    textures.push_back(LoadTexture(path.c_str()));
    mapping[path] = id;
    reverse_mapping.push_back(path);
    return id;
}

TextureStore::~TextureStore(){
    for(auto t: textures){
        UnloadTexture(t);
    }
}

std::string TextureStore::GetTextureNameFromId(textureId id){
    return reverse_mapping[id];
}