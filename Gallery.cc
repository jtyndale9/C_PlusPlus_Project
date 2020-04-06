#include "Gallery.h"
#include <algorithm>
#include <fstream>
#include <sstream>

using namespace std;

Gallery::Gallery(const string filename1,const string filename2,const string filename3,const string filename4,const string filename5,const string filename6){
    vector<string> providedFiles;
    providedFiles.push_back(filename1);
    providedFiles.push_back(filename2);
    providedFiles.push_back(filename3);
    providedFiles.push_back(filename4);
    providedFiles.push_back(filename5);
    providedFiles.push_back(filename6);

    providedFiles.erase(remove_if(providedFiles.begin(), providedFiles.end(), [](const string filename) { return filename.empty(); }),providedFiles.end());

    //try to find all key files within this list
    vector<string> keyfiles;
    vector<string> enemyfiles;
    for(auto fname:providedFiles){
        // try to ese enemy to try to read this keyfile, if it fails => it was not a keyfile!
        try{
            Enemy mockEnemy(fname);
            keyfiles.push_back(fname);
        } catch(...){
            //assume that all files that were bad as keyfiles, could be valid enemy files
            enemyfiles.push_back(fname);
        }
    }
    if(keyfiles.size()<1)
        throw runtime_error("No valid keyfiles were provided");
    if(keyfiles.size()>1)
        throw runtime_error("More than 1 valid keyfile was provided");
    string keyfile=keyfiles[0];

    //start reading enemy files
    for(auto filename:enemyfiles){
        this->read(filename,keyfile);
    }
}

void Gallery::read(const std::string filename, const std::string keyfilename){
    ifstream inp(filename);
    if(!inp)
        throw runtime_error("Could not open file: "+filename);

    bool couldCreate=false;
    do{
        Enemy e(keyfilename);
        couldCreate= e.read(inp);
        if(couldCreate)
            this->add(e);
    }while(couldCreate);
}
void Gallery::add(const Enemy& e){
    this->enemies.push_back(e);
    auto lastAdded = this->enemies.end();
    lastAdded--;
    lastAdded->gallery = this;
}

Enemy* Gallery::get(size_t n){
    if(n>=this->size()) {
        stringstream msg("Index ");
        msg<<n<<" Is out of range."<<" Max index is: "<<this->size()-1;        
        throw range_error(msg.str());
    }

    return &(enemies[n]);
}

const Enemy* Gallery::get(size_t n) const{
    if(n>=this->size()) {
        stringstream msg("Index ");
        msg<<n<<" Is out of range."<<" Max index is: "<<this->size()-1;        
        throw range_error(msg.str());
    }

    const Enemy* e= &(enemies[n]);
    return e;
}

std::ostream& operator<<(std::ostream &os,const Gallery& g) {
    for(size_t enemy_index =0; enemy_index<g.size()-1; enemy_index++){
        os<<*(g.get(enemy_index))<<"\n";
    }
    os<<*(g.get(g.size()-1));

    return os;
}
