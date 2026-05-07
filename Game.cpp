#include <SFML/Graphics.hpp>
#include <iostream>
#include <ctime>
#include <cmath>
#include "include/PlayerCar.h"
#include "include/EnemyCar.h"

using namespace sf;

const int WINDOW_WIDTH  = 500;
const int WINDOW_HEIGHT = 800;
const int MAX_ENEMIES   = 10;

// ================= DIFFICULTY =================
enum Difficulty { EASY, MEDIUM, HARD };

struct DifficultySettings
{
    float initialGameSpeed;      // Starting scroll / base enemy speed
    float speedIncreaseRate;     // How fast gameSpeed grows per second
    float initialSpawnDelay;     // First spawn interval (seconds)
    float minSpawnDelay;         // Spawn delay floor (seconds)
    float spawnDelayDecreaseRate;// How much spawnDelay shrinks each spawn
    float enemySpeedMinMult;     // Minimum random multiplier for enemy speed
    float enemySpeedMaxMult;     // Maximum random multiplier for enemy speed
};

DifficultySettings getDifficultySettings(Difficulty d)
{
    switch (d)
    {
    case EASY:
        return { 130.f, 1.0f, 3.0f, 1.8f, 0.005f, 0.85f, 1.05f };
    case MEDIUM:
        return { 200.f, 2.5f, 1.2f, 0.6f, 0.04f, 0.90f, 1.25f };
    case HARD:
        return { 260.f, 4.0f, 0.8f, 0.3f, 0.07f, 0.95f, 1.50f };
    }
    return { 200.f, 2.5f, 1.2f, 0.6f, 0.04f, 0.90f, 1.25f }; // fallback
}

// ================= RESET FUNCTION =================
void resetGame(EnemyCar enemies[], bool active[], float &score,
               float &gameSpeed, float &spawnTimer, float &spawnDelay,
               bool &gameOver, const DifficultySettings &settings)
{
    for (int i = 0; i < MAX_ENEMIES; i++)
        active[i] = false;

    score      = 0;
    gameSpeed  = settings.initialGameSpeed;
    spawnTimer = 0;
    spawnDelay = settings.initialSpawnDelay;
    gameOver   = false;
}

