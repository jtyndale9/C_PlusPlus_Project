#ifndef Enemy_H_INCLUDED
#define Enemy_H_INCLUDED

#include <string>
#include <vector>
#include <memory>

class Gallery;
class Enemy{

    typedef std::pair<std::string,std::string> KeyValue;
    public:
        Enemy();
        Enemy(const std::string keyfile);
        Enemy(const Enemy &); //copy ctor 
        Enemy &operator=(const Enemy &); ////Assignment operator
        ~Enemy()=default; //destructor

        bool read(std::istream&);
        void write(std::ostream&) const;
        void write(const std::string filename) const;

        std::string field(const std::string& key) const;

        void show_name(bool option=true){this->showName=option;};
        void show_other(bool option=true){this->showOther=option;};
        void show_link(bool option=true){this->showLink=option;};
        
        void clear(){this->content.clear();};

        size_t size() const;
        bool empty() const;

        Gallery* gallery;
        Enemy* link(const std::string relation) const;


        //operators
        std::pair<std::string, std::string> operator[](const size_t n) const{ return this->get(n);};
        std::string operator[](const char* key) const{ return this->field(key);};
        std::string operator[](const std::string& key) const{ return this->field(key);};
        operator bool() const { return !this->empty(); };
		bool operator==(const Enemy &other) const;      
        bool operator!=(const Enemy &other) const{ return !(*this==other);};  
		

        typedef std::vector<KeyValue>::const_iterator iterator;
        typedef std::vector<KeyValue>::const_iterator const_iterator;
        
        const_iterator begin() const{ return this->content.begin(); }
        const_iterator cbegin() const{ return  this->content.cbegin(); }   

        const_iterator end() const{ return  this->content.end(); }
        const_iterator cend() const{ return  this->content.cend(); }
    private:
        bool showName = true, showOther = true, showLink = true;
        
        bool keyFileProvided = false;
        std::vector<std::string> keys;	       //keys from the key file
        std::vector<KeyValue> content;	       //key-value pairs

        KeyValue get(const size_t n) const;

        void reorderAndStorePairs(const std::vector<KeyValue>& tempStorage);

        void saveKeyValuePair(std::string& currentKey,std::string& currentValue,std::vector<KeyValue>& storage);
        bool lineIsEmpty(std::string line);
        bool lineHasKey(std::string line);

        std::unique_ptr<std::vector<KeyValue>> filterContent()const;
    
        bool checkKeys(const std::vector<KeyValue>& toCheck) const;
};

inline std::ostream& operator<<(std::ostream &os,const Enemy& e) {
    e.write(os);
    return os;
}

#endif /* Enemy_H_INCLUDED */
