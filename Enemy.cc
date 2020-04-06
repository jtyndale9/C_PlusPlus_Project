#include "Enemy.h"
#include "Gallery.h"
#include <fstream>
#include <sstream>
#include <algorithm>

using namespace std;
typedef std::pair<std::string,std::string> KeyValue;

Enemy::Enemy():keyFileProvided(false){}
Enemy::Enemy(const string keyfile):keyFileProvided(true){	//Associate a keyfile with this enemy, for error-checking. 
    ifstream inFile(keyfile);	//define inFile
    if (!inFile.is_open()) //throw an error if could not open this file
        throw runtime_error("Could not open key file: "+string(keyfile));

    //File successfully opened: now can read it line-by-line
	string line;
    while(getline(inFile,line)){

        if (!isalnum(line[0]) || isspace(line[0]))
            throw runtime_error("Each line in keyfile should start with a number or letter: "+string(keyfile));
        stringstream words(line);
        
        string key;
        words>>key;
        if (!key.empty())
	        keys.push_back(key); //put the first word in a line to keys
        else
            throw runtime_error("Found empty line in key file: "+string(keyfile));
        
        
        if (words>>key)
            throw runtime_error("Incorrect formatting: multiple keys in same line in "+string(keyfile));
    }
    if (keys.empty())
        throw runtime_error("Provided keyfile is empty"+ keyfile);
    inFile.close();
}
Enemy::Enemy(const Enemy &other) :keyFileProvided(other.keyFileProvided), content(other.content) { 

    this->showName = other.showName;
    this->showOther = other.showOther;
    this->showLink = other.showLink;

    this->keys = other.keys;
    this->gallery = other.gallery;
}

Enemy &Enemy::operator=(const Enemy &rhs){
    this->content = rhs.content;

    this->showName = rhs.showName;
    this->showOther = rhs.showOther;
    this->showLink = rhs.showLink;

    this->keys = rhs.keys;
    this->keyFileProvided = rhs.keyFileProvided;
    this->gallery = rhs.gallery;
    return *this;
}


bool Enemy::lineIsEmpty(string line){
    for (char c: line){
        if (isalnum(c))
            return false;
    }
    return true;
}
bool Enemy::lineHasKey(string line){
    return isalnum(line[0]);
}
void Enemy::saveKeyValuePair(string& currentKey,string& currentValue,vector<KeyValue>& tempStorage){
    if(currentValue.empty())
        throw runtime_error("Found a key "+currentKey+" with no corresponding value" );
    else{
        currentValue = currentValue.substr(0, currentValue.size()-1); //we were adding a whitespace after every word in value, now need to remove the redundant last one
        tempStorage.push_back(make_pair(currentKey,currentValue));                
    }
}

bool Enemy::read(std::istream& in){
    this->clear();
    vector<KeyValue> tempStorage; //temporary storage for read pairs

    string line;

    string currentKey;
    string currentValue;
    while(getline(in,line)){
        if (lineIsEmpty(line) && currentKey.empty()) //skip all preceding empty lines
            continue; 

        stringstream words(line);

        if (lineHasKey(line)){ //found a line with key in it
            if(!currentKey.empty())
                saveKeyValuePair(currentKey,currentValue,tempStorage);
            words>>currentKey;
            currentValue.clear();
        }

        string word;
        while(words>>word){
            currentValue+=word+' ';
        }

        if(lineIsEmpty(line)&& !currentKey.empty()){ //once found empty line after the contents, save and exit
            if(!currentKey.empty())
                saveKeyValuePair(currentKey,currentValue,tempStorage);
            currentKey.clear();
            currentValue.clear();
            break;
        }
    }
    if(!currentKey.empty()&&!currentValue.empty()){ 
        saveKeyValuePair(currentKey,currentValue,tempStorage);
    }

    if(currentKey.empty()&&!currentValue.empty())
        throw runtime_error("Incorrectly formatted input");

    if(tempStorage.empty())
        return false;
    this->checkKeys(tempStorage);
    this->reorderAndStorePairs(tempStorage);

    return true;
}
/*      
        If a non-alphanumeric key is encountered, throw a runtime_error, mentioning the bad key.
*/
bool Enemy::checkKeys(const vector<KeyValue>& toCheck) const{	//Return true if Enemy satisfies constraints on keys

    //If no Name key is read, throw a runtime_error.
    auto foundName = find_if(toCheck.begin(), toCheck.end(), [] (const KeyValue& keyValuePair) { return keyValuePair.first == "Name"; });
    if (foundName == toCheck.end())
        throw runtime_error("No Name key is provided");

    //If duplicate keys are encountered, throw a runtime_error, mentioning the bad key
    for(auto p=toCheck.begin(); p<toCheck.end(); p++){
        string key = p->first;
        auto foundDuplicateBefore = find_if(toCheck.begin(), p, [key] (const KeyValue& keyValuePair) { return keyValuePair.first == key; });
        auto foundDuplicateAfter = find_if(p+1, toCheck.end(), [key] (const KeyValue& keyValuePair) { return keyValuePair.first == key; });

        if(foundDuplicateBefore!=p || foundDuplicateAfter!=toCheck.end())
            throw runtime_error("Found duplicate key: "+p->first);
    }

    //If a key was found that was not given in the keyfile (unknown key)
    if(this->keyFileProvided){
        for(auto pair:toCheck){
            auto foundKey = find_if(this->keys.begin(), this->keys.end(), [pair] (const string& key) { return key == pair.first; });
            if(foundKey==this->keys.end())
                throw runtime_error("Key "+pair.first+" is not in keyfile");
        }
    }
    return true;
}

