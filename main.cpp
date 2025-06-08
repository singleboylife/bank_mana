#include <graphics.h>
#include <conio.h>
#include <windows.h>
#include <vector>
#include <algorithm>

// 游戏常量
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int PLAYER_SPEED = 8;
const int BULLET_SPEED = 15;
const int ENEMY_SPEED = 3;

// 子弹类
class Bullet {
public:
    int x, y;
    bool active;

    Bullet(int startX, int startY) : x(startX), y(startY), active(true) {}

    void update() {
        y -= BULLET_SPEED;
        if (y < 0) active = false;
    }

    void draw() {
        if (!active) return;
        setfillcolor(YELLOW);
        fillrectangle(x - 2, y - 5, x + 2, y + 5);
    }
};

// 敌机类
class Enemy {
public:
    int x, y;
    int width, height;
    bool active;

    Enemy(int startX) : x(startX), y(0), width(40), height(30), active(true) {}

    void update() {
        y += ENEMY_SPEED;
        if (y > SCREEN_HEIGHT) active = false;
    }

    void draw() {
        if (!active) return;
        setfillcolor(RED);
        fillrectangle(x - width / 2, y - height / 2, x + width / 2, y + height / 2);

        // 敌机细节
        setfillcolor(DARKGRAY);
        fillrectangle(x - width / 6, y - height / 4, x + width / 6, y + height / 4);
    }
};

// 玩家飞机类
class Player {
public:
    int x, y;
    int width, height;
    std::vector<Bullet> bullets;
    int lastShotTime;

    Player() {
        x = SCREEN_WIDTH / 2;
        y = SCREEN_HEIGHT - 100;
        width = 60;
        height = 80;
        lastShotTime = 0;
    }

    void handleInput() {
        // 移动
        if (GetAsyncKeyState('A') || GetAsyncKeyState(VK_LEFT)) {
            x -= PLAYER_SPEED;
        }
        if (GetAsyncKeyState('D') || GetAsyncKeyState(VK_RIGHT)) {
            x += PLAYER_SPEED;
        }
        if (GetAsyncKeyState('W') || GetAsyncKeyState(VK_UP)) {
            y -= PLAYER_SPEED;
        }
        if (GetAsyncKeyState('S') || GetAsyncKeyState(VK_DOWN)) {
            y += PLAYER_SPEED;
        }

        // 射击
        if (GetAsyncKeyState(VK_SPACE) && lastShotTime > 10) {
            bullets.push_back(Bullet(x, y - height / 2));
            lastShotTime = 0;
        }
        lastShotTime++;

        // 边界检测
        if (x - width / 2 < 0) x = width / 2;
        if (x + width / 2 > SCREEN_WIDTH) x = SCREEN_WIDTH - width / 2;
        if (y - height / 2 < 0) y = height / 2;
        if (y + height / 2 > SCREEN_HEIGHT) y = SCREEN_HEIGHT - height / 2;
    }

    void update() {
        // 更新子弹
        for (auto& bullet : bullets) {
            bullet.update();
        }

        // 移除无效子弹
        bullets.erase(
            std::remove_if(bullets.begin(), bullets.end(),
                [](const Bullet& b) { return !b.active; }),
            bullets.end()
        );
    }

    void draw() {
        // 绘制飞机
        setfillcolor(BLUE);
        fillrectangle(x - width / 2, y - height / 2, x + width / 2, y + height / 2);

        setfillcolor(LIGHTBLUE);
        fillrectangle(x - width / 4, y - height / 2, x + width / 4, y - height / 4);

        setfillcolor(CYAN);
        fillrectangle(x - width / 2, y - height / 6, x + width / 2, y + height / 6);

        // 绘制子弹
        for (auto& bullet : bullets) {
            bullet.draw();
        }
    }
};

// 游戏管理类
class Game {
public:
    Player player;
    std::vector<Enemy> enemies;
    int score;
    int enemySpawnTimer;

    Game() : score(0), enemySpawnTimer(0) {}

    void spawnEnemy() {
        if (enemySpawnTimer > 60) { // 每60帧生成一个敌机
            int x = rand() % (SCREEN_WIDTH - 80) + 40;
            enemies.push_back(Enemy(x));
            enemySpawnTimer = 0;
        }
        enemySpawnTimer++;
    }

    void checkCollisions() {
        // 子弹击中敌机
        for (auto& bullet : player.bullets) {
            if (!bullet.active) continue;

            for (auto& enemy : enemies) {
                if (!enemy.active) continue;

                // 简单碰撞检测
                if (abs(bullet.x - enemy.x) < enemy.width / 2 &&
                    abs(bullet.y - enemy.y) < enemy.height / 2) {
                    bullet.active = false;
                    enemy.active = false;
                    score += 10;
                }
            }
        }

        // 玩家撞击敌机（简单提示）
        for (auto& enemy : enemies) {
            if (!enemy.active) continue;

            if (abs(player.x - enemy.x) < (player.width + enemy.width) / 2 &&
                abs(player.y - enemy.y) < (player.height + enemy.height) / 2) {
                enemy.active = false;
                // 这里可以添加生命系统
            }
        }
    }

    void update() {
        player.handleInput();
        player.update();

        spawnEnemy();

        // 更新敌机
        for (auto& enemy : enemies) {
            enemy.update();
        }
<<<<<<< fixv2

        // 移除无效敌机
        enemies.erase(
            std::remove_if(enemies.begin(), enemies.end(),
                [](const Enemy& e) { return !e.active; }),
            enemies.end()
        );

=======

        // 移除无效敌机
        enemies.erase(
            std::remove_if(enemies.begin(), enemies.end(),
                [](const Enemy& e) { return !e.active; }),
            enemies.end()
        );

>>>>>>> local
        checkCollisions();
    }

    void draw() {
        // 绘制星空背景
        setcolor(WHITE);
        for (int i = 0; i < 50; i++) {
            int x = rand() % SCREEN_WIDTH;
            int y = rand() % SCREEN_HEIGHT;
            putpixel(x, y, WHITE);
        }

        // 绘制游戏对象
        player.draw();

        for (auto& enemy : enemies) {
            enemy.draw();
        }

        // 绘制UI
        setcolor(WHITE);
        settextcolor(WHITE);
        settextstyle(20, 0, _T("Arial"));

        wchar_t scoreText[50];
        swprintf(scoreText, 50, L"得分: %d", score);
        outtextxy(10, 10, scoreText);

        outtextxy(10, 40, _T("WASD: 移动"));
        outtextxy(10, 70, _T("空格: 射击"));
        outtextxy(10, 100, _T("ESC: 退出"));
    }
};

int main() {
    initgraph(SCREEN_WIDTH, SCREEN_HEIGHT);
    setbkcolor(BLACK);
    srand(GetTickCount());
    BeginBatchDraw();
    Game game;
    bool gameRunning = true;

    while (gameRunning) {
        cleardevice();

        if (GetAsyncKeyState(VK_ESCAPE)) {
            gameRunning = false;
        }

        game.update();
        game.draw();
<<<<<<< fixv2

=======
        FlushBatchDraw();
>>>>>>> local
        Sleep(16);
    }
    EndBatchDraw();
    closegraph();
    return 0;
}