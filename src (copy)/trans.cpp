#include <SFML/Graphics.hpp>

// Comment this define to only make the background transparent and keep all other things drawn to the window opaque
#define TRANSPARENT_DRAWING

const unsigned char backgroundOpacity = 50;

int main3()
{
    sf::Image backgroundImage;
    backgroundImage.loadFromFile("image.png");

    sf::RenderWindow window(sf::VideoMode(backgroundImage.getSize().x, backgroundImage.getSize().y, 32), "Transparent Window");
    window.setPosition(sf::Vector2i((sf::VideoMode::getDesktopMode().width - backgroundImage.getSize().x) / 2,
                                    (sf::VideoMode::getDesktopMode().height - backgroundImage.getSize().y) / 2));

    sf::Texture backgroundTexture;
    sf::Sprite backgroundSprite;
    backgroundTexture.loadFromImage(backgroundImage);
    backgroundSprite.setTexture(backgroundTexture);

#ifdef TRANSPARENT_DRAWING
    sf::RenderTexture renderTexture;
//    renderTexture.create(backgroundImage.getSize().x, backgroundImage.getSize().y);

    renderTexture.create(backgroundImage.getSize().x, backgroundImage.getSize().y, 32);

#endif

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed || (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape))
                window.close();
        }

#ifdef TRANSPARENT_DRAWING
        renderTexture.clear(sf::Color::Transparent);
        renderTexture.draw(backgroundSprite);
        renderTexture.display();

        sf::Sprite renderTextureSprite(renderTexture.getTexture());
        renderTextureSprite.setColor({255, 255, 255, backgroundOpacity});

        window.clear({0, 0, 0, backgroundOpacity});
        window.draw(renderTextureSprite);
        window.display();
#else
        window.clear({0, 0, 0, backgroundOpacity});
        window.draw(backgroundSprite);
        window.display();
#endif
    }

    return 0;
}
