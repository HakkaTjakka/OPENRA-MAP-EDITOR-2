#include <SFML/Graphics.hpp>
#include <functions.hpp>

#if defined (SFML_SYSTEM_WINDOWS)
    #include <windows.h>

    bool setShape(HWND hWnd, const sf::Image& image)
    {
        const sf::Uint8* pixelData = image.getPixelsPtr();
        HRGN hRegion = CreateRectRgn(0, 0, image.getSize().x, image.getSize().y);

        // Determine the visible region
        for (unsigned int y = 0; y < image.getSize().y; y++)
        {
            for (unsigned int x = 0; x < image.getSize().x; x++)
            {
                if (pixelData[y * image.getSize().x * 4 + x * 4 + 3] == 0)
                {
                    HRGN hRegionDest = CreateRectRgn(0, 0, 1, 1);
                    HRGN hRegionPixel = CreateRectRgn(x, y, x+1, y+1);
                    CombineRgn(hRegionDest, hRegion, hRegionPixel, RGN_XOR);
                    DeleteObject(hRegion);
                    DeleteObject(hRegionPixel);
                    hRegion = hRegionDest;
                }
            }
        }

        SetWindowRgn(hWnd, hRegion, true);
        DeleteObject(hRegion);
        return true;
    }

    bool setTransparency(HWND hWnd, unsigned char alpha)
    {
        SetWindowLong(hWnd, GWL_EXSTYLE, GetWindowLong(hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
        SetLayeredWindowAttributes(hWnd, 0, alpha, LWA_ALPHA);
        return true;
    }

#elif defined (SFML_SYSTEM_LINUX)
    #include <X11/Xatom.h>
    #include <X11/extensions/shape.h>
    bool setShape(Window wnd, const sf::Image& image)
    {
        const sf::Uint8* pixelData = image.getPixelsPtr();
        Display* display = XOpenDisplay(NULL);

        // Try to set the window shape
        int event_base;
        int error_base;
        if (XShapeQueryExtension(display, &event_base, &error_base))
        {
            Pixmap pixmap = XCreatePixmap(display, wnd, image.getSize().x, image.getSize().y, 1);
            GC gc = XCreateGC(display, pixmap, 0, NULL);

            XSetForeground(display, gc, 1);
            XFillRectangle(display, pixmap, gc, 0, 0, image.getSize().x, image.getSize().y);
            XSetForeground(display, gc, 0);

            for (unsigned int y = 0; y < image.getSize().y; y++)
            {
                for (unsigned int x = 0; x < image.getSize().x; x++)
                {
                    if (pixelData[y * image.getSize().x * 4 + x * 4 + 3] == 0)
                        XFillRectangle(display, pixmap, gc, x, y, 1, 1);
                }
            }

            XShapeCombineMask(display, wnd, ShapeBounding, 0, 0, pixmap, ShapeSet);
            XFreePixmap(display, pixmap);
            XFreeGC(display, gc);
            XFlush(display);
            return true;
        }
        return true;
    }

    bool setTransparency(Window wnd, unsigned char alpha)
    {
        Display* display = XOpenDisplay(NULL);
        unsigned long opacity = (0xffffffff / 0xff) * alpha;
        Atom property = XInternAtom(display, "_NET_WM_WINDOW_OPACITY", false);
        if (property != None)
        {
            XChangeProperty(display, wnd, property, XA_CARDINAL, 32, PropModeReplace, (unsigned char*)&opacity, 1);
            XFlush(display);
            return true;
        }
        else
            return false;
    }

    #undef None // None conflicts with SFML
#elif defined (SFML_SYSTEM_MACOS)
    bool setShape(sf::WindowHandle handle, const sf::Image& image);
    bool setTransparency(sf::WindowHandle handle, unsigned char alpha);
#else
    bool setShape(sf::WindowHandle handle, const sf::Image& image)
    {
        return false;
    }

    bool setTransparency(sf::WindowHandle handle, unsigned char alpha)
    {
        return false;
    }
#endif

int my_window_stop=0;
int my_window_running=0;
int my_window_update=0;

void my_window_thread() {
    printf("my_window_thread starting\n");
    do_window( );
    printf("my_window_thread stopped\n");
    my_window_stop=0;
    my_window_running=0;
    my_window_update=0;
}

sf::Thread my_window(&my_window_thread);

void launch_my_window() {


    if (my_window_running==1) {
        printf("my_window_thread  already running\n");
        return;
    }
    my_window_running=1;
    my_window.launch();

}

void terminate_my_window() {
//    my_window.terminate();
}


//extern sf::Sprite my_backgroundSprite;
//extern sf::Texture my_backgroundTexture;
//extern sf::RenderTexture backgroundRenderTexture;

sf::Mutex window_mutex;

sf::RenderTexture* backgroundRenderTexturePointer;
sf::RenderTexture* backgroundTexturePointer;

int do_window()
{
    sf::RenderWindow window;
//    sf::ContextSettings contextSettings(0,0,2,3,0);

    window.create(sf::VideoMode(1920,1080, 32), "Transparent Window", sf::Style::None );
//    window.create(sf::VideoMode(1920,1080, 32), "Transparent Window", sf::Style::None ,contextSettings);
    window.setPosition(sf::Vector2i(0,0));
    window.setVerticalSyncEnabled(true);
//    window.setActive(false);

    window.setActive(false);

    sf::RenderTexture backgroundRenderTexture;
    backgroundRenderTexturePointer = &backgroundRenderTexture;

//    const unsigned char opacity = 255;

    sf::Image backgroundImage;

    backgroundImage.loadFromFile("image.png");


//    setShape(window.getSystemHandle(), backgroundImage);
//    setTransparency(window.getSystemHandle(), opacity);

    sf::Texture t;
    sf::Sprite s;

    sf::RenderTexture backgroundTexture;
    backgroundTexturePointer = &backgroundTexture;

    sf::Sprite backgroundSprite;

    t.loadFromImage(backgroundImage);
    s.setTexture(t,true);
    backgroundTexture.create(t.getSize().x,t.getSize().y);
    backgroundTexture.clear(sf::Color::Transparent);
    backgroundTexture.draw(s);
    backgroundTexture.display();

    backgroundSprite.setTexture(backgroundTexture.getTexture(),true);

//    backgroundTexture.loadFromImage(backgroundImage);
//    backgroundSprite.setTexture(backgroundTexture,true);


//    backgroundSprite.setPosition( ( 1920 - backgroundTexture.getSize().x ) / 2, ( 1080 - backgroundTexture.getSize().y) / 2 );

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed || (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)) {
                window.close();
            }
        }
        if (my_window_update >= 1) {
            if (my_window_update == 1) {
                backgroundImage.loadFromFile("out.png");
//                setShape(window.getSystemHandle(), backgroundImage);

//                backgroundTexture.create(backgroundImage.getSize().x,backgroundImage.getSize().y );
//                backgroundTexture.loadFromImage(backgroundImage);
//                backgroundSprite.setTexture(backgroundTexture,true);

                t.loadFromImage(backgroundImage);
                s.setTexture(t,true);

                backgroundTexture.create(t.getSize().x,t.getSize().y);
                backgroundTexture.clear(sf::Color::Blue);
                backgroundTexture.draw(s);
                backgroundTexture.display();
                backgroundSprite.setTexture(backgroundTexture.getTexture(),true);

//                backgroundSprite.setPosition( ( 1920 - backgroundTexture.getSize().x ) / 2, ( 1080 - backgroundTexture.getSize().y) / 2 );

            } else if (my_window_update == 2) { //full

                backgroundImage.create(1920,1080,sf::Color(255,255,255,255));
//                setShape(window.getSystemHandle(), backgroundImage);

                t.create(backgroundTexture.getTexture().getSize().x,backgroundTexture.getTexture().getSize().y);
                t.update(backgroundTexture.getTexture());

                backgroundTexture.create(1920,1080);
                backgroundTexture.clear(sf::Color::Cyan);

                s.setTexture(t,true);
                backgroundTexture.draw(s);
                backgroundTexture.display();
                backgroundSprite.setTexture(backgroundTexture.getTexture(),true);
/*

                backgroundRenderTexture.create(1920,1080);
                backgroundRenderTexture.clear(sf::Color::Blue);
                backgroundRenderTexture.draw(backgroundSprite);
                backgroundRenderTexture.display();
                backgroundSprite.setTexture(backgroundRenderTexture.getTexture(),true);
*/

            } else if (my_window_update == 3) { //replace - update ?
//                static int cnt=0;
//                cnt++;
//                char fname[20];
//                sprintf(fname,"file%2d.png",cnt);
                printf(".");

                backgroundRenderTexture.setActive(true);
//                backgroundRenderTexture.display();

                s.setTexture(backgroundRenderTexture.getTexture(), true);
                s.setTextureRect({0,0,1920,1080});
                s.setPosition(0,0);
                backgroundTexture.draw(s);
                backgroundTexture.display();
                backgroundSprite.setTexture(backgroundTexture.getTexture(),true);
//                backgroundSprite.setTexture(backgroundTexturePointer->getTexture(),true);
//                backgroundTexture.getTexture().copyToImage().saveToFile(fname);
                backgroundRenderTexture.setActive(false);

            } else if (my_window_update == 4) {
                backgroundRenderTexture.setActive(true);
                backgroundRenderTexturePointer->create(1920,1080);
                backgroundRenderTexturePointer->clear(sf::Color(255,0,0,255));
                backgroundRenderTexturePointer->display();
                printf("set red\n");
                backgroundRenderTexture.setActive(false);
            }

            my_window_update = 0;

        }
        window.clear(sf::Color::Transparent);
//        window_mutex.lock();
        window.draw(backgroundSprite);
        window.display();
//        window_mutex.unlock();
    }

    return 0;
}

