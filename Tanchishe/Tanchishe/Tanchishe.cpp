#include<stdio.h>
#include<time.h>
#include<windows.h>
#include<stdlib.h>
#include<string.h>
#include<conio.h>

#define U 1
#define D 2
#define L 3
#define R 4       //蛇的状态，U：上 ；D：下；L:左 R：右

typedef struct SNAKE //蛇身的一个节点
{
    int x;
    int y;
    struct SNAKE* next;
}snake;

// 用户结构体
typedef struct USER {
    char username[50];
    char password[50];
    struct USER* next;
} User;

// 游戏日志结构体
typedef struct GAMELOG {
    char username[50];
    time_t startTime;
    int duration;
    int score;
    struct GAMELOG* next;
} GameLog;

//全局变量//
int score = 0, add = 10;//总得分与每次吃食物得分。
int status, sleeptime = 200;//每次运行的时间间隔
snake* head, * food;//蛇头指针，食物指针
snake* q;//遍历蛇的时候用到的指针
int endgamestatus = 0; //游戏结束的情况，1：撞到墙；2：咬到自己；3：主动退出游戏。

// 用户和日志相关全局变量
User* userList = NULL;
GameLog* gameLogs = NULL;
char currentUser[50] = {0};
time_t gameStartTime;

//声明全部函数//
void Pos(int x, int y);
void creatMap();
void initsnake();
int biteself();
void createfood();
void cantcrosswall();
void snakemove();
void pause();
void gamecircle();
void welcometogame();
void endgame();
void gamestart();

// 新增函数声明
void registerUser();
int loginUser();
void saveUsersToFile();
void loadUsersFromFile();
void saveGameLog(const char* username, time_t startTime, int duration, int score);
void loadGameLogs();
void showGameLogs();
void displayUserInfo();

void Pos(int x, int y)//设置光标位置
{
    COORD pos;
    HANDLE hOutput;
    pos.X = x;
    pos.Y = y;
    hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleCursorPosition(hOutput, pos);
}

void creatMap()//创建地图
{
    int i;
    for (i = 0; i < 58; i += 2)//打印上下边框
    {
        Pos(i, 0);
        printf("■");
        Pos(i, 26);
        printf("■");
    }
    for (i = 1; i < 26; i++)//打印左右边框
    {
        Pos(0, i);
        printf("■");
        Pos(56, i);
        printf("■");
    }
}

void initsnake()//初始化蛇身
{
    snake* tail;
    int i;
    tail = (snake*)malloc(sizeof(snake));//从蛇尾开始，头插法，以x,y设定开始的位置//
    tail->x = 24;
    tail->y = 5;
    tail->next = NULL;
    for (i = 1; i <= 4; i++)
    {
        head = (snake*)malloc(sizeof(snake));
        head->next = tail;
        head->x = 24 + 2 * i;
        head->y = 5;
        tail = head;
    }
    while (tail != NULL)//从头到为，输出蛇身
    {
        Pos(tail->x, tail->y);
        printf("■");
        tail = tail->next;
    }
}

int biteself()//判断是否咬到了自己
{
    snake* self;
    self = head->next;
    while (self != NULL)
    {
        if (self->x == head->x && self->y == head->y)
        {
            return 1;
        }
        self = self->next;
    }
    return 0;
}

void createfood()//随机出现食物
{
    snake* food_1;
    srand((unsigned)time(NULL));
    food_1 = (snake*)malloc(sizeof(snake));
    while ((food_1->x % 2) != 0)    //保证其为偶数，使得食物能与蛇头对其
    {
        food_1->x = rand() % 52 + 2;
    }
    food_1->y = rand() % 24 + 1;
    q = head;
    while (q->next == NULL)
    {
        if (q->x == food_1->x && q->y == food_1->y) //判断蛇身是否与食物重合
        {
            free(food_1);
            createfood();
        }
        q = q->next;
    }
    Pos(food_1->x, food_1->y);
    food = food_1;
    printf("■");
}

