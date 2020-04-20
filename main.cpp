#include <stdlib.h>
#include <stdio.h>
#include <SFML/Graphics.hpp>

using std::vector;

class Texture_Manager
{
public:
    enum ID {logo, castle, map};

private:

    std::map <Texture_Manager::ID, sf::Texture*> Texture_Map; 

public:

    void load (Texture_Manager::ID id, const std::string& file_name);
    sf::Texture& get_texture (Texture_Manager::ID id);
};

void Texture_Manager::load (Texture_Manager::ID id, const std::string& file_name)
{
    sf::Texture* texture = new sf::Texture();
    texture->loadFromFile (file_name);
    this->Texture_Map.insert (std::pair <Texture_Manager::ID, sf::Texture*> (id, texture));
}

sf::Texture& Texture_Manager::get_texture (Texture_Manager::ID id)
{
    return *(this->Texture_Map.find (id)->second);
}


Texture_Manager textures;


class Abstract_Field : public sf::Sprite
{

public:
    Abstract_Field (void) {};
    Abstract_Field (float x, float y) { this->setPosition (x, y);}
    Abstract_Field (float x, float y, Texture_Manager::ID id) : Abstract_Field (x, y)
    {
        this->setTexture (textures.get_texture (id));
    }

    void set_texture (Texture_Manager::ID id);

    bool is_transparent = 0;
    void set_transparent (bool transparent);
    bool get_transparent (void);

    virtual int handle_Mouse_Pressed (float x, float y) {return 0;};
    virtual bool is_on_Mouse (float x, float y);
    virtual void render (sf::RenderWindow& window);
};

void Abstract_Field::set_transparent (bool transparent)
{
    this->is_transparent = transparent;
}

bool Abstract_Field::get_transparent (void)
{
    return this->is_transparent;
}

void Abstract_Field::set_texture (Texture_Manager::ID id)
{
    this->setTexture (textures.get_texture (id));
}


bool Abstract_Field::is_on_Mouse (float x, float y)
{
    sf::FloatRect bound = this->getGlobalBounds();
    
    if (bound.contains (x, y))
        return 1;

    return 0;
}

void Abstract_Field::render (sf::RenderWindow& window)
{
    if (!this->get_transparent())
        window.draw (*this);
}

class Manager : public Abstract_Field 
{
public:

    Manager (void) 
    : Abstract_Field (){}

    Manager (int number) 
    : Abstract_Field (){array.reserve (number);}

    Manager (float x, float y)
    : Abstract_Field (x, y) {}

    Manager (float x, float y, Texture_Manager::ID id) 
    : Abstract_Field (x, y, id){}

    vector <Abstract_Field*> array;

    virtual int handle_Mouse_Pressed (float x, float y) override;
    //virtual bool is_on_Mouse (float x, float y) override;
    virtual void render (sf::RenderWindow& window);
    void insert (Abstract_Field* new_field);

    ~Manager () {}
};

void Manager::insert (Abstract_Field* new_field)
{
    this->array.push_back (new_field);
}

int Manager::handle_Mouse_Pressed (float x, float y)
{
    int num = this->array.size();
    int found = 0;

    for (int i = 0; i < num; ++i)
        if (this->array[i]->is_on_Mouse (x, y))
        {
            this->array[i]->handle_Mouse_Pressed (x, y);
            ++found;
        }
    
    //if (found == 0)
        //printf ("YOU MISSED!\n\n");

    return found;
}

void Manager::render (sf::RenderWindow& window)
{
    int size = this->array.size();
    int counter = 0;

    if (!this->get_transparent())
        window.draw (*this);
    
    for (; counter < size; ++counter)
        this->array[counter]->render(window);
}

/*
class Button_Overflow : public Abstract_Field
{
public:
    Button_Overflow (float x, float y, float width, float height, sf::Color color)
    :Abstract_Field (x, y, width, height, color) {}
    
    virtual int handle_Mouse_Pressed (float x, float y) override;
};

int Button_Overflow::handle_Mouse_Pressed (float x, float y)
{
    printf ("His buffer is only 10 symbols long. He expects '*' in the 11-th symbol.\n\n");
}
*/

class Game
{
public:
    Game (void);
    Game (int width, int height);

    sf::RenderWindow window;
    Manager manager;

    void run (void);
    void render (void);
    void update (void);
    void load_map (void);
    void handle_Events (void);
    void load_textures (void);
    void preprocessing (void);
};

Game::Game (void) : window (sf::VideoMode (1000, 800), "Sons of War")
{
    this->window.setPosition (sf::Vector2i (600, 150));
    this->manager = *(new Manager (50, 50));
}

Game::Game (int width, int height) : window (sf::VideoMode (width, height), "Sons of War")
{
    this->window.setPosition (sf::Vector2i (600, 150));
    this->manager = *(new Manager (50, 50));
}

void Game::update (void)
{

}

void Game::load_textures (void)
{
    textures.load (Texture_Manager::logo, "Objects/Pictures/logo.png");
    textures.load (Texture_Manager::castle, "Objects/Pictures/Castles/cas1.png");
    textures.load (Texture_Manager::map, "Objects/Pictures/Maps/map1.png");
}

void Game::load_map (void)
{
    Manager* game_map = new Manager (0, 0, Texture_Manager::map);

    Abstract_Field* castle = new Abstract_Field (100, 200, Texture_Manager::castle);

    game_map->insert (castle);

    this->manager.insert (game_map);

}

void Game::preprocessing (void)
{
    this->load_textures();
}

void Game::render (void)
{
    this->window.clear (sf::Color(0, 204, 204, 255));
    
    this->manager.render (this->window);
    
    this->window.display();
}

void Game::handle_Events (void)
{
    sf::Event event;
    while (this->window.pollEvent(event))
    {
        switch (event.type)
        {
            case sf::Event::MouseButtonPressed:
                this->manager.is_on_Mouse (event.mouseButton.x, event.mouseButton.y);
            break;
            
            case sf::Event::Closed:
                this->window.close();
            break;

            default:
            break;
        }
    }
}

void Game::run (void)
{
    this->preprocessing();
    this->load_map();

    while (this->window.isOpen())
    {
        this->handle_Events();
        this->update();
        this->render();
    }
}


int main()
{

    Game game;
    game.run();

    Abstract_Field test (100, 100, Texture_Manager::logo);
   //test.setTexture (textures.get_texture (Texture_Manager::logo));




    //Button_Overflow btn4 (400, 400, 200, 200, sf::Color::Magenta);
    game.manager.array.push_back (&test);


    return 0;
}