void Enemy::reorderAndStorePairs(const vector<KeyValue>& tempStorage){
    vector<KeyValue> notNameOrLinks;
    vector<KeyValue> links;

    for(auto pair:tempStorage){
        if(pair.first=="Name"){
            this->content.push_back(pair);
            continue;
        }
        
        if(pair.first.substr(0,4)=="Link"){
            links.push_back(pair);
            continue;
        }

        //if it is not Name or Link..., add it to vector
        notNameOrLinks.push_back(pair);
    }

    for(auto pair:notNameOrLinks)
        this->content.push_back(pair);

    for(auto pair:links)
        this->content.push_back(pair);
}

void Enemy::write(ostream& os) const{

    auto filtered = this->filterContent();
    auto longestKey = max_element(filtered->begin(), filtered->end(), 
        [] (const KeyValue lhs, const KeyValue rhs) {
            return lhs.first.size()<rhs.first.size();
        });
    int longestKeySize = longestKey->first.size();

    for(auto pair : *filtered){
        
        int paddingSize = longestKeySize - pair.first.size();

        os<<pair.first<<string(paddingSize, ' ')<<' '<<pair.second<<'\n';
    }
}
void Enemy::write(const string filename) const{
    ofstream fs(filename);
    if(!fs)
        throw runtime_error("Could not open file to write: "+string(filename));
    this->write(fs);
}
std::unique_ptr<std::vector<KeyValue>> Enemy::filterContent() const{
    auto filtered = make_unique<vector<KeyValue>>();

    //IMPORTANT: assuming that content vector was reordered properly on read!!!
    for(auto pair: this->content){
        if(pair.first=="Name"){
            if(showName)
                filtered->push_back(pair);
            continue;
        }        

        if(pair.first.substr(0,4)=="Link"){
            if(showLink)
                filtered->push_back(pair);            
            continue;
        }

        if(showOther){
            filtered->push_back(pair);
            continue;
        }
    }
    return filtered;
}

string Enemy::field(const string& key) const{
    auto foundValue = find_if(this->content.begin(), this->content.end(), [key] (const KeyValue& keyValuePair) { return keyValuePair.first == key; });        
    if (foundValue == this->content.end())
        throw range_error("No such field: "+key);
    return foundValue->second;
}

bool Enemy::empty() const{	//Return true if Enemy has no key/value pairs.
    if(content.empty())
  	    return true;
    else
	    return false;
}

size_t Enemy::size() const{	//Return a size_t containing the number of key/value pairs in this Enemy.
    //cout << "actual: " << actualContent.size() << '\n';
    //cout << "file: " << fileContent.size() << '\n';
    return this->content.size();
    //cout << "2!\n";
}

KeyValue Enemy::get(size_t n) const{   
    if(n>=this->size()) {
        stringstream msg("Index ");
        msg<<n<<" Is out of range."<<" Max index is: "<<this->size()-1;        
        throw range_error(msg.str());
    }

    return this->content[n];
}

Enemy* Enemy::link(const string relation) const{
    string linkedName = this->field("Link"+relation);
    if(!this->gallery)
        throw runtime_error("This enemy is not a part of any gallery");

    auto foundEnemy = find_if(this->gallery->enemies.begin(), this->gallery->enemies.end(), 
        [linkedName] (const Enemy& other) { return other.field("Name") == linkedName; });
    if(foundEnemy ==this->gallery->enemies.end())        
        throw range_error("Linked enemy not found in this gallery");
    
    return &(*foundEnemy);
}

bool Enemy::operator==(const Enemy &other) const{
    if(this->content.size()!= other.content.size())
        return false;
    
    return is_permutation(this->content.begin(),this->content.end(),other.content.begin());
}