void cantcrosswall()//不能穿墙
{
    if (head->x == 0 || head->x == 56 || head->y == 0 || head->y == 26)
    {
        endgamestatus = 1;
        endgame();
    }
}

void snakemove()//蛇前进,上U,下D,左L,右R
{
    snake* nexthead;
    cantcrosswall();

    nexthead = (snake*)malloc(sizeof(snake));
    if (status == U)
    {
        nexthead->x = head->x;
        nexthead->y = head->y - 1;
        if (nexthead->x == food->x && nexthead->y == food->y)//如果下一个有食物//
        {
            nexthead->next = head;
            head = nexthead;
            q = head;
            while (q != NULL)
            {
                Pos(q->x, q->y);
                printf("■");
                q = q->next;
            }
            score = score + add;
            createfood();
        }
        else                                               //如果没有食物//
        {
            nexthead->next = head;
            head = nexthead;
            q = head;
            while (q->next->next != NULL)
            {
                Pos(q->x, q->y);
                printf("■");
                q = q->next;
            }
            Pos(q->next->x, q->next->y);
            printf("  ");
            free(q->next);
            q->next = NULL;
        }
    }
    if (status == D)
    {
        nexthead->x = head->x;
        nexthead->y = head->y + 1;
        if (nexthead->x == food->x && nexthead->y == food->y)  //有食物
        {
            nexthead->next = head;
            head = nexthead;
            q = head;
            while (q != NULL)
            {
                Pos(q->x, q->y);
                printf("■");
                q = q->next;
            }
            score = score + add;
            createfood();
        }
        else                               //没有食物
        {
            nexthead->next = head;
            head = nexthead;
            q = head;
            while (q->next->next != NULL)
            {
                Pos(q->x, q->y);
                printf("■");
                q = q->next;
            }
            Pos(q->next->x, q->next->y);
            printf("  ");
            free(q->next);
            q->next = NULL;
        }
    }
    if (status == L)
    {
        nexthead->x = head->x - 2;
        nexthead->y = head->y;
        if (nexthead->x == food->x && nexthead->y == food->y)//有食物
        {
            nexthead->next = head;
            head = nexthead;
            q = head;
            while (q != NULL)
            {
                Pos(q->x, q->y);
                printf("■");
                q = q->next;
            }
            score = score + add;
            createfood();
        }
        else                                //没有食物
        {
            nexthead->next = head;
            head = nexthead;
            q = head;
            while (q->next->next != NULL)
            {
                Pos(q->x, q->y);
                printf("■");
                q = q->next;
            }
            Pos(q->next->x, q->next->y);
            printf("  ");
            free(q->next);
            q->next = NULL;
        }
    }
    if (status == R)
    {
        nexthead->x = head->x + 2;
        nexthead->y = head->y;
        if (nexthead->x == food->x && nexthead->y == food->y)//有食物
        {
            nexthead->next = head;
            head = nexthead;
            q = head;
            while (q != NULL)
            {
                Pos(q->x, q->y);
                printf("■");
                q = q->next;
            }
            score = score + add;
            createfood();
        }
        else                                         //没有食物
        {
            nexthead->next = head;
            head = nexthead;
            q = head;
            while (q->next->next != NULL)
            {
                Pos(q->x, q->y);
                printf("■");
                q = q->next;
            }
            Pos(q->next->x, q->next->y);
            printf("  ");
            free(q->next);
            q->next = NULL;
        }
    }
    if (biteself() == 1)       //判断是否会咬到自己
    {
        endgamestatus = 2;
        endgame();
    }
}

void pause()//暂停
{
    while (1)
    {
        Sleep(300);
        if (GetAsyncKeyState(VK_SPACE))
        {
            break;
        }
    }
}

void displayUserInfo() {
    Pos(64, 5);
    printf("当前用户: %s", currentUser);
    Pos(64, 6);
    printf("按F5显示游戏日志");
}

