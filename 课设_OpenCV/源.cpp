//解决台式机问题的代码
#include "stdio.h"
#ifdef __cplusplus 
extern "C"
#endif 
FILE* __cdecl __iob_func(unsigned i) {
	return __acrt_iob_func(i);
}

#include <graphics.h>
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <math.h>
#include <time.h>

#include <highgui.h>
#include <opencv2/opencv.hpp>
using namespace cv;

#define HEIGHT 600
#define WIDTH 800
#define BORDER 480
#define LEFT 1
#define RIGHT 2


//初始化
void windowInit();
void gameFramwork(struct Settings settings);
//struct PLAYER *initPlayer();
void initPlayer(struct PLAYER *player1, struct PLAYER *player2);
void initItemToZero();
void loadAllImages();
//TODO 准备设置难度，利用重载
//struct PLAYER *initPlayer();

//新的链表函数
void initItem();
void addItem();
int delItem();
struct ITEM *get_first_item();
struct ITEM *get_next_item();

//画图函数
void drawItem(struct ITEM *item);
void coverItem(int x, int y, enum TYPE type);
void drawPlayer(struct PLAYER *player, int direction);
void drawPlayer2(struct PLAYER *player, int direction);
void drawScore(int score, int score2);
void drawPlayerName(struct PLAYER *player, struct PLAYER *player2);
void drawOver(struct PLAYER *player1, struct PLAYER *player2);
void drawTransparent(IMAGE trans_a, IMAGE trans_b);
//双人重载 不用重载
//void drawPlayerName(struct PLAYER *player1, struct PLAYER *player2);
// TODO 加地图移动

//界面函数
void Menu();
//void changeSettings();
int Pause(struct PLAYER *player1, struct PLAYER *player2);
//void Help();
//void Rank();
int returnToMenu();
void Exit();
void drawBackground();
void gameStart();



//判断函数
void judgeState(struct ITEM *item, struct PLAYER *player);
int judgeOver(struct PLAYER *player1, struct PLAYER *player2);
enum MOUSE_POSITION judgeMouse();
void itemMove(struct ITEM *item);
int playerMove(struct PLAYER *player);
int playerMove2(struct PLAYER *player);
int AIMove(struct PLAYER *player, struct ITEM *item);

//其他函数
void playerBubbleSort(struct PLAYER a[], int len);
void Save(struct PLAYER *player);
void showScore();


enum STATE { //物体的状态
	ON_SCREEN, //在屏幕上
	OFF_SCREEN, //离开了屏幕
	EATED // 被玩家吃掉
};

enum TYPE {
	UMBRELLA,
	CAKE,
	BOMB
};

enum MOUSE_POSITION {
	NOTHING,
	ON_START,
	ON_HELP,
	ON_SETTINGS,
	ON_SINGLE,
	ON_MULTI,
	ON_AI,
	ON_RANK,
	ON_EXIT,
	HIT_START,
	HIT_HELP,
	HIT_SETTINGS,
	HIT_SINGLE,
	HIT_MULTI,
	HIT_AI,
	HIT_RANK,
	HIT_EXIT,
	OFF
};

struct SPEED { //TODO 填写速度,后期设置难度,可以改速度
	double player;
	double cake;
	double umbrella;
	double bomb;
};

struct PLAYER {
	char name[10]; //玩家名称
	double x = 400;
	double y = 480; //横坐标是一个确定的数
	int score = 0; //玩家分数
	int life = 1; //玩家的命
};

struct Settings {
	int difficult; //难度, 1简单，2中等，3困难
	int mode; //模式 1为单人，2为双人，3为AI
	int time; //游戏时间
};
//新的数据结构
typedef struct item_list {
	struct ITEM *this_item;
	struct item_list *next;
}ItemList;

typedef struct ITEM {
	enum TYPE type; //具体是哪一种物品
	enum STATE state;
	double x;
	double y;
	//struct ITEM *next;
}Item;


//全局变量
struct SPEED speed = {//初始化速度
	0.5,//player
	0.11,//cake
	0.14,//umbrella
	0.19//bomb
};

struct item_list item_list_head;
struct item_list *curr_list;

