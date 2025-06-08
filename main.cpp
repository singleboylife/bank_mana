#include <graphics.h>
#include <conio.h>
#include <windows.h>
#include <vector>
#include <algorithm>
#include <string>
#include <cmath>

// 游戏常量
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int PLAYER_SPEED = 8;
const int BULLET_SPEED = 15;

// 游戏状态枚举
enum GameState {
    MENU,
    PLAYING,
    PAUSED,
    GAME_OVER
};

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
        fillellipse(x - 3, y - 8, x + 3, y + 8);
        setfillcolor(WHITE);
        fillellipse(x - 1, y - 6, x + 1, y + 6);
    }
};

// 敌机类
class Enemy {
public:
    int x, y;
    int width, height;
    int speed;
    int type;
    bool active;

    Enemy(int startX, int enemyType = 1) : x(startX), y(0), type(enemyType), active(true) {
        switch (type) {
        case 1: width = 40; height = 30; speed = 3; break;
        case 2: width = 60; height = 45; speed = 2; break;
        case 3: width = 80; height = 60; speed = 1; break;
        default: width = 40; height = 30; speed = 3;
        }
    }

    void update() {
        y += speed;
        if (y > SCREEN_HEIGHT) active = false;
    }

    void draw() {
        if (!active) return;

        COLORREF color = (type == 1) ? RED : (type == 2) ? MAGENTA : DARKGRAY;

        // 敌机主体
        setfillcolor(color);
        fillrectangle(x - width / 2, y - height / 2, x + width / 2, y + height / 2);

        // 敌机驾驶舱
        setfillcolor(BLACK);
        fillellipse(x - width / 6, y - height / 4, x + width / 6, y + height / 4);

        // 敌机机翼
        setfillcolor(color);
        fillrectangle(x - width / 2 - 5, y, x - width / 2, y + height / 4);
        fillrectangle(x + width / 2, y, x + width / 2 + 5, y + height / 4);
    }

    int getScore() const { return type * 10; }
};

// 玩家飞机类
class Player {
public:
    int x, y;
    int width, height;
    int health;
    std::vector<Bullet> bullets;
    int lastShotTime;

    Player() {
        reset();
    }

    void reset() {
        x = SCREEN_WIDTH / 2;
        y = SCREEN_HEIGHT - 100;
        width = 60;
        height = 80;
        health = 3;
        bullets.clear();
        lastShotTime = 0;
    }

    void handleInput() {
        if (GetAsyncKeyState('A') || GetAsyncKeyState(VK_LEFT)) x -= PLAYER_SPEED;
        if (GetAsyncKeyState('D') || GetAsyncKeyState(VK_RIGHT)) x += PLAYER_SPEED;
        if (GetAsyncKeyState('W') || GetAsyncKeyState(VK_UP)) y -= PLAYER_SPEED;
        if (GetAsyncKeyState('S') || GetAsyncKeyState(VK_DOWN)) y += PLAYER_SPEED;

        if (GetAsyncKeyState(VK_SPACE) && lastShotTime > 8) {
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
        for (auto& bullet : bullets) bullet.update();
        bullets.erase(std::remove_if(bullets.begin(), bullets.end(),
            [](const Bullet& b) { return !b.active; }), bullets.end());
    }

    void takeDamage() {
        health--;
    }

    bool isAlive() const { return health > 0; }

    void draw() {
        // 飞机主体 - 蓝色机身
        setfillcolor(RGB(0, 100, 255));
        fillrectangle(x - width / 3, y - height / 2, x + width / 3, y + height / 2);

        // 飞机头部 - 尖锐的前端
        setfillcolor(RGB(100, 150, 255));
        POINT triangle[3] = {
            {x, y - height / 2 - 15},  // 顶点
            {x - width / 3, y - height / 2},  // 左下
            {x + width / 3, y - height / 2}   // 右下
        };
        fillpolygon(triangle, 3);

        // 主机翼 - 较大的翅膀
        setfillcolor(RGB(0, 150, 200));
        fillrectangle(x - width / 2 - 10, y - height / 6, x + width / 2 + 10, y + height / 6);

        // 副机翼 - 较小的后翼
        setfillcolor(RGB(0, 120, 180));
        fillrectangle(x - width / 4, y + height / 3, x + width / 4, y + height / 2);

        // 驾驶舱 - 中央亮部
        setfillcolor(RGB(200, 220, 255));
        fillellipse(x - width / 6, y - height / 4, x + width / 6, y + height / 4);

        // 发动机尾焰效果
        if (lastShotTime % 4 < 2) {  // 闪烁效果
            setfillcolor(RGB(255, 100, 0));
            fillellipse(x - 8, y + height / 2, x + 8, y + height / 2 + 12);
            setfillcolor(RGB(255, 200, 0));
            fillellipse(x - 4, y + height / 2 + 3, x + 4, y + height / 2 + 8);
        }

        // 绘制子弹
        for (auto& bullet : bullets) bullet.draw();

        // 生命值指示器（在飞机周围显示红色护盾）
        if (health <= 1) {
            setcolor(RED);
            setlinestyle(PS_SOLID, 2);
            circle(x, y, width / 2 + 5);
        }
    }
};

// 游戏管理类
class Game {
public:
    GameState state;
    Player player;
    std::vector<Enemy> enemies;
    int score;
    int level;
    int enemySpawnTimer;
    int enemySpawnRate;
    bool keyPressed;
    int frameCount;

