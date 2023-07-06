#include <functions.hpp>

//sf::Texture my_backgroundTexture;
//sf::Sprite my_backgroundSprite;

extern sf::RenderTexture* backgroundRenderTexturePointer;

char tilesetfile[100] = "tilesets/interior.yaml";

int main(int argc, char ** argv) {
    XInitThreads();

    sf::RenderTexture backgroundRenderTexture;
//    sf::RenderTexture* backgroundRenderTexturePtr;
//    backgroundRenderTexturePtr = &backgroundRenderTexture;

    if (argc<2) {
        printf("%s option <filename>'\n",argv[0]);
        launch_my_window();
        return -1;
    }

    std::string option=std::string() + argv[1];
    transform(option.begin(), option.end(), option.begin(), ::tolower);

//    printf("option=\"%s\"\n",option.c_str());
////    launch_my_window();
////    sf::sleep(sf::seconds(1.0));
//    if ( option == "test2" ) {
//        launch_my_window();
//        return 0;
//    }
//    sf::sleep(sf::seconds(1.0));
    sf::RenderTexture my_rendertexture;

    unsigned char* bin = NULL;
    unsigned char* bin2 = NULL;
    long size;
    long size2;

//    std::map<int, struct Tileset> Tileset;
    std::map<int, int> Tileset;

//extern int main_window();

    if ( !(option == std::string() +"test") &&
         !(option == std::string() +"showbin") &&
         !(option == std::string() +"insert") &&
         !(option == std::string() +"insert-range") &&
         !(option == std::string() +"fromimage") &&
         !(option == std::string() +"resize") ) {
//        printf("Launching window.......%s %d\n",option.c_str(), option == "readbin" );
        launch_my_window();
        sf::sleep(sf::seconds(1.0));
    }


    if (option == "test") {
        main_window( bin, my_rendertexture, Tileset );

//        main_window();


    } else  if (option == "insert") {
        if (argc<4) {
            printf( "%s insert <path_to_map.bin> <path_from_map.bin>'\n", argv[0] );
            return -2;
        }
        printf("Inserting %s into %s to map.bin.insert\n" ,argv[3], argv[2] );
//        exit(0);

        if ( ( size = main_readbin( argv[2], bin ) ) > 0 ) {
            if ( ( size2 = main_readbin( argv[3], bin2 ) ) > 0 ) {

                int ret = insert_bin( bin, bin2, size, size2 );
                if ( ret == 0 ) {
                    printf( "Error map-size vs. file-size\n" );
                    return -1;
                }
                save_bin( bin, size, "map.bin.insert" );
                if ( bin != NULL ) free( bin );
                if ( bin2 != NULL ) free( bin2 );
                printf("Don't forget to save/edit your map.yaml file(s) before/after!!!\n\n");
                printf("If moving map.bin.resize, to free space for the insert, move within the in-game editor, map.yaml changes to\n\n");
                printf("After insert merge resized+moved map.yaml with inserted map.yaml to fix coords.\n\n");
            } else {
                if ( bin != NULL ) free( bin );
                if ( bin2 != NULL ) free( bin2 );
                return size;
            }
        } else {
            if ( bin != NULL ) free( bin );
            return size;
        }
    } else  if (option == "insert-range") {
        if (argc<8) {
            printf( "%s insert-range <path_to_map.bin> <path_from_map.bin> <left> <top> <width> <height>'\n", argv[0] );
            return -2;
        }
        int left = atoi(argv[4]);
        int top = atoi(argv[5]);
        int width = atoi(argv[6]);
        int height = atoi(argv[7]);
        printf("Inserting %s into %s to map.bin.insert from location left=%d top=%d width=%d height=%d\n" ,argv[3], argv[2], left, top, width, height );
//        exit(0);

        if ( ( size = main_readbin( argv[2], bin ) ) > 0 ) {
            if ( ( size2 = main_readbin( argv[3], bin2 ) ) > 0 ) {

                int ret = insert_bin_range( bin, bin2, size, size2, left, top, width, height );
                if ( ret == 0 ) {
                    printf( "Error map-size vs. file-size\n" );
                    return -1;
                }
                save_bin( bin, size, "map.bin.insert" );
                if ( bin != NULL ) free( bin );
                if ( bin2 != NULL ) free( bin2 );
                printf("Don't forget to save/edit your map.yaml file(s) before/after!!!\n\n");
                printf("If moving map.bin.resize, to free space for the insert, move within the in-game editor, map.yaml changes to\n\n");
                printf("After insert merge resized+moved map.yaml with inserted map.yaml to fix coords.\n\n");
            } else {
                if ( bin != NULL ) free( bin );
                if ( bin2 != NULL ) free( bin2 );
                return size;
            }
        } else {
            if ( bin != NULL ) free( bin );
            return size;
        }
    } else  if (option == "resize") {
        if (argc<5) {
            printf( "%s resize <path_to_map.bin> <size_x> <size_y>'\n", argv[0] );
            return -2;
        }
        int size_x = atoi(argv[3]);
        int size_y = atoi(argv[4]);
        printf("Converting %s to map.bin.resize with size %dx%d\n" ,argv[2], size_x, size_y );
//        exit(0);

        if ( ( size = main_readbin( argv[2], bin ) ) > 0 ) {

            unsigned char* ret = resize_bin( bin, size, size_x, size_y );
            if ( ret == NULL ) {
                printf( "Error map-size vs. file-size\n" );
                return -1;
            }
            save_bin( ret, 17 + 5 * size_x * size_y, "map.bin.resize" );
            if ( bin != NULL ) free( bin );
            if ( ret != NULL ) free( ret );
            printf("Don't forget to change your map.yaml file to adapt size(s)!!!\n\n");
            printf("MapSize: %d,%d\n\n", size_x, size_y);
            printf("Bounds: 1,1,%d,%d\n\n", size_x-2, size_y-2);
        } else {
            if ( bin != NULL ) free( bin );
            return size;
        }

    } else  if (option == "readbin") {


        if (argc<3) {
            printf( "%s readbin <path_to_map.bin>'\n", argv[0] );
            return -2;
        }

        if ( ( size = main_readbin( argv[2], bin ) ) > 0 ) {

            int ret = edit_bin( bin, size );
            if ( ret != 0 ) {
                printf( "Error map-size vs. file-size\n" );
                return ret;
            }
            make_bin( bin, size );
            save_bin( bin, size, "map.bin.readbin" );
            if ( bin != NULL ) free( bin );

        } else {
            if ( bin != NULL ) free( bin );
            return size;
        }
    } else  if (option == "fromimage") {

        if (argc<4) {
            printf( "%s fromimage <path_to_image.png> <path_to_map.bin>'\n", argv[0] );
            return -2;
        }

        if ( ( size = main_readbin( argv[3], bin ) ) > 0 ) {

//            int ret = edit_bin( bin, size );
//            if ( ret != 0 ) {
//                printf( "Error map-size vs. file-size\n" );
//                return ret;
//            }
            picture_bin( bin, size, argv[2] );
            save_bin( bin, size, "map.bin.picture" );
            if ( bin != NULL ) free( bin );

        } else {
            if ( bin != NULL ) free( bin );
            return size;
        }

    } else if ( option == "readtileset" ) {

        if (argc<3) {
            printf("%s readtileset <filename>'\n",argv[0]);
            printf("Example: %s readtileset %s>'\n", tilesetfile, argv[0] );

            return -1;
        }
//        sf::sleep(sf::seconds(2.0));
        if ( main_readtileset( argv[2], my_rendertexture, Tileset ) != 0) {
            printf( "Error reading tileset %s\n", argv[2] );
            return -1;
        }
        my_rendertexture.getTexture().copyToImage().saveToFile("out.png");
        my_window_update = 1;

    } else if ( option == "showbin" ) {


        if (argc<3) {
            printf( "%s showbin <path_to_map.bin>'\n", argv[0] );
            return -2;
        }

//        if ( ! file_exists("out.png") ) {

            printf( "Creating tileset with: %s readtileset %s\n", tilesetfile, argv[0]);
            if ( main_readtileset( tilesetfile, my_rendertexture, Tileset ) != 0) {
                printf( "Error reading tileset %s\n", tilesetfile );
                if ( bin != NULL ) free( bin );
                return -1;
            }
            my_rendertexture.getTexture().copyToImage().saveToFile( "out.png" );
//        } else {
//            sf::Texture t;
//            t.loadFromFile("out.png");
//            sf::Sprite s;
//            s.setTexture(t,true);
//            my_rendertexture.create(t.getSize().x,t.getSize().y);
//            my_rendertexture.clear(sf::Color::Transparent);
//            my_rendertexture.draw(s);
//            my_rendertexture.display();
//        }

//        sf::Texture tiles_texture;

//        launch_my_window();

        sf::sleep(sf::seconds(0.5));

        printf("Updating window\n");
        my_window_update = 1;
        sf::sleep(sf::seconds(1));

        printf("Updating window\n");
        my_window_update = 2;
        sf::sleep(sf::seconds(1));

        if ( ( size = main_readbin( argv[2], bin ) ) > 0 ) {

            int ret = edit_bin( bin, size );
            if ( ret != 0 ) {
                printf( "Error map-size vs. file-size\n" );
                return ret;
            }

//            make_bin( bin, size );

//            save_bin( bin, size );

        } else {
            if ( bin != NULL ) free( bin );
            return -1;
        }

//        tiles_texture.loadFromFile("out.png");

//knabbel
        main_window( bin, my_rendertexture, Tileset );

//        showbin( bin, my_rendertexture, Tileset);

        if ( bin != NULL ) free( bin );

//extern int my_window_running;

//        while ( my_window_running == 1 ) {
//            sf::sleep(sf::seconds(0.1));
//        };
        sf::sleep(sf::seconds(0.1));

//        my_backgroundSprite.setTexture(my_rendertexture.getTexture(),true);
//        extern int do_window2();
//        do_window2();
    } else {

        printf( "%s readbin <path_to_map.bin>'\n", argv[0] );
        printf( "%s readtileset <path_to_tileset.yaml>'\n", argv[0] );
        printf( "%s showbin <path_to_map.bin>'\n", argv[0] );

        return -3;

        //printf("\n");
    }
    return 0;
}

