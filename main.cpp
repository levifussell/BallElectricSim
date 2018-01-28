#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <stdexcept>
#include <math.h>
#include <time.h>

//COMPILE: g++ main.cpp -lsfml-audio -lsfml-network -lsfml-graphics -lsfml-window -lsfml-system

const int SCREEN_WIDTH = 900;
const int SCREEN_HEIGHT = 900;
const int MARGIN = 5;

const int INIT_BALLS_COUNT = 3500;

const int MAX_COLLISION_DISTANCE = 10;
const int MAX_ELECTRIC_DISTANCE = 15;

const float PROBABILITY_OF_BLUE_SEED = 0.2f;

const int MAX_STICKY_COUNT = 20;

const int BALL_DRAW_SIZE = 1;

const int tickrate = 1000;

const int SENSOR_COUNT = 2;
const sf::Vector2f sensorPositions[SENSOR_COUNT] = {
    sf::Vector2f(20, 150),
    sf::Vector2f(740, 160)
};

const sf::Vector2f sensorSizes[SENSOR_COUNT] = {
    sf::Vector2f(10, 10),
    sf::Vector2f(10, 10)
};

unsigned long intervalEndTime;


float randFloat();

class Ball
{
    public:
    enum STATE { MOVING, FROZEN, STICKY, ELECTRIC };
    static const int ELECTRIC_MAX = 5;

    private:
        sf::Vector2f position;
        sf::Vector2f velocity;
        STATE state;
        int stickyCount;
        int electricCount;
        int electricBuildUp;

    public:
        Ball()
        {
            this->position = sf::Vector2f(0.0, 0.0);
            this->velocity = sf::Vector2f(0.0, 0.0);
            this->state = MOVING;
            this->stickyCount = 0;
            this->electricCount = 0;
            this->electricBuildUp = 0;
        }
        Ball(sf::Vector2f pos, sf::Vector2f velocity)
        {
            this->position = pos;
            this->velocity = velocity;
            this->state = MOVING;
            this->stickyCount = 0;
            this->electricCount = 0;
            this->electricBuildUp = 0;
        }
        Ball(const Ball& obj)
        {
            this->position = obj.position;
            this->velocity = obj.velocity;
            this->state = obj.state;
            this->stickyCount = obj.stickyCount;
            this->electricCount = obj.electricCount;
            this->electricBuildUp = obj.electricBuildUp;
        }

        void update(const int SCREEN_WIDTH, const int SCREEN_HEIGHT)
        {
            switch(this->state)
            {
                case STICKY:
                    this->stickyCount--;
                case MOVING:
                    this->position += this->velocity;
                    //if(abs(this->velocity.x) > abs(this->velocity.y))
                        //this->position += sf::Vector2f(this->velocity.x, 0.0f);
                    //else
                        //this->position += sf::Vector2f(0.0f, this->velocity.y);

                    break;
                case FROZEN:
                    break;
                case ELECTRIC:
                    this->electricCount--;
                    break;
            }

            if(this->stickyCount <= 0 && this->state == STICKY)
            {
                //this->stickyCount = 0;
                this->setState(MOVING);
            }

            if(this->electricCount <= 0 && this->state == ELECTRIC)
            {
                this->setState(FROZEN);
            }

            if(this->position.x > SCREEN_WIDTH - MARGIN)
            {
                this->velocity.x = -abs(this->velocity.x);
                //this->position.x = SCREEN_WIDTH - MARGIN;
                //this->setState(STICKY);
            }
            if(this->position.x < MARGIN)
            {
                this->velocity.x = abs(this->velocity.x);
                //this->position.x = MARGIN;
                //this->setState(STICKY);
            }
            if(this->position.y > SCREEN_HEIGHT - MARGIN)
            {
                this->velocity.y = -abs(this->velocity.y);
                //this->position.y = SCREEN_HEIGHT - MARGIN;
                //this->setState(STICKY);

                //if(this->position.x > 50.0 && this->position.x < 70.0)
                //{
                    //this->setState(STICKY);
                    //this->velocity = sf::Vector2f(0.0, -8.0);
                    //this->stickyCount = 0;
                //}
            }
            if(this->position.y < MARGIN)
            {
                this->velocity.y = abs(this->velocity.y);
                //this->position.y = MARGIN;
                //this->setState(STICKY);

                //if(this->position.x > 50.0 && this->position.x < 70.0)
                //{
                    //this->setState(STICKY);
                    //this->velocity = sf::Vector2f(0.0, 8.0);
                    //this->stickyCount = 0;
                //}
            }

            for(int i = 0; i < SENSOR_COUNT; ++i)
            {
                if(this->position.y > sensorPositions[i].y && this->position.y < sensorPositions[i].y + sensorSizes[i].y
                        && this->position.x > sensorPositions[i].x && this->position.x < sensorPositions[i].x + sensorSizes[i].x)
                {
                    this->setState(STICKY);
                }
            }

            // make everything an integer
            this->position.x = int(this->position.x);
            this->position.y = int(this->position.y);
        }