/*
bool setShape2(HWND hWnd, const sf::Image& image)
{
    const sf::Uint8* pixelData = image.getPixelsPtr();
    HRGN hRegion = CreateRectRgn(0, 0, image.getSize().x, image.getSize().y);

    // Determine the visible region
    for (unsigned int y = 0; y < image.getSize().y; y++)
    {
        for (unsigned int x = 0; x < image.getSize().x; x++)
        {

            if (pixelData[y * image.getSize().x * 4 + x * 4 + 3] == 0)
            {
                HRGN hRegionDest = CreateRectRgn(0, 0, 1, 1);
                HRGN hRegionPixel = CreateRectRgn(x, y, x+1, y+1);
                CombineRgn(hRegionDest, hRegion, hRegionPixel, RGN_XOR);
                DeleteObject(hRegion);
                DeleteObject(hRegionPixel);
                hRegion = hRegionDest;
            }
        }
    }

    SetWindowRgn(hWnd, hRegion, true);
    DeleteObject(hRegion);
    return true;
}

bool setTransparency2(HWND hWnd, unsigned char alpha)
{
    SetWindowLong(hWnd, GWL_EXSTYLE, GetWindowLong(hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);

//    SetLayeredWindowAttributes(hWnd, 0, alpha, LWA_ALPHA);
//    SetLayeredWindowAttributes(hWnd, RGB(200,100,50), alpha, LWA_ALPHA);
    SetLayeredWindowAttributes(hWnd, 0, alpha, LWA_COLORKEY);
//    SetLayeredWindowAttributes(hWnd, RGB(200,100,50), alpha, LWA_COLORKEY);

    return true;

}

*/

