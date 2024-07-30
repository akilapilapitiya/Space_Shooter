//Library Imports
#include <SFML/Graphics.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <sstream>
#include <iostream>
#include <fstream>

//Classs for the Game
class AsteroidDodger {
public:
	
//	Game Window Configuration
    AsteroidDodger()
        : window(sf::VideoMode(800, 600), "Asteroid Dodger"),	 //Windows Size Configuration
		  playerSpeed(250.f), asteroidSpeed(150.f),				 //Spaceship and Asteriod Movement Speed 
          spawnInterval(0.8f), 									 //Asteroid Spwan Speed
		  score(0), 											 //Reset Score to '0' at every Restart 
		  gameOver(false),										 //Initialize Gamer over boolen value to 'false' 
		  inMenu(true), 										 //Calling the in game main Menue
		   multiplayerMode(false) {								 //Initialize multiplayer mode boolen value to 'false'
        window.setFramerateLimit(30); 							 //Set In-game FrameRate


//		FILE IMPORTS and AVAILABILITY check
        if (!font.loadFromFile("Assets\\Font\\arial.ttf")) { 						//Load Ingame Fonts
            std::cerr << "Error loading font\n";
        }

        if (!playerTexture.loadFromFile("Assets\\Spaceship\\spaceship.png")) {    	//Load the Spaceship
            std::cerr << "Error loading spaceship texture\n";
        }

        if (!asteroidTexture.loadFromFile("Assets\\Asteroid\\asteroid.png")) {		//Load the Asteroid
            std::cerr << "Error loading asteroid texture\n";
        }

        setupPlayers(); // Set up initial player positions
        setupViews();   // Set up views for split screen


        scoreText.setFont(font);
        scoreText.setCharacterSize(20);
        scoreText.setFillColor(sf::Color::White);
        scoreText.setPosition(10.f, 10.f);

		//GAME OVER Display
        gameOverText.setFont(font);
        gameOverText.setCharacterSize(50);
        gameOverText.setFillColor(sf::Color::Red);
        gameOverText.setPosition(200.f, 250.f);
        gameOverText.setString("Game Over!");

        menuText.setFont(font);
        menuText.setCharacterSize(30);
        menuText.setFillColor(sf::Color::White);
        menuText.setPosition(150.f, 150.f);
        menuText.setString("========== Asteroid Dodger ==========\n1. Display Player Details with Scores\n2. Single Player Mode\n3. Multiplayer Mode\n4. Exit");

        srand(static_cast<unsigned>(time(NULL)));
    }

    void run() {
        sf::Clock clock;
        sf::Time spawnTime = sf::Time::Zero;

        while (window.isOpen()) {
            sf::Time dt = clock.restart();
            spawnTime += dt;

            processEvents(dt.asSeconds());
            if (!inMenu && !gameOver) {
                if (spawnTime.asSeconds() >= spawnInterval) {
                    spawnAsteroid();
                    spawnTime = sf::Time::Zero;
                }
                update(dt.asSeconds());
            }
            render();
        }
    }

private:
    sf::RenderWindow window;
    sf::Sprite player1;
    sf::Sprite player2;
    sf::Texture playerTexture;
    std::vector<sf::Sprite> asteroids;
    sf::Texture asteroidTexture;
    float playerSpeed;
    float asteroidSpeed;
    float spawnInterval;
    float score;
    bool gameOver;
    bool inMenu;
    bool multiplayerMode;
    sf::Font font;
    sf::Text scoreText;
    sf::Text gameOverText;
    sf::Text menuText;
    std::string playerName1;
    std::string playerName2;
    sf::View view1;
    sf::View view2;

    void setupPlayers() {
        player1.setTexture(playerTexture);
        player1.setScale(0.3f, 0.3f); // Scale down the player sprite
        player2.setTexture(playerTexture);
        player2.setScale(0.3f, 0.3f); // Scale down the player sprite
        resetPlayers(); // Initialize players to default positions
    }

