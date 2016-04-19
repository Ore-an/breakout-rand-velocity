//
// breakout.c
//
// Computer Science 50
// Problem Set 3
//

// standard libraries
#define _XOPEN_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Stanford Portable Library
#include <spl/gevents.h>
#include <spl/gobjects.h>
#include <spl/gwindow.h>

// height and width of game's window in pixels
#define HEIGHT 600
#define WIDTH 400

// number of rows of bricks
#define ROWS 5

// number of columns of bricks
#define COLS 10

// radius of ball in pixels
#define RADIUS 10

// lives
#define LIVES 3

// height and width of paddle
#define PHEIGHT 8
#define PWIDTH 60

// spacing of bricks
#define SPACING 3.0

// distance between top border and first row of bricks
#define FIRST_ROW_HEIGHT 30.0

// ball velocity for y
const double BALL_Y_VEL = 1;

// mid point for ball
const double X_BALL = (WIDTH - RADIUS / 2) / 2;
const double Y_BALL = (HEIGHT - RADIUS / 2) / 2;
 
// prototypes
void initBricks(GWindow window);
GOval initBall(GWindow window);
GRect initPaddle(GWindow window);
GLabel initScoreboard(GWindow window);
void updateScoreboard(GWindow window, GLabel label, int points);
GObject detectCollision(GWindow window, GOval ball);

int main(void)
{
    // seed pseudorandom number generator
    srand48(time(NULL));

    // instantiate window
    GWindow window = newGWindow(WIDTH, HEIGHT);

    // instantiate bricks
    initBricks(window);

    // instantiate ball, centered in middle of window
    GOval ball = initBall(window);

    // instantiate paddle, centered at bottom of window
    GRect paddle = initPaddle(window);

    // instantiate scoreboard, centered in middle of window, just above ball
    GLabel label = initScoreboard(window);

    // number of bricks initially
    int bricks = COLS * ROWS;

    // number of lives initially
    int lives = LIVES;

    // number of points initially
    int points = 0;

    // to not check paddle's y every time
    double yPaddle = getY(paddle);
    
        
    // move ball
    double xVelocity = drand48() * 0.1;
    double yVelocity = -BALL_Y_VEL * 0.1;
    
    // keep playing until game over
    while (lives > 0 && bricks > 0)
    {
        // check mouse event
        GEvent event = getNextEvent(MOUSE_EVENT);
        
        // If there's a movement make paddle follow x of mouse
        if (event != NULL)
        {
            if (getEventType(event) == MOUSE_MOVED)
            {
                double x = getX(event) - PWIDTH / 2;
                if (x > 0 && x < WIDTH - PWIDTH)
                {
                    setLocation(paddle, x, yPaddle);
                }
            }
        }

        // move ball, bounce it
        move(ball, xVelocity, yVelocity);
        if (getX(ball) + RADIUS >= WIDTH || getX(ball) <= 0)
        {
            xVelocity = -xVelocity;
        }
        if (getY(ball) <= 0)
        {
            yVelocity = -yVelocity;
        }
        if (getY(ball) + RADIUS >= HEIGHT)
        {
            // lose a life, reset?
            lives--;
            waitForClick();
            setLocation(ball, X_BALL, Y_BALL);
            xVelocity = drand48() * 0.1;
            yVelocity = -yVelocity;
        }
        
        GObject object = detectCollision(window, ball);

        if (object != NULL)
        {
            if (object == paddle)
            {

                // Trying random direction and velocity on hit
                xVelocity = (rand() % 2 * 2 - 1) * (drand48() * 0.1);
                yVelocity = -yVelocity;
            }
            else if (strcmp(getType(object), "GRect") == 0)
            {
                removeGWindow(window, object);
                xVelocity = (rand() % 2 * 2 - 1) * (drand48() * 0.1);
                yVelocity = -yVelocity;
                points++;
                bricks--;
                updateScoreboard(window, label, points);
            }
        }
    }

    // wait for click before exiting
    waitForClick();

    // game over
    closeGWindow(window);
    return 0;
}

