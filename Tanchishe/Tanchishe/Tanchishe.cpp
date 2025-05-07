#include<stdio.h>
#include<time.h>
#include<windows.h>
#include<stdlib.h>
#include<string.h>
#include<conio.h>

#define U 1
#define D 2
#define L 3
#define R 4       //�ߵ�״̬��U���� ��D���£�L:�� R����

typedef struct SNAKE //�����һ���ڵ�
{
    int x;
    int y;
    struct SNAKE* next;
}snake;

// �û��ṹ��
typedef struct USER {
    char username[50];
    char password[50];
    struct USER* next;
} User;

// ��Ϸ��־�ṹ��
typedef struct GAMELOG {
    char username[50];
    time_t startTime;
    int duration;
    int score;
    struct GAMELOG* next;
} GameLog;

//ȫ�ֱ���//
int score = 0, add = 10;//�ܵ÷���ÿ�γ�ʳ��÷֡�
int status, sleeptime = 200;//ÿ�����е�ʱ����
snake* head, * food;//��ͷָ�룬ʳ��ָ��
snake* q;//�����ߵ�ʱ���õ���ָ��
int endgamestatus = 0; //��Ϸ�����������1��ײ��ǽ��2��ҧ���Լ���3�������˳���Ϸ��

// �û�����־���ȫ�ֱ���
User* userList = NULL;
GameLog* gameLogs = NULL;
char currentUser[50] = {0};
time_t gameStartTime;

//����ȫ������//
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

// ������������
void registerUser();
int loginUser();
void saveUsersToFile();
void loadUsersFromFile();
void saveGameLog(const char* username, time_t startTime, int duration, int score);
void loadGameLogs();
void showGameLogs();
void displayUserInfo();

void Pos(int x, int y)//���ù��λ��
{
    COORD pos;
    HANDLE hOutput;
    pos.X = x;
    pos.Y = y;
    hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleCursorPosition(hOutput, pos);
}

void creatMap()//������ͼ
{
    int i;
    for (i = 0; i < 58; i += 2)//��ӡ���±߿�
    {
        Pos(i, 0);
        printf("��");
        Pos(i, 26);
        printf("��");
    }
    for (i = 1; i < 26; i++)//��ӡ���ұ߿�
    {
        Pos(0, i);
        printf("��");
        Pos(56, i);
        printf("��");
    }
}

void initsnake()//��ʼ������
{
    snake* tail;
    int i;
    tail = (snake*)malloc(sizeof(snake));//����β��ʼ��ͷ�巨����x,y�趨��ʼ��λ��//
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
    while (tail != NULL)//��ͷ��Ϊ���������
    {
        Pos(tail->x, tail->y);
        printf("��");
        tail = tail->next;
    }
}

int biteself()//�ж��Ƿ�ҧ�����Լ�
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

void createfood()//�������ʳ��
{
    snake* food_1;
    srand((unsigned)time(NULL));
    food_1 = (snake*)malloc(sizeof(snake));
    while ((food_1->x % 2) != 0)    //��֤��Ϊż����ʹ��ʳ��������ͷ����
    {
        food_1->x = rand() % 52 + 2;
    }
    food_1->y = rand() % 24 + 1;
    q = head;
    while (q->next == NULL)
    {
        if (q->x == food_1->x && q->y == food_1->y) //�ж������Ƿ���ʳ���غ�
        {
            free(food_1);
            createfood();
        }
        q = q->next;
    }
    Pos(food_1->x, food_1->y);
    food = food_1;
    printf("��");
}

void cantcrosswall()//���ܴ�ǽ
{
    if (head->x == 0 || head->x == 56 || head->y == 0 || head->y == 26)
    {
        endgamestatus = 1;
        endgame();
    }
}