int main2()
{
    char map[1000][1000];
    FILE* names;
    if ((names = fopen("output33.txt","r"))!=NULL)
    {
        char line[65000];
        int xx,yy=0;
        int len=0;
        while (fgets(line,65000,names)!=NULL) {
            xx=0;
            printf("%s",line);
            len=strlen(line);
//        printf("len=%d\n",len);
            for (int n=0; n<len; n++) {

                if (line[n]==' ') {
                    map[xx][yy]=' ';
                } else if (line[n]=='x') {
                    map[xx][yy]='x';
                } else {
                    map[xx][yy]='O';
                }
                xx++;
            }
            yy++;
        }
        printf("\n");

        int y_len=yy;

        for (yy=0; yy<y_len; yy++) {
            printf("%3d ",yy);
            for (xx=0; xx<len; xx++) {
                printf("%c",map[xx][yy]);
            }
            printf("\n");
        }

        for (yy=0; yy<y_len; yy++) {
            for (xx=0; xx<len; xx++) {

                if ( ! (xx==0 || xx==len-1 || yy==0 || yy==y_len-1) ) {
                    if (map[xx][yy]=='O') {
                        int f=0;
                        for (int y1=-1; y1<=1; y1++) {
                            for (int x1=-1; x1<=1; x1++) {
                                if (map[xx+x1][yy+y1]==' ' || map[xx+x1][yy+y1]=='x') {
                                    f=1;
                                }
                            }
                        }
                        if (f==1) {
                            map[xx][yy]='O';
                        } else {
                            map[xx][yy]='.';
                        }
                    }
                } else {
                    map[xx][yy]='O';
                }
                printf("%c",map[xx][yy]);
            }
            printf("\n");
        }
        printf("\nx=%d y=%d\n",len,y_len);
        int actor=0;
        for (yy=0; yy<y_len; yy++) {
            for (xx=0; xx<len; xx++) {

                if (map[xx][yy]=='O') {
                    printf("	Actor%d: brik\n",actor);
                    printf("		Owner: Neutral\n");
                    printf("		Location: %d,%d\n",xx,yy);
                    actor++;
//                } else if (map[xx][yy]=='.' && (xx==1 || yy==1 || xx==len-2 || yy==y_len-2) ) {
//                    printf("	Actor%d: brik\n",actor);
//                    printf("	Actor%d: minv\n",actor);
//                    printf("		Owner: Neutral\n");
//                    printf("		Location: %d,%d\n",xx,yy);
//                    actor++;
                } else if (map[xx][yy]=='x') {
                    if ((xx%8)==5 && (yy%8)==5) {
                        printf("	Actor%d: gmine\n",actor);
                        printf("		Owner: Neutral\n");
                        printf("		Location: %d,%d\n",xx,yy);
                        actor++;
                    }
                }
            }
//            printf("\n");
        }

    }

    cout << "Hello world!" << endl;
    return 0;
}
