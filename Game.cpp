#include <SFML/Graphics.hpp>
#include <iostream>
#include <ctime>
#include <cmath>
#include "include/PlayerCar.h"
#include "include/EnemyCar.h"

using namespace sf;

const int WINDOW_WIDTH = 500;
const int WINDOW_HEIGHT = 800;
const int MAX_ENEMIES = 10;

// ================= RESET FUNCTION =================
void resetGame(EnemyCar enemies[], bool active[], float &score, float &gameSpeed, float &spawnTimer, float &spawnDelay, bool &gameOver)
{
    for (int i = 0; i < MAX_ENEMIES; i++)
        active[i] = false;

    score = 0;
    gameSpeed = 200;
    spawnTimer = 0;
    spawnDelay = 1.0f;
    gameOver = false;
}

// ================= SPAWN FUNCTION =================
void spawnEnemy(EnemyCar enemies[], bool active[], float laneX[], Texture textures[], float gameSpeed)
{
    for (int attempt = 0; attempt < 3; attempt++)
    {
        int lane = rand() % 3;

        float spawnY = -(150 + rand() % 300);
        float spawnX = laneX[lane];

        bool safe = true;

        for (int j = 0; j < MAX_ENEMIES; j++)
        {
            if (active[j])
            {
                float dx = std::abs(enemies[j].getPosition().x - spawnX);

                if (dx < 20)
                {
                    float dy = std::abs(enemies[j].getPosition().y - spawnY);
                    float minGap = enemies[j].getBounds().height + 60;

                    if (dy < minGap)
                    {
                        safe = false;
                        break;
                    }
                }
            }
        }

        if (safe)
        {
            for (int i = 0; i < MAX_ENEMIES; i++)
            {
                if (!active[i])
                {
                    int texIdx = rand() % 5;

                    enemies[i] = EnemyCar(
                        textures[texIdx],
                        spawnX,
                        spawnY,
                        gameSpeed,
                        WINDOW_HEIGHT);

                    active[i] = true;
                    return;
                }
            }
        }
    }
}

