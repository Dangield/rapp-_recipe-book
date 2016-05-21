#include <iostream>
#include <fstream>
#include <string>

#include <rapp-robots-api/info/info.hpp>
#include <rapp-robots-api/communication/communication.hpp>

std::vector<std::string> getRecipies(){
    std::vector<std::string> recipies;
    recipies.push_back("one");
    recipies.push_back("two");
    recipies.push_back("three");
    recipies.push_back("four");
    recipies.push_back("five");

    return recipies;
}

void tellRecipies(rapp::robot::communication comm)
{
    comm.text_to_speech("Say one for pancake.");
}

std::string getRecipeName(std::string result)
{
    std::string name;

    if(result == "one")
        name = "share/recipe-book/pancake.txt";
    else if(result == "two")
        name = "share/recipe-book/pancake.txt";
    else if(result == "three")
        name = "share/recipe-book/pancake.txt";
    else if(result == "four")
        name = "share/recipe-book/pancake.txt";
    else if(result == "five")
        name = "share/recipe-book/pancake.txt";
    else if(result == "six")
        name = "share/recipe-book/pancake.txt";
    else if(result == "seven")
        name = "share/recipe-book/pancake.txt";

    return name;
}


int main(int argc, char * argv[]) {
    rapp::robot::info info(argc, argv);
    rapp::robot::communication comm(argc, argv);

    comm.text_to_speech("What do you want to cook?");
    tellRecipies(comm);

    // create list of words for recognition
    std::vector<std::string> words = getRecipies();
    words.push_back("abort");
    std::string result;


    // start word spotting until proper word will be recognized
    do {
        result = comm.word_spotting(words);

        if (result == "abort") {
            comm.text_to_speech("Ok, Goodbye!");
            return 0;
        }

        else{
            bool isUnderstand = false;
            for(int i = 0 ; i < words.size() ; i++)
                if(words[i] == result){
                    isUnderstand = true;
                    break;
                }
            if(isUnderstand)
                break;
        }

        comm.text_to_speech("Sorry, I don't understand you!");

    } while (1);

    comm.text_to_speech(result);
    result = getRecipeName(result);

    std::ifstream recipeFile;
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