struct Settings settings = { //默认设置
	1,
	1,
	120
};
int ItemCount = 0;
const int ItemNumber = 10;//屏幕上物品的总数

IMAGE background, lost, cake, umbrella, bomb_a, bomb_b, imgplayer;
Mat allBackground(600, 800, CV_8UC3, Scalar(255, 255, 255)), startBackground;
String windowName("测试");

int main()
{
	windowInit();
	loadAllImages();
	waitKey(0);
	imshow(windowName, startBackground);
	waitKey(0);
	//initgraph(WIDTH, HEIGHT, SHOWCONSOLE);
	//loadAllImages();
	//Menu();

	//closegraph();
}

//初始化

void windowInit()
{
	namedWindow(windowName, WINDOW_NORMAL);
	resizeWindow(windowName, 800, 600);
	imshow(windowName, allBackground);
}

void gameFramwork(struct Settings settings)
{
	// TODO 应该先数据再界面
	//界面初始化
	//setbkmode(TRANSPARENT);//设置文字背景透明
	line(0, BORDER, WIDTH, BORDER);
	//开始按钮
	RECT start = { 30, BORDER + 40, 100, BORDER + 80 };
	rectangle(30, BORDER + 40, 100, BORDER + 80);
	drawtext(_T("暂停"), &start, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
	//结束按钮
	RECT end = { 150, BORDER + 40, 220, BORDER + 80 };
	rectangle(150, BORDER + 40, 220, BORDER + 80);
	drawtext(_T("主菜单"), &end, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
	//分数
	//玩家1
	RECT scoreTextAera = { WIDTH - 300, BORDER + 40 - 20, WIDTH - 230, BORDER + 80 - 20 };
	RECT playerNameAera = { WIDTH - 300, BORDER + 80 - 20, WIDTH - 230, BORDER + 120 - 20 };
	rectangle(WIDTH - 300, BORDER + 40 - 20, WIDTH - 180, BORDER + 80 - 20);
	rectangle(WIDTH - 300, BORDER + 80 - 20, WIDTH - 180, BORDER + 120 - 20);

	drawtext(_T("分数："), &scoreTextAera, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
	drawtext(_T("玩家1： "), &playerNameAera, DT_CENTER | DT_SINGLELINE | DT_VCENTER);

	//玩家2
	if (settings.mode == 2)
	{
		RECT scoreTextAera2 = { WIDTH - 180, BORDER + 40 - 20, WIDTH - 110, BORDER + 80 - 20 };
		RECT playerNameAera2 = { WIDTH - 180, BORDER + 80 - 20, WIDTH - 110, BORDER + 120 - 20 };
		rectangle(WIDTH - 180, BORDER + 40 - 20, WIDTH - 60, BORDER + 80 - 20);
		rectangle(WIDTH - 180, BORDER + 80 - 20, WIDTH - 60, BORDER + 120 - 20);

		drawtext(_T("分数："), &scoreTextAera2, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
		drawtext(_T("玩家2： "), &playerNameAera2, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
	}
}

void initPlayer(struct PLAYER *player1, struct PLAYER *player2)
{
	player1->x = 350;
	player1->y = 459;
	player1->life = 1;
	player1->score = 0;
	player2->x = 450;
	player2->y = 459;
	player2->life = 1;
	player2->score = 0;
}

void initItemToZero()
{
	struct ITEM *item;
	item = get_first_item();
	while (item != NULL)
	{
		item->state = ON_SCREEN;
		item->x = 200 + rand() % 400;
		item->y = -(rand() % 550);
		item = get_next_item();
	}
}

void loadAllImages()
{
	startBackground = imread("img\\bkg.jpg", IMREAD_COLOR);
	/*loadimage(&background, _T("MyImage"), MAKEINTRESOURCE(4));
	loadimage(&cake, _T("img\\cake.jpg"), 40, 40);
	loadimage(&umbrella, _T("img\\umbrella.gif"), 40, 40);
	loadimage(&bomb_a, _T("img\\bomb_a.bmp"), 40, 40);
	loadimage(&bomb_b, _T("img\\bomb_b.bmp"), 40, 40);
	loadimage(&imgplayer, _T("img\\player.gif"), 40, 40);
	loadimage(&lost, _T("img\\background.jpg"));*/
}

//新的链表函数
void initItem()
{
	item_list_head.next = NULL;
	item_list_head.this_item = NULL;
	curr_list = NULL;
}

void addItem()//把new和add函数合成一个函数了
{
	//先创建小节点
	Item *newItem = (Item *)malloc(sizeof(Item));
	//srand((unsigned int)time(0));
	int randomNumber = rand() % 3;
	// TODO 以下写到函数randomType()里，返回值为enum，加概率
	switch (randomNumber)
	{
	case 0:newItem->type = UMBRELLA; break;
	case 1:newItem->type = CAKE; break;
	case 2:newItem->type = BOMB; break;
	}
	////
	newItem->state = ON_SCREEN;
	newItem->x = 200 + rand() % 400;
	newItem->y = -(rand() % 550);
	//	newItem->next = NULL;
	//创建大节点
	struct item_list *list_tail = &item_list_head;
	while (list_tail->next != NULL)
		list_tail = list_tail->next;
	ItemList *newList = (ItemList *)malloc(sizeof(ItemList));
	newList->next = NULL;
	newList->this_item = newItem;
	list_tail->next = newList;
	ItemCount++;
}

int delItem()
{
	struct item_list *this_node = &item_list_head;
	struct item_list *prev_node = &item_list_head;
	this_node = this_node->next;
	Item *item = this_node->this_item;
	while (this_node != NULL && item != NULL)
	{
		while (item->state == ON_SCREEN && this_node != NULL)
		{
			this_node = this_node->next;
			prev_node = prev_node->next;
			if (this_node != NULL)
				item = this_node->this_item;
		}
		if (this_node != NULL)
		{
			free(item);
			this_node->this_item = NULL;
			item = NULL;
			prev_node->next = this_node->next;
			free(this_node);
			curr_list->this_item = NULL;
			curr_list->next = NULL;
			this_node = NULL;
			ItemCount--;
			return 1;
		}
	}
	return 0;
}

struct ITEM *get_first_item()
{
	curr_list = item_list_head.next;
	if (curr_list == NULL)
		return NULL;
	return curr_list->this_item;
}

struct ITEM *get_next_item()
{
	if (curr_list == NULL || curr_list->next == NULL)
		return NULL;
	/*if (curr_list->next == (item_list *)0xdddddddd)
	{
	curr_list->next = NULL;
	return NULL;
	}*/
	curr_list = curr_list->next;
	return curr_list->this_item;
}

//画图函数
void drawItem(struct ITEM *item)
{
	switch (item->type)
	{
	case UMBRELLA:
		putimage((int)item->x - 20, (int)item->y - 20, &umbrella);
		setlinecolor(WHITE);
		break;
	case CAKE:
		putimage((int)item->x - 20, (int)item->y - 20, &cake);
		setlinecolor(GREEN);
		break;
	case BOMB:
		putimage((int)item->x - 20, (int)item->y - 20, &bomb_a);
		setlinecolor(RED);

		break;
	}
}

void coverItem(int x, int y, enum TYPE type)
{
	switch (type)
	{
	case UMBRELLA:
		setlinecolor(BLACK);
		circle(x, y, 20);// 擦除前一个
		break;
	case CAKE:
		setlinecolor(BLACK);
		circle(x, y, 20);// 擦除前一个
		break;
	case BOMB:
		setlinecolor(BLACK);
		circle(x, y, 20);
		break;
	}
}

void drawPlayer(struct PLAYER *player, int direction)
{
	/*int flag = 0;
	switch (direction)
	{
	case LEFT: flag = -1; break;
	case RIGHT: flag = 1; break;
	}*/
	setlinecolor(WHITE);
	//circle((int)player->x, (int)player->y, 20);
	//putimage((int)player->x - 20, (int)player->y - 20, &player);
	putimage((int)player->x - 20, (int)player->y - 20, &imgplayer);
}

void drawPlayer2(struct PLAYER *player, int direction)
{
	/*int flag = 0;
	switch (direction)
	{
	case LEFT: flag = -1; break;
	case RIGHT: flag = 1; break;
	}*/
	//setlinecolor(BLACK);
	//circle(player->x - (speed.player)*flag, player->y, 20);// 擦除前一个
	setlinecolor(RED);
	circle((int)player->x, (int)player->y, 20);
}

void drawScore(int score, int score2)
{
	char scoreNumber[10];
	sprintf_s(scoreNumber, "%d", score);
	setlinecolor(WHITE);
	RECT scoreNumberAera = { WIDTH - 300, BORDER + 40 - 20, WIDTH - 150, BORDER + 80 - 20 };
	drawtext((LPCTSTR)scoreNumber, &scoreNumberAera, (DT_CENTER | DT_SINGLELINE | DT_VCENTER));
	//玩家2
	if (settings.mode == 2)
	{
		char scoreNumber2[10];
		sprintf_s(scoreNumber2, "%d", score2);
		RECT scoreNumberAera2 = { WIDTH - 180, BORDER + 40 - 20, WIDTH - 30, BORDER + 80 - 20 };
		drawtext((LPCTSTR)scoreNumber2, &scoreNumberAera2, (DT_CENTER | DT_SINGLELINE | DT_VCENTER));
	}
}

void drawPlayerName(struct PLAYER *player, struct PLAYER *player2)
{
	RECT playerNameAera = { WIDTH - 300, BORDER + 80 - 20, WIDTH - 150, BORDER + 120 - 20 };
	drawtext((LPCTSTR)(player->name), &playerNameAera, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
	//玩家2	
	if (settings.mode == 2)
	{
		RECT playerNameAera2 = { WIDTH - 180, BORDER + 80 - 20, WIDTH - 30, BORDER + 120 - 20 };
		drawtext((LPCTSTR)(player2->name), &playerNameAera2, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
	}
}

void drawOver(struct PLAYER *player1, struct PLAYER *player2)
{
	cleardevice();
	MOUSEMSG mouse;
	mouse.uMsg = NULL;
	setlinecolor(WHITE);
	while (1)
	{
		char score[20] = { '\0' };
		sprintf_s(score, "你的成绩是: %d 分", player1->score);
		outtextxy(300, 300, _T("游戏结束"));
		outtextxy(300, 320, (LPCTSTR)score);

		RECT saveScore = { 200, 400, 200 + 100, 400 + 40 };
		rectangle(200, 400, 200 + 100, 400 + 40);
		drawtext(_T("保存成绩"), &saveScore, DT_CENTER | DT_SINGLELINE | DT_VCENTER);

		RECT toMenu = { 200 + 110, 400, 200 + 100 + 110, 400 + 40 };
		rectangle(200 + 110, 400, 200 + 100 + 110, 400 + 40);
		drawtext(_T("返回主菜单"), &toMenu, DT_CENTER | DT_SINGLELINE | DT_VCENTER);

		if (MouseHit())
		{
			mouse = GetMouseMsg();
			if (((200 + 110) <= mouse.x && mouse.x <= (200 + 100 + 110)) && (400 <= mouse.y && mouse.y <= (400 + 40)) && mouse.uMsg == WM_LBUTTONDOWN)
				Menu();
			if (((200) <= mouse.x && mouse.x <= (200 + 100)) && (400 <= mouse.y && mouse.y <= (400 + 40)) && mouse.uMsg == WM_LBUTTONDOWN) {
				Save(player1);
			}

		}
		else
			mouse.uMsg = NULL;
		FlushBatchDraw();
	}
}

void drawTransparent(IMAGE trans_a, IMAGE trans_b)
{
	SetWorkingImage(&trans_a);
	putimage(0, 0, &trans_b, SRCINVERT);
	SetWorkingImage(&background);
	putimage(100, 100, &trans_b, SRCAND);
	putimage(100, 100, &trans_a, SRCPAINT);
}

//界面函数
void Menu()
{
	cleardevice();
	while (1)
	{
		BeginBatchDraw();
		setfillcolor(CYAN);
		switch (settings.mode)
		{
		case 1:fillrectangle(460, 240, 460 + 100, 240 + 50); break;
		case 2:fillrectangle(570, 240, 570 + 100, 240 + 50); break;
		case 3:fillrectangle(680, 240, 680 + 100, 240 + 50); break;
		}
		setlinecolor(WHITE);
		setbkmode(TRANSPARENT);
		//开始
		RECT start = { 350, 120, 350 + 100, 120 + 50 };
		rectangle(350, 120, 350 + 100, 120 + 50);
		drawtext(_T("开始游戏"), &start, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
		//帮助
		RECT help = { 350, 180, 350 + 100, 180 + 50 };
		rectangle(350, 180, 350 + 100, 180 + 50);
		drawtext(_T("帮助"), &help, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
		//设置/模式
		RECT mode = { 350, 240, 350 + 100, 240 + 50 };
		rectangle(350, 240, 350 + 100, 240 + 50);
		drawtext(_T("模式"), &mode, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
		//单人
		RECT singleplayer = { 460, 240, 460 + 100, 240 + 50 };
		rectangle(460, 240, 460 + 100, 240 + 50);
		drawtext(_T("单人"), &singleplayer, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
		//双人
		RECT multiplayer = { 570, 240, 570 + 100, 240 + 50 };
		rectangle(570, 240, 570 + 100, 240 + 50);
		drawtext(_T("双人"), &multiplayer, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
		//AI
		RECT ai = { 680, 240, 680 + 100, 240 + 50 };
		rectangle(680, 240, 680 + 100, 240 + 50);
		drawtext(_T("AI"), &ai, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
		//排名
		RECT rank = { 350, 300, 350 + 100, 300 + 50 };
		rectangle(350, 300, 350 + 100, 300 + 50);
		drawtext(_T("排名"), &rank, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
		//退出
		RECT exit = { 350, 360, 350 + 100, 360 + 50 };
		rectangle(350, 360, 350 + 100, 360 + 50);
		drawtext(_T("退出"), &exit, DT_CENTER | DT_SINGLELINE | DT_VCENTER);

		setfillcolor(CYAN);
		//FlushBatchDraw();
		EndBatchDraw();
		BeginBatchDraw();
		switch (judgeMouse())
		{
		case ON_START: fillrectangle(350, 120, 350 + 100, 120 + 50);  break;
		case HIT_START: gameStart(); break;
		case ON_HELP: fillrectangle(350, 180, 350 + 100, 180 + 50); break;
			//case HIT_HELP: Help(); break;
		case ON_SINGLE: fillrectangle(460, 240, 460 + 100, 240 + 50); break;
		case HIT_SINGLE: settings.mode = 1; break;
		case ON_MULTI: fillrectangle(570, 240, 570 + 100, 240 + 50); break;
		case HIT_MULTI: settings.mode = 2; break;
		case ON_AI: fillrectangle(680, 240, 680 + 100, 240 + 50); break;
		case HIT_AI: settings.mode = 3; break;
		case ON_RANK: fillrectangle(350, 300, 350 + 100, 300 + 50); break;
			//case HIT_RANK: Rank(); break;
		case ON_EXIT: fillrectangle(350, 360, 350 + 100, 360 + 50); break;
		case HIT_EXIT: Exit(); break;

		case OFF:cleardevice(); break;
		}
	}

	EndBatchDraw();
}

int Pause(struct PLAYER *player1, struct PLAYER *player2)
{
	int flag = 0;
	MOUSEMSG mouse;
	//(30 <= mouse.x && mouse.x <= 100) && ((BORDER + 40) <= mouse.y && mouse.y <= (BORDER + 80)) && 
	if (MouseHit())
	{
		mouse = GetMouseMsg();
		if ((30 <= mouse.x && mouse.x <= 100) && ((BORDER + 40) <= mouse.y && mouse.y <= (BORDER + 80)) && mouse.uMsg == WM_LBUTTONDOWN)
			while (1)
			{
				clearrectangle(30, BORDER + 40, 100, BORDER + 80);
				RECT start = { 30, BORDER + 40, 100, BORDER + 80 };
				rectangle(30, BORDER + 40, 100, BORDER + 80);
				drawtext(_T("继续"), &start, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
				drawPlayerName(player1, player2);
				drawScore(player1->score, player2->score);
				FlushBatchDraw();
				if (MouseHit())
				{
					mouse = GetMouseMsg();
					if ((30 <= mouse.x && mouse.x <= 100) && ((BORDER + 40) <= mouse.y && mouse.y <= (BORDER + 80)) && mouse.uMsg == WM_LBUTTONDOWN)
						break;
				}
				else
					mouse.uMsg = NULL;
				if (GetAsyncKeyState(0x50))
					break;
				if (returnToMenu())
				{
					flag = 1;
					break;
				}

			}
	}
	else
		mouse.uMsg = NULL;

	if (GetAsyncKeyState(0x50))
	{
		while (1)
		{
			clearrectangle(30, BORDER + 40, 100, BORDER + 80);
			RECT start = { 30, BORDER + 40, 100, BORDER + 80 };
			rectangle(30, BORDER + 40, 100, BORDER + 80);
			drawtext(_T("开始"), &start, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
			drawPlayerName(player1, player2);
			drawScore(player1->score, player2->score);
			FlushBatchDraw();
			if (MouseHit())
			{
				mouse = GetMouseMsg();
				if ((30 <= mouse.x && mouse.x <= 100) && ((BORDER + 40) <= mouse.y && mouse.y <= (BORDER + 80)) && mouse.uMsg == WM_LBUTTONDOWN)
					break;
			}
			else
				mouse.uMsg = NULL;
			if (GetAsyncKeyState(0x50))
				break;
			if (returnToMenu())
			{
				flag = 1;
				break;
			}

		}
	}

	return flag;
}

int returnToMenu()
{
	MOUSEMSG mouse;
	mouse.uMsg = NULL;
	if (MouseHit())
	{
		mouse = GetMouseMsg();
		if ((150 <= mouse.x && mouse.x <= 220) && ((BORDER + 40) <= mouse.y && mouse.y <= (BORDER + 80)) && mouse.uMsg == WM_LBUTTONDOWN)
			return 1;
	}
	else
		mouse.uMsg = NULL;
	if (GetAsyncKeyState(0x1B))
		return 1;
	return 0;
}

void Exit()
{
	exit(0);
}

void drawBackground()
{
	putimage(0, 0, &background);
}

void gameStart()
{
	cleardevice();
	// 数据初始化
	srand((unsigned int)time(0));

	struct PLAYER playerOne;
	struct PLAYER *player1 = &playerOne;
	struct PLAYER playerTwo;
	struct PLAYER *player2 = &playerTwo;
	int direction = 0;
	int direction2 = 0;
	initPlayer(player1, player2);

	struct ITEM *item;
	initItemToZero();


	switch (settings.mode)
	{
	case 1:	InputBox((LPTSTR)&(player1->name), 10, (LPCTSTR)"请输入名字："); break;
	case 2: {
		InputBox((LPTSTR)&(player1->name), 10, (LPCTSTR)"请输入玩家1的名字：");
		InputBox((LPTSTR)&(player2->name), 10, (LPCTSTR)"请输入玩家2的名字：");
	}
			break;
	case 3:	player1->name[0] = 'A'; player1->name[1] = 'I'; player1->name[2] = '\0'; break;
	}

	while (1)
	{
		for (int i = ItemNumber - ItemCount; i > 0; i--)
			addItem();

		BeginBatchDraw();

		//画背景和框架
		drawBackground();
		gameFramwork(settings);

		//玩家动画
		if (settings.mode != 3)
		{
			direction = playerMove(player1);
			drawPlayer(player1, direction);

			if (settings.mode == 2)
			{
				direction2 = playerMove2(player2);
				drawPlayer2(player2, direction2);
			}
		}

		//物体动画
		item = get_first_item();
		while (item != NULL)
		{
			if (Pause(player1, player2) || returnToMenu())
				return;

			judgeState(item, player1);
			if (settings.mode != 3)
				judgeOver(player1, player2);
			if (settings.mode == 2)
				judgeState(item, player2);
			//储存item里临时的数据，因为删除之后就无法读取item，无法在图形上删除item了
			int temp_x = (int)item->x;
			int temp_y = (int)item->y;
			enum TYPE temp_type = item->type;

			if (delItem())
				coverItem(temp_x, temp_y, temp_type);
			else {
				itemMove(item);
				//AI移动
				if (settings.mode == 3)
				{
					direction = AIMove(player1, item);
					drawPlayer(player1, direction);
				}
				drawItem(item);
			}
			//Sleep(1); //不用好像更流畅

			item = get_next_item();
		}
		drawPlayerName(player1, player2);
		drawScore(player1->score, player2->score);

		EndBatchDraw();
	}
}

//判断函数
void judgeState(struct ITEM *item, struct PLAYER *player)
{
	if (item->y >= 480 - 20 - speed.umbrella)
	{
		item->state = OFF_SCREEN;
		return;
	}
	int distance = (int)sqrt((player->x - item->x)*(player->x - item->x) + (player->y - item->y)*(player->y - item->y));
	if (distance <= 40)
	{
		item->state = EATED;
		if (item->type == CAKE)
			player->score += 10;
		if (item->type == BOMB)
			player->life = 0; //TODO 减一
	}
}

int judgeOver(struct PLAYER *player1, struct PLAYER *player2)
{
	if (player1->life == 0 || player2->life == 0)
		drawOver(player1, player2);
	else
		return 0;
}

enum MOUSE_POSITION judgeMouse()
{
	MOUSEMSG mouse = GetMouseMsg();
	//开始
	if ((350 <= mouse.x && mouse.x <= (350 + 100)) && (120 <= mouse.y && mouse.y <= (120 + 50)))
		if (mouse.mkLButton)//按下左键
			return HIT_START;
		else
			return ON_START;
	//帮助
	if ((350 <= mouse.x && mouse.x <= (350 + 100)) && (180 <= mouse.y && mouse.y <= (180 + 50)))
		if (mouse.mkLButton)//按下左键
			return HIT_HELP;
		else
			return ON_HELP;
	//设置/模式 不要
	//if ((350 <= mouse.x && mouse.x <= (350 + 100)) && (180 <= mouse.y && mouse.y <= (180 + 50)))
	//	if (mouse.mkLButton)//按下左键
	//		return HIT_HELP;
	//	else
	//		return ON_HELP;
	//else
	//	return OFF_START;

	//单人
	if ((460 <= mouse.x && mouse.x <= (460 + 100)) && (240 <= mouse.y && mouse.y <= (240 + 50)))
		if (mouse.mkLButton)//按下左键
			return HIT_SINGLE;
		else
			return ON_SINGLE;
	//双人
	if ((570 <= mouse.x && mouse.x <= (570 + 100)) && (240 <= mouse.y && mouse.y <= (240 + 50)))
		if (mouse.mkLButton)//按下左键
			return HIT_MULTI;
		else
			return ON_MULTI;
	//AI
	if ((680 <= mouse.x && mouse.x <= (680 + 100)) && (240 <= mouse.y && mouse.y <= (240 + 50)))
		if (mouse.mkLButton)//按下左键
			return HIT_AI;
		else
			return ON_AI;
	//排名
	if ((350 <= mouse.x && mouse.x <= (350 + 100)) && (300 <= mouse.y && mouse.y <= (300 + 50)))
		if (mouse.mkLButton)//按下左键
			return HIT_RANK;
		else
			return ON_RANK;
	//退出
	if ((350 <= mouse.x && mouse.x <= (350 + 100)) && (360 <= mouse.y && mouse.y <= (360 + 50)))
		if (mouse.mkLButton)//按下左键
			return HIT_EXIT;
		else
			return ON_EXIT;

	return OFF;
}

void itemMove(struct ITEM *item)
{
	switch (item->type)
	{
	case UMBRELLA:item->y += speed.umbrella; break;
	case CAKE:item->y += speed.cake; break;
	case BOMB:item->y += speed.bomb; break;
	}
}

int playerMove(struct PLAYER *player)
{
	int key = 0;
	if (GetAsyncKeyState(0x41))
	{
		player->x -= speed.player;
		if (player->x <= 0 + speed.player || player->x >= WIDTH - speed.player) //边界检测
			player->x += speed.player;
		return LEFT;
	}
	if (GetAsyncKeyState(0x44))
	{
		player->x += speed.player;
		if (player->x <= 0 + speed.player || player->x >= WIDTH - speed.player)
			player->x -= speed.player;
		return RIGHT;
	}
	return 0;
}

int playerMove2(struct PLAYER *player)
{
	int key = 0;
	if (GetAsyncKeyState(VK_LEFT))
	{
		player->x -= speed.player;
		if (player->x <= 0 + speed.player || player->x >= WIDTH - speed.player) //边界检测
			player->x += speed.player;
		return LEFT;
	}
	if (GetAsyncKeyState(VK_RIGHT))
	{
		player->x += speed.player;
		if (player->x <= 0 + speed.player || player->x >= WIDTH - speed.player)
			player->x -= speed.player;
		return RIGHT;
	}
	return 0;
}

int AIMove(struct PLAYER *player, struct ITEM *item)
{
	int distance = (int)sqrt((player->x - item->x)*(player->x - item->x) + (player->y - item->y)*(player->y - item->y));
	if (distance <= 300)
	{
		if (item->type == BOMB)
		{
			if (((item->x - 20) <= (player->x + 20)) && ((player->x + 20) <= item->x) && ((item->x - 20) - (player->x + 20)) <= 50 || player->x == item->x)
			{
				player->x -= speed.player;
				if (player->x <= 0 + speed.player || player->x >= WIDTH - speed.player) //边界检测
					player->x += speed.player;
				return LEFT;
			}

			if (((player->x - 20) >= item->x) && ((player->x - 20) <= (item->x + 20)))
			{
				player->x += speed.player;
				if (player->x <= 0 + speed.player || player->x >= WIDTH - speed.player)
					player->x -= speed.player;
				return RIGHT;
			}
		}
		if (item->type == CAKE)
		{
			if (((item->x - 20) - (player->x + 20)) > 0)
			{
				player->x += speed.player;
				if (player->x <= 0 + speed.player || player->x >= WIDTH - speed.player) //边界检测
					player->x -= speed.player;
				return RIGHT;
			}
			if (((player->x - 20) - (item->x + 20)) > 0)
			{
				player->x -= speed.player;
				if (player->x <= 0 + speed.player || player->x >= WIDTH - speed.player)
					player->x += speed.player;
				return LEFT;
			}
		}
	}

	return 0;
}

//其他函数

void playerBubbleSort(struct PLAYER a[], int len)
{
	struct PLAYER temp;
	for (int i = 0; i < len - 1; i++)
		for (int j = 0; j < len - 1 - i; j++)
			if (a[j].score < a[j + 1].score)
			{
				temp = a[j];
				a[j] = a[j + 1];
				a[j + 1] = temp;
			}
}

void Save(struct PLAYER *player)
{
	FILE *fp = NULL;

	if (fopen_s(&fp, "data", "ab+"))
	{
		outtextxy(50, 50, _T("保存失败"));
		return;
	}
	fwrite(player, sizeof(struct PLAYER), 1, fp);
	fclose(fp);
	outtextxy(50, 50, _T("保存成功"));
	FlushBatchDraw();
	showScore();
}

void showScore()
{
	int const rankNumber = 10;
	int lineCount = 0;
	char rankLine[10][20] = { '\0' };
	struct PLAYER *pplayer = (struct PLAYER *)malloc(sizeof(struct PLAYER)*rankNumber);
	struct PLAYER static *pplayer_head = pplayer;
	FILE *fp = NULL;

	if (fopen_s(&fp, "data", "ab+"))
	{
		outtextxy(50, 100, _T("读取失败"));
		FlushBatchDraw();
		return;
	}

	for (int i = 0; i < rankNumber; i++, lineCount++, pplayer++)
	{
		fread(pplayer, sizeof(struct PLAYER), 1, fp);
		if (feof(fp))
			break;
	}
	pplayer = pplayer_head;
	playerBubbleSort(pplayer, lineCount);
	pplayer = pplayer_head;
	for (int i = 0; i < lineCount; i++, pplayer++)
	{
		sprintf_s(rankLine[i], "姓名：%s, 成绩：%d", pplayer->name, pplayer->score);
		outtextxy(50, 100 + i * 20, (LPCTSTR)rankLine[i]);
		FlushBatchDraw();
	}
	fclose(fp);
}