int main()
{
    srand(time(0));

    RenderWindow window(VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Car Dodging Game");
    Clock clock;

    // ================= ROAD =================
    RectangleShape road(Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT));
    road.setFillColor(Color(50, 50, 50));

    RectangleShape line(Vector2f(10, 80));
    line.setFillColor(Color::White);

    float offset = 0;

    // ================= LANES =================
    float laneWidth = WINDOW_WIDTH / 3.0f;
    float laneX[3] = {
        laneWidth / 2,
        laneWidth * 1.5f,
        laneWidth * 2.5f};

    // ================= PLAYER =================
    Texture playerCarTexture;
    playerCarTexture.loadFromFile("Assets/graphics/WhiteCar.png");

    PlayerCar player(playerCarTexture, laneX[1], WINDOW_HEIGHT - 170, 300, 0, WINDOW_WIDTH - 100);

    // ================= ENEMIES =================
    EnemyCar enemies[MAX_ENEMIES];
    Texture textures[5];

    textures[0].loadFromFile("Assets/graphics/RedCar1.png");
    textures[1].loadFromFile("Assets/graphics/RedCar2.png");
    textures[2].loadFromFile("Assets/graphics/YellowCar1.png");
    textures[3].loadFromFile("Assets/graphics/YellowCar2.png");
    textures[4].loadFromFile("Assets/graphics/YellowCar3.png");

    bool active[MAX_ENEMIES] = {false};

    float spawnTimer = 0;
    float spawnDelay = 1.0f;

    bool gameOver = false;
    bool gameStarted = false;

    float score = 0;
    float gameSpeed = 200;

    float blinkTimer = 0;
    bool blinkVisible = true;

    // ================= UI =================
    Font font;
    font.loadFromFile("Assets/fonts/KOMIKAP_.ttf");

    Text scoreText, speedText;
    scoreText.setFont(font);
    speedText.setFont(font);

    scoreText.setCharacterSize(24);
    speedText.setCharacterSize(24);

    scoreText.setPosition(20, 10);
    speedText.setPosition(WINDOW_WIDTH - 150, 10);

    Text pressEnterText;
    pressEnterText.setFont(font);
    pressEnterText.setCharacterSize(26);
    pressEnterText.setFillColor(Color::White);
    pressEnterText.setString("Press ENTER to Start");
    FloatRect peRect = pressEnterText.getLocalBounds();
    pressEnterText.setOrigin(peRect.width / 2, peRect.height / 2);
    pressEnterText.setPosition(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 + 10);

    Text gameOverText;
    gameOverText.setFont(font);
    gameOverText.setCharacterSize(48);
    gameOverText.setFillColor(Color::Red);
    gameOverText.setString("GAME OVER");

    FloatRect goRect = gameOverText.getLocalBounds();
    gameOverText.setOrigin(goRect.width / 2, goRect.height / 2);
    gameOverText.setPosition(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 - 50);

    Text restartText;
    restartText.setFont(font);
    restartText.setCharacterSize(24);
    restartText.setFillColor(Color::White);

    // ================= GAME LOOP =================
    while (window.isOpen())
    {
        float dt = clock.restart().asSeconds();

        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
                window.close();
        }

        if (!gameStarted && Keyboard::isKeyPressed(Keyboard::Return))
        {
            gameStarted = true;
            clock.restart();
        }

        if (gameOver && Keyboard::isKeyPressed(Keyboard::R))
        {
            resetGame(enemies, active, score, gameSpeed, spawnTimer, spawnDelay, gameOver);
            player.setPosition(laneX[1], WINDOW_HEIGHT - 170);
        }

        if (gameStarted && !gameOver)
        {
            score += gameSpeed * dt * 0.005f;
            gameSpeed += dt * 2.5f;

            scoreText.setString("Score: " + std::to_string((int)score));
            speedText.setString("Speed: " + std::to_string((int)gameSpeed / 10));

            float dirX = 0;
            if (Keyboard::isKeyPressed(Keyboard::Left))
                dirX = -1;
            if (Keyboard::isKeyPressed(Keyboard::Right))
                dirX = 1;

            player.update(dirX, 0, dt);
            player.enforceBounds();

            for (int i = 0; i < MAX_ENEMIES; i++)
            {
                if (active[i])
                {
                    enemies[i].setSpeed(gameSpeed);
                    enemies[i].update(dt);
                    enemies[i].enforceBounds(0, WINDOW_WIDTH - 100);

                    if (enemies[i].isOffScreen())
                        active[i] = false;
                }
            }

            spawnTimer += dt;
            if (spawnTimer >= spawnDelay)
            {
                spawnEnemy(enemies, active, laneX, textures, gameSpeed);
                spawnTimer = 0;
                spawnDelay = 0.8f + (rand() % 120) / 100.0f;
            }

            for (int i = 0; i < MAX_ENEMIES; i++)
            {
                FloatRect playerBox = player.getBounds();
                playerBox.left   += playerBox.width  * 0.05f;
                playerBox.width  *= 0.9f;
                playerBox.top    += playerBox.height * 0.05f;
                playerBox.height *= 0.9f;

                FloatRect enemyBox = enemies[i].getBounds();
                enemyBox.left   += enemyBox.width  * 0.05f;
                enemyBox.width  *= 0.9f;
                enemyBox.top    += enemyBox.height * 0.05f;
                enemyBox.height *= 0.9f;

                if (active[i] && playerBox.intersects(enemyBox))
                {
                    gameOver = true;
                    break;
                }
            }

            offset += gameSpeed * dt;
            if (offset > 100)
                offset = 0;
        }

        if (gameOver)
        {
            restartText.setString(
                "Final Score: " + std::to_string((int)score) +
                "\nPress R to Restart");

            FloatRect rRect = restartText.getLocalBounds();
            restartText.setOrigin(rRect.width / 2, rRect.height / 2);
            restartText.setPosition(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 + 40);
        }

        blinkTimer += dt;
        if (blinkTimer >= 0.5f)
        {
            blinkVisible = !blinkVisible;
            blinkTimer = 0;
        }

        // DRAW
        if (!gameStarted)
        {
            window.clear(Color::Black);
            if (blinkVisible)
                window.draw(pressEnterText);
            window.display();
        }
        else
        {
            window.clear();
            window.draw(road);

            float centerX = WINDOW_WIDTH / 2;
            for (int i = 0; i < 10; i++)
            {
                line.setPosition(centerX - 5, i * 100 + offset);
                window.draw(line);
            }

            for (int i = 0; i < MAX_ENEMIES; i++)
                if (active[i])
                    window.draw(enemies[i].getSprite());

            window.draw(player.getSprite());
            window.draw(scoreText);
            window.draw(speedText);

            if (gameOver)
            {
                window.draw(gameOverText);
                window.draw(restartText);
            }

            window.display();
        }
    }

    return 0;
}