    Game() : state(MENU), score(0), level(1), enemySpawnTimer(0),
        enemySpawnRate(60), keyPressed(false), frameCount(0) {}

    void handleInput() {
        bool anyKeyPressed = (GetAsyncKeyState(VK_RETURN) || GetAsyncKeyState(VK_ESCAPE) || GetAsyncKeyState('Q'));

        if (anyKeyPressed && !keyPressed) {
            if (GetAsyncKeyState(VK_RETURN)) {
                if (state == MENU) {
                    resetGame();
                    state = PLAYING;
                }
                else if (state == GAME_OVER) {
                    state = MENU;
                }
            }

            if (GetAsyncKeyState(VK_ESCAPE)) {
                if (state == PLAYING) state = PAUSED;
                else if (state == PAUSED) state = PLAYING;
            }

            if (GetAsyncKeyState('Q')) {
                exit(0);
            }
        }

        keyPressed = anyKeyPressed;
    }

    void resetGame() {
        player.reset();
        enemies.clear();
        score = 0;
        level = 1;
        enemySpawnTimer = 0;
        enemySpawnRate = 60;
        frameCount = 0;
    }

    void spawnEnemy() {
        if (enemySpawnTimer > enemySpawnRate) {
            int x = rand() % (SCREEN_WIDTH - 80) + 40;
            int type = (rand() % 100 < 70) ? 1 : (rand() % 100 < 90) ? 2 : 3;
            enemies.push_back(Enemy(x, type));
            enemySpawnTimer = 0;
        }
        enemySpawnTimer++;
    }

    bool isColliding(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2) {
        return (abs(x1 - x2) < (w1 + w2) / 2 && abs(y1 - y2) < (h1 + h2) / 2);
    }

    void checkCollisions() {
        // 子弹击中敌机
        for (auto& bullet : player.bullets) {
            if (!bullet.active) continue;

            for (auto& enemy : enemies) {
                if (!enemy.active) continue;

                if (isColliding(bullet.x, bullet.y, 6, 16, enemy.x, enemy.y, enemy.width, enemy.height)) {
                    bullet.active = false;
                    enemy.active = false;
                    score += enemy.getScore();

                    // 爆炸效果
                    drawExplosion(enemy.x, enemy.y);
                }
            }
        }

        // 玩家撞击敌机
        for (auto& enemy : enemies) {
            if (!enemy.active) continue;

            if (isColliding(player.x, player.y, player.width, player.height,
                enemy.x, enemy.y, enemy.width, enemy.height)) {
                enemy.active = false;
                player.takeDamage();

                // 碰撞效果
                drawExplosion(enemy.x, enemy.y);
            }
        }
    }

    void drawExplosion(int x, int y) {
        // 简单的爆炸效果
        setfillcolor(RGB(255, 100, 0));
        fillcircle(x, y, 20);
        setfillcolor(RGB(255, 200, 0));
        fillcircle(x, y, 12);
        setfillcolor(RGB(255, 255, 100));
        fillcircle(x, y, 6);
    }

    void updateDifficulty() {
        int newLevel = score / 200 + 1;
        if (newLevel > level) {
            level = newLevel;
            enemySpawnRate = (std::max)(20, 60 - level * 5);
        }
    }

    void update() {
        handleInput();
        frameCount++;

        if (state != PLAYING) return;

        player.handleInput();
        player.update();

        spawnEnemy();

        for (auto& enemy : enemies) enemy.update();
        enemies.erase(std::remove_if(enemies.begin(), enemies.end(),
            [](const Enemy& e) { return !e.active; }), enemies.end());

        checkCollisions();
        updateDifficulty();

        if (!player.isAlive()) {
            state = GAME_OVER;
        }
    }

    void drawBackground() {
        // 动态星空背景
        setcolor(WHITE);
        for (int i = 0; i < 80; i++) {
            int x = (i * 137 + frameCount) % SCREEN_WIDTH;
            int y = (i * 211 + frameCount * 2) % SCREEN_HEIGHT;
            putpixel(x, y, WHITE);
        }

        // 额外的移动星星
        setcolor(RGB(150, 150, 255));
        for (int i = 0; i < 30; i++) {
            int x = (i * 97 + frameCount * 3) % SCREEN_WIDTH;
            int y = (i * 179 + frameCount) % SCREEN_HEIGHT;
            putpixel(x, y, RGB(150, 150, 255));
        }
    }