        void setState(STATE state)
        {
            this->state = state;

            if(this->state == STICKY)
            {
                this->stickyCount = MAX_STICKY_COUNT;
            }

            if(this->state == ELECTRIC)
            {
                //if(this->electricBuildUp > ELECTRIC_MAX*3)
                    this->electricCount = ELECTRIC_MAX;
                //else
                    //this->state = FROZEN;
            }
        }
        STATE getState() { return this->state; }

        sf::Color getStateColor()
        {
            switch(this->state)
            {
                case MOVING:
                    return sf::Color::Red;
                case STICKY:
                    return sf::Color::Green;
                    //return sf::Color(0.0f, 255.0f, 0.0f, std::min(this->stickyCount*15, 255));
                case FROZEN:
                    return sf::Color::Blue;
                    //return sf::Color(0.0f, 0.0f, 255.0f, std::min(this->stickyCount*3, 255));
                case ELECTRIC:
                    return sf::Color::Yellow;
                    //return sf::Color(255.0f, 0.0f, std::min(this->electricCount*25, 255));
            }
        }

        void draw(sf::RenderWindow& window)
        {
            //water rec
            sf::RectangleShape ball;
            ball.setSize(sf::Vector2f(BALL_DRAW_SIZE, BALL_DRAW_SIZE));
            ball.setPosition(this->position);
            ball.setFillColor(this->getStateColor());


            window.draw(ball);


        }

        sf::Vector2f getPosition() { return this->position; }
        void setPosition(sf::Vector2f pos) { this->position = pos; }

        sf::Vector2f setVelocity(sf::Vector2f v)
        {
            this->velocity = v;
        }

        int getStickyCount() { return this->stickyCount; }
        void incrementStick() { this->stickyCount += 1; }
        void decrementStick() { this->stickyCount -= 1; }
        void addStick(int v) { this->stickyCount -= v; }
        int getElectricCount() { return this->electricCount; }
        void resetElectricBuildUp() { this->electricBuildUp = 0; }
        void degradeElectricBuildUp() { this->electricBuildUp = (int)((float)this->electricBuildUp * 0.8f); }
        //void degradeElectricBuildUp() { this->electricBuildUp = (int)((float)this->electricBuildUp * 1.0f); }
        void addElectricBuildUp(int v) { this->electricBuildUp += v; }
        int getElectricBuildUp() { return this->electricBuildUp; }
};

bool collision(Ball* ballA, Ball* ballB);
void update(Ball* balls, const int BALLS_COUNT);
void print(sf::RenderWindow& window, Ball* balls, const int BALLS_COUNT);


