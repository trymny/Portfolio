#include <SFML/Graphics.hpp>
#include <random>
#include <math.h>
#include <ctime>
#include <vector>

const int WINDOW_X = 2048;          //Resolution of the window
const int WINDOW_Y = 2048;          //Resolution of the window
const int SET_SIZE = 9;             //must be a 2^n Number
const int SIZE = pow(2,SET_SIZE)+1;
const int PIXELSIZE_X = 3;
const int PIXELSIZE_Y = 3;
const int BUMPINESS = 40;           //The roughness of the heightmap
const int VARIATION = 300;          //Lower values give darker colors,higher values give brighter colors

int heightMap[SIZE][SIZE];
std::vector<sf::RectangleShape> rects(SIZE);

std::seed_seq seed{time(0)};        //Seedes the number every time you open the program
std::mt19937 ranGenerator{ seed };  //Create random number generator

//A distribution that produces values in specified range
std::uniform_int_distribution<> distr(0, VARIATION);

int computeSquare(int x, int y, int halfside)
{
    std::uniform_int_distribution<> distr(-1, 1);
    int value = 0;
    int average = 0;
    int randomValue = distr(ranGenerator);

    //Calculate the average midpoint value
    //NB! All the if statements is just to be sure that I don't go outside the array limit
    if((x-halfside) >= 0   || (y-halfside) >= 0)
		average =  heightMap[x-halfside][y-halfside];
    if((x+halfside) < SIZE || (y-halfside) >= 0)
		average += heightMap[x+halfside][y-halfside];
    if((y+halfside) < SIZE || (x-halfside) >= 0)
		average += heightMap[x-halfside][y+halfside];
    if((x+halfside) < SIZE || (y+halfside) < SIZE)
		average += heightMap[x+halfside][y+halfside];

    average /= 4.0;

    value = average+randomValue*BUMPINESS;
    return value;
}

int computeDiamond(int x, int y, int halfside)
{
    std::uniform_int_distribution<> distr(-1, 1);
    int value = 0;
    int average = 0;
    int randomValue = distr(ranGenerator);

    //Calculate the average midpoint value
    //NB! All the if statements is just to be sure that I don't go outside the array limit
    if((x-halfside) >= 0)
		average =  heightMap[x-halfside][y];
    if((x+halfside) < SIZE)
		average += heightMap[x+halfside][y];
    if((y-halfside) >= 0)
		average += heightMap[x][y-halfside];
    if((y+halfside) < SIZE)
		average += heightMap[x][y+halfside];

    average /= 4.0;

    value = average+randomValue*BUMPINESS;
    return value;
}

void drawHeightMap(sf::RenderTexture &texture)
{
    for (int i = 0; i < SIZE; i++)
    {
        for (int n = 0; n < SIZE; n++)
        {
            for(int k = 0; k < SIZE; k++)
            {
                //Size
                rects[k].setSize(sf::Vector2f(PIXELSIZE_X,PIXELSIZE_Y));
                //Position
                rects[k].setPosition(i*PIXELSIZE_X,n*PIXELSIZE_Y);

                //Must make sure that I don't go outside the "color" limit
                if(heightMap[i][n] > 255)
                    heightMap[i][n] = 255;
                if(heightMap[i][n] < 0)
                    heightMap[i][n] = 0;

                //Set color to black and white
                rects[k].setFillColor(sf::Color(heightMap[i][n],heightMap[i][n],heightMap[i][n],heightMap[i][n]));
                texture.draw(rects[k]);
            }
        }
    }
}

int main()
{
    sf::RenderWindow window(sf::VideoMode(WINDOW_X,WINDOW_Y), "My window");
    window.setFramerateLimit(0);

    //initialize corners
    heightMap[0][0] = distr(ranGenerator);
    heightMap[0][SIZE-1] = distr(ranGenerator);
    heightMap[SIZE-1][0] = distr(ranGenerator);
    heightMap[SIZE-1][SIZE-1] = distr(ranGenerator);

    int halfSide;

    for (int sideLength = SIZE-1; sideLength > 1; sideLength /= 2 )
    {
         halfSide = sideLength / 2;

         // Compute square step
         for (int x = halfSide; x < SIZE-1+halfSide; x += sideLength )
         {
             for (int y = halfSide; y < SIZE-1+halfSide; y += sideLength )
             {
                 heightMap[x][y] = computeSquare(x,y,halfSide);
             }
         }

        // Compute diamond step
        for (int x = 0; x < SIZE; x += sideLength)
        {
            for (int y = 0; y < SIZE; y += sideLength)
            {
               //NB! All the if statements is just to be sure that I don't go outside the array limit
               if((x+halfSide) < SIZE)
				   heightMap[x+halfSide][y] = computeDiamond(x+halfSide,y,halfSide);
               if((y+halfSide) < SIZE)
				   heightMap[x][y+halfSide] = computeDiamond(x,y+halfSide,halfSide);
            }
        }
    }

    //Initialize texture
    sf::RenderTexture texture;
    texture.create(WINDOW_X,WINDOW_Y);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {

            if (event.type == sf::Event::Closed)
            {
                window.close();
                exit(0);
            }
        }
        //Assign color vaules
        drawHeightMap(texture);

        //Everything is set, texture is ready for drawing
        texture.display();

        //Clear window
        window.clear(sf::Color::Black);

        //Texture must be assign to a sprite
        sf::Sprite sprite(texture.getTexture());

        //Draw sprite
        window.draw(sprite);

        //End the current frame and display its contents on screen
        window.display();
    }

    return 0;
}