void gamecircle()//控制游戏
{
    displayUserInfo();
    Pos(64, 15);
    printf("不能穿墙，不能咬到自己\n");
    Pos(64, 16);
    printf("用↑.↓.←.→分别控制蛇的移动.");
    Pos(64, 17);
    printf("F1 为加速，F2 为减速\n");
    Pos(64, 18);
    printf("ESC ：退出游戏.space：暂停游戏.");
    Pos(64, 20);
    status = R;
    while (1)
    {
        Pos(64, 10);
        printf("得分：%d  ", score);
        Pos(64, 11);
        printf("每个食物得分：%d分", add);
        if (GetAsyncKeyState(VK_UP) && status != D)
        {
            status = U;
        }
        else if (GetAsyncKeyState(VK_DOWN) && status != U)
        {
            status = D;
        }
        else if (GetAsyncKeyState(VK_LEFT) && status != R)
        {
            status = L;
        }
        else if (GetAsyncKeyState(VK_RIGHT) && status != L)
        {
            status = R;
        }
        else if (GetAsyncKeyState(VK_SPACE))
        {
            pause();
        }
        else if (GetAsyncKeyState(VK_ESCAPE))
        {
            endgamestatus = 3;
            break;
        }
        else if (GetAsyncKeyState(VK_F1))
        {
            if (sleeptime >= 50)
            {
                sleeptime = sleeptime - 30;
                add = add + 2;
                if (sleeptime == 320)
                {
                    add = 2;//防止减到1之后再加回来有错
                }
            }
        }
        else if (GetAsyncKeyState(VK_F2))
        {
            if (sleeptime < 350)
            {
                sleeptime = sleeptime + 30;
                add = add - 2;
                if (sleeptime == 350)
                {
                    add = 1;  //保证最低分为1
                }
            }
        }
        else if (GetAsyncKeyState(VK_F5))
        {
            showGameLogs();
            displayUserInfo();
        }
        Sleep(sleeptime);
        snakemove();
    }
}

void welcometogame()//开始界面
{
    system("cls");
    Pos(40, 12);
    printf("欢迎来到贪食蛇游戏！");
    Pos(40, 13);
    printf("1. 注册新用户");
    Pos(40, 14);
    printf("2. 登录已有用户");
    Pos(40, 15);
    printf("3. 退出游戏");
    Pos(40, 17);
    printf("请选择(1-3): ");

    int choice;
    scanf("%d", &choice);

    switch(choice) {
        case 1:
            registerUser();
            break;
        case 2:
            if(loginUser()) {
                return;
            } else {
                welcometogame();
            }
            break;
        case 3:
            exit(0);
        default:
            printf("无效选择，请重新输入！");
            Sleep(1000);
            welcometogame();
    }
}

void endgame()//结束游戏
{
    // 记录游戏日志
    time_t endTime = time(NULL);
    int duration = (int)difftime(endTime, gameStartTime);
    saveGameLog(currentUser, gameStartTime, duration, score);

    system("cls");
    Pos(24, 12);
    if (endgamestatus == 1)
    {
        printf("对不起，您撞到墙了。游戏结束!");
    }
    else if (endgamestatus == 2)
    {
        printf("对不起，您咬到自己了。游戏结束!");
    }
    else if (endgamestatus == 3)
    {
        printf("您已经结束了游戏。");
    }
    Pos(24, 13);
    printf("您的得分是%d\n", score);
    Pos(24, 14);
    printf("游戏时长: %d秒\n", duration);
    Pos(24, 16);
    system("pause");
    exit(0);
}

void gamestart()//游戏初始化
{
    system("mode con cols=100 lines=30");
    loadUsersFromFile();
    loadGameLogs();
    welcometogame();
    gameStartTime = time(NULL);
    creatMap();
    initsnake();
    createfood();
}

// 用户注册功能
void registerUser() {
    system("cls");
    User* newUser = (User*)malloc(sizeof(User));

    Pos(40, 10);
    printf("用户注册");
    Pos(40, 12);
    printf("请输入用户名: ");
    scanf("%s", newUser->username);

    // 检查用户名是否已存在
    User* temp = userList;
    while(temp != NULL) {
        if(strcmp(temp->username, newUser->username) == 0) {
            Pos(40, 14);
            printf("用户名已存在!");
            Sleep(1000);
            free(newUser);
            registerUser();
            return;
        }
        temp = temp->next;
    }

    Pos(40, 13);
    printf("请输入密码: ");
    scanf("%s", newUser->password);

    newUser->next = userList;
    userList = newUser;

    saveUsersToFile();

    Pos(40, 15);
    printf("注册成功! 自动登录...");
    strcpy(currentUser, newUser->username);
    Sleep(1000);
    system("cls");
}

