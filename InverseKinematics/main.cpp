#include <SFML/Graphics.hpp>
#include <math.h>
#include <iostream>

const int WINDOW_X = 1800;
const int WINDOW_Y = 1800;
const float LENGTH = 200.0f;

//Compute the dot product
double dot(sf::Vector2f v1, sf::Vector2f v2)
{
    double dotProduct = v1.x*v2.x + v1.y*v2.y;
    return dotProduct;
}

//Compute the determinant
double det(sf::Vector2f v1, sf::Vector2f v2)
{
    double determinant = v1.x*v2.y - v1.y*v2.x;
    return determinant;
}

//Compute the length of the vector
float mag(sf::Vector2f v)
{
    float length = sqrt((pow(v.x,2)+pow(v.y,2)));
    return length;
}

class Arm
{
private:
    sf::Vector2f v, w, pos;
    double angleDiff, angle;
    Arm* parent;
    sf::CircleShape joint;

public:
    Arm(sf::Vector2f v)
    {
        joint.setRadius(25);
        joint.setOrigin(joint.getRadius(), joint.getRadius());
        parent = nullptr;
        pos = v;
        angle = 0;
    }

    void setParent(Arm* p)
    {
        parent = p;
    }

    double totalAngle()
    {
        if(parent==nullptr) return angle;
        else return (angle + parent->totalAngle());
    }

    sf::Vector2f endPos()
    {
        sf::Vector2f result;
        result.x = (LENGTH * cos(totalAngle()));
        result.y = (LENGTH * sin(totalAngle()));
        result += pos;
        return result;
    }

    void ccd(sf::Vector2f target, sf::Vector2f effector)
    {
        v = effector - pos;
        w = target - pos;

        //Compute angle between vector v and vector w
        angle = angle +  atan2(det(v,w), dot(v,w));

        //Create a newEffector
        sf::Vector2f newEffector = pos + LENGTH * (w/mag(w));

        if(parent != nullptr)
        {
            //Update position
            pos = parent->endPos();

            //Compute ccd on the other bones
            parent->ccd(target, newEffector);

            //Update position
            pos = parent->endPos();
        }
    }

    void drawArm(sf::RenderWindow &window)
    {
        sf::VertexArray lines(sf::Lines ,2);
        joint.setPosition(pos);
        lines[1].position = pos;
        lines[0].position = endPos();
        window.draw(lines);
        window.draw(joint);
    }
};

int main()
{
    sf::RenderWindow window(sf::VideoMode(WINDOW_X,WINDOW_Y), "CCD ROBOT");

    //Set framerate
    window.setFramerateLimit(30);

    //Init start position
    Arm arm4(sf::Vector2f(WINDOW_X/2,WINDOW_Y/2)),arm3(arm4.endPos()),arm2(arm3.endPos()), arm1(arm2.endPos());

    //Assign parents
    arm1.setParent(&arm2);
    arm2.setParent(&arm3);
    arm3.setParent(&arm4);

    sf::Vector2f mousePos;

    //Game loop
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
        window.clear(sf::Color::Black);

        //Find mouseposition
        mousePos.x = double(sf::Mouse::getPosition(window).x);
        mousePos.y = double(sf::Mouse::getPosition(window).y);

        //Compute cyclic coordinate descent (ccd)
        arm1.ccd(mousePos, arm1.endPos());

        //Draw to screen
        arm4.drawArm(window);
        arm3.drawArm(window);
        arm2.drawArm(window);
        arm1.drawArm(window);

        window.display();
    }

    return 0;
}

