#include <iostream>
#include <fstream>
#include <string>

#include <rapp-robots-api/info/info.hpp>
#include <rapp-robots-api/communication/communication.hpp>

std::vector<std::string> getRecipies(){
    std::vector<std::string> recipies;
    recipies.push_back("pancake");

    return recipies;
}


int main(int argc, char * argv[]) {
    rapp::robot::info info(argc, argv);
    rapp::robot::communication comm(argc, argv);

    comm.text_to_speech("What do you want to cook?");

    // create list of words for recognition
    std::vector<std::string> words = getRecipies();
    words.push_back("nothing");
    std::string result;


    // start word spotting until proper word will be recognized
    do {
        result = comm.word_spotting(words);

        if (result == "nothing") {
            comm.text_to_speech("Ok, Goodbye!");
            return 0;
        }

        else{
            bool isUnderstand=false;
            for(int i = 0 ; i < words.size() ; i++)
                if(words[i] == result){
                    isUnderstand=true;
                    break;
                }
            if(isUnderstand)break;
        }

        comm.text_to_speech("Sorry, I don't understand you!");

    } while (1);

    std::ifstream recipeFile;
    result="share/recipe-book/"+result+".txt";
    //const char* resultChar=result.c_str();
    recipeFile.open(info.get_path(result));
    if(!recipeFile.is_open()){
        comm.text_to_speech("Warning! Recipe missing!");
        return 0;
    }

    std::vector<std::string> ingredients;
    comm.text_to_speech("We need the following ingredients.");
    std::string line;
    while(std::getline(recipeFile, line)){
        if(line == "@")
            break;
        else{
            ingredients.push_back(line);
            comm.text_to_speech(line);
        }
    }

    words = ingredients;
    words.push_back("all");
    words.push_back("help");

    comm.text_to_speech("Tell me what do you have?");
    do{
       result = comm.word_spotting(words);

       if(result == "all"){
           break;
       }

       else if (result=="help"){
           comm.text_to_speech("We need.");
           for(int i = 0 ; i < words.size()-2 ; i++){
               comm.text_to_speech(words[i]);
           }
       }

       else{
           bool isUnderstand=false;
           for(int i = 0 ; i < words.size()-2 ; i++){
               if(words[i] == result){
                   words.erase(words.begin() + i);
                   break;
                   isUnderstand=true;
               }
           }
           if(!isUnderstand)comm.text_to_speech("Sorry, I don't understand you!");
       }

       if(words.size() == 2)
           break;

    }while(1);


    comm.text_to_speech("We have all of the ingredients.");
    comm.text_to_speech("Lets start cooking!");
    words.clear();
    words.push_back("done");
    words.push_back("repeat");
    std::getline(recipeFile, line);

    do{

        comm.text_to_speech(line);
        result = comm.word_spotting(words);
        if(result=="repeat")continue;
        else if(result=="done"){
            std::getline(recipeFile, line);
                if(line=="@")break;
        }
            else comm.text_to_speech("Sorry, I don't understand you!");


    }while(1);


    comm.text_to_speech("We cooked a dish!");
    comm.play_audio(info.get_path("share/talk_to_me/cheer.wav"));
    return 0;
}
