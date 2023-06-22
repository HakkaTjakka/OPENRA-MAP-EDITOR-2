#include <functions.hpp>
#include <math.h>

#include <SFML/System/Thread.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Sprite.hpp>

//sf::Image m_image;

bool border_check(sf::Image &m_image, int xx, int yy) {
    static int x_max = m_image.getSize().x;
    static int y_max = m_image.getSize().y;

    for (int y = yy - 5; y <= yy + 5; y++ ) {
        for (int x = xx - 5; x <= xx + 5; x++ ) {
            if ( y>=0 && y<y_max && x>=0 && x<x_max ) {
                if ( sqrt( (x-xx) * (x-xx) + (y-yy) * (y-yy) ) < 4.0) {
                    sf::Color pixel = m_image.getPixel( x, y );
//                    if (pixel.a != 0 ) return false;
                    int avg = (pixel.r + pixel.g + pixel.b ) / 3;
                    if (avg < 128 && pixel.a >= 128) return false;
                }
//                if ( pixel.a == 0) return false;
            }
        }
    }
    return true;
}

int picture_bin( unsigned char* bin, long size ) {
    uint16_t size_x = *(uint16_t*)(bin + 1);
    uint16_t size_y = *(uint16_t*)(bin + 3);

    sf::Image m_image;
    if ( m_image.loadFromFile("world15.png") ) {
        uint16_t* val1a_ptr;
        uint8_t*  val1b_ptr;
        uint16_t* val2_ptr;

        printf( "\nIMAGE: x=%4d y=%4d\n", m_image.getSize().x, m_image.getSize().y);
        printf( "\nMAP:   x=%4d y=%4d\n", size_x, size_y);
//        bool brk = false;

        unsigned int max_x, max_y;
        max_x = size_x;
        max_y = size_y;
        if (m_image.getSize().x < size_x) {
            max_x = m_image.getSize().x;
        }
        if (m_image.getSize().y < size_y) {
            max_y = m_image.getSize().y;
        }

        if (m_image.getSize().x != size_x) {
            printf("image x != map x\n");
//            brk = true;
        }
        if (m_image.getSize().y != size_y) {
            printf("image y != map y\n");
//            brk = true;
        }
//        if (brk) exit(1);

        unsigned int offset;

//        uint16_t* first_a_ptr =    (uint16_t*) ( bin + 17       );
//        uint8_t*  first_b_ptr =    (uint8_t*)  ( bin + 17 + 2   );
        int actor = 0;
        sf::Color pixel, pixel_left, pixel_up, pixel_left_up;
        for (unsigned int yy = 0; yy < max_y; yy++ ) {
            for (unsigned int xx = 0; xx < max_x; xx++ ) {

                offset = yy + size_y * xx;


//                offset = xx + size_x * yy;

//                val1a = (int16_t) ( *( (uint16_t*) ( bin + 17                            + offset*3  )  ) );
//                val1b = (int8_t)  ( *( (uint8_t*)  ( bin + 17 + 2                        + offset*3  )  ) );

                val1a_ptr =    (uint16_t*) ( bin + 17                            + offset*3  )  ;
                val1b_ptr =    (uint8_t*)  ( bin + 17 + 2                        + offset*3  )  ;
                val2_ptr  =    (uint16_t*) ( bin + 17 + ( size_x * size_y ) * 3  + offset*2  )  ;

//                val2  = (int16_t) ( *( (uint16_t*) ( bin + 17 + ( size_x * size_y ) * 3  + offset*3  )  ) );

                bool left;
                bool up;
                bool left_up;

                pixel = m_image.getPixel( xx, yy );
                if ( pixel.a != 0 ) {

                    if ( xx > 0) {
                        int avg_left = 0;
                        pixel_left = m_image.getPixel( xx-1, yy );
                        avg_left = (pixel_left.r + pixel_left.g + pixel_left.b ) ;
                        if ( avg_left > 128*3  || pixel_left.a==0 ) left = false; else left = true;
                    }

                    if ( yy > 0) {
                        int avg_up = 0;
                        pixel_up = m_image.getPixel( xx, yy-1 );
                        avg_up = (pixel_up.r + pixel_up.g + pixel_up.b ) ;
                        if ( avg_up > 128*3  || pixel_up.a==0 ) up = false; else up = true;
                    }

                    if ( yy > 0 && xx > 0) {
                        int avg_left_up = 0;
                        pixel_left_up = m_image.getPixel( xx-1, yy-1 );
                        avg_left_up = (pixel_left_up.r + pixel_left_up.g + pixel_left_up.b ) ;
                        if ( avg_left_up > 128*3  || pixel_left_up.a==0 ) left_up = false; else left_up = true;
                    }

                    int avg = (pixel.r + pixel.g + pixel.b ) ;
    //1                if (pixel.r == 255 && pixel.g < 200 && pixel.b < 200) {
    //1                    *val2_ptr =  0x0302;
                    if ( avg <= 128*3 ) {
                        *val1a_ptr = 340 + ( rand() % 12 );
                        while ( *val1a_ptr == 347 || *val1a_ptr == 348 ) *val1a_ptr = 340 + ( rand() % 12 );
    //                    *val1b_ptr = ( rand() % 12 );
                        *val1b_ptr = 0;


                    } else {

                        *val1a_ptr = 268;
                        *val1b_ptr = ( rand() % 12 );

                        if (left && up) *val1a_ptr = 271;
                        else if (left && !left_up) *val1a_ptr = 273;
                        else if (up && !left_up) *val1a_ptr = 274;
                        else if (!left && !up && left_up) *val1a_ptr = 272;
                        else if (left ) *val1a_ptr = 270;
                        else if (up ) *val1a_ptr = 269;


//                        if ( ! border_check(m_image, xx, yy) ) {
//                            *val2_ptr =  0x0c01;
//                        }
                    }
                } else {
                    if ( xx > 0) {
                        int avg_left = 0;
                        pixel_left = m_image.getPixel( xx-1, yy );
                        avg_left = (pixel_left.r + pixel_left.g + pixel_left.b ) ;
                        if ( avg_left > 128*3 || pixel_left.a==0 ) left = false; else left = true;
                    }

                    if ( yy > 0) {
                        int avg_up = 0;
                        pixel_up = m_image.getPixel( xx, yy-1 );
                        avg_up = (pixel_up.r + pixel_up.g + pixel_up.b ) ;
                        if ( avg_up > 128*3 || pixel_up.a==0  ) up = false; else up = true;
                    }

                    if ( yy > 0 && xx > 0) {
                        int avg_left_up = 0;
                        pixel_left_up = m_image.getPixel( xx-1, yy-1 );
                        avg_left_up = (pixel_left_up.r + pixel_left_up.g + pixel_left_up.b ) ;
                        if ( avg_left_up > 128*3 || pixel_left_up.a==0  ) left_up = false; else left_up = true;
                    }

                    *val1a_ptr = 268;
                    *val1b_ptr = ( rand() % 12 );

                    if (left && up) *val1a_ptr = 271;
                    else if (left && !left_up) *val1a_ptr = 273;
                    else if (up && !left_up) *val1a_ptr = 274;
                    else if (!left && !up && left_up) *val1a_ptr = 272;
                    else if (left ) *val1a_ptr = 270;
                    else if (up ) *val1a_ptr = 269;

                    if ( border_check(m_image, xx, yy) ) {
                        *val2_ptr =  0x0c01;
                    }
                }


//1                } else if ( avg < 128 ) {
//1                    *val2_ptr =  0x0c01;

////                if ( pixel.a == 0 ) {
//                if ( xx > (unsigned int)100 && xx < (unsigned int)(size_x-100) && yy > (unsigned int)100 && yy < (unsigned int)(size_y-100)  ) {
//                    *val1a_ptr = *first_a_ptr;
//                    *val1b_ptr = *first_b_ptr;

////                    *val1a_ptr =  0x0002;
////                    *val1b_ptr =  ( rand() % 4 );


//                *(uint16_t*)(bin + 17 + offset  * 3)     = 0x0001;
//                *(uint8_t*) (bin + 17 + offset  * 3 + 2) = 0x00;

//                    *val1b_ptr = (uint8_t) (  ( rand() % 12 )  );
//                    *val1a_ptr = (uint16_t) ( 340 + ( rand() % 7 ) );
//                    *val1b_ptr = (uint8_t) ( 0 );
//                } else {
//                    if ( border_check(m_image, xx, yy) ) *val2_ptr =  0x0c01;

////                    *val2_ptr =  0x0c01;

/*                    if ( ! (rand() % 300 ) ) {
                        if ( border_check(m_image, xx, yy) ) {
                            printf( "	Actor%d: mine\n", actor );
                            printf( "		Owner: Neutral\n" );
                            printf( "		Location: %d,%d\n", xx, yy);
                            actor++;
                        }
                    } else if ( ! (rand() % 1000 ) ) {
                        if ( border_check(m_image, xx, yy) ) {
                            printf( "	Actor%d: gmine\n", actor );
                            printf( "		Owner: Neutral\n" );
                            printf( "		Location: %d,%d\n", xx, yy);
                            actor++;
                        }

                    }

*/
//                                      02 03
//                                    01 0c

//                    *val1a_ptr = (uint16_t) ( 268 );
//                    *val1b_ptr = (uint8_t) (  ( rand() % 12 )  );
            }
//            printf("\n");
        }

    } else {
        printf("Error reading image\n");
        exit(1);
    }
    return 0;
}


