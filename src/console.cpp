#include <raylib.h>
#include <iostream>
#include <format>
#include "console.hpp"
#include "entity.hpp"


void Console::Draw(){
    ECS* ecs = ECS::instance();
    if(IsKeyPressed(KEY_GRAVE)){
        toggled = !toggled;
        if(toggled){
            ecs->SwitchState(State::CONSOLE);
        } else {
            ecs->SwitchState(State::PLAY);
        }
    }
    if(!toggled){
        return;
    }
    char c = 0;

        
    auto key = GetKeyPressed();
    if(key == KEY_GRAVE){
        key = GetKeyPressed();
    }
    if(key == KEY_ENTER && current_command.length() > 0){
        ParseCommand();
        history.push_back(current_command);
        current_command.clear();
    } else if(key == KEY_UP){
        current_command = history.back();
        history.pop_back();
    } else if(key == KEY_BACKSPACE && current_command.length() > 0){
        current_command.pop_back();
    } else if((key >= 32) && (key <= 125)){
        current_command.push_back((char)std::tolower(key));
    }


    DrawRectangle(0,0,SCREEN_WIDTH,SCREEN_HEIGHT,GRAY);
    for(int i = 0 ; i < history.size(); i ++){
        DrawText(history[i].c_str(),32,SCREEN_HEIGHT*0.9 + 16 - (((history.size() - i) + 1) * 32),32,DARKGRAY);
    }
    DrawRectangle(0,SCREEN_HEIGHT*0.9,SCREEN_WIDTH,SCREEN_HEIGHT,DARKGRAY);
    DrawText(current_command.c_str(),32,SCREEN_HEIGHT*0.9 + 16,32,BLACK);
}

void Console::ParseCommand(){
    size_t pos = 0;
    std::string s = current_command;
    std::string token;
    std::vector<std::string> parts;
    while ((pos = s.find(' ')) != std::string::npos) {
        token = s.substr(0, pos);
        parts.push_back(token);
        s.erase(0, pos + 1);
    }
    parts.push_back(s);
    for(auto part : parts){
        std::cout << part << std::endl;
    }
    if(parts[0] == "tmload"){
        if(parts.size() != 2){
            history.push_back("ERROR: Wrong amount of commands passed into the console, use tmload like: tmload <filepath>");
            return;
        }
        std::string path = parts[1];
        ECS* ecs = ECS::instance();
        ecs->tilemap->Load(path);
        return;
    }

    if(parts[0] == "tmsave"){
        if(parts.size() != 2){
            history.push_back("ERROR: Wrong amount of commands passed into the console, use tmsave like: tmsave <filepath>");
            return;
        }
        std::string path = parts[1];
        ECS* ecs = ECS::instance();
        ecs->tilemap->Save(path);
        return;
    }
    
    if(parts[0] == "help"){
        if(parts.size() != 1){
            history.push_back("ERROR: Wrong amount of commands passed into the console, use help like: help");
            return;
        }
        history.push_back("HELP     - shows commands and how to use them");
        history.push_back("TMSAVE   - saves a tilemap into a file: TMSAVE <filename>");
        history.push_back("TMLOAD   - loads a tilemap from a file: TMLOAD <filename>");
        history.push_back("TMEDIT   - allows you to edit the tilemao");
        history.push_back("TDADD - adds new tile definition: TDADD <texture path> <collision>");
    }

    if(parts[0] == "tmedit"){
        if(parts.size() > 2){
            history.push_back("ERROR: Wrong amount of commands passed into the console, use tmedit like: tmedit <tileid>");
            return;
        }
        ECS* ecs = ECS::instance();
        if(parts.size() == 1){
            history.push_back("NOTICE: Turned off editing mode.");
            ecs->tilemap->editing = false;
            return;
        }
        tileId id = stoi(parts[1]);
        history.push_back(std::format("Editing with tileId: {}", id));
        ecs->tilemap->editing = true;
        ecs->tilemap->brush = id;
    }

    if(parts[0] == "tdadd"){
        if(parts.size() != 3){
            history.push_back("ERROR: Wrong amount of commands passed into the console, use tdadd like: tdadd <sprite-path> <collision>");
            return;
        }
        std::string path = parts[1];
        std::string scollision = parts[2];
        bool collision;
        if(scollision == "1"){
            collision = true;
        }else if(scollision == "0"){
            collision = false;
        } else {
            history.push_back("ERROR: expected 1 or 0");
            return;
        }
        ECS* ecs = ECS::instance();
        TextureStore* txt = TextureStore::instance();
        tileId id = ecs->tilemap->InsertTileDefinition(TileDefinition(txt->LoadTextureWithPath(path),collision));
        history.push_back(std::format("Successfully inserted tile with id: {}",id));
    }
}