    void resetPlayers() {
        player1.setPosition(300.f, 500.f);
        player2.setPosition(500.f, 500.f); // Reset second player to top-right
    }

    void setupViews() {
        // Setup views for split screen
        view1.setSize(400, 500);
        view1.setCenter(300, 400);
        view1.setViewport(sf::FloatRect(0.f, 0.1f, 0.5f, 1.f));

        view2.setSize(400, 500);
        view2.setCenter(600, 400);
        view2.setViewport(sf::FloatRect(0.5f, 0.1f, 0.5f, 1.f));
    }

    void processEvents(float dt) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (inMenu) {
                if (event.type == sf::Event::KeyPressed) {
                    if (event.key.code == sf::Keyboard::Num1) {
                        displayPlayerDetails();
                    } else if (event.key.code == sf::Keyboard::Num2) {
                        std::cout << "Enter your name (Player 1): ";
                        std::cin >> playerName1;
                        inMenu = false;
                        score = 0;
                        gameOver = false;
                        asteroids.clear();
                        resetPlayers(); // Reset players on game start
                    } else if (event.key.code == sf::Keyboard::Num3) {
                        std::cout << "Enter your name (Player 1): ";
                        std::cin >> playerName1;
                        std::cout << "Enter your name (Player 2): ";
                        std::cin >> playerName2;
                        inMenu = false;
                        multiplayerMode = true;
                        score = 0;
                        gameOver = false;
                        asteroids.clear();
                        resetPlayers(); // Reset players on game start
                    } else if (event.key.code == sf::Keyboard::Num4) {
                        window.close();
                    }
                }
            } else {
                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Num1) {
                    inMenu = true;
                    multiplayerMode = false;
                }
            }

            if (!inMenu && !multiplayerMode) {
                handlePlayerInput(player1, sf::Keyboard::Left, sf::Keyboard::Right, sf::Keyboard::Up, sf::Keyboard::Down, dt);
            } else if (!inMenu && multiplayerMode) {
                handlePlayerInput(player1, sf::Keyboard::A, sf::Keyboard::D, sf::Keyboard::W, sf::Keyboard::S, dt);
                handlePlayerInput(player2, sf::Keyboard::Left, sf::Keyboard::Right, sf::Keyboard::Up, sf::Keyboard::Down, dt);
            }
        }
    }

    void handlePlayerInput(sf::Sprite& player, sf::Keyboard::Key leftKey, sf::Keyboard::Key rightKey, sf::Keyboard::Key upKey, sf::Keyboard::Key downKey, float dt) {
        if (sf::Keyboard::isKeyPressed(leftKey)) {
            player.move(-playerSpeed * dt, 0.f);
            if (player.getPosition().x < 0.f) {
                player.setPosition(0.f, player.getPosition().y);
            }
        }
        if (sf::Keyboard::isKeyPressed(rightKey)) {
            player.move(playerSpeed * dt, 0.f);
            if (player.getPosition().x + player.getGlobalBounds().width > window.getSize().x) {
                player.setPosition(window.getSize().x - player.getGlobalBounds().width, player.getPosition().y);
            }
        }
        if (sf::Keyboard::isKeyPressed(upKey)) {
            player.move(0.f, -playerSpeed * dt);
            if (player.getPosition().y < 0.f) {
                player.setPosition(player.getPosition().x, 0.f);
            }
        }
        if (sf::Keyboard::isKeyPressed(downKey)) {
            player.move(0.f, playerSpeed * dt);
            if (player.getPosition().y + player.getGlobalBounds().height > window.getSize().y) {
                player.setPosition(player.getPosition().x, window.getSize().y - player.getGlobalBounds().height);
            }
        }
    }

    void update(float dt) {
        score += dt;
        asteroidSpeed += dt * 10.f; // Increase speed over time

        // Update player 1
        for (std::vector<sf::Sprite>::iterator it = asteroids.begin(); it != asteroids.end(); ) {
            it->move(0.f, asteroidSpeed * dt);

            if (it->getGlobalBounds().intersects(player1.getGlobalBounds())) {
                gameOver = true;
                saveScore(playerName1, static_cast<int>(score));
            }

            if (it->getPosition().y > window.getSize().y) {
                it = asteroids.erase(it);
            } else {
                ++it;
            }
        }

        // Update player 2 (if multiplayer mode)
        if (multiplayerMode) {
            for (std::vector<sf::Sprite>::iterator it = asteroids.begin(); it != asteroids.end(); ) {
                it->move(0.f, asteroidSpeed * dt);

                if (it->getGlobalBounds().intersects(player2.getGlobalBounds())) {
                    gameOver = true;
                    saveScore(playerName2, static_cast<int>(score));
                }

                if (it->getPosition().y > window.getSize().y) {
                    it = asteroids.erase(it);
                } else {
                    ++it;
                }
            }
        }

        std::ostringstream ss;
        ss << "Score: " << static_cast<int>(score);
        scoreText.setString(ss.str());
    }

    void render() {
        window.clear();

        if (inMenu) {
            window.draw(menuText);
        } else {
            if (multiplayerMode) {
                window.setView(view1);
                window.draw(player1);
                for (std::vector<sf::Sprite>::iterator it = asteroids.begin(); it != asteroids.end(); ++it) {
                    window.draw(*it);
                }

                window.setView(view2);
                window.draw(player2);
                for (std::vector<sf::Sprite>::iterator it = asteroids.begin(); it != asteroids.end(); ++it) {
                    window.draw(*it);
                }

                // Draw white border between views
                sf::RectangleShape border(sf::Vector2f(5.f, static_cast<float>(window.getSize().y)));
                border.setFillColor(sf::Color::White);
                border.setPosition(window.getSize().x / 2.f - 2.5f, 0.f); // Center the border
                window.setView(window.getDefaultView()); // Reset to default view to draw HUD
                window.draw(border);

                window.draw(scoreText);
                if (gameOver) {
                    window.draw(gameOverText);
                }
            } else {
                window.draw(player1);
                for (std::vector<sf::Sprite>::iterator it = asteroids.begin(); it != asteroids.end(); ++it) {
                    window.draw(*it);
                }
                window.draw(scoreText);
                if (gameOver) {
                    window.draw(gameOverText);
                }
            }
        }

        window.display();
    }

    void spawnAsteroid() {
        sf::Sprite asteroid(asteroidTexture);
        asteroid.setScale(0.2f, 0.2f); // Scale down the asteroid sprite
        float x = static_cast<float>(rand() % window.getSize().x);
        asteroid.setPosition(x, -asteroid.getGlobalBounds().height);
        asteroids.push_back(asteroid);
    }

    void saveScore(const std::string& name, int score) {
        std::ofstream file("scores.txt", std::ios::app);
        if (file.is_open()) {
            file << name << " " << score << std::endl;
            file.close();
        } else {
            std::cerr << "Unable to open scores file\n";
        }
    }

    void displayPlayerDetails() {
        window.clear();
        sf::Text detailsText;
        detailsText.setFont(font);
        detailsText.setCharacterSize(20);
        detailsText.setFillColor(sf::Color::White);
        detailsText.setPosition(150.f, 150.f);

        std::ifstream file("scores.txt");
        std::string line;
        std::ostringstream detailsStream;
        detailsStream << "Player Details with Scores:\n";
        while (std::getline(file, line)) {
            detailsStream << line << "\n";
        }
        detailsText.setString(detailsStream.str());

        window.draw(detailsText);
        window.display();

        while (true) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::KeyPressed) {
                    return;
                }
                if (event.type == sf::Event::Closed) {
                    window.close();
                    return;
                }
            }
        }
    }
};

int main() {
    AsteroidDodger game;
    game.run();
    return 0;
}