/**
 * Initializes window with a grid of bricks.
 */
void initBricks(GWindow window)
{
    // initialize bricks. Do I have to name every single one of them?
    // hope I don't get too affectionate
    int bricks = COLS * ROWS;
    GRect rectArr[bricks];
    
    double x = SPACING;
    double y = FIRST_ROW_HEIGHT;

    // the width of the bricks is the space / number of columns
    // height is relative to width
    double bWidth = (WIDTH - SPACING * (COLS + 1)) / COLS;
    double bHeight = bWidth / 3;
    
    int k = 0;
    // you can change the colours for the rows here.
    // Yay! White was removed cause of BG colour
    char *colours[12] = {"BLACK", "BLUE", "CYAN", "DARK_GRAY", "GRAY", "GREEN", "LIGHT_GRAY", "MAGENTA", "ORANGE", "PINK", "RED", "YELLOW"};
    
    // creates the bricks, changes x and y adding brick's width + chosen spacing
    for (int i = 0; i < ROWS; i++)
    {
        char *colour = colours[i % 13];
        for (int j = 0; j < COLS; j++)
        {
            rectArr[k] = newGRect(x, y, bWidth, bHeight);
            setFilled(rectArr[k], true);
            setColor(rectArr[k], colour);
            add(window, rectArr[k]);
            k++;
            x = x + bWidth + SPACING;
        }
        y = y + bHeight + SPACING;
        x = SPACING;
    }
}

/**
 * Instantiates ball in center of window.  Returns ball.
 */
GOval initBall(GWindow window)
{

    // putting ball in the middle
    GOval ball = newGOval (X_BALL, Y_BALL, RADIUS, RADIUS);
    setFilled(ball, true);
    setColor(ball, "MAGENTA");
    add(window, ball);
    return ball;
}

/**
 * Instantiates paddle in bottom-middle of window.
 */
GRect initPaddle(GWindow window)
{
    double padInitX = WIDTH / 2 - PWIDTH / 2;
    double padInitY = HEIGHT - (HEIGHT / 15) - (PHEIGHT / 2);
    GRect paddle = newGRect(padInitX, padInitY, PWIDTH, PHEIGHT);
    setFilled(paddle, true);
    setColor(paddle, "BLUE");
    add(window, paddle);
    return paddle;
}

/**
 * Instantiates, configures, and returns label for scoreboard.
 */
GLabel initScoreboard(GWindow window)
{
    GLabel label = newGLabel("0");
    setFont(label, "SansSerif-40");
    add(window, label);
    return label;
}

/**
 * Updates scoreboard's label, keeping it centered in window.
 */
void updateScoreboard(GWindow window, GLabel label, int points)
{
    // update label
    char s[12];
    sprintf(s, "%i", points);
    setLabel(label, s);

    // center label in window
    double x = (getWidth(window) - getWidth(label)) / 2;
    double y = (getHeight(window) - getHeight(label)) / 2;
    setLocation(label, x, y);
}

/**
 * Detects whether ball has collided with some object in window
 * by checking the four corners of its bounding box (which are
 * outside the ball's GOval, and so the ball can't collide with
 * itself).  Returns object if so, else NULL.
 */
GObject detectCollision(GWindow window, GOval ball)
{
    // ball's location
    double x = getX(ball);
    double y = getY(ball);

    // for checking for collisions
    GObject object;

    // check for collision at ball's top-left corner
    object = getGObjectAt(window, x, y);
    if (object != NULL)
    {
        return object;
    }

    // check for collision at ball's top-right corner
    object = getGObjectAt(window, x + 2 * RADIUS, y);
    if (object != NULL)
    {
        return object;
    }

    // check for collision at ball's bottom-left corner
    object = getGObjectAt(window, x, y + 2 * RADIUS);
    if (object != NULL)
    {
        return object;
    }

    // check for collision at ball's bottom-right corner
    object = getGObjectAt(window, x + 2 * RADIUS, y + 2 * RADIUS);
    if (object != NULL)
    {
        return object;
    }

    // no collision
    return NULL;

}
