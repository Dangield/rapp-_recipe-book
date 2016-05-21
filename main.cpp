#include <iostream>
#include <fstream>
#include <string>

#include <rapp-robots-api/info/info.hpp>
#include <rapp-robots-api/communication/communication.hpp>
#include <rapp-robots-api/navigation/navigation.hpp>


std::vector<std::string> getRecipies(){
    std::vector<std::string> recipies;
    recipies.push_back("one");
    recipies.push_back("two");

    return recipies;
}

void tellRecipies(rapp::robot::communication &comm)
{
    comm.text_to_speech("Say one for pancake.");
    comm.text_to_speech("Say two for omlette.");
    return;
}

std::string getRecipeName(std::string result)
{
    std::string name;

    if(result == "one")
        name = "share/recipe-book/pancake.txt";
    else if(result == "two")
        name = "share/recipe-book/omlette.txt";

    return name;
}

void sorry(rapp::robot::navigation &nav)
{
    nav.moveJoint({"HeadYaw"}, {0.5}, 1.0f);
    nav.moveJoint({"HeadYaw"}, {-0.5}, 1.0f);
    nav.moveJoint({"HeadYaw"}, {0}, 1.0f);
}

void highFive(rapp::robot::navigation &nav){
    nav.moveJoint({"LShoulderPitch","LShoulderRoll","LElbowYaw", "LElbowRoll", "LWristYaw", "LHand"},
    {0,1.5,-1.6,-1.6,0,1}, 0.5);
}


int main(int argc, char * argv[]) {
    rapp::robot::info info(argc, argv);
    rapp::robot::communication comm(argc, argv);
    rapp::robot::navigation nav(argc, argv);

    nav.moveJoint({"HeadYaw","HeadPitch"}, {0,0}, 1.0f);

    nav.rest("SitRelax");

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

        sorry(nav);
        comm.text_to_speech("Sorry, I don't understand you!");

    } while (1);

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

    words.clear();
    words.push_back("yes");
    words.push_back("no");
    words.push_back("all");
    words.push_back("help");

    int i=0;
    while(ingredients.size()!=0){

        comm.text_to_speech("Do we have "+ingredients[i]+"?");
        result = comm.word_spotting(words);

        if(result=="all")break;

        else if(result=="help")
        {
            comm.text_to_speech("We need the following ingredients.");
            for(int j=0;j<ingredients.size();j++)
                comm.text_to_speech(ingredients[j]);
            continue;
        }

        else if(result=="yes")
        {
            ingredients.erase(ingredients.begin()+i);
            if (i==ingredients.size())i=0;
        }

        else if(result=="no")
        {
            i++;
            if (i==ingredients.size())i=0;
        }

        else
        {
            sorry(nav);
            comm.text_to_speech("Sorry, I don't understand you.");
        }
    }


    comm.text_to_speech("We have all of the ingredients.");
    comm.text_to_speech("Lets start cooking!");
    words.clear();
    words.push_back("done");
    words.push_back("repeat");
    std::getline(recipeFile, line);

    bool isUnderstand=true;
    do{

        if(isUnderstand)comm.text_to_speech(line);
        result = comm.word_spotting(words);
        isUnderstand=true;
        if(result=="repeat")
            continue;
        else if(result=="done"){
            std::getline(recipeFile, line);
            if(line=="@")break;
        }
        else {
            sorry(nav);
            comm.text_to_speech("Sorry, I don't understand you!");
            isUnderstand=false;
        }


    }while(1);


    comm.text_to_speech("We cooked a dish!");
    highFive(nav);
    comm.text_to_speech("High five!");
    return 0;
}
