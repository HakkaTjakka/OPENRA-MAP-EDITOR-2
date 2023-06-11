#ifndef FUNCTIONS_HPP_INCLUDED
#define FUNCTIONS_HPP_INCLUDED


#include <iostream>
#include <algorithm>
//#include <conio.h>
#include <stdlib.h>
#include <cstdlib>
#include <string.h>
#include <sys/stat.h>
#include <filesystem>
#include <map>
#include <vector>

#include <SFML/System/Thread.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <X11/Xlib.h>


using namespace std;


struct Template {
    int Id;
    std::string Images;
    sf::Vector2i Size;
    sf::Vector2i Realsize;
    std::string Catagories;
    std::map< int, std::string > Tiles;
    sf::Image Image;

    bool operator < (const Template &B) const
    {
        return Id < B.Id;
    }
};


struct Tileset {
    long Lookup;    // id * 256 + tile (3 bytes ( * (x + x*y) ) from first block (offset 17) from .bin file)

    bool operator = (const Tileset &B) const
    {
        return Lookup == B.Lookup;
    }
};


long int read_bin_file( string filename, unsigned char* &mem);
long main_readbin(char* filename, unsigned char* &bin);

void launch_my_window();
int do_window();

extern int my_window_update;
int main2();
bool file_exists(const char * filename);
int edit_bin(unsigned char* bin, long size);
int make_bin(unsigned char* bin, long size);
int picture_bin(unsigned char* bin, long size);
int save_bin(unsigned char* bin, long size, std::string filename);
int main_readtileset( char* filename, sf::RenderTexture &my_rendertexture, std::map<int, int> &Tileset );
unsigned char* resize_bin(unsigned char* bin, long filesize, int new_x, int new_y );
int insert_bin(unsigned char* bin, unsigned char* bin2, long filesize, long filesize2);
int insert_bin_range(unsigned char* bin, unsigned char* bin2, long filesize, long filesize2, int top, int left, int width, int height );

struct Drawing {
    sf::RenderWindow window;
    sf::RenderTexture rendertexture;
    sf::Sprite sprite;
    sf::Texture tiles;
    bool kill=false;
    sf::Event event;
    sf::Sprite backgroundsprite;
    bool smooth=true;
    bool setverticalsync=true;
    bool wantsetverticalsync=true;
    float rotation=0.0;

};

int showbin(unsigned char* bin, sf::RenderTexture &tiles_texture, std::map<int, int> &Tileset, struct Drawing* drawing );
//int showbin(unsigned char* bin, sf::RenderTexture &tiles_texture, std::map<int, int> &Tileset );
int main_window(unsigned char* bin, sf::RenderTexture &tiles_texture, std::map<int, int> &Tileset );

//int main_window(unsigned char* bin, std::map<int, int> &Tileset );
int do_events(struct Drawing* drawing);




#endif // FUNCTIONS_HPP_INCLUDED