int main(int n, char** vars)
{
    srand(time(NULL));

    intervalEndTime = clock() + tickrate;

    //create some random balls
    //sf::Vector2f balls[INIT_BALLS_COUNT];
    Ball balls[INIT_BALLS_COUNT];
    // place each ball randomly on the screen
    for(int i = 0; i < INIT_BALLS_COUNT; ++i)
    {
        //balls[i] = sf::Vector2f(randFloat() * SCREEN_WIDTH, randFloat() * SCREEN_HEIGHT);
        balls[i] = Ball(sf::Vector2f(randFloat() * SCREEN_WIDTH, randFloat() * SCREEN_HEIGHT),
                        sf::Vector2f(randFloat()*1.0, randFloat()*1.0));

        float r = float(rand()) / RAND_MAX;
        if(r < PROBABILITY_OF_BLUE_SEED)
            balls[i].setState(Ball::FROZEN);
    }
    //balls[0].setPosition(sf::Vector2f(20, 20));
    //balls[0].setState(Ball::FROZEN);
    //balls[1].setPosition(sf::Vector2f(400, 400));
    //balls[1].setState(Ball::FROZEN);

    sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Ball Machine");

    while(window.isOpen())
    {
        sf::Event event;
        while(window.pollEvent(event))
        {
            if(event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();

        //move player each timestep
        if(intervalEndTime < clock())
        {
            //runPlayerThroughValues(values, transitions, grid);
            intervalEndTime = clock() + 50;
            update(balls, INIT_BALLS_COUNT);
            //std::cout << "UPDATE\n";
            intervalEndTime = clock() + tickrate;
        }

        print(window, balls, INIT_BALLS_COUNT);

        window.display();
    }

    return 0;
}

void update(Ball* balls, const int BALLS_COUNT)
{
    // reset electric build-up
    //for(int i = 0; i < BALLS_COUNT; ++i)
    //{
        //if(balls[i].getState() != Ball::ELECTRIC)
            //balls[i].degradeElectricBuildUp();
    //}

    for(int i = 0; i < BALLS_COUNT; ++i)
    {
        balls[i].update(SCREEN_WIDTH, SCREEN_HEIGHT);

        int numCollisions = 0;

        for(int j = i + 1; j < BALLS_COUNT; ++j)
        {
            if(i != j)
            {
                bool col = collision(&balls[i], &balls[j]);
                if(col)
                    numCollisions++;
            }
        }

        if(numCollisions < 0 && balls[i].getState() == Ball::FROZEN)
            balls[i].setState(Ball::MOVING);
    }
}

bool collision(Ball* ballA, Ball* ballB)
{
    //if(ballA->getState() == Ball::STICKY && ballB->getState() == Ball::STICKY)
    //{
        //stick together

        sf::Vector2f Apos = ballA->getPosition();
        sf::Vector2f Bpos = ballB->getPosition();
        float diffX = Apos.x - Bpos.x;
        float diffY = Apos.y - Bpos.y;
        float dist = sqrt(diffX*diffX+diffY*diffY);

        bool collision = dist < 30.0f;

        if(dist < MAX_COLLISION_DISTANCE)
        {
            //if((ballA->getState() == Ball::STICKY && ballB->getState() == Ball::STICKY)
                //||(ballA->getState() == Ball::FROZEN && ballB->getState() == Ball::STICKY)
                //||(ballA->getState() == Ball::STICKY && ballB->getState() == Ball::FROZEN))
            //if((ballA->getState() == Ball::STICKY && ballB->getState() == Ball::STICKY)
                if((ballA->getState() == Ball::FROZEN && ballB->getState() == Ball::STICKY))
                //||(ballA->getState() == Ball::STICKY && ballB->getState() == Ball::FROZEN))
            {
                //ballA->incrementStick();
                //ballB->incrementStick();

                //if(ballA->getStickyCount() > 50)
                //{
                    //ballA->setState(Ball::FROZEN);
                    //ballA->addStick(200);
                //}
                //if(ballB->getStickyCount() > 50)
                //{
                    ballB->setState(Ball::FROZEN);
                    ballA->addElectricBuildUp(Ball::ELECTRIC_MAX);
                    ballA->setState(Ball::ELECTRIC);
                    //ballB->addStick(200);
                //}
            }
            if((ballB->getState() == Ball::FROZEN && ballA->getState() == Ball::STICKY))
            {
                ballA->setState(Ball::FROZEN);
                ballB->addElectricBuildUp(Ball::ELECTRIC_MAX);
                ballB->setState(Ball::ELECTRIC);
            }
            //else if((ballA->getState() == Ball::FROZEN && ballB->getState() == Ball::MOVING)
                //|| (ballA->getState() == Ball::MOVING && ballB->getState() == Ball::FROZEN))
            //{
                //ballA->decrementStick();
                //ballB->decrementStick();
            //}
            //if(ballA->getState() == Ball::FROZEN || ballB->getState() == Ball::FROZEN)
            {
                ballA->setVelocity(sf::Vector2f(diffX, diffY));
                ballB->setVelocity(sf::Vector2f(-diffX, -diffY));
            }

           if(ballA->getState() == Ball::MOVING && ballB->getState() == Ball::ELECTRIC)
            {
                ballA->setState(Ball::STICKY);
            }
            else if(ballB->getState() == Ball::MOVING && ballA->getState() == Ball::ELECTRIC)
            {
                ballB->setState(Ball::STICKY);
            }

        }

        if(dist < MAX_ELECTRIC_DISTANCE)
        {
            if(((ballA->getState() == Ball::FROZEN /*|| ballA->getState() == Ball::ELECTRIC*/) && ballB->getState() == Ball::ELECTRIC)
                    && ballB->getElectricCount() >= Ball::ELECTRIC_MAX -1 )
            {
                ballA->addElectricBuildUp(ballB->getElectricBuildUp());
                //ballA->addElectricBuildUp(BuildUp());
                ballA->setState(Ball::ELECTRIC);
            }
            else if(((ballB->getState() == Ball::FROZEN /*|| ballB->getState() == Ball::ELECTRIC*/) && ballA->getState() == Ball::ELECTRIC)
                    && ballA->getElectricCount() >= Ball::ELECTRIC_MAX -1 )
            {
                ballB->addElectricBuildUp(ballA->getElectricBuildUp());
                ballB->setState(Ball::ELECTRIC);
            }
        }


    //}
    //else
    //{
        //bounce away

        //sf::Vector2f Apos = ballA->getPosition();
        //sf::Vector2f Bpos = ballB->getPosition();
        //float diffX = Apos.x - Bpos.x;
        //float diffY = Apos.y - Bpos.y;

        //float dist = sqrt(diffX*diffX+diffY*diffY);

        //if(dist < 10.0)
        //{
            //ballA->setVelocity(sf::Vector2f(diffX, diffY));
            //ballB->setVelocity(sf::Vector2f(-diffX, -diffY));
        //}
    //}

    return collision;
}

void print(sf::RenderWindow& window, Ball* balls, const int BALLS_COUNT)
{
    //draw sensors
    for(int i = 0; i < SENSOR_COUNT; ++i)
    {
        sf::RectangleShape sensor;
        sensor.setSize(sensorSizes[i]);
        sensor.setPosition(sensorPositions[i]);
        sensor.setFillColor(sf::Color(30, 30, 30));
        window.draw(sensor);
    }

    //draw balls
    for(int i = 0; i < BALLS_COUNT; ++i)
    {
        balls[i].draw(window);
    }
}

float randFloat()
{
    return float(rand()) / float(RAND_MAX);
}
