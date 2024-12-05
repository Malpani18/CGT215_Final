// CGT215_Final.cpp : This file contains the 'main' function. Program execution begins and ends there.
//


#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFPhysics.h>
#include <vector>
#include <SFML/Audio.hpp>

using namespace std;
using namespace sf;
using namespace sfp;

const float KB_SPEED = 0.45;

void LoadTex(Texture& tex, string filename) {
    if (!tex.loadFromFile(filename)) {
        cout << "Could not load " << filename << endl;
    }
}

void MoveController(PhysicsSprite& Controller, int elapsedMS) {
    if (Keyboard::isKeyPressed(Keyboard::Right)) {
        Vector2f newPos(Controller.getCenter());
        newPos.x = newPos.x + (KB_SPEED * elapsedMS);
        Controller.setCenter(newPos);
    }
    if (Keyboard::isKeyPressed(Keyboard::Left)) {
        Vector2f newPos(Controller.getCenter());
        newPos.x = newPos.x - (KB_SPEED * elapsedMS);
        Controller.setCenter(newPos);
    }
}


Vector2f GetTextSize(Text text) {
    FloatRect r = text.getGlobalBounds();
    return Vector2f(r.width, r.height);
}

int main()
{
    RenderWindow window(VideoMode(800, 800), "BreakIN");
    World world(Vector2f(0, 0));
    int score(0);
    int arrows(5);

    PhysicsSprite& controller = *new PhysicsSprite();
    Texture controllerTex;
    LoadTex(controllerTex, "images/controller.png");
    controller.setTexture(controllerTex);
    Vector2f sz = controller.getSize();
    controller.setCenter(Vector2f(400,750 - (sz.y / 2)));

    PhysicsSprite arrow;
    Texture arrowTex;
    LoadTex(arrowTex, "images/arrow.png");
    arrow.setTexture(arrowTex);
    bool drawingArrow(false);

    PhysicsRectangle top;
    top.setSize(Vector2f(800, 20));
    top.setCenter(Vector2f(400, 65));
    top.setStatic(true);
    world.AddPhysicsBody(top);

    PhysicsRectangle bottom;
    bottom.setSize(Vector2f(800, 10));
    bottom.setCenter(Vector2f(400,795));
    bottom.setStatic(true);
    world.AddPhysicsBody(bottom);

    PhysicsRectangle left;
    left.setSize(Vector2f(20, 800));
    left.setCenter(Vector2f(0, 400));
    left.setStatic(true);
    world.AddPhysicsBody(left);


    PhysicsRectangle right;
    right.setSize(Vector2f(20, 800));
    right.setCenter(Vector2f(800, 400));
    right.setStatic(true);
    world.AddPhysicsBody(right);

    Texture redTex;
    LoadTex(redTex, "images/redblock.png");
    PhysicsShapeList<PhysicsSprite> redblocks;
    for (int j(0); j <10; j++) {
        for (int i(0); i < 10; i++) {
            PhysicsSprite& redblock = redblocks.Create();
            redblock.setTexture(redTex);
            Vector2f sz = redblock.getSize();
            redblock.setCenter(Vector2f((77.75 * i) + 50, 140+(25*j)));
            redblock.setStatic(true);
            world.AddPhysicsBody(redblock);
            redblock.onCollision =
                [&drawingArrow, &world, &arrow, &redblock, &redblocks, &score, &left, &right]
                (PhysicsBodyCollisionResult result) {
                if (result.object2 == arrow) {
                    drawingArrow = false;
                    world.RemovePhysicsBody(arrow);
                    world.RemovePhysicsBody(redblock);
                    redblocks.QueueRemove(redblock);
                    score += 10;
                }
                };
        }
    }

    top.onCollision = [&drawingArrow, &world, &arrow]
    (PhysicsBodyCollisionResult result) {
        if (result.object2 == arrow) {
            drawingArrow = false;
            world.RemovePhysicsBody(arrow);
        }
        };

    Font fnt;
    if (!fnt.loadFromFile("breakout.ttf")) {
        cout << "Could not load font." << endl;
        exit(3);
    }
    Clock clock;
    Time lastTime(clock.getElapsedTime());
    Time currentTime(lastTime);
    while ((arrows > 0) || drawingArrow) {
        currentTime = clock.getElapsedTime();
        Time deltaTime = currentTime - lastTime;
        long deltaMS = deltaTime.asMilliseconds();
        if (deltaMS > 9) {
            lastTime = currentTime;
            world.UpdatePhysics(deltaMS);
            MoveController(controller, deltaMS);
            if (Keyboard::isKeyPressed(Keyboard::Space) &&
                !drawingArrow) {
                drawingArrow = true;
                arrows = arrows - 1;
                arrow.setCenter(controller.getCenter());
                arrow.setVelocity(Vector2f(0, -1));
                world.AddPhysicsBody(arrow);

            }
            double yaxis(0);
            if (controller.getCenter().x < 50) {
                yaxis = controller.getCenter().y;
                controller.setCenter(Vector2f(50,yaxis));
            }
            else if (-50+controller.getCenter().x + controller.getGlobalBounds().width > window.getSize().x) {
                yaxis = controller.getCenter().y;
                controller.setCenter(Vector2f((window.getSize().x - controller.getGlobalBounds().width + 50),yaxis));

            }
               
            window.clear();
            if (drawingArrow) {
                window.draw(arrow);
            }
            redblocks.DoRemovals();
            for (PhysicsShape& redblock : redblocks) {
                window.draw((PhysicsSprite&)redblock);
            }
            window.draw(controller);
            Text scoreText;
            scoreText.setString(to_string(score));
            scoreText.setFont(fnt);
            scoreText.setPosition(Vector2f(780 - GetTextSize(scoreText).x, 10));
            window.draw(scoreText);
            Text arrowCountText;
            arrowCountText.setString(to_string(arrows));
            arrowCountText.setFont(fnt);
            arrowCountText.setPosition(Vector2f(35 - GetTextSize(arrowCountText).x, 10));
            window.draw(arrowCountText);
            Text BreakInText;
            BreakInText.setString("BREAKIN");
            BreakInText.setFont(fnt);
            BreakInText.setPosition(Vector2f(460 - GetTextSize(BreakInText).x, 10));
            window.draw(BreakInText);
            window.draw(right);
            window.draw(left);
            window.draw(top);
            //world.VisualizeAllBounds(window);

            window.display();

        }
    }
    window.display(); // this is needed to see the last frame
    window.clear(Color(0, 0, 0));
    Text gameOverText;
    gameOverText.setString("GAME OVER");
    gameOverText.setFont(fnt);
    sz = GetTextSize(gameOverText);
    gameOverText.setPosition(400 - (sz.x / 2), 400 - (sz.y / 2));
    window.draw(gameOverText);
    window.display();
    while (true);

}


// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file


/*
#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFPhysics.h>
using namespace std;
using namespace sf;
using namespace sfp;
int main()
{
    // Create our window and world with gravity 0,1
    RenderWindow window(VideoMode(800, 600), "Bounce");
    World world(Vector2f(0, 1));
    // Create the ball
    PhysicsCircle ball;
    ball.setCenter(Vector2f(400, 300));
    ball.setRadius(20);
    world.AddPhysicsBody(ball);
    // Create the floor
    PhysicsRectangle floor;
    floor.setSize(Vector2f(800, 20));
    floor.setCenter(Vector2f(400, 590));
    floor.setStatic(true);
    world.AddPhysicsBody(floor);
    int thudCount(0);
    floor.onCollision = [&thudCount](PhysicsBodyCollisionResult result) {
        cout << "thud " << thudCount << endl;
        thudCount++;
        };
    Clock clock;
    Time lastTime(clock.getElapsedTime());
    while (true) {
        // calculate MS since last frame
        Time currentTime(clock.getElapsedTime());
        Time deltaTime(currentTime - lastTime);
        int deltaTimeMS(deltaTime.asMilliseconds());
        if (deltaTimeMS > 0) {
            world.UpdatePhysics(deltaTimeMS);
            lastTime = currentTime;
        }
        window.clear(Color(0, 0, 0));
        window.draw(ball);
        window.draw(floor);
        window.display();
    }
}
*/