bool active = true;

int do_events(struct Drawing* drawing) {
//    sf::Context context;

    while (drawing->window.pollEvent(drawing->event)) {
        if ( drawing->event.type == sf::Event::KeyPressed ) {
            static float zoom = 1.0;
            static float rotating = 0.0;

            if ( drawing->event.type == sf::Event::Closed) {
                if ( drawing->event.key.code == sf::Keyboard::Escape )
                printf ("Close window, leaving thread.!\n");
                drawing->kill = true;
            }

            if ( drawing->event.key.control ) {
                switch(drawing->event.text.unicode) {
                    case sf::Keyboard::PageUp:
                        rotating += 10.0;
                        drawing->rotation=rotating;
                        printf( "Rotated %f\n", rotating );
                        break;
                    case sf::Keyboard::PageDown:
                        rotating -= 10.0;
                        drawing->rotation=rotating;
                        printf( "Rotated %f\n", rotating );
                        break;
                }
            } else {
                switch(drawing->event.text.unicode) {

/*
                    case sf::Keyboard::Left:
                        drawing->backgroundsprite.setPosition(
                            drawing->backgroundsprite.getPosition().x+24,
                            drawing->backgroundsprite.getPosition().y
                        );
//                        printf( "Panning left %f\n", drawing->backgroundsprite.getPosition().x );
                        break;
                    case sf::Keyboard::Right:
                        drawing->backgroundsprite.setPosition(
                            drawing->backgroundsprite.getPosition().x-24,
                            drawing->backgroundsprite.getPosition().y
                        );
//                        printf( "Panning right %f\n", drawing->backgroundsprite.getPosition().x );
                        break;
                    case sf::Keyboard::Up:
                        drawing->backgroundsprite.setPosition(
                            drawing->backgroundsprite.getPosition().x,
                            drawing->backgroundsprite.getPosition().y+1
                        );
//                        printf( "Panning up %f\n", drawing->backgroundsprite.getPosition().x );
                        break;
                    case sf::Keyboard::Down:
                        drawing->backgroundsprite.setPosition(
                            drawing->backgroundsprite.getPosition().x,
                            drawing->backgroundsprite.getPosition().y-1
                        );
//                        printf( "Panning down %f\n", drawing->backgroundsprite.getPosition().x );
                        break;
*/

                    case sf::Keyboard::PageUp:
                        zoom=zoom*1.05;
                        drawing->backgroundsprite.setScale(zoom,zoom);
                        printf( "Zooming in %f\n", zoom );
                        break;
                    case sf::Keyboard::PageDown:
                        zoom=zoom/1.05;
                        drawing->backgroundsprite.setScale(zoom,zoom);
                        printf( "Zooming out %f\n", zoom );
                        break;

                    case sf::Keyboard::Escape:
                        printf ("Escape pressed, \n");
                        drawing->kill = true;
                        break;
                    case sf::Keyboard::S:
                        drawing->smooth = ! drawing->smooth;
                        if (drawing->smooth) printf ("Toggle smooth: ON.\n");
                        else printf ("Toggle smooth: OFF.\n");
                        break;
                    case sf::Keyboard::V:
                        if ( drawing->setverticalsync ) {
                            drawing->wantsetverticalsync = false;
                        } else {
                            drawing->wantsetverticalsync = true;
                        }
                        if (drawing->wantsetverticalsync) printf ("Wanting verticalSync: ON.\n");
                        else printf ("Wanting verticalSync: OFF.\n");
                        break;
                }
            }
        }
    }
    return 0;
}