long main_readbin(char* filename, unsigned char* &bin) {
    long size;


        if ( ( size = read_bin_file( std::string() + filename, bin ) ) >= 0 ) {
//                printf( "%ld bytes read.\n", size );
            return size;
        } else {
            printf("Error %ld reading file %s\n", size, filename);
            if ( bin != NULL ) free( bin );
            return (int)size;
        }

}

// File layout .bin file (inside the .oramap files, .zip file...)
// First 17 bytes: map info
//      Byte 1+2: 16 bit integer x-size map
//      Byte 3+4: 16 bit integer y-size map
//      Byte 13+14+15+16: 32 bit integer start address resources
//      Rest: Unknown for now
//
// From byte 17: 3 bytes * size_x * size_y
//      Every 3 bytes: One 16 bit (2 bytes) integer for the TILENUMBER,
//          and one 8 bit (1 byte) integer for the VARIATIONS of this tile.
//
//      List of filenames of tiles and their corresponding (16 bin integer) code used in .bin file comes from:
//      File: OpenRA/mods/ra/tilesets/interior.yaml
//      GitHub source: https://github.com/OpenRA/OpenRA/mods/ra/tilesets/interior.yaml
//      And looks like:
/*
    ...
	Template@350:                   // one tile tile
		Id: 350                     // 16 bit integer from .bin
		Images: wall0022.int
		Size: 1,1                   // dimensions (# of tiles)
		Categories: Wall
		Tiles:                      // tiles / layout
			0: Wall                 // numbering (dimentions) left-right top-down (depending on shape... see the build in editor)
	Template@351:                   // 4 tile tile (2 x 2)
		Id: 351
		Images: wall0023.int
		Size: 2,2
		Categories: Wall
		Tiles:
			0: Wall                 // upper left  (0,0)
			1: Wall                 // upper right (0,1)
			2: Wall                 // lower left  (1,0) (3 would be 1,1)
    ...
*/
//
//      Starting with the interior tiles: (there are also winter desert etc.)
//      Tiles: the .png tiles comes from some .mix file of the OpenRA (interior.mix) found in the gamedir,
//           and where extracted with a tool called XCC to .png
// From there (byte 17 + 3 bytes * size_x * size_y)
//      Total: 2 bytes * size_x * size_y
//      Every 2 bytes: One 16 bit integer for the tile status, like emerald or shroud / damage stuff or so.
//      Values need to be determined what is what. On map with no spice they are all 0x0000 (hex)