    void drawText(const std::wstring& text, int x, int y, int size = 20) {
        settextstyle(size, 0, _T("Arial"));
        settextcolor(WHITE);
        outtextxy(x, y, text.c_str());
    }

    void drawMenu() {
        // 动态背景
        drawBackground();

        // 标题动画效果
        int titleY = 150 + sin(frameCount * 0.05) * 10;
        setcolor(RGB(100, 200, 255));
        settextstyle(40, 0, _T("Arial"));
        settextcolor(RGB(100, 200, 255));
        outtextxy(SCREEN_WIDTH / 2 - 80, titleY, L"飞机大战");

        drawText(L"按 ENTER 开始游戏", SCREEN_WIDTH / 2 - 120, 250);
        drawText(L"按 Q 退出游戏", SCREEN_WIDTH / 2 - 100, 300);

        drawText(L"操作说明:", 50, 400);
        drawText(L"WASD/方向键: 移动", 50, 430);
        drawText(L"空格键: 射击", 50, 460);
        drawText(L"ESC: 暂停/继续", 50, 490);

        // 版本信息
        drawText(L"版本 3.0 - 完整功能版", SCREEN_WIDTH - 200, SCREEN_HEIGHT - 30, 16);
    }

    void drawPlaying() {
        drawBackground();

        player.draw();
        for (auto& enemy : enemies) enemy.draw();

        // UI信息背景
        setfillcolor(RGB(0, 0, 0));
        fillrectangle(0, 0, 200, 90);
        setcolor(WHITE);
        rectangle(0, 0, 200, 90);

        // UI信息
        wchar_t text[100];
        swprintf(text, 100, L"得分: %d", score);
        drawText(text, 10, 10);

        swprintf(text, 100, L"等级: %d", level);
        drawText(text, 10, 35);

        swprintf(text, 100, L"生命: %d", player.health);
        drawText(text, 10, 60);

        drawText(L"ESC: 暂停", SCREEN_WIDTH - 100, 10);
    }

    void drawPaused() {
        drawPlaying();

        // 暂停遮罩
        setfillcolor(RGB(0, 0, 0));
        setfillstyle(BS_HATCHED, HS_DIAGCROSS);
        fillrectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

        // 暂停文字背景
        setfillcolor(RGB(50, 50, 50));
        fillrectangle(SCREEN_WIDTH / 2 - 120, SCREEN_HEIGHT / 2 - 60, SCREEN_WIDTH / 2 + 120, SCREEN_HEIGHT / 2 + 60);
        setcolor(WHITE);
        rectangle(SCREEN_WIDTH / 2 - 120, SCREEN_HEIGHT / 2 - 60, SCREEN_WIDTH / 2 + 120, SCREEN_HEIGHT / 2 + 60);

        drawText(L"游戏暂停", SCREEN_WIDTH / 2 - 60, SCREEN_HEIGHT / 2 - 30, 30);
        drawText(L"按 ESC 继续", SCREEN_WIDTH / 2 - 70, SCREEN_HEIGHT / 2 + 20);
    }

    void drawGameOver() {
        drawBackground();

        drawText(L"游戏结束", SCREEN_WIDTH / 2 - 60, 200, 30);

        wchar_t text[100];
        swprintf(text, 100, L"最终得分: %d", score);
        drawText(text, SCREEN_WIDTH / 2 - 80, 280);

        swprintf(text, 100, L"达到等级: %d", level);
        drawText(text, SCREEN_WIDTH / 2 - 80, 320);

        drawText(L"按 ENTER 返回菜单", SCREEN_WIDTH / 2 - 120, 380);
        drawText(L"按 Q 退出游戏", SCREEN_WIDTH / 2 - 100, 420);

        // 显示排行榜提示
        drawText(L"恭喜！你的表现很出色！", SCREEN_WIDTH / 2 - 140, 460, 16);
    }

    void draw() {
        switch (state) {
        case MENU: drawMenu(); break;
        case PLAYING: drawPlaying(); break;
        case PAUSED: drawPaused(); break;
        case GAME_OVER: drawGameOver(); break;
        }
    }
};

int main() {
    initgraph(SCREEN_WIDTH, SCREEN_HEIGHT);
    BeginBatchDraw();
    setbkcolor(BLACK);
    srand(GetTickCount());

    Game game;

    while (true) {
        cleardevice();
        game.update();
        game.draw();
        Sleep(30);

        FlushBatchDraw();
    }
    EndBatchDraw();
    closegraph();
    return 0;
}