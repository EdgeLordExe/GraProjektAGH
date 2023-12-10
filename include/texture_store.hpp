#ifndef HPP_TEXTURE_STORE
#define HPP_TEXTURE_STORE
#include <cstdint>
#include <string>
#include <raylib.h>
#include <vector>
#include <unordered_map>

typedef uint64_t textureId;

class TextureStore{


    std::vector<Texture2D> textures;
    std::unordered_map<std::string,textureId> mapping;
    std::vector<std::string> reverse_mapping;

    TextureStore(){};
    ~TextureStore();
    static TextureStore* self;

    public:
    TextureStore(const TextureStore& obj)= delete; 
        static TextureStore* instance(){
            if (self == NULL) {
                self = new TextureStore(); 
                return self; 
            }else{
                return self;
            }
        }

    textureId LoadTextureWithPath(std::string path);

    std::string GetTextureNameFromId(textureId id);
    
    Texture2D GetTexture(textureId id);
};

#endif