void renderingThread(struct Drawing* drawing)
{
//    sf::Context context;

    sf::Window* window = &(drawing->window);
    sf::RenderTexture* rendertexture = &(drawing->rendertexture);
    sf::Sprite* sprite = &(drawing->sprite);
    sf::Sprite* backgroundsprite = &(drawing->backgroundsprite);
    sf::Texture* tiles = &(drawing->tiles);
//    sf::Event* event = &(drawing->event);

    sf::Image backgroundImage;
    backgroundImage.loadFromFile("image.png");

    sf::Texture t;
    sf::Sprite s;

    sf::RenderTexture backgroundTexture;
    sf::Sprite backgroundSprite;

//    drawing->backgroundsprite = &(backgroundSprite);

    t.loadFromImage(backgroundImage);
    s.setTexture(t,true);

    sf::ContextSettings contextSettings(0,0,2,3,0);
    contextSettings.antialiasingLevel = 2;

    backgroundTexture.create(t.getSize().x,t.getSize().y,contextSettings);
    backgroundTexture.generateMipmap();


    backgroundTexture.clear(sf::Color::Transparent);
    backgroundTexture.draw(s);
    backgroundTexture.display();
    backgroundSprite.setTexture(backgroundTexture.getTexture(),true);

//    backgroundTexturePointer = &backgroundTexture;

    rendertexture->create(1920,1080,contextSettings);
    rendertexture->generateMipmap();

    rendertexture->clear(sf::Color(128,128,128,255));
    sprite->setTexture(drawing->rendertexture.getTexture(),true);

//    backgroundRenderTexturePointer = &(drawing->rendertexture);
    tiles->loadFromFile("out.png");
    tiles->generateMipmap();

//    tiles->setSmooth(true);
//    t.setSmooth(true);
//    rendertexture->setSmooth(true);
    backgroundTexture.setSmooth(drawing->smooth);

    window->setActive(true);

    backgroundsprite->setOrigin(backgroundTexture.getTexture().getSize().x/2.0, backgroundTexture.getTexture().getSize().y/2.0 );
    backgroundsprite->setPosition(window->getSize().x/2.0, window->getSize().y/2.0 );

    while (window->isOpen() && drawing->kill == false)
    {

        if (backgroundTexture.isSmooth() != drawing->smooth) backgroundTexture.setSmooth(drawing->smooth);

        if ( active ) {
            do_events( drawing );
            if ( drawing->wantsetverticalsync != drawing->setverticalsync ) {
                if ( drawing->wantsetverticalsync == true ) {
                    printf ("Setting verticalSync: ON.\n");
                    window->setVerticalSyncEnabled( true );
                    drawing->setverticalsync = true;
                } else {
                    printf ("Setting verticalSync: OFF.\n");
                    window->setVerticalSyncEnabled( false );
                    drawing->setverticalsync = false;
                }

            }
        }
        if (my_window_update >= 1) {
            if (my_window_update == 1) {
                backgroundImage.loadFromFile("out.png");

                t.loadFromImage(backgroundImage);
                s.setTexture(t,true);

                s.setOrigin(t.getSize().x/2.0, t.getSize().y/2.0 );
                s.setPosition(backgroundTexture.getTexture().getSize().x/2.0, backgroundTexture.getTexture().getSize().y/2.0 );

                backgroundTexture.create(t.getSize().x,t.getSize().y);
                backgroundTexture.setSmooth(drawing->smooth);

                backgroundTexture.clear(sf::Color::Blue);
                backgroundTexture.draw(s);
                backgroundTexture.display();
                backgroundSprite.setTexture(backgroundTexture.getTexture(),true);

                backgroundsprite->setOrigin(backgroundTexture.getTexture().getSize().x/2.0, backgroundTexture.getTexture().getSize().y/2.0 );

                my_window_update = 0;
            } else if (my_window_update == 2) { //full


                t.create(backgroundTexture.getTexture().getSize().x,backgroundTexture.getTexture().getSize().y);
                t.update(backgroundTexture.getTexture());

                backgroundTexture.create(1920,1080);
                backgroundTexture.setSmooth(drawing->smooth);
                backgroundTexture.clear(sf::Color::Cyan);

                backgroundsprite->setOrigin(backgroundTexture.getTexture().getSize().x/2.0, backgroundTexture.getTexture().getSize().y/2.0 );

                s.setOrigin(t.getSize().x/2.0, t.getSize().y/2.0 );
                s.setPosition(backgroundTexture.getTexture().getSize().x/2.0, backgroundTexture.getTexture().getSize().y/2.0 );

                s.setTexture(t,true);
                backgroundTexture.draw(s);
                backgroundTexture.display();
                backgroundSprite.setTexture(backgroundTexture.getTexture(),true);

                my_window_update = 0;
            } else if (my_window_update == 3) { //replace - update ?
                printf(".");

//                rendertexture->setActive(true);

                s.setTexture(rendertexture->getTexture(), true);
                s.setTextureRect({0,0,1920,1080});
                s.setPosition(0,0);
                backgroundTexture.draw(s);
                backgroundTexture.display();

//                rendertexture->setActive(true);

                backgroundSprite.setTexture(backgroundTexture.getTexture(),true);
                backgroundSprite.setTextureRect({0,0,1920,1080});
                backgroundSprite.setPosition(0,0);
//                backgroundSprite.setTexture(backgroundTexturePointer->getTexture(),true);
//                backgroundTexture.getTexture().copyToImage().saveToFile(fname);
//                backgroundRenderTexture.setActive(false);

                my_window_update = 0;
            } else if (my_window_update == 4) {
//                rendertexture->create(1920,1080);
//                rendertexture->clear(sf::Color(255,0,0,255));
//                rendertexture->display();
                printf("set red\n");
//                backgroundRenderTexture.setActive(false);
                my_window_update = 0;
            } else if (my_window_update == 10) {
                s.setTexture(*tiles, true);
//                s.setPosition(200,200);

                s.setOrigin(tiles->getSize().x/2.0, tiles->getSize().y/2.0 );
                s.setPosition(backgroundTexture.getTexture().getSize().x/2.0, backgroundTexture.getTexture().getSize().y/2.0 );

                backgroundTexture.clear(sf::Color(0,255,0,255));
                backgroundTexture.draw(s);
                backgroundTexture.display();
                my_window_update = 0;
            } else if (my_window_update == 11) {
                backgroundTexture.clear(sf::Color(255,0,255,255));
                backgroundTexture.display();
                my_window_update = 0;
            } else if (my_window_update == 13) {
                if ( drawing->setverticalsync ) {
                    window->setVerticalSyncEnabled( false );
                    drawing->setverticalsync = false;
                    drawing->wantsetverticalsync = false;
                }
//                while (my_window_update == 13) {
                sprite->setTexture(*tiles);
                rendertexture->draw(*sprite);
                backgroundTexture.draw(*sprite);

//                    my_window_update = 0;
//                    sf::sleep(sf::seconds(0.001));
//                }
                backgroundTexture.display();
                my_window_update = 0;
            } else if (my_window_update == 14) {
                rendertexture->display();
                backgroundTexture.clear(sf::Color::Transparent);
                sprite->setTexture( rendertexture->getTexture(),true );
                sprite->setTextureRect( { 0,0, 1920,1080 } );
                sprite->setPosition( 0,0 );
                sprite->setOrigin( 0,0 );
                backgroundTexture.draw( *sprite );

                backgroundTexture.display();
                if ( ! drawing->setverticalsync ) {
                    window->setVerticalSyncEnabled( true );
                    drawing->setverticalsync = true;
                    drawing->wantsetverticalsync = true;
                }
                my_window_update = 0;
            }
        }

        drawing->window.clear(sf::Color::Transparent);

//        window_mutex.lock();
//        static float r=0.0;
//        r=r+0.01;
        static float rot=0.0;
        static float posx=0.0;
        static float posy=0.0;

        float of,div;
        if ( drawing->setverticalsync ) {
            of=24;
            div=15.0;
        } else {
            of=0.5;
            div=999.0;
        }

        float ns = ( backgroundSprite.getScale().x * div + backgroundsprite->getScale().x ) / ( div+1.0 );
        backgroundSprite.setScale       ( ns, ns );

        backgroundSprite.setOrigin      ( backgroundsprite->getOrigin() );

//        backgroundSprite.setPosition    ( backgroundsprite->getPosition() );

        rot = ( rot * div + drawing->rotation ) / ( div + 1.0 );
        backgroundSprite.setRotation( rot );

        if ( sf::Keyboard::isKeyPressed(sf::Keyboard::Left) ) {
            backgroundsprite->setPosition(
                backgroundsprite->getPosition().x+of,
                backgroundsprite->getPosition().y
            );
        } else if ( sf::Keyboard::isKeyPressed(sf::Keyboard::Right) ) {
            backgroundsprite->setPosition(
                backgroundsprite->getPosition().x-of,
                backgroundsprite->getPosition().y
            );
        } else if ( sf::Keyboard::isKeyPressed(sf::Keyboard::Up) ) {
            backgroundsprite->setPosition(
                backgroundsprite->getPosition().x,
                backgroundsprite->getPosition().y+of
            );
        } else if ( sf::Keyboard::isKeyPressed(sf::Keyboard::Down) ) {
            backgroundsprite->setPosition(
                backgroundsprite->getPosition().x,
                backgroundsprite->getPosition().y-of
            );
        }
        posx =  ( posx * div + backgroundsprite->getPosition().x ) / (div+1.0) ;
        posy =  ( posy * div + backgroundsprite->getPosition().y ) / (div+1.0) ;

        backgroundSprite.setPosition( posx, posy );

//        backgroundSprite.setRotation( ( backgroundSprite.getRotation() * 15.0 + backgroundsprite->getRotation() ) / 16.0 );


//        backgroundSprite.setRotation    ( backgroundsprite->getRotation() );
//        backgroundSprite.setOrigin(backgroundTexture.getTexture().getSize().x/2.0, backgroundTexture.getTexture().getSize().y/2.0 );
//        backgroundSprite.setPosition(window->getSize().x/2.0, window->getSize().y/2.0 );


        drawing->window.draw(backgroundSprite);
        drawing->window.display();
//        window_mutex.unlock();
    }
    window->setActive(false);
//    sf::Context context;
    printf (" leaving thread.\n");
    drawing->kill = false;
}


