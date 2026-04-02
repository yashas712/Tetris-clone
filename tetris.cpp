#include <algorithm>
#include <ctime>
#include <iostream>

#include <SFML/Graphics.hpp>

const int M = 20;
const int N = 10;

int field[M][N] = {0};

struct Point {
    int x, y;
} a[4], b[4];

int figures[7][4] = {{1, 3, 5, 7}, {2, 4, 5, 7}, {3, 5, 4, 6}, {3, 5, 4, 7}, {2, 3, 5, 7}, {3, 5, 7, 6}, {2, 3, 4, 5}};
// I, Z, S, T, L, J, O

bool escapeButtonStatus = false;
bool isPaused = false;
unsigned score = 0; // Can only hold non negative values

bool check() {
    for (int i = 0; i < 4; ++i) {
        if (a[i].x < 0 || a[i].x >= N || a[i].y >= M) { // Going past boundaries
            return false;
        }
        if (field[a[i].y][a[i].x]) { // If that place is already occupied, then it's a collision
            return false;
        }
    }
    return true;
}

int main() {
    srand(time(nullptr));
    sf::RenderWindow window(sf::VideoMode({320, 480}), "---- Tetris ----"); // Specifies the width, height of the window
    sf::Texture t1, t2, t3;
    t1.loadFromFile("images/tiles.png");
    t2.loadFromFile("images/background.png");
    t3.loadFromFile("images/frame.png");
    sf::Sprite s(t1), background(t2), frame(t3);
    int dx = 0, colorNum = 1;
    bool rotate = 0;
    float timer = 0, delay = 0.3;
    sf::Clock clock;

    {
        const int firstFigure = rand() % 7;
        for (int i=0; i<4; i++) {
            a[i].x = figures[firstFigure][i] % 2 + N / 2 - 1;
            a[i].y = figures[firstFigure][i] / 2;
        }
    }
    while (window.isOpen()) {
        if (!isPaused && !escapeButtonStatus) { // If not paused and escape button is not pressed, keep running the timer timer
            timer += clock.restart().asSeconds();
        }
        while (const std::optional<sf::Event> event = window.pollEvent()) { // std::optional is a container that may/may not have a value
            if (event->is<sf::Event::Closed>()) { // is<...>() is a template function which checks "Are you a value of this type"?
                window.close();
            } else if (event->is<sf::Event::FocusLost>()) {
                isPaused = true;
            } else if (event->is<sf::Event::FocusGained>()) {
                isPaused = false;
            } else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                if (keyPressed->code == sf::Keyboard::Key::Escape) {
                    escapeButtonStatus = !escapeButtonStatus;
                } else if (keyPressed->code == sf::Keyboard::Key::Up) {
                    rotate = true;
                } else if (keyPressed->code == sf::Keyboard::Key::Left) {
                    dx = -1;
                } else if (keyPressed->code == sf::Keyboard::Key::Right) {
                    dx = 1;
                }
            }
        }
        if (isPaused || escapeButtonStatus) {
            sf::sleep(sf::milliseconds(15));
            continue;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)) {
            delay = 0.05;
        }
        // Move
        for (int i = 0; i < 4; i++) { b[i] = a[i]; a[i].x += dx; }
        if (!check()) { for (int i = 0; i < 4; i++) { a[i] = b[i]; } }

        // Rotate
        if (rotate) {
            Point p = a[1]; // center of rotation
            for (int i = 0; i < 4; ++i) {
                a[i].x = p.x - a[i].y + p.y;
                a[i].y = p.y + a[i].x - p.x;
            }
            if (!check()) {
                for (int i = 0; i < 4; ++i) {
                    a[i] = b[i];
                }
            }
        }
        // Timer
        if (timer>=delay)
        {
            for (int i=0;i<4;i++) { b[i]=a[i]; a[i].y+=1; }
            if (!check()) {
                for (int i=0;i<4;i++) field[b[i].y][b[i].x]=colorNum;
                colorNum=1+rand()%7;
                int n=rand()%7;
                for (int i=0;i<4;i++) {
                    a[i].x = figures[n][i] % 2 + N / 2 - 1;
                    a[i].y = figures[n][i] / 2;
                }
                if (!check())   {
                    window.close();
                    break;
                }
            }
            timer=0;
        } else {
            sf::sleep(sf::milliseconds(10));
        }
        // Line clearing logic
        int k=M-1;
        for (int i=M-1;i>0;i--) {
            int count=0;
            for (int j=0;j<N;j++) {
                if (field[i][j]) {
                    count++;
                }
                field[k][j]=field[i][j];
            }
            if (count < N) {
                k--;
            } else {
                score++;
            }
        }
        if (k != 0) { // Everything from the top to k'th row is now trash, so we fill it with 0
            std::fill(field[0], field[0] + (N * (k + 1)), 0);
        }
        dx=0; rotate=0; delay=0.3;

        window.clear(sf::Color::White);
        window.draw(background);

        // Draw the landed blocks (the field)
        for (int i = 0; i < M; i++) { // i = row
            for (int j = 0; j < N; j++) { // j = col
                if (field[i][j] == 0) continue;

                s.setTextureRect(sf::IntRect({field[i][j] * 18, 0}, {18, 18}));

                s.setPosition({static_cast<float>(j * 18), static_cast<float>(i * 18)});

                s.move({28.f, 31.f});
                window.draw(s);
            }
        }

        // Draw the active falling piece
        for (int i = 0; i < 4; i++) {
            s.setTextureRect(sf::IntRect({colorNum * 18, 0}, {18, 18}));
            s.setPosition({static_cast<float>(a[i].x * 18), static_cast<float>(a[i].y * 18)});
            s.move({28.f, 31.f});
            window.draw(s);
        }

        // Finalize the frame
        window.draw(frame);
        window.display();
    }
    std::cout << "Game Over\n";
    std::cout << "Your score : " << score << std::endl;

    return 0;
}