// ================= SPAWN FUNCTION =================
// Each spawned enemy gets a randomized speed within the difficulty's multiplier range.
// Same-lane enemies always get a speed <= the slowest car already ahead of them,
// so faster cars can never catch and overlap a slower one.
void spawnEnemy(EnemyCar enemies[], bool active[], float laneX[],
                Texture textures[], float gameSpeed,
                const DifficultySettings &settings)
{
    // Lane width used for same-lane matching (half of WINDOW_WIDTH/3)
    const float LANE_HALF_WIDTH = (500.f / 3.f) * 0.5f;
    // Minimum Y gap between two cars in the same lane (car height ~120 + padding)
    const float MIN_Y_GAP = 180.f;

    for (int attempt = 0; attempt < 3; attempt++)
    {
        int   lane   = rand() % 3;
        float spawnX = laneX[lane];
        float spawnY = -(150 + rand() % 200);   // tighter range so gap check is meaningful

        bool  safe        = true;
        float maxAllowedSpeed = gameSpeed * settings.enemySpeedMaxMult; // will be clamped below

        for (int j = 0; j < MAX_ENEMIES; j++)
        {
            if (!active[j]) continue;

            float dx = std::abs(enemies[j].getPosition().x - spawnX);
            if (dx < LANE_HALF_WIDTH)   // same lane
            {
                float dy = std::abs(enemies[j].getPosition().y - spawnY);

                // If too close vertically, this spawn position is unsafe
                if (dy < MIN_Y_GAP)
                {
                    safe = false;
                    break;
                }

                // If the existing car is ahead (lower Y = further up screen, will
                // appear below new spawn since new spawn starts above screen),
                // the new car must not be faster than the car in front of it.
                // "In front" here means: the existing car has a LARGER y (closer
                // to the player) — it spawned earlier and is further down.
                if (enemies[j].getPosition().y > spawnY)
                {
                    // Cap new car's speed so it can never catch the car ahead
                    maxAllowedSpeed = std::min(maxAllowedSpeed, enemies[j].getSpeed());
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

                    // Random speed within difficulty bounds, capped by lane constraint
                    float range    = settings.enemySpeedMaxMult - settings.enemySpeedMinMult;
                    float randMult = settings.enemySpeedMinMult +
                                     (rand() % 1000) / 1000.f * range;
                    float enemySpd = std::min(gameSpeed * randMult, maxAllowedSpeed);
                    // Never go below the minimum multiplier speed
                    enemySpd = std::max(enemySpd, gameSpeed * settings.enemySpeedMinMult);

                    enemies[i] = EnemyCar(textures[texIdx], spawnX, spawnY,
                                          enemySpd, WINDOW_HEIGHT);
                    active[i]  = true;
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
    float laneX[3]  = { laneWidth / 2, laneWidth * 1.5f, laneWidth * 2.5f };

    // ================= PLAYER =================
    Texture playerCarTexture;
    playerCarTexture.loadFromFile("Assets/graphics/WhiteCar.png");
    PlayerCar player(playerCarTexture, laneX[1], WINDOW_HEIGHT - 170, 300, 0, WINDOW_WIDTH - 100);

    // ================= ENEMIES =================
    EnemyCar enemies[MAX_ENEMIES];
    Texture  textures[5];
    textures[0].loadFromFile("Assets/graphics/RedCar1.png");
    textures[1].loadFromFile("Assets/graphics/RedCar2.png");
    textures[2].loadFromFile("Assets/graphics/YellowCar1.png");
    textures[3].loadFromFile("Assets/graphics/YellowCar2.png");
    textures[4].loadFromFile("Assets/graphics/YellowCar3.png");

    bool active[MAX_ENEMIES] = {false};

    // ================= GAME STATE =================
    float spawnTimer = 0, spawnDelay = 1.0f;
    bool  gameOver = false, gameStarted = false;
    float score = 0, gameSpeed = 200;
    float blinkTimer = 0;
    bool  blinkVisible = true;

    // Difficulty selection state
    Difficulty      selectedDifficulty = MEDIUM;
    DifficultySettings diffSettings    = getDifficultySettings(selectedDifficulty);
    bool            difficultyChosen   = false;   // true once player picks a level

    // ================= UI =================
    Font font;
    font.loadFromFile("Assets/fonts/KOMIKAP_.ttf");

    // Score / speed HUD
    Text scoreText, speedText;
    scoreText.setFont(font); scoreText.setCharacterSize(24); scoreText.setPosition(20, 10);
    speedText.setFont(font); speedText.setCharacterSize(24); speedText.setPosition(WINDOW_WIDTH - 150, 10);

    // "Press ENTER to Start"
    Text pressEnterText;
    pressEnterText.setFont(font);
    pressEnterText.setCharacterSize(26);
    pressEnterText.setFillColor(Color::White);
    pressEnterText.setString("Press ENTER to Start");
    FloatRect peRect = pressEnterText.getLocalBounds();
    pressEnterText.setOrigin(peRect.width / 2, peRect.height / 2);
    pressEnterText.setPosition(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 + 80);

    // Difficulty menu labels
    Text diffTitle;
    diffTitle.setFont(font); diffTitle.setCharacterSize(30);
    diffTitle.setFillColor(Color::White);
    diffTitle.setString("Select Difficulty");
    FloatRect dtRect = diffTitle.getLocalBounds();
    diffTitle.setOrigin(dtRect.width / 2, dtRect.height / 2);
    diffTitle.setPosition(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 - 120);

    // Three difficulty option texts
    const char* diffLabels[3] = { "[1] Easy", "[2] Medium", "[3] Hard" };
    Color        diffColors[3] = { Color(80, 200, 80), Color(240, 200, 40), Color(220, 60, 60) };
    Text         diffTexts[3];
    for (int i = 0; i < 3; i++)
    {
        diffTexts[i].setFont(font);
        diffTexts[i].setCharacterSize(26);
        diffTexts[i].setString(diffLabels[i]);
        FloatRect r = diffTexts[i].getLocalBounds();
        diffTexts[i].setOrigin(r.width / 2, r.height / 2);
        diffTexts[i].setPosition(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 - 40 + i * 50);
    }

    // Active difficulty indicator shown during gameplay
    Text diffBadge;
    diffBadge.setFont(font); diffBadge.setCharacterSize(18);
    diffBadge.setPosition(20, WINDOW_HEIGHT - 30);

    // Game-over texts
    Text gameOverText;
    gameOverText.setFont(font); gameOverText.setCharacterSize(48);
    gameOverText.setFillColor(Color::Red);
    gameOverText.setString("GAME OVER");
    FloatRect goRect = gameOverText.getLocalBounds();
    gameOverText.setOrigin(goRect.width / 2, goRect.height / 2);
    gameOverText.setPosition(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 - 50);

    Text restartText;
    restartText.setFont(font); restartText.setCharacterSize(24);
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

            // Difficulty selection (key pressed once)
            if (!difficultyChosen && event.type == Event::KeyPressed)
            {
                if (event.key.code == Keyboard::Num1) { selectedDifficulty = EASY;   difficultyChosen = true; }
                if (event.key.code == Keyboard::Num2) { selectedDifficulty = MEDIUM; difficultyChosen = true; }
                if (event.key.code == Keyboard::Num3) { selectedDifficulty = HARD;   difficultyChosen = true; }

                if (difficultyChosen)
                {
                    diffSettings = getDifficultySettings(selectedDifficulty);
                    gameSpeed    = diffSettings.initialGameSpeed;
                    spawnDelay   = diffSettings.initialSpawnDelay;

                    const char* badgeStr = (selectedDifficulty == EASY)   ? "Easy"   :
                                           (selectedDifficulty == MEDIUM)  ? "Medium" : "Hard";
                    Color badgeColor     = diffColors[(int)selectedDifficulty];
                    diffBadge.setString(std::string("Difficulty: ") + badgeStr);
                    diffBadge.setFillColor(badgeColor);
                }
            }
        }

        // Start game after difficulty is chosen
        if (difficultyChosen && !gameStarted && Keyboard::isKeyPressed(Keyboard::Return))
        {
            gameStarted = true;
            clock.restart();
        }

        // Restart after game over
        if (gameOver && Keyboard::isKeyPressed(Keyboard::R))
        {
            resetGame(enemies, active, score, gameSpeed, spawnTimer, spawnDelay, gameOver, diffSettings);
            player.setPosition(laneX[1], WINDOW_HEIGHT - 170);
        }

        // ---- UPDATE ----
        if (gameStarted && !gameOver)
        {
            score     += gameSpeed * dt * 0.005f;
            gameSpeed += dt * diffSettings.speedIncreaseRate;

            scoreText.setString("Score: " + std::to_string((int)score));
            speedText.setString("Speed: " + std::to_string((int)gameSpeed / 10));

            float dirX = 0;
            if (Keyboard::isKeyPressed(Keyboard::Left))  dirX = -1;
            if (Keyboard::isKeyPressed(Keyboard::Right)) dirX =  1;

            player.update(dirX, 0, dt);
            player.enforceBounds();

            // Runtime overtake prevention:
            // If a faster car is close behind a slower car in the same lane,
            // cap its speed to the slower car's speed to prevent overlap.
            const float LANE_HALF_W    = (500.f / 3.f) * 0.5f;
            const float MIN_FOLLOW_GAP = 160.f;

            for (int i = 0; i < MAX_ENEMIES; i++)
            {
                if (!active[i]) continue;

                float effectiveSpeed = enemies[i].getSpeed();

                for (int j = 0; j < MAX_ENEMIES; j++)
                {
                    if (i == j || !active[j]) continue;

                    float dx = std::abs(enemies[i].getPosition().x - enemies[j].getPosition().x);
                    if (dx >= LANE_HALF_W) continue;   // different lane

                    float yi = enemies[i].getPosition().y;
                    float yj = enemies[j].getPosition().y;
                    if (yj <= yi) continue;            // j is not ahead of i

                    float gap = yj - yi;
                    if (gap < MIN_FOLLOW_GAP)
                        effectiveSpeed = std::min(effectiveSpeed, enemies[j].getSpeed());
                }

                enemies[i].setSpeed(effectiveSpeed);
                enemies[i].update(dt);
                enemies[i].enforceBounds(0, WINDOW_WIDTH - 100);
                if (enemies[i].isOffScreen()) active[i] = false;
            }

            // Spawn logic — density increases by shrinking spawnDelay over time
            spawnTimer += dt;
            if (spawnTimer >= spawnDelay)
            {
                spawnEnemy(enemies, active, laneX, textures, gameSpeed, diffSettings);
                spawnTimer = 0;

                // Gradually reduce spawn delay down to the difficulty floor
                float nextDelay = 0.5f + (rand() % 120) / 100.0f
                                  - diffSettings.spawnDelayDecreaseRate * (score / 10.f);
                spawnDelay = std::max(diffSettings.minSpawnDelay, nextDelay);
            }

            // Collision detection (shrunk hitboxes)
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
            if (offset > 100) offset = 0;
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
        if (blinkTimer >= 0.5f) { blinkVisible = !blinkVisible; blinkTimer = 0; }

        // ---- DRAW ----
        if (!difficultyChosen)
        {
            // Difficulty selection screen
            window.clear(Color::Black);
            window.draw(diffTitle);
            for (int i = 0; i < 3; i++)
            {
                diffTexts[i].setFillColor(diffColors[i]);
                window.draw(diffTexts[i]);
            }
            window.display();
        }
        else if (!gameStarted)
        {
            window.clear(Color::Black);
            if (blinkVisible) window.draw(pressEnterText);
            window.draw(diffBadge);
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
                if (active[i]) window.draw(enemies[i].getSprite());

            window.draw(player.getSprite());
            window.draw(scoreText);
            window.draw(speedText);
            window.draw(diffBadge);

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