int edit_bin(unsigned char* bin, long filesize) {
//    printf( "bin=0x%p filesize=%ld\n", bin, filesize );
    for ( int n=0; n<17; n++ ) {
        if ( n>0 && n<17 ) printf(" ");
        printf("%02X", (unsigned char)bin[n]);
    }
    printf("\n");

//    uint16_t size_x = (uint16_t) ( *( (uint16_t*) ( bin + 1 ) ) );
//    uint16_t size_y = (uint16_t) ( *( (uint16_t*) ( bin + 3 ) ) );

    uint16_t size_x = *(uint16_t*)(bin + 1);
    uint16_t size_y = *(uint16_t*)(bin + 3);

    printf("size_x=%4d (%04X)\nsize_y=%4d (%04X)\n", size_x, size_x, size_y, size_y);

    int bytes=17 + 5 * size_x * size_y;

    if ( filesize != bytes ) {

        printf( "bin=( %d x %d ) x 5 + 17 = %d bytes. FAIL!!!\n", size_x, size_y, bytes );

        printf( "Filesize does not match map resolution.\n" );

        return -1;

    } else {

        printf( "bin=( %d x %d ) x 5 + 17 = %d bytes. OK!\n", size_x, size_y, bytes );

    }

//    sf::Image image;
//    image.create( size_x,  size_y, sf::Color( 255, 0, 0, 255 ) );
//    sf::Color color = sf::Color( 0, 0, 0, 0 );

//    unsigned char* val1a_ptr = ( bin + 17 );
//    unsigned char* val1b_ptr = ( bin + 17 + 2 );
//    unsigned char* val2_ptr  = ( bin + 17 + ( size_x * size_y ) * 3 );

    unsigned int offset;
    uint16_t val1a;
    uint8_t  val1b;
//    uint16_t val2;

    FILE* scan;
//    char str[4096];

    if ( ( scan = fopen( "scan.txt", "w" ) ) == NULL ) {

        printf( "Can't open %s for writing.\n", "scan.txt " );
        return -1;

    }
    for (int y=0; y < size_y; y++) {

        fprintf( scan, "%04d ", y );

        for (int x=0; x < size_x; x++) {
            offset = x + size_x * y;

            val1a = (uint16_t) ( *( (uint16_t*) ( bin + 17                            + offset*3  )  ) );
            val1b = (uint8_t)  ( *( (uint8_t*)  ( bin + 17 + 2                        + offset*3  )  ) );
//            val2  = (int16_t) ( *( (uint16_t*) ( bin + 17 + ( size_x * size_y ) * 3  + offset*3  )  ) );

            if ( val1a >= 351 && val1a <= 377 ) {
                fprintf( scan, "*%04X-%02X",  val1a, val1b );
            } else {
                fprintf( scan, "|%04X-%02X",  val1a, val1b );
            }

            if ( val1a == 275 ) {
                printf( "." );
            } else {
//                if ( val1a >= 351 && val1a <= 377 ) {
//                    printf("x=%4d y=%4d val1a=%04X=%5d val1b=%02X=%3d val2=%04X=%5d\n", x, y, val1a, val1a, val1b, val1b, val2, val2 );
//                    sprintf( str, "echo \"| x=%4d y=%4d | val1a=0x%04X (%5d) | val1b=0x%02X (%3d) | val2=0x%04X (%5d) |\" >> fail.txt\n", x, y, val1a, val1a, val1b, val1b, val2, val2 );
//                    int ret = system( str );
//                    if ( ret != 0 ) printf( "Error system( %s )\n" ,str);
//                    printf( "\033[1;31m*\033[0m" );
//                } else {
                    printf( "*" );
//                }
            }
        }
        printf( "\n" );
        fprintf( scan, "|\n" );
    }

    fclose( scan );
    return 0;
}

int insert_bin_range(unsigned char* bin, unsigned char* bin2, long filesize, long filesize2, int left, int top, int width, int height  ) {
    for ( int n=0; n<17; n++ ) {
        if ( n>0 && n<17 ) printf(" ");
        printf("%02X", (unsigned char)bin[n]);
    }
    printf("\n");

    uint16_t size_x = *(uint16_t*)(bin + 1);
    uint16_t size_y = *(uint16_t*)(bin + 3);

    printf("size_x=%4d (%04X)\nsize_y=%4d (%04X)\n", size_x, size_x, size_y, size_y);

    int bytes=17 + 5 * size_x * size_y;

    if ( filesize != bytes ) {

        printf( "bin=( %d x %d ) x 5 + 17 = %d bytes. FAIL!!!\n", size_x, size_y, bytes );

        printf( "Filesize does not match map resolution.\n" );

        return 0;

    } else {

        printf( "bin=( %d x %d ) x 5 + 17 = %d bytes. OK!\n", size_x, size_y, bytes );

    }

    for ( int n=0; n<17; n++ ) {
        if ( n>0 && n<17 ) printf(" ");
        printf("%02X", (unsigned char)bin2[n]);
    }
    printf("\n");

    uint16_t size_x2 = *(uint16_t*)(bin2 + 1);
    uint16_t size_y2 = *(uint16_t*)(bin2 + 3);

    printf("size_x=%4d (%04X)\nsize_y=%4d (%04X)\n", size_x2, size_x2, size_y2, size_y2);

    int bytes2=17 + 5 * size_x2 * size_y2;

    if ( filesize2 != bytes2 ) {

        printf( "bin=( %d x %d ) x 5 + 17 = %d bytes. FAIL!!!\n", size_x2, size_y2, bytes2 );

        printf( "Filesize does not match map resolution.\n" );

        return 0;

    } else {

        printf( "bin=( %d x %d ) x 5 + 17 = %d bytes. OK!\n", size_x2, size_y2, bytes2 );

    }

    int offset;
    int offset2;


    for (int yy = top; yy < top + height; yy++ ) {
        for ( int xx = left; xx < left + width; xx++ ) {

            offset  = yy + size_y * xx;
            offset2 = yy + size_y2 * xx;

            if ( xx < size_x && yy < size_y ) {
                memcpy( bin  + 17 + offset  * 3,
                        bin2 + 17 + offset2 * 3,
                        3
                );
                memcpy( bin  + 17 + offset  * 2 + ( size_x  * size_y  ) * 3,
                        bin2 + 17 + offset2 * 2 + ( size_x2 * size_y2 ) * 3,
                        2
                );
            }
        }
    }


    return 1;
}