void snakemove()//��ǰ��,��U,��D,��L,��R
{
    snake* nexthead;
    cantcrosswall();

    nexthead = (snake*)malloc(sizeof(snake));
    if (status == U)
    {
        nexthead->x = head->x;
        nexthead->y = head->y - 1;
        if (nexthead->x == food->x && nexthead->y == food->y)//�����һ����ʳ��//
        {
            nexthead->next = head;
            head = nexthead;
            q = head;
            while (q != NULL)
            {
                Pos(q->x, q->y);
                printf("��");
                q = q->next;
            }
            score = score + add;
            createfood();
        }
        else                                               //���û��ʳ��//
        {
            nexthead->next = head;
            head = nexthead;
            q = head;
            while (q->next->next != NULL)
            {
                Pos(q->x, q->y);
                printf("��");
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
        if (nexthead->x == food->x && nexthead->y == food->y)  //��ʳ��
        {
            nexthead->next = head;
            head = nexthead;
            q = head;
            while (q != NULL)
            {
                Pos(q->x, q->y);
                printf("��");
                q = q->next;
            }
            score = score + add;
            createfood();
        }
        else                               //û��ʳ��
        {
            nexthead->next = head;
            head = nexthead;
            q = head;
            while (q->next->next != NULL)
            {
                Pos(q->x, q->y);
                printf("��");
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
        if (nexthead->x == food->x && nexthead->y == food->y)//��ʳ��
        {
            nexthead->next = head;
            head = nexthead;
            q = head;
            while (q != NULL)
            {
                Pos(q->x, q->y);
                printf("��");
                q = q->next;
            }
            score = score + add;
            createfood();
        }
        else                                //û��ʳ��
        {
            nexthead->next = head;
            head = nexthead;
            q = head;
            while (q->next->next != NULL)
            {
                Pos(q->x, q->y);
                printf("��");
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
        if (nexthead->x == food->x && nexthead->y == food->y)//��ʳ��
        {
            nexthead->next = head;
            head = nexthead;
            q = head;
            while (q != NULL)
            {
                Pos(q->x, q->y);
                printf("��");
                q = q->next;
            }
            score = score + add;
            createfood();
        }
        else                                         //û��ʳ��
        {
            nexthead->next = head;
            head = nexthead;
            q = head;
            while (q->next->next != NULL)
            {
                Pos(q->x, q->y);
                printf("��");
                q = q->next;
            }
            Pos(q->next->x, q->next->y);
            printf("  ");
            free(q->next);
            q->next = NULL;
        }
    }
    if (biteself() == 1)       //�ж��Ƿ��ҧ���Լ�
    {
        endgamestatus = 2;
        endgame();
    }
}

void pause()//��ͣ
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
    printf("��ǰ�û�: %s", currentUser);
    Pos(64, 6);
    printf("��F5��ʾ��Ϸ��־");
}

void gamecircle()//������Ϸ
{
    displayUserInfo();
    Pos(64, 15);
    printf("���ܴ�ǽ������ҧ���Լ�\n");
    Pos(64, 16);
    printf("�á�.��.��.���ֱ�����ߵ��ƶ�.");
    Pos(64, 17);
    printf("F1 Ϊ���٣�F2 Ϊ����\n");
    Pos(64, 18);
    printf("ESC ���˳���Ϸ.space����ͣ��Ϸ.");
    Pos(64, 20);
    status = R;
    while (1)
    {
        Pos(64, 10);
        printf("�÷֣�%d  ", score);
        Pos(64, 11);
        printf("ÿ��ʳ��÷֣�%d��", add);
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
                    add = 2;//��ֹ����1֮���ټӻ����д�
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
                    add = 1;  //��֤��ͷ�Ϊ1
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

void welcometogame()//��ʼ����
{
    system("cls");
    Pos(40, 12);
    printf("��ӭ����̰ʳ����Ϸ��");
    Pos(40, 13);
    printf("1. ע�����û�");
    Pos(40, 14);
    printf("2. ��¼�����û�");
    Pos(40, 15);
    printf("3. �˳���Ϸ");
    Pos(40, 17);
    printf("��ѡ��(1-3): ");

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
            printf("��Чѡ�����������룡");
            Sleep(1000);
            welcometogame();
    }
}

void endgame()//������Ϸ
{
    // ��¼��Ϸ��־
    time_t endTime = time(NULL);
    int duration = (int)difftime(endTime, gameStartTime);
    saveGameLog(currentUser, gameStartTime, duration, score);

    system("cls");
    Pos(24, 12);
    if (endgamestatus == 1)
    {
        printf("�Բ�����ײ��ǽ�ˡ���Ϸ����!");
    }
    else if (endgamestatus == 2)
    {
        printf("�Բ�����ҧ���Լ��ˡ���Ϸ����!");
    }
    else if (endgamestatus == 3)
    {
        printf("���Ѿ���������Ϸ��");
    }
    Pos(24, 13);
    printf("���ĵ÷���%d\n", score);
    Pos(24, 14);
    printf("��Ϸʱ��: %d��\n", duration);
    Pos(24, 16);
    system("pause");
    exit(0);
}

void gamestart()//��Ϸ��ʼ��
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

// �û�ע�Ṧ��
void registerUser() {
    system("cls");
    User* newUser = (User*)malloc(sizeof(User));

    Pos(40, 10);
    printf("�û�ע��");
    Pos(40, 12);
    printf("�������û���: ");
    scanf("%s", newUser->username);

    // ����û����Ƿ��Ѵ���
    User* temp = userList;
    while(temp != NULL) {
        if(strcmp(temp->username, newUser->username) == 0) {
            Pos(40, 14);
            printf("�û����Ѵ���!");
            Sleep(1000);
            free(newUser);
            registerUser();
            return;
        }
        temp = temp->next;
    }

    Pos(40, 13);
    printf("����������: ");
    scanf("%s", newUser->password);

    newUser->next = userList;
    userList = newUser;

    saveUsersToFile();

    Pos(40, 15);
    printf("ע��ɹ�! �Զ���¼...");
    strcpy(currentUser, newUser->username);
    Sleep(1000);
    system("cls");
}

// �û���¼����
int loginUser() {
    system("cls");
    char username[50];
    char password[50];

    Pos(40, 10);
    printf("�û���¼");
    Pos(40, 12);
    printf("�������û���: ");
    scanf("%s", username);

    Pos(40, 13);
    printf("����������: ");
    scanf("%s", password);

    User* temp = userList;
    while(temp != NULL) {
        if(strcmp(temp->username, username) == 0 && strcmp(temp->password, password) == 0) {
            strcpy(currentUser, username);
            Pos(40, 15);
            printf("��¼�ɹ�!");
            Sleep(1000);
            system("cls");
            return 1;
        }
        temp = temp->next;
    }

    Pos(40, 15);
    printf("�û������������!");
    Sleep(1000);
    return 0;
}

// �����û����ݵ��ļ�
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

// ���ļ������û�����
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

// ������Ϸ��־
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

// ������Ϸ��־
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

// ��ʾ��Ϸ��־
void showGameLogs() {
    system("cls");
    Pos(10, 2);
    printf("��Ϸ��־ - �û�: %s", currentUser);
    Pos(10, 4);
    printf("�û���        ��ʼʱ��            ����ʱ��(��)  �÷�");

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
    printf("�������������Ϸ...");
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
