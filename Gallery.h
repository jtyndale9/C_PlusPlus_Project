#ifndef Gallery_H_INCLUDED
#define Gallery_H_INCLUDED

#include "Enemy.h"
#include <string>
#include <vector>

class Gallery{
    friend class Enemy;
    public:
        Gallery(const std::string filename1,const std::string filename2,const std::string filename3="",const std::string filename4="",const std::string filename5="",const std::string filename6="");

        void read(const std::string filename, const std::string keyfilename);
        void add(const Enemy& e);

        void clear(){this->enemies.clear();};
        
        size_t size() const{return this->enemies.size();};
        size_t empty() const{return this->enemies.size()==0;};

        Enemy* get(size_t n);
        const Enemy* get(size_t n) const;

        //operators
        Enemy& operator[](const size_t n){ return *(this->get(n));};
        const Enemy& operator[](const size_t n) const{ return *(this->get(n));};
        operator bool() const { return !this->empty(); };


        typedef std::vector<Enemy>::const_iterator iterator;
        typedef std::vector<Enemy>::const_iterator const_iterator;

        const_iterator begin() const{ return this->enemies.begin(); }
        const_iterator cbegin() const{ return  this->enemies.cbegin(); }   

        const_iterator end() const{ return  this->enemies.end(); }
        const_iterator cend() const{ return  this->enemies.cend(); }
    private:    
        std::vector<Enemy> enemies;
};
std::ostream& operator<<(std::ostream &,const Gallery&);
#endif  /* Gallery_H_INCLUDED */
