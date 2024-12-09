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

const float KB_SPEED = 0.5;
const float velocityIncreaseInterval = 15000;
const float velocityIncrement = 0.025;
float lastVelocityIncreaseTime= 0;

void LoadTex(Texture& tex, string filename) {
    if (!tex.loadFromFile(filename)) {
        cout << "Could not load " << filename << endl;
    }
}

void MoveController(PhysicsRectangle& Controller, int elapsedMS) {
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

    PhysicsRectangle controller;
    controller.setSize(Vector2f(75, 15));
    Vector2f sz = controller.getSize();
    controller.setCenter(Vector2f(400,750 - (sz.y / 2)));
    world.AddPhysicsBody(controller);

    // Create the ball
    PhysicsCircle ball;
    ball.setCenter(Vector2f(390, 620));
    ball.setRadius(12);
    world.AddPhysicsBody(ball);
    bool ballRemoved = false;
    ball.applyImpulse(Vector2f(0.1, -0.25));

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
    for (int j(0); j <9; j++) {
        for (int i(0); i < 11; i++) {
            PhysicsSprite& redblock = redblocks.Create();
            redblock.setTexture(redTex);
            Vector2f sz = redblock.getSize();
            redblock.setCenter(Vector2f((70 * i) + 50, 140+(30*j)));
            redblock.setStatic(true);
            world.AddPhysicsBody(redblock);
            redblock.onCollision =
                [&world, &redblock, &redblocks, &score, &left, &right, &ball ,&ballRemoved]
                (PhysicsBodyCollisionResult result) {
                if (result.object2 == ball) {
                    world.RemovePhysicsBody(redblock);
                    redblocks.QueueRemove(redblock);
                    score += 1;
                    Vector2f velocity = ball.getVelocity();
                    ball.setVelocity(Vector2f(velocity.x, -velocity.y));
                    if (score == 99) {
                        world.RemovePhysicsBody(ball);
                        ballRemoved = true;
                    }
                }
                };
        }
    }

    top.onCollision = [ &world, &ball]
    (PhysicsBodyCollisionResult result) {
        if (result.object2 == ball) {
            Vector2f velocity = ball.getVelocity();
            ball.setVelocity(Vector2f(velocity.x, -velocity.y));
        }
        };

    right.onCollision = [&world, &ball]
    (PhysicsBodyCollisionResult result) {
        if (result.object2 == ball) {
            Vector2f velocity = ball.getVelocity();
            ball.setVelocity(Vector2f(-velocity.x, velocity.y));  
        }
        };

    left.onCollision = [&world, &ball]
    (PhysicsBodyCollisionResult result) {
        if (result.object2 == ball) {
            Vector2f velocity = ball.getVelocity();
            ball.setVelocity(Vector2f(-velocity.x, velocity.y));  
        }
        };

    bottom.onCollision = [&world, &ball , &ballRemoved]
    (PhysicsBodyCollisionResult result) {
        if (result.object2 == ball){
            world.RemovePhysicsBody(ball);
            ballRemoved = true;
        }

    };

    controller.onCollision = [&world, &ball, &controller](PhysicsBodyCollisionResult result) {
        if (result.object2 == ball) {
            Vector2f velocity = ball.getVelocity();
            Vector2f ballPos = ball.getCenter();
            Vector2f controllerPos = controller.getCenter();
            Vector2f controllerSize = controller.getSize();
            float relativeHitPosition = ballPos.x - (controllerPos.x - controllerSize.x / 2);
            float normalizedHitPosition = relativeHitPosition / (controllerSize.x / 2);
            if (normalizedHitPosition < 0.5) {
                ball.setVelocity(Vector2f(velocity.x - 0.1, -velocity.y));  
            }
            else if (normalizedHitPosition > 0.5) {
                ball.setVelocity(Vector2f(velocity.x + 0.1, -velocity.y)); 
            }
            else {
                ball.setVelocity(Vector2f(velocity.x, -velocity.y)); 
            }
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
    while (ballRemoved == false) {
        currentTime = clock.getElapsedTime();
        Time deltaTime = currentTime - lastTime;
        long deltaMS = deltaTime.asMilliseconds();
        if (deltaMS > 1) {
            lastTime = currentTime;
            world.UpdatePhysics(deltaMS);
            MoveController(controller, deltaMS);
            if (currentTime.asMilliseconds() - lastVelocityIncreaseTime >=velocityIncreaseInterval) {
                Vector2f currentVelocity = ball.getVelocity();
                ball.setVelocity(Vector2f(currentVelocity.x * (1 +velocityIncrement), currentVelocity.y * (1 +velocityIncrement)));

                lastVelocityIncreaseTime = currentTime.asMilliseconds();
            }
            double yaxis(0);
            if (controller.getCenter().x < 50) {
                yaxis = controller.getCenter().y;
                controller.setCenter(Vector2f(50,yaxis));
            }
            else if (-45+controller.getCenter().x + controller.getGlobalBounds().width > window.getSize().x) {
                yaxis = controller.getCenter().y;
                controller.setCenter(Vector2f((window.getSize().x - controller.getGlobalBounds().width + 45),yaxis));

            }
               
            window.clear();
            redblocks.DoRemovals();
            for (PhysicsShape& redblock : redblocks) {
                window.draw((PhysicsSprite&)redblock);
            }
            window.draw(controller);
            window.draw(ball);
            Text scoreText;
            scoreText.setString(to_string(score));
            scoreText.setFont(fnt);
            scoreText.setPosition(Vector2f(780 - GetTextSize(scoreText).x, 10));
            window.draw(scoreText);
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
    if (score == 99) {
        gameOverText.setString("YOU WIN");
    }
    else{
        gameOverText.setString("GAME OVER");
    }
    gameOverText.setFont(fnt);
    sz = GetTextSize(gameOverText);
    gameOverText.setPosition(400 - (sz.x / 2), 400 - (sz.y / 2));
    window.draw(gameOverText);
    window.display();
    while (true);

}