// 用户登录功能
int loginUser() {
    system("cls");
    char username[50];
    char password[50];

    Pos(40, 10);
    printf("用户登录");
    Pos(40, 12);
    printf("请输入用户名: ");
    scanf("%s", username);

    Pos(40, 13);
    printf("请输入密码: ");
    scanf("%s", password);

    User* temp = userList;
    while(temp != NULL) {
        if(strcmp(temp->username, username) == 0 && strcmp(temp->password, password) == 0) {
            strcpy(currentUser, username);
            Pos(40, 15);
            printf("登录成功!");
            Sleep(1000);
            system("cls");
            return 1;
        }
        temp = temp->next;
    }

    Pos(40, 15);
    printf("用户名或密码错误!");
    Sleep(1000);
    return 0;
}

// 保存用户数据到文件
void saveUsersToFile() {
    FILE* file = fopen("users.dat", "wb");
    if(file == NULL) return;

    User* temp = userList;
    while(temp != NULL) {
        fwrite(temp, sizeof(User), 1, file);
        temp = temp->next;
    }

    fclose(file);
}

// 从文件加载用户数据
void loadUsersFromFile() {
    FILE* file = fopen("users.dat", "rb");
    if(file == NULL) return;

    User user;
    while(fread(&user, sizeof(User), 1, file))
          {
        User* newUser = (User*)malloc(sizeof(User));
        strcpy(newUser->username, user.username);
        strcpy(newUser->password, user.password);
        newUser->next = userList;
        userList = newUser;
    }

    fclose(file);
}

// 保存游戏日志
void saveGameLog(const char* username, time_t startTime, int duration, int score) {
    FILE* file = fopen("gamelogs.dat", "ab");
    if(file == NULL) return;

    GameLog log;
    strcpy(log.username, username);
    log.startTime = startTime;
    log.duration = duration;
    log.score = score;

    fwrite(&log, sizeof(GameLog), 1, file);
    fclose(file);
}

// 加载游戏日志
void loadGameLogs() {
    FILE* file = fopen("gamelogs.dat", "rb");
    if(file == NULL) return;

    GameLog log;
    while(fread(&log, sizeof(GameLog), 1, file))
          {
        GameLog* newLog = (GameLog*)malloc(sizeof(GameLog));
        strcpy(newLog->username, log.username);
        newLog->startTime = log.startTime;
        newLog->duration = log.duration;
        newLog->score = log.score;
        newLog->next = gameLogs;
        gameLogs = newLog;
    }

    fclose(file);
}

// 显示游戏日志
void showGameLogs() {
    system("cls");
    Pos(10, 2);
    printf("游戏日志 - 用户: %s", currentUser);
    Pos(10, 4);
    printf("用户名        开始时间            持续时间(秒)  得分");

    int y = 6;
    GameLog* temp = gameLogs;
    while(temp != NULL) {
        if(strcmp(temp->username, currentUser) == 0) {
            struct tm* timeinfo = localtime(&temp->startTime);
            char timeStr[20];
            strftime(timeStr, 20, "%Y-%m-%d %H:%M:%S", timeinfo);

            Pos(10, y);
            printf("%-12s  %s  %-12d  %d",
                  temp->username,
                  timeStr,
                  temp->duration,
                  temp->score);
            y++;
        }
        temp = temp->next;
    }

    Pos(10, y+2);
    printf("按任意键返回游戏...");
    _getch();
    system("cls");
    creatMap();
    initsnake();
    createfood();
    displayUserInfo();
}

int main()
{
    gamestart();
    gamecircle();
    endgame();
    return 0;
}