int insert_bin(unsigned char* bin, unsigned char* bin2, long filesize, long filesize2 ) {
    for ( int n=0; n<17; n++ ) {
        if ( n>0 && n<17 ) printf(" ");
        printf("%02X", (unsigned char)bin[n]);
    }
    printf("\n");

    uint16_t size_x = *(uint16_t*)(bin + 1);
    uint16_t size_y = *(uint16_t*)(bin + 3);

    printf("size_x=%4d (%04X)\nsize_y=%4d (%04X)\n", size_x, size_x, size_y, size_y);

    int bytes=17 + 5 * size_x * size_y;

    if ( filesize != bytes ) {

        printf( "bin=( %d x %d ) x 5 + 17 = %d bytes. FAIL!!!\n", size_x, size_y, bytes );

        printf( "Filesize does not match map resolution.\n" );

        return 0;

    } else {

        printf( "bin=( %d x %d ) x 5 + 17 = %d bytes. OK!\n", size_x, size_y, bytes );

    }

    for ( int n=0; n<17; n++ ) {
        if ( n>0 && n<17 ) printf(" ");
        printf("%02X", (unsigned char)bin2[n]);
    }
    printf("\n");

    uint16_t size_x2 = *(uint16_t*)(bin2 + 1);
    uint16_t size_y2 = *(uint16_t*)(bin2 + 3);

    printf("size_x=%4d (%04X)\nsize_y=%4d (%04X)\n", size_x2, size_x2, size_y2, size_y2);

    int bytes2=17 + 5 * size_x2 * size_y2;

    if ( filesize2 != bytes2 ) {

        printf( "bin=( %d x %d ) x 5 + 17 = %d bytes. FAIL!!!\n", size_x2, size_y2, bytes2 );

        printf( "Filesize does not match map resolution.\n" );

        return 0;

    } else {

        printf( "bin=( %d x %d ) x 5 + 17 = %d bytes. OK!\n", size_x2, size_y2, bytes2 );

    }

    int offset;
    int offset2;

    for (int yy = 0; yy < size_y2; yy++ ) {
        for ( int xx = 0; xx < size_x2; xx++ ) {

            offset  = yy + size_y * xx;
            offset2 = yy + size_y2 * xx;

            if ( xx < size_x && yy < size_y ) {
                memcpy( bin  + 17 + offset  * 3,
                        bin2 + 17 + offset2 * 3,
                        3
                );
                memcpy( bin  + 17 + offset  * 2 + ( size_x  * size_y  ) * 3,
                        bin2 + 17 + offset2 * 2 + ( size_x2 * size_y2 ) * 3,
                        2
                );
            }
        }
    }


    return 1;
}

unsigned char* resize_bin(unsigned char* bin, long filesize, int new_x, int new_y ) {
//    printf( "bin=0x%p filesize=%ld\n", bin, filesize );
    for ( int n=0; n<17; n++ ) {
        if ( n>0 && n<17 ) printf(" ");
        printf("%02X", (unsigned char)bin[n]);
    }
    printf("\n");

    uint16_t size_x = *(uint16_t*)(bin + 1);
    uint16_t size_y = *(uint16_t*)(bin + 3);

    printf("size_x=%4d (%04X)\nsize_y=%4d (%04X)\n", size_x, size_x, size_y, size_y);

    int bytes=17 + 5 * size_x * size_y;
    int new_bytes=17 + 5 * new_x * new_y;

    if ( filesize != bytes ) {

        printf( "bin=( %d x %d ) x 5 + 17 = %d bytes. FAIL!!!\n", size_x, size_y, bytes );

        printf( "Filesize does not match map resolution.\n" );

        return NULL;

    } else {

        printf( "bin=( %d x %d ) x 5 + 17 = %d bytes. OK!\n", size_x, size_y, bytes );

    }


    long fz = new_bytes;
    unsigned char* mem=(unsigned char*)	calloc(fz,1);
    if (mem==NULL) {

        printf("ERROR ALLECATING MEMORY\n");
        return NULL;

    } else {

        printf( "0x%p, converting\n", mem );
        fflush( stdout );

    }
    memset( mem, 0, fz);
    memcpy( mem, bin, 17);

    *(uint16_t*)(mem + 1) = (uint16_t)new_x;
    *(uint16_t*)(mem + 3) = (uint16_t)new_y;
    *(uint32_t*)(mem + 13) = (uint32_t)( 17 + ( new_x  * new_y  ) * 3 ); //damn!!! Took me hours to find out!

    if ( fz != new_bytes ) {

        printf( "bin=( %d x %d ) x 5 + 17 = %d bytes. FAIL!!!\n", new_x, new_y, new_bytes );

        printf( "Filesize does not match map resolution.\n" );

        return NULL;

    } else {

        printf( "bin=( %d x %d ) x 5 + 17 = %d bytes. OK!\n", new_x, new_y, new_bytes );

    }

    printf(" new_x=%4d (%04X)\n new_y=%4d (%04X)\n", new_x, new_x, new_y, new_y);

    int offset;
    int offset2;
    for (int yy = 0; yy < new_y; yy++ ) {
        for ( int xx = 0; xx < new_x; xx++ ) {

            offset  = yy + size_y * xx;
            offset2 = yy + new_y * xx;
            if ( xx < size_x && yy < size_y ) {
                memcpy( mem + 17 + offset2 * 3,
                        bin + 17 + offset  * 3,
                        3
                );
                memcpy( mem + 17 + offset2 * 2 + ( new_x  * new_y  ) * 3,
                        bin + 17 + offset  * 2 + ( size_x * size_y ) * 3,
                        2
                );
            } else {
                memcpy( mem + 17 + offset2 * 3,
                        bin + 17,
                        3
                );
                memset( mem + 17 + offset2 * 2 + ( new_x  * new_y  ) * 3,
                        0,
                        2
                );
            }
            uint16_t tile_main = *(uint16_t*)(mem + 17 + offset2  * 3);
            if ( tile_main == 0x0002) {
                *(uint16_t*)(mem + 17 + offset  * 3)     = 0x0001;
                *(uint8_t*) (mem + 17 + offset  * 3 + 2) = 0x00;
            }
        }
    }

/*
    int to_x = size_x, to_y = size_y;
    if ( new_x < size_x) to_x = new_x;
    if ( new_y < size_y) to_y = new_y;
    memcpy( mem, bin, 17);

    *(uint16_t*)(mem + 1) = new_x;
    *(uint16_t*)(mem + 3) = new_y;

    for (int y=0; y < to_y; y++) {
        memcpy(
            mem + 17 + ( new_x * y * 3),
            bin + 17 + ( size_x * y * 3),
            (size_t) (to_x * 3)
        );
    }
    for (int y=0; y < to_y; y++) {
        memset( mem + 17 + ( new_x * y * 2) + (  new_x *  new_y ) * 3, 0, to_x * 2 );
//        memcpy(
//            mem + 17 + ( new_x * y * 2) + (  new_x *  new_y ) * 3,
//            bin + 17 + ( size_x * y * 2) + (  size_x *  size_y ) * 3,
//            (size_t) (to_x * 2)
//        );
//        printf( "\n" );
    }
*/
    return mem;
}


long read_bin_file( string filename, unsigned char* &mem) {

//    printf( "filename: %s\n", filename.c_str() );
//    char* mem=bin;

    if ( file_exists( filename.c_str() ) ) {

        std::filesystem::path p{ filename.c_str() };
        long fz = std::filesystem::file_size(p);

        printf(
            "Filesize of \"%s\" is: %lu bytes. malloc()="
            ,filename.c_str()
            ,fz
        );
        fflush(stdout);

        if (FILE* file = fopen(filename.c_str(), "r")) {

            long n;

            mem=(unsigned char*)	calloc(fz,1);

            if (mem==NULL) {

                printf("ERROR ALLECATING MEMORY\n");
                return -10;

            } else {

                printf( "0x%p, reading, ", mem );
                fflush( stdout );

            }

            if ( ( n = (long) fread( mem, 1, fz, file ) ) != fz ) {

                printf( "FATAL ERROR: unable to read file \"%s\".\n", filename.c_str() );
                free(mem);
                mem=NULL;
                fclose( file );
                return -11;

            } else {

                printf( "%lu bytes read, ", n );
                fflush( stdout );
                fclose( file );
                printf( "ok.\n" );
                return n;

            }

        } else {

            printf( "ERROR OPENING FILE: %s\n", filename.c_str() );
            return -12;

        }

    } else {

        printf( "File \"%s\" does not exist.\n", filename.c_str() );
        return -13;

    }
}