//int main_window() {
//int main_window(unsigned char* bin, std::map<int, int> &Tileset ) {
int main_window(unsigned char* bin, sf::RenderTexture &tiles_texture, std::map<int, int> &Tileset ) {

    struct Drawing drawing;
    sf::Window* window = &(drawing.window);
//    sf::Event* event = &(drawing.event);
//    sf::RenderTexture* rendertexture = &(drawing.rendertexture);
//    sf::Sprite* sprite = &(drawing.sprite);
//    sf::Texture* tiles = &(drawing.tiles);
//    sf::Sprite* backgroundsprite = &(drawing.backgroundsprite);

    my_window_update = 0;

//    sf::RenderWindow window;
    sf::ContextSettings contextSettings(0,0,2,3,0);
    contextSettings.antialiasingLevel = 2;
//    window->create(sf::VideoMode(1920,1080, 32), "Transparent Window", sf::Style::None );
    window->create(sf::VideoMode(1920,1080, 32), "Transparent Window", sf::Style::None ,contextSettings);
    window->setPosition(sf::Vector2i(0,0));
    window->setVerticalSyncEnabled(true);

/*
    drawing.rendertexture.create(1920,1080);
    drawing.rendertexture.clear(sf::Color(128,128,128,255));
    drawing.sprite.setTexture(drawing.rendertexture.getTexture(),true);

    backgroundRenderTexturePointer = &(drawing.rendertexture);
*/

//    drawing.rendertexture.setActive(false);

    window->setActive(false);

    sf::Thread thread(&renderingThread, &drawing);
    thread.launch();

    sf::sleep(sf::seconds(2.0));
    active = true;
    showbin( bin, tiles_texture, Tileset, &drawing);
    drawing.kill = false;
//    active = false;

//    showbin( bin, tiles_texture, Tileset);

//    sf::Event event;


    while (window->isOpen() && drawing.kill == false)  {
//        while ( active ) sf::sleep( sf::seconds( 0.001 ) );

        do_events(&drawing);
/*
        while ( window->pollEvent( event ) )
        {

            if ( event.type == sf::Event::KeyPressed ) {
                static float zoom = backgroundsprite->getScale().x;

//                if ( event->type == sf::Event::KeyPressed ) printf ("Key pressed. %d\n", event->key.code);
//                printf("sf::Keyboard::Add = %d\n", sf::Keyboard::Add);
//                printf("sf::Keyboard::Subtract = %d\n", sf::Keyboard::Subtract);
                if ( event.key.code == sf::Keyboard::PageUp) {
                    zoom=zoom*1.01;
                    backgroundsprite->setScale(zoom,zoom);
                    printf( "Zooming in %f\n", zoom );
                } else if ( event.key.code == sf::Keyboard::PageDown) {
                    zoom=zoom/1.01;
                    backgroundsprite->setScale(zoom,zoom);
                    printf( "Zooming out %f\n", zoom );
                }

            }

            if (event.type == sf::Event::Closed || (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)) {

                if ( event.key.code == sf::Keyboard::Escape ) printf ("Escape pressed, doei!\n");
                drawing.kill = true;
                while ( drawing.kill ) sf::sleep(sf::seconds(0.01));
                window->close();
            }
        }
*/
    }
    while ( drawing.kill ) sf::sleep(sf::seconds(0.01));
    if (window->isOpen()) window->close();
    printf ("Closing window, returning.\n");

    return 0;
}
