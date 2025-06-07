#include <graphics.h>
#include <conio.h>
#include <windows.h>

// 游戏常量
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int PLAYER_SPEED = 8;

// 玩家飞机类
class Player {
public:
    int x, y;
    int width, height;

    Player() {
        x = SCREEN_WIDTH / 2;
        y = SCREEN_HEIGHT - 100;
        width = 60;
        height = 80;
    }

    void handleInput() {
        // WASD移动
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

        // 边界检测
        if (x - width / 2 < 0) x = width / 2;
        if (x + width / 2 > SCREEN_WIDTH) x = SCREEN_WIDTH - width / 2;
        if (y - height / 2 < 0) y = height / 2;
        if (y + height / 2 > SCREEN_HEIGHT) y = SCREEN_HEIGHT - height / 2;
    }

    void draw() {
        // 绘制飞机主体
        setfillcolor(BLUE);
        fillrectangle(x - width / 2, y - height / 2, x + width / 2, y + height / 2);

        // 绘制飞机头部
        setfillcolor(LIGHTBLUE);
        fillrectangle(x - width / 4, y - height / 2, x + width / 4, y - height / 4);

        // 绘制机翼
        setfillcolor(CYAN);
        fillrectangle(x - width / 2, y - height / 6, x + width / 2, y + height / 6);
    }
};

void drawBackground() {
    // 绘制星空背景
    setcolor(WHITE);
    for (int i = 0; i < 50; i++) {
        int x = rand() % SCREEN_WIDTH;
        int y = rand() % SCREEN_HEIGHT;
        putpixel(x, y, WHITE);
    }
}

void drawUI() {
    setcolor(WHITE);
    settextcolor(WHITE);
    settextstyle(20, 0, _T("Arial"));

    outtextxy(10, 10, _T("飞机大战 v1.0"));
    outtextxy(10, 40, _T("WASD/方向键: 移动"));
    outtextxy(10, 70, _T("ESC: 退出"));
}

int main() {
    // 初始化图形窗口
    initgraph(SCREEN_WIDTH, SCREEN_HEIGHT);
    setbkcolor(BLACK);
    BeginBatchDraw();
    Player player;
    bool gameRunning = true;

    while (gameRunning) {
        // 清屏
        cleardevice();

        // 检查退出
        if (GetAsyncKeyState(VK_ESCAPE)) {
            gameRunning = false;
        }

        // 更新
        player.handleInput();

        // 绘制
        drawBackground();
        player.draw();
        drawUI();

        // 控制帧率
        Sleep(16); // 约60FPS

        EndBatchDraw();
    }

    closegraph();
    return 0;
}