int make_bin( unsigned char* bin, long size ) {

    char map[1000][1000];
    FILE* names;

    int16_t size_x = *(int16_t*) (bin + 1);
//    int16_t size_y = *(int16_t*) (bin + 3);

    // wall 340-346
    if ( ( names = fopen( "output33.txt", "r" ) ) != NULL )
    {
        char line[65000];
        int xx, yy = 0;
        int len = 0;

        while ( fgets( line, 65000, names ) != NULL ) {

            xx = 0;
            printf( "%s", line );
            len = strlen( line );
//        printf("len=%d\n",len);
            for ( int n = 0; n < len; n++ ) {

                if ( line[n] == ' ' ) {
                    map[xx][yy] = ' ';
                } else if ( line[n] == 'x' ) {
                    map[xx][yy] = 'x';
                } else {
                    map[xx][yy] = 'O';
                }
                xx++;
            }
            yy++;
        }
        printf( "\n" );

        int y_len = yy;

        for ( yy = 0; yy < y_len; yy++) {
            printf( "%3d ", yy);
            for ( xx = 0; xx < len; xx++ ) {
                printf( "%c", map[xx][yy] );
            }
            printf( "\n" );
        }

/*
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
*/
        uint16_t* val1a_ptr;
        uint8_t*  val1b_ptr;
//        uint16_t* val2_ptr;

        printf( "\nx=%d y=%d\n", len,y_len );
        int actor = 0;

        unsigned int offset;

        for (yy = 0; yy < y_len; yy++ ) {
            for ( xx = 0; xx < len; xx++ ) {

                offset = xx + size_x * yy;

//                val1a = (int16_t) ( *( (uint16_t*) ( bin + 17                            + offset*3  )  ) );
//                val1b = (int8_t)  ( *( (uint8_t*)  ( bin + 17 + 2                        + offset*3  )  ) );

                val1a_ptr =    (uint16_t*) ( bin + 17                            + offset*3  )  ;
                val1b_ptr =    (uint8_t*)  ( bin + 17 + 2                        + offset*3  )  ;
//                val2  = (int16_t) ( *( (uint16_t*) ( bin + 17 + ( size_x * size_y ) * 3  + offset*3  )  ) );


                if (map[xx][yy]=='O') {
    // wall 340-346 0
                    *val1a_ptr = (uint16_t) ( 340 + ( rand() % 7 ) );
                    *val1b_ptr = (uint8_t) ( 0 );
//                    printf("	Actor%d: brik\n",actor);
//                    printf("		Owner: Neutral\n");
//                    printf("		Location: %d,%d\n",xx,yy);
//                    actor++;
//                } else if (map[xx][yy]=='.' && (xx==1 || yy==1 || xx==len-2 || yy==y_len-2) ) {
//                    printf("	Actor%d: brik\n",actor);
//                    printf("	Actor%d: minv\n",actor);
//                    printf("		Owner: Neutral\n");
//                    printf("		Location: %d,%d\n",xx,yy);
//                    actor++;
                } else if (map[xx][yy]==' ') {

    // floor 268 0-11
                    *val1a_ptr = (uint16_t) ( 268 );
                    *val1b_ptr = (uint8_t) (  ( rand() % 12 )  );

                } else if (map[xx][yy]=='x') {

                    *val1a_ptr = (uint16_t) ( 268 );
                    *val1b_ptr = (uint8_t) (  ( rand() % 12 )  );

                    if ( (xx % 8 ) == 5 && ( yy % 8 ) == 5 ) {
                        printf( "	Actor%d: gmine\n", actor );
                        printf( "		Owner: Neutral\n" );
                        printf( "		Location: %d,%d\n", xx, yy);
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


int save_bin(unsigned char* bin, long filesize, std::string filename) {
    FILE* f_ptr;

    if ( ( f_ptr = fopen( filename.c_str(), "w" ) ) == NULL ) {

        printf( "Error opening %s for writing.\n", filename.c_str() );
        return -1;

    } else {

        size_t bytes = fwrite( bin, 1, filesize, f_ptr );

        printf( "File %s %lu bytes written\n", filename.c_str(), bytes );
        fclose( f_ptr );
        return 0;

    }
}

//	Template@362:
//		Id: 362
//		Images: wall0034.int
//		Size: 2,2
//		Categories: Wall
//		Tiles:
//			0: Wall
//			2: Wall
//			3: Wall

int main_readtileset( char* filename, sf::RenderTexture &my_rendertexture, std::map<int, int> &Tileset ) {

//    std::vector< struct Template > Templates;
//    std::vector< struct Template >::iterator it_Templates;

    struct Template Template;
//    struct Tileset Tile;
    std::map<int, struct Template> Templates;
    std::map<int, struct Template>::iterator it_Templates;
    std::map<int, std::string>::iterator it_Tiles;
    std::map<int, int>::iterator it_Tileset;
//    std::vector<struct Tileset> Tileset;

    if ( !file_exists( filename ) ) {

        printf( "File does not exist: %s\n", filename );
        return -1;

    }

    printf( "Filename: %s\n", filename );

    FILE* f;

    if ( ( f = fopen( filename, "r" ) ) == NULL ) {

        printf( "Can not open file: %s\n", filename );
        return -1;

    } else {

        char line[65000];
        int n = 1;
        char a[5][50];
        bool tiles = false;

        int nnn=0;
        while ( fgets( line, 65000, f ) != NULL ) {
//            printf( "%d: ", 0);
//            printf( "%s", line );
//            n++;

            int num=sscanf( line, "%50[\t]%50[^ ] %50[^ ] %50[^ ] %50[^ ] %50[^ ]", a[0], a[1], a[2], a[3], a[4], a[5] );

            printf( "%3d: %s", n, a[0] );

            a[ num - 1 ][ strcspn( a[ num-1 ], "\n" ) ]  = 0;

            for ( int i = 1; i < num ; i++) {

                sprintf( line, "\033[1;%lum%s\033[0m" , 31 + (strlen( a[0] ) + i*2 +1) % 6 , a[i] );
                printf( "%s ", line);
//                printf( "%s ", a[i]);
            }
            printf("\n");

            if ( strcmp( a[1], "Id:" ) == 0 ) {

                if ( tiles == true ) {
                    tiles = false;
                    Templates.insert( std::make_pair( Template.Id, Template ) );
                    Template.Tiles.clear();
                }

                tiles = false;
//                printf( "Id: %s\n", a[2] );
                Template.Id = atoi(a[2]);

            } else if ( strcmp( a[1], "Images:" ) == 0 ) {

                tiles = false;
                Template.Images = std::string() + a[2];
//                printf( "Images: %s\n", a[2] );

            } else if ( strcmp( a[1], "Size:") == 0) {

                tiles = false;
                sscanf( a[2], "%d,%d", &(Template.Size.x), &(Template.Size.y ) );
//                printf( "Size: %d,%d\n", Template.Size.x, Template.Size.y );

            } else if ( strcmp( a[1], "Tiles:") == 0) {

                tiles = true;
                nnn=0;
            } else if ( tiles == true) {

//                printf( "%s %s\n", a[1], a[2] );
                Template.Tiles.insert( std::make_pair( atoi( a[1] ), a[2] ) );
                nnn++;

            }
            n++;
        }
        if ( tiles == true ) {
            tiles = false;
            Templates.insert( std::make_pair( Template.Id, Template ) );
        }

        fclose( f );
        printf( "\n" );
        printf( "SIZE=%lu\n", Templates.size() );

        int error = 0;
        int loaded = 0;

        for (it_Templates = Templates.begin(); it_Templates != Templates.end(); it_Templates++) {

            Template = it_Templates->second;

            printf( "Id: %5d     Size: %d,%d     Images: %15s ",
                Template.Id,
                Template.Size.x,
                Template.Size.y,
                Template.Images.c_str()
            );
            for (it_Tiles = Template.Tiles.begin(); it_Tiles != Template.Tiles.end(); it_Tiles++) {
                if (  it_Tiles != Template.Tiles.begin() ) {
                    printf( "                                                    " );
                }
                printf( "%2d: %s\n", it_Tiles->first, it_Tiles->second.c_str()  );
            }
            std::string path=std::string() + "tiles/" + Template.Images;

            if ( path.find_last_of( "." ) != std::string::npos ) {
                path = path.substr( 0, path. find_last_of( "." ) ) + ".png";
            }
//            path = "tiles/" + Template.Images;
            if ( file_exists( path.c_str() ) ) {
                printf( "Loading image Id: %d  %s ", Template.Id, path.c_str() );
                if ( Template.Image.loadFromFile( path.c_str() ) ) {
                    int x,y;
                    x = Template.Image.getSize().x / 24;
                    y = Template.Image.getSize().y / 24;
                    Template.Realsize.x = x;
                    Template.Realsize.y = y;
                    printf( "Ok. Size: %d,%d / %d,%d  Resolution: %d x %d\n",
                        Template.Size.x,
                        Template.Size.y,
                        Template.Realsize.x,
                        Template.Realsize.y,
                        Template.Image.getSize().x,
                        Template.Image.getSize().y
                    );
                    loaded++;

                } else {
                    printf( "Error.\n" );
                    error++;
                }
                it_Templates->second = Template;
            } else {
                printf( "File does noet exist: %s\n", path.c_str() );
                error++;
            }
//            printf( "\n" );
//    std::map< int, std::string>::iterator it_tiles;

        }

//        sf::RenderTexture my_rendertexture;

        my_rendertexture.create( 32 * 24, 16 * 24 );
//my_rendertexture.create()
        my_rendertexture.clear(sf::Color(255,0,0,0));
        sf::Sprite my_sprite;
        sf::Texture my_texture;
        int tile_num = 0;

        sf::RenderTexture my_tiny_texture;
        sf::Sprite my_tiny_sprite;
        sf::Sprite my_tiny_sprite2;

        my_tiny_texture.create(24,24);
        my_tiny_texture.clear(sf::Color::Transparent);
        my_tiny_texture.display();
        my_tiny_sprite2.setTexture( my_tiny_texture.getTexture(), true );
//        my_tiny_sprite2.setScale(1.0,-1.0);

//        my_tiny_sprite2.setScale( 1.0,-1.0 );
        for (it_Templates = Templates.begin(); it_Templates != Templates.end(); it_Templates++) {

            Template = it_Templates->second;
//            Template.Image.flipVertically();
            my_texture.loadFromImage( Template.Image );
            my_sprite.setTexture( my_texture );
            my_tiny_sprite.setTexture( my_texture );


            printf( "Id: %5d\tSize: %d,%d / %d,%d \tResolution: %d x %d\n",
                Template.Id,
                Template.Size.x,
                Template.Size.y,
                Template.Realsize.x,
                Template.Realsize.y,
                Template.Image.getSize().x,
                Template.Image.getSize().y
            );


            for (it_Tiles = Template.Tiles.begin(); it_Tiles != Template.Tiles.end(); it_Tiles++) {

                int X_START = ( it_Tiles->first % Template.Realsize.x ) * 24;
                int Y_START = (int)( it_Tiles->first / Template.Realsize.x ) * 24;

//                my_sprite.setTextureRect( { X_START, Y_START, 24, 24 } );
                my_tiny_sprite.setTextureRect( { X_START, Y_START, 24, 24 } );
                my_tiny_texture.draw(my_tiny_sprite);
                my_tiny_texture.display();

//                char str[100];
//                Template.Id * 256 + it_Tiles->first
//                sprintf( str, "test/0x%06x.png", Template.Id * 256 + it_Tiles->first );

//              int tile = (int)val1a * 256 + (int)val1b;

//                mkdir( "test", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH );
//                my_tiny_texture.getTexture().copyToImage().saveToFile(str);

                int x_pos = tile_num % 32;
                int y_pos = tile_num / 32;

                my_tiny_sprite2.setPosition( x_pos * 24, y_pos * 24 );
                my_rendertexture.draw(my_tiny_sprite2);

//                my_sprite.setPosition( x_pos * 24, y_pos * 24 );
//                my_rendertexture.draw(my_sprite);

//                Tile.Lookup = Template.Id * 256 + it_Tiles->first;

                Tileset.insert( std::make_pair( Template.Id * 256 + it_Tiles->first, tile_num ) );

//                Tileset.push_back(Tile);
//                if (  it_Tiles == Template.Tiles.begin() ) {
                    printf( "                                                    " );
//                }
                printf( "%2d: pos(%3d,%3d) -> (%3d,%3d)  %s\n",
                    it_Tiles->first, X_START, Y_START,
                    x_pos * 24, y_pos * 24,
                    it_Tiles->second.c_str()  );

                tile_num++;
            }

        }
        my_rendertexture.display();
        printf( "%d errors loading %d images.\n", error, loaded );

        printf("Templates.size()=%lu\n",Templates.size());
        printf("Tileset.size()=%lu\n",Tileset.size());

        for (it_Tileset = Tileset.begin(); it_Tileset != Tileset.end(); it_Tileset++) {
            printf( "Tile #%d = 0x%06X\n", it_Tileset->second, it_Tileset->first );
        }

/*
        int n1=Templates.size();
        it_Templates=Templates.begin();
        for ( int n2=0; n2 < n1; n2++ ) {

            struct Template* Template_ptr;
            Template_ptr = &(it_Templates->second);
            printf("n2=%d Id=%d\n",n2,Template_ptr->Id);
            it_Templates++;

        }
*/


//        for ( long unsigned int i=0; i < Templates.size(); i++ ) {
//            Template = Templates[i];
//            printf("i=%lu Id=%d\n",i,Templates[i].Id);
//        }

    }
    return 0;
}

//extern sf::RenderTexture backgroundRenderTexture;
extern sf::RenderTexture* backgroundRenderTexturePointer;
extern sf::RenderTexture* backgroundTexturePointer;
extern sf::Mutex window_mutex;


//int showbin(unsigned char* bin, sf::RenderTexture &tiles_texture,  std::map<int, int> &Tileset) {
int showbin(unsigned char* bin, sf::RenderTexture &tiles_texture, std::map<int, int> &Tileset, struct Drawing* drawing ) {
    printf("Tileset.size()=%lu\n",Tileset.size());
    std::map<int, int>::iterator it_Tileset;

    uint16_t size_x = *(uint16_t*)(bin + 1);
    uint16_t size_y = *(uint16_t*)(bin + 3);

    my_window_update = 2;
    while ( ! my_window_update == 0 ) sf::sleep(sf::seconds(0.001));
    sf::sleep(sf::seconds(1.0));

    sf::Texture* tiles = &(drawing->tiles);
    sf::Sprite* sprite = &(drawing->sprite);
    sf::Sprite s;

    my_window_update = 10;
    while ( ! my_window_update == 0 ) sf::sleep(sf::seconds(0.001));
    sf::sleep(sf::seconds(0.5));


    my_window_update = 11;
    while ( ! my_window_update == 0 ) sf::sleep(sf::seconds(0.001));
    sf::sleep(sf::seconds(0.5));

    sprite->setTexture(*tiles);

    uint16_t val1a;
    uint8_t  val1b;

    FILE* scan;
    if ( ( scan = fopen( "scan2.txt", "w" ) ) == NULL ) {

        printf( "Can't open %s for writing.\n", "scan.txt " );
        return -1;

    }

//    uint16_t size_x = *(uint16_t*)(bin + 1);
//    uint16_t size_y = *(uint16_t*)(bin + 3);

    for (int y=0; y < size_y; y++) {

        fprintf( scan, "%04d ", y );

        for (int x=0; x < size_x; x++) {
            unsigned int offset = x + size_x * y;

            val1a = (uint16_t) ( *( (uint16_t*) ( bin + 17                            + offset*3  )  ) );
            val1b = (uint8_t)  ( *( (uint8_t*)  ( bin + 17 + 2                        + offset*3  )  ) );
//            uint16_t val1a = (uint16_t) ( *( (uint16_t*) ( bin + 17                            + offset*3  )  ) );
//            uint8_t val1b  = (uint8_t)  ( *( (uint8_t*)  ( bin + 17 + 2                        + offset*3  )  ) );
            int tile = (int)val1a * 256 + (int)val1b;

            bool found=false;

            it_Tileset = Tileset.find(tile);
            if ( it_Tileset != Tileset.end()) {
                found = true;
                if ( val1a >= 351 && val1a <= 377 ) {
                    fprintf( scan, "*%04X-%02X",  val1a, val1b );
                } else {
                    fprintf( scan, "|%04X-%02X",  val1a, val1b );
                }
            } else {
                if ( val1a >= 351 && val1a <= 377 ) {
                    fprintf( scan, "*%04X+%02X",  val1a, val1b );
                } else {
                    fprintf( scan, "|%04X+%02X",  val1a, val1b );
                }
            }
//            it_Tileset = Tileset.find(tile);

            if (drawing->kill) return (0);

            if ( found ) {
//            if ( it_Tileset != Tileset.end()) {
                int X_START = ( it_Tileset->second % 32 ) * 24;
                int Y_START = int( it_Tileset->second / 32 ) * 24;

                while ( ! my_window_update == 0 ) {
                    sf::sleep(sf::seconds(0.0001));
//                    cnt++;
//                    cnt2++;
//                    if (cnt2 == 100) {
//                        printf("n=%d, avg=%7f Wait %d\r",avg_cnt, avg, cnt);
//                        cnt2=0;
//                    }
//                    sf::sleep(sf::seconds(0.0000001));
                }

                sprite->setTextureRect( { X_START, Y_START, 24, 24 } );
                sprite->setPosition( x * 24, y * 24 );
                my_window_update = 13;
//                tot+=cnt;
//                avg=tot / avg_cnt;
//                avg_cnt++;
//                printf("n=%d, avg=%7f Wait %d\n",avg_cnt, avg, cnt);
//                printf("             \r");
//                while ( ! my_window_update == 0 ) sf::sleep(sf::seconds(0.000001));

//                printf("x=%3d y=%3d  Index = %6d  Tile=0x%06X  Id=%3d  pos(%3d,%3d)\n ",
//                    x, y, it_Tileset->second, it_Tileset->first,
//                    (int)val1a,
//                    X_START, Y_START );
            } else {
                printf("x=%3d y=%3d  TILE NOT FOUND  Tile=0x%06X\n ", x, y,  tile );
            }
//cout << distance(mymap.begin(),mymap.find("198765432"));
//            val2  = (int16_t) ( *( (uint16_t*) ( bin + 17 + ( size_x * size_y ) * 3  + offset*3  )  ) );
        }
        fprintf( scan, "|\n" );
    }
    fclose( scan );

    while ( ! my_window_update == 0 ) sf::sleep(sf::seconds(0.00001));
    my_window_update = 14;

//    backgroundRenderTexturePointer->display();
    printf("draw\n");

//    rendertexture->display();
//    rendertexture->getTexture().copyToImage().saveToFile("pointer4a.png");
//    rendertexture->setActive(false);

//    backgroundRenderTexturePointer->display();
//    backgroundRenderTexturePointer->setActive(false);

//    my_window_update = 3;
//    while ( ! my_window_update == 0 ) sf::sleep(sf::seconds(0.001));
//    backgroundRenderTexturePointer->getTexture().copyToImage().saveToFile("pointer4a.png");
//    backgroundTexturePointer->getTexture().copyToImage().saveToFile("pointer4b.png");

    int p=0x016503;
    it_Tileset = Tileset.find(p);
    if ( it_Tileset != Tileset.end()) {
        printf("Index = %6d  Tile=0x%06X\n", it_Tileset->second, it_Tileset->first );
    } else {
        printf("TILE NOT FOUND  Tile=0x%06X\n", p );
    }

    return 0;
}

bool file_exists( const char * filename ) {

    struct stat stat_buffer;

    if ( stat( filename, &stat_buffer ) == 0 ) return true;
    return false;
}
