//解决台式机问题的代码
#include "stdio.h"
#ifdef __cplusplus 
extern "C"
#endif 
FILE* __cdecl __iob_func(unsigned i) {
	return __acrt_iob_func(i);
}

//#include <graphics.h>
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
void windowInit(); //OpenCV改写
void gameFramwork(struct Settings settings); //OpenCV改写
void initPlayer(struct PLAYER *player1, struct PLAYER *player2);
void initItemToZero();
void loadAllImages(); //OpenCV改写
//TODO 准备设置难度，利用重载
//struct PLAYER *initPlayer();

//新的链表函数
void initItem();
void addItem();
int delItem();
struct ITEM *get_first_item();
struct ITEM *get_next_item();

//画图函数 //OpenCV改写
void drawItem(struct ITEM *item);
void coverItem(int x, int y, enum TYPE type);
void drawPlayer(struct PLAYER *player, int direction);
void drawPlayer2(struct PLAYER *player, int direction);
void drawScore(int score, int score2);
void drawPlayerName(struct PLAYER *player, struct PLAYER *player2);
int drawOver(struct PLAYER *player1, struct PLAYER *player2);
void drawTransparent(Mat image, Mat logo, Mat mask, int x, int y);
void drawImage(Mat image, Mat logo, int x, int y);

//界面函数 //OpenCV改写
void Menu();
//void changeSettings();
int Pause(struct PLAYER *player1, struct PLAYER *player2, int *clickFlag);
//void Help();
//void Rank();
int returnToMenu(int *clickFlag);
void Exit();
void drawBackground();
void gameStart();

//判断函数
void judgeState(struct ITEM *item, struct PLAYER *player);
int judgeOver(struct PLAYER *player1, struct PLAYER *player2);
void itemMove(struct ITEM *item);
int playerMove(struct PLAYER *player);
int playerMove2(struct PLAYER *player);
int AIMove(struct PLAYER *player, struct ITEM *item);

//其他函数
void playerBubbleSort(struct PLAYER a[], int len);
void Save(struct PLAYER *player);
void showScore();

//OpenCV 独有
void overlayImage(Mat* src, Mat* overlay, const Point& location);
void Move();
static void menuOnMouse(int EVENT, int x, int y, int flags, void *userdata);
static void gameOnMouse(int EVENT, int x, int y, int flags, void *userdata);
static void overOnMouse(int EVENT, int x, int y, int flags, void *userdata);

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
	char name[10] = { '\0' }; //玩家名称
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
	5,//player
	1,//cake
	4,//umbrella
	3//bomb
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

Mat allBackground(600, 800, CV_8UC3, Scalar(0, 0, 0)), startBackground(600, 800, CV_8UC3, Scalar(0, 0, 0)), startBackgroundBackUp, overBackground(600, 800, CV_8UC3, Scalar(0, 0, 0)), start_b_h, start_b_n, help_b_h, help_b_n, mode_b_h, mode_b_n, settings_b_h, settings_b_n, rank_b_h, rank_b_n, exit_b_h, exit_b_n;
Mat img_player, img_umbrella, img_bomb, img_cake, player_m, umbrella_m, bomb_m, cake_m;
const String windowName("测试");

int main()
{
	windowInit();
	loadAllImages();
	
	imshow(windowName, startBackground);
	
	Menu();

	destroyAllWindows();
	return 0;
}

//初始化

void windowInit()
{
	//初始化窗口并将分辨率设为指定大小
	namedWindow(windowName, WINDOW_NORMAL);
	resizeWindow(windowName, WIDTH, HEIGHT);
	imshow(windowName, allBackground);
}

void gameFramwork(struct Settings settings)
{
	// TODO 应该先数据再界面
	//界面初始化
	//setbkmode(TRANSPARENT);//设置文字背景透明
	line(startBackground, Point(0, BORDER), Point(WIDTH, BORDER), Scalar(255, 255, 255));
	//开始按钮
	Rect start(30, BORDER + 40, 70, 40);
	rectangle(startBackground, Point(30, BORDER + 40), Point(100, BORDER + 80), Scalar(255, 255, 255));
	putText(startBackground, String("Pause"), Point(30, BORDER + 40 + 40), CV_FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
	//结束按钮
	Rect end(150, BORDER + 40, 70, 40);
	rectangle(startBackground, Point(150, BORDER + 40), Point(150 + 70, BORDER + 80), Scalar(255, 255, 255));
	putText(startBackground, String("Menu"), Point(150, BORDER + 80), CV_FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
	//分数
	//玩家1
	Rect scoreTextAera(WIDTH - 300, BORDER + 40 - 20, 70, 20);
	Rect playerNameAera(WIDTH - 300, BORDER + 80 - 20, 70, 20);
	rectangle(startBackground, Point(WIDTH - 300, BORDER + 40 - 20), Point(WIDTH - 180, BORDER + 80 - 20), Scalar(255, 255, 255));
	rectangle(startBackground, Point(WIDTH - 300, BORDER + 80 - 20), Point(WIDTH - 180, BORDER + 120 - 20), Scalar(255, 255, 255));

	putText(startBackground, String("Score:"), Point(WIDTH - 300, BORDER + 80 - 20), CV_FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
	putText(startBackground, String("Player1:"), Point(WIDTH - 300, BORDER + 120 - 20), CV_FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));

	//玩家2
	if (settings.mode == 2)
	{
		//RECT scoreTextAera2 = { WIDTH - 180, BORDER + 40 - 20, WIDTH - 110, BORDER + 80 - 20 };
		//RECT playerNameAera2 = { WIDTH - 180, BORDER + 80 - 20, WIDTH - 110, BORDER + 120 - 20 };
		rectangle(startBackground, Point(WIDTH - 180, BORDER + 40 - 20), Point(WIDTH - 60, BORDER + 80 - 20), Scalar(255, 255, 255));
		rectangle(startBackground, Point(WIDTH - 180, BORDER + 80 - 20), Point(WIDTH - 60, BORDER + 120 - 20), Scalar(255, 255, 255));

		putText(startBackground, String("Score:"), Point(WIDTH - 180, BORDER + 80 - 20), CV_FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
		putText(startBackground, String("Player2:"), Point(WIDTH - 180, BORDER + 120 - 20), CV_FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
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
		item->y = 0;
		//item->y = -(rand() % 550);
		item = get_next_item();
	}
}

void loadAllImages()
{
	startBackground = (600, 800, CV_8UC3, Scalar(0, 0, 0));
	startBackgroundBackUp = imread("img\\bkg.png");
	start_b_h = imread("img\\buttons\\start_h.png");
	start_b_n = imread("img\\buttons\\start_n.png");
	help_b_h = imread("img\\buttons\\help_h.png");
	help_b_n = imread("img\\buttons\\help_n.png");
	mode_b_h = imread("img\\buttons\\mode_h.png");
	mode_b_n = imread("img\\buttons\\mode_n.png");
	settings_b_h = imread("img\\buttons\\settings_h.png");
	settings_b_n = imread("img\\buttons\\settings_n.png");
	rank_b_h = imread("img\\buttons\\rank_h.png");
	rank_b_n = imread("img\\buttons\\rank_n.png");
	exit_b_h = imread("img\\buttons\\exit_h.png");
	exit_b_n = imread("img\\buttons\\exit_n.png");
	img_player = imread("img\\main\\player.bmp");
	img_umbrella = imread("img\\main\\umbrella.bmp");
	img_bomb = imread("img\\main\\bomb.bmp");
	img_cake = imread("img\\main\\cake.bmp");
	player_m = imread("img\\main\\player_b.bmp");
	umbrella_m = imread("img\\main\\umbrella_b.bmp");
	bomb_m = imread("img\\main\\bomb_b.bmp");
	cake_m = imread("img\\main\\cake_b.bmp");
	
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
	newItem->y = 0;
	//newItem->y = -(rand() % 550);
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
		//TODO 取消了-20
		drawTransparent(startBackground, img_umbrella, umbrella_m, (int)item->x, (int)item->y);

		break;
	case CAKE:
		drawTransparent(startBackground, img_cake, cake_m, (int)item->x, (int)item->y);

		break;
	case BOMB:
		drawTransparent(startBackground, img_bomb, bomb_m, (int)item->x, (int)item->y);

		break;
	}
}

void coverItem(int x, int y, enum TYPE type)
{
	// TODO 待修改
	switch (type)
	{
	case UMBRELLA:
		
		break;
	case CAKE:
		
		break;
	case BOMB:
		
		break;
	}
}

void drawPlayer(struct PLAYER *player, int direction)
{
	// TODO 待修改
	/*int flag = 0;
	switch (direction)
	{
	case LEFT: flag = -1; break;
	case RIGHT: flag = 1; break;
	}*/
	drawTransparent(startBackground, img_player, player_m, (int)player->x - 20, (int)player->y - 20);
}

void drawPlayer2(struct PLAYER *player, int direction)
{
	// TODO 待修改
	/*int flag = 0;
	switch (direction)
	{
	case LEFT: flag = -1; break;
	case RIGHT: flag = 1; break;
	}*/
	drawTransparent(startBackground, img_player, player_m, (int)player->x - 20, (int)player->y - 20);
}

void drawScore(int score, int score2)
{
	char scoreNumber[10];
	sprintf_s(scoreNumber, "%d", score);
	putText(startBackground, scoreNumber, Point(WIDTH - 240, BORDER + 80 - 20), CV_FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
		//玩家2
	if (settings.mode == 2)
	{
		char scoreNumber2[10];
		sprintf_s(scoreNumber2, "%d", score2);
		putText(startBackground, scoreNumber2, Point(WIDTH - 120, BORDER + 80 - 20), CV_FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
		//RECT scoreNumberAera2 = { WIDTH - 180, BORDER + 40 - 20, WIDTH - 30, BORDER + 80 - 20 };
	}
}

void drawPlayerName(struct PLAYER *player, struct PLAYER *player2)
{
	//RECT playerNameAera = { WIDTH - 300, BORDER + 80 - 20, WIDTH - 150, BORDER + 120 - 20 };
	putText(startBackground, player->name, Point(WIDTH - 240, BORDER + 120 - 20), CV_FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
	//玩家2	
	if (settings.mode == 2)
	{
		putText(startBackground, player->name, Point(WIDTH - 120, BORDER + 120 - 20), CV_FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
		//RECT playerNameAera2 = { WIDTH - 180, BORDER + 80 - 20, WIDTH - 30, BORDER + 120 - 20 };
	}
}

int drawOver(struct PLAYER *player1, struct PLAYER *player2)
{
	overBackground = (600, 800, CV_8UC3, Scalar(0, 0, 0));
	int clickFlag = 0;
	setMouseCallback(windowName, overOnMouse, &clickFlag);
	while (1)
	{
		char score[20] = { '\0' };
		sprintf_s(score, "Your score is: %d", player1->score);
		putText(overBackground, String("Game Over"), Point(300, 300), CV_FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
		putText(overBackground, (String)score, Point(300, 320), CV_FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));

		//RECT saveScore = { 200, 400, 200 + 100, 400 + 40 };
		putText(overBackground, String("Save Score"), Point(200, 400 + 40), CV_FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));

		//RECT toMenu = { 200 + 110, 400, 200 + 100 + 110, 400 + 40 };
		putText(overBackground, String("Return To Menu"), Point(200 + 110, 400 + 40), CV_FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));

		if (clickFlag == 1)
			return 1;
		if (clickFlag == 2)
			Save(player1);
		clickFlag = 0;
		imshow(windowName, overBackground);
		waitKey(33);
	}
	return 0;
}

void drawTransparent(Mat image, Mat logo, Mat mask, int x, int y)
{
	Mat imageROI;
	imageROI = image(Rect(x, y, logo.cols, logo.rows));
	logo.copyTo(imageROI, mask);
}

void drawImage(Mat image, Mat logo, int x, int y)
{
	Mat imageROI;
	imageROI = image(Rect(x, y, logo.cols, logo.rows));
	logo.copyTo(imageROI, logo);
}

//界面函数
void Menu()
{
	Point p(0, 0);
	while (1)
	{
		drawBackground();
		//TODO 需要
		
		//开始
		Rect start(350, 120, 100, 50);
		drawImage(startBackground, start_b_h, 250, 120);
		
		//帮助
		Rect help(350, 180, 100, 50);
		drawImage(startBackground, help_b_h, 250, 120 + 70);
		
		//设置/模式
		//RECT mode = { 350, 240, 350 + 100, 240 + 50 };
		drawImage(startBackground, mode_b_h, 250, 120 + 70 + 70);

		//单人
		Rect singleplayer(460, 240, 100, 50);
		rectangle(startBackground, singleplayer, Scalar(255, 255, 255));
		putText(startBackground, String("Single"), Point(460, 240), CV_FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
		//双人
		Rect multiplayer(570, 240, 100, 50);
		rectangle(startBackground, multiplayer, Scalar(255, 255, 255));
		putText(startBackground, String("Multiplayer"), Point(570, 240), CV_FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
		//rectangle(570, 240, 570 + 100, 240 + 50);
		//drawtext(_T("双人"), &multiplayer, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
		//AI
		Rect ai(680, 240, 100, 50);
		rectangle(startBackground, ai, Scalar(255, 255, 255));
		putText(startBackground, String("AI"), Point(680, 240), CV_FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
		//drawtext(_T("AI"), &ai, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
		//排名
		//RECT rank = { 350, 300, 350 + 100, 300 + 50 };
		drawImage(startBackground, rank_b_h, 250, 120 + 70 + 70 + 70);
		//退出
		//RECT exit = { 350, 360, 350 + 100, 360 + 50 };
		drawImage(startBackground, exit_b_h, 250, 120 + 70 + 70 + 70 + 70);

		circle(startBackground, p, 10, Scalar(0, 255, 255), 3);
		switch (settings.mode)
		{
		case 1:circle(startBackground, Point(460, 240), 3, Scalar(255, 255, 0), 3); break;
		case 2:circle(startBackground, Point(570, 240), 3, Scalar(255, 255, 0), 3);  break;
		case 3:circle(startBackground, Point(680, 240), 3, Scalar(255, 255, 0), 3); break;
		}

		setMouseCallback(windowName, menuOnMouse, &p);
		imshow(windowName, startBackground);
		waitKey(1);

	}

	//EndBatchDraw();
}

int Pause(struct PLAYER *player1, struct PLAYER *player2, int *clickFlag)
{
	while (*clickFlag)
	{
		drawBackground();
		gameFramwork(settings);
		putText(startBackground, String("Continue"), Point(30, BORDER + 40 + 40), CV_FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
		imshow(windowName, startBackground);
		waitKey(33);
	}
	
	return 0;
}

int returnToMenu(int *clickFlag)
{
	if (*clickFlag == 2)
		return 1;
	return 0;
}

void Exit()
{
	destroyAllWindows();
	exit(0);
}

void drawBackground()
{
	startBackground = (600, 800, CV_8UC3, Scalar(0, 0, 0)); //将startBackground重置为背景图，相当于画了一遍背景，因为不知道overlayImage()效率如何
	drawImage(startBackground, startBackgroundBackUp, 0, 0);
	//imshow(windowName, startBackground);
}

void gameStart()
{
	//cleardevice();
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


	/*switch (settings.mode)
	{
	case 1:	InputBox((LPTSTR)&(player1->name), 10, (LPCTSTR)"请输入名字："); break;
	case 2: {
		InputBox((LPTSTR)&(player1->name), 10, (LPCTSTR)"请输入玩家1的名字：");
		InputBox((LPTSTR)&(player2->name), 10, (LPCTSTR)"请输入玩家2的名字：");
	}
			break;
	case 3:	player1->name[0] = 'A'; player1->name[1] = 'I'; player1->name[2] = '\0'; break;
	}*/
	int clickFlag = 0;
	setMouseCallback(windowName, gameOnMouse, &clickFlag);
	while (1)
	{
		for (int i = ItemNumber - ItemCount; i > 0; i--)
			addItem();

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
			if (returnToMenu(&clickFlag) || Pause(player1, player2, &clickFlag))
				return;

			judgeState(item, player1);
			if (settings.mode != 3 && judgeOver(player1, player2))
					return;
			if (settings.mode == 2)
				judgeState(item, player2);
			//储存item里临时的数据，因为删除之后就无法读取item，无法在图形上删除item了
			int temp_x = (int)item->x;
			int temp_y = (int)item->y;
			enum TYPE temp_type = item->type;

			if (delItem())
			{
				//coverItem(temp_x, temp_y, temp_type);
			}
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
			item = get_next_item();
		}
		drawPlayerName(player1, player2);
		drawScore(player1->score, player2->score);

		imshow(windowName, startBackground);
		waitKey(1);
	}
}

//判断函数
void judgeState(struct ITEM *item, struct PLAYER *player)
{
	//TODO 取消-20
	if (item->y >= 480 - 70 - speed.umbrella)
	{
		item->state = OFF_SCREEN;
		return;
	}
	int distance = (int)sqrt((player->x - item->x)*(player->x - item->x) + (player->y - item->y)*(player->y - item->y));
	if (distance <= 80)
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
	{
		setMouseCallback(windowName, NULL, NULL);
		if (drawOver(player1, player2))
			return 1;
	}

	return 0;
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
//	char c = waitKey(1);
	int key = 0;
	if (/*c == 97*/GetAsyncKeyState(0x41))
	{
		player->x -= speed.player;
		if (player->x <= 0 + speed.player || player->x >= WIDTH - speed.player) //边界检测
			player->x += speed.player;
		return LEFT;
	}
	if (/*c == 100*/GetAsyncKeyState(0x44))
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
		putText(overBackground, String("Save Failed."), Point(50, 50), CV_FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
		return;
	}
	if (player->name[0] == '\0')
		strcpy(player->name, "NULL");
	
	fwrite(player, sizeof(struct PLAYER), 1, fp);
	fclose(fp);
	putText(overBackground, String("Saved Successfully."), Point(50, 50), CV_FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
	imshow(windowName, overBackground);
	waitKey(33);
	showScore();
}

void showScore()
{
	int const rankNumber = 10;
	int lineCount = 0;
	char rankLine[15][50] = { '\0' };
	struct PLAYER *pplayer = NULL;
	FILE *fp = NULL;
	pplayer = (struct PLAYER *)malloc(sizeof(struct PLAYER)*rankNumber);

	if (fopen_s(&fp, "data", "ab+"))
	{
		putText(overBackground, String("File Reading Failed."), Point(50, 50), CV_FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255)); 
		imshow(windowName, overBackground);
		waitKey(33);
		return;
	}
	
	for (int i = 0; i < rankNumber; i++, lineCount++)
	{
		fread(&pplayer[i], sizeof(struct PLAYER), 1, fp);
		if (feof(fp))
			break;
	}
	
	playerBubbleSort(pplayer, lineCount);

	for (int i = 0; i < lineCount; i++)
	{
		sprintf(rankLine[i], "Name: %-10s, Score: %d", pplayer[i].name, pplayer[i].score);
		putText(overBackground, (String)rankLine[i], Point(50, 100 + i * 20), CV_FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255)); 
	}
	imshow(windowName, overBackground);
	waitKey(33);
	free(pplayer);
	pplayer = NULL;
	fclose(fp);
}

void overlayImage(Mat* src, Mat* overlay, const Point& location)
{
	for (int y = max(location.y, 0); y < src->rows; ++y)
	{
		int fY = y - location.y;

		if (fY >= overlay->rows)
			break;

		for (int x = max(location.x, 0); x < src->cols; ++x)
		{
			int fX = x - location.x;

			if (fX >= overlay->cols)
				break;

			double opacity = ((double)overlay->data[fY * overlay->step + fX * overlay->channels() + 3]) / 255;

			for (int c = 0; opacity > 0 && c < src->channels(); ++c)
			{
				unsigned char overlayPx = overlay->data[fY * overlay->step + fX * overlay->channels() + c];
				unsigned char srcPx = src->data[y * src->step + x * src->channels() + c];
				src->data[y * src->step + src->channels() * x + c] = srcPx * (1. - opacity) + overlayPx * opacity;
			}
		}
	}
}

void Move()
{
	for (int i = 0; i < 70; i+=1)
	{
		overlayImage(&startBackground, &startBackgroundBackUp, Point(0, 0));
		//overlayImage(&startBackground, &exit_b_n, Point(250, 120 + 70 + 70 + 70 + 70 + i));
		
		imshow(windowName, startBackground);
		cvWaitKey(100);
	}
}

static void menuOnMouse(int EVENT, int x, int y, int flags, void *userdata)
{
	Point *p = (Point *)userdata;

	if (350 <= x && x <= 350 + 100 && 120 <= y && y <= 120 + 50) //开始
	{
		p->x = 350;
		p->y = 120;
		if (EVENT == EVENT_LBUTTONDOWN)
		{
			setMouseCallback(windowName, NULL, NULL);
			gameStart();
		}
	}
	else if (350 <= x && x <= 350 + 100 && 180 <= y && y <= 180 + 50) //帮助
	{
		p->x = 350;
		p->y = 180;
		if (EVENT == EVENT_LBUTTONDOWN)
		{
			/*setMouseCallback(windowName, NULL, NULL);
			gameStart();*/
		}
	}
	else if (350 <= x && x <= 350 + 100 && 240 <= y && y <= 240 + 50) //设置
	{
		p->x = 350;
		p->y = 240;
		if (EVENT == EVENT_LBUTTONDOWN)
		{
			/*setMouseCallback(windowName, NULL, NULL);
			gameStart();*/
		}
	}
	else if (460 <= x && x <= 460 + 100 && 240 <= y && y <= 240 + 50) //单人
	{
		p->x = 460;
		p->y = 240;
		if (EVENT == EVENT_LBUTTONDOWN)
		{
			settings.mode = 1;
			/*setMouseCallback(windowName, NULL, NULL);
			gameStart();*/
		}
	}
	else if (570 <= x && x <= 570 + 100 && 240 <= y && y <= 240 + 50) //双人
	{
		p->x = 570;
		p->y = 240;
		if (EVENT == EVENT_LBUTTONDOWN)
		{
			settings.mode = 2;
			/*setMouseCallback(windowName, NULL, NULL);
			gameStart();*/
		}
	}
	else if (680 <= x && x <= 680 + 100 && 240 <= y && y <= 240 + 50) //AI
	{
		p->x = 680;
		p->y = 240;
		if (EVENT == EVENT_LBUTTONDOWN)
		{
			settings.mode = 3;
			circle(startBackground, Point(680, 240), 10, Scalar(255, 255, 255), 3);
			/*setMouseCallback(windowName, NULL, NULL);
			gameStart();*/
		}
	}
	else if (350 <= x && x <= 350 + 100 && 300 <= y && y <= 300 + 50) //排名
	{
		p->x = 350;
		p->y = 300;
		if (EVENT == EVENT_LBUTTONDOWN)
		{
			setMouseCallback(windowName, NULL, NULL);
			showScore();
		}
	}
	else if (350 <= x && x <= 350 + 100 && 360 <= y && y <= 360 + 50) //排名
	{
		p->x = 350;
		p->y = 360;
		if (EVENT == EVENT_LBUTTONDOWN)
		{
			setMouseCallback(windowName, NULL, NULL);
			Exit();
		}
	}
	else
	{
		p->x = 0;
		p->y = 0;
	}
}

static void gameOnMouse(int EVENT, int x, int y, int flags, void *userdata)
{
	int pauseFlag = 0;
	int *clickFlag = (int *)userdata;

	//暂停
	if (30 <= x && x <= 100 && (BORDER + 40) <= y && y <= (BORDER + 80) && EVENT == EVENT_LBUTTONDOWN)
		*clickFlag = !*clickFlag;

	//开始
	if ((150 <= x && x <= 220) && ((BORDER + 40) <= y && y <= (BORDER + 80)) && EVENT == EVENT_LBUTTONDOWN)
		*clickFlag = 2;
}

static void overOnMouse(int EVENT, int x, int y, int flags, void *userdata)
{
	int *clickFlag = (int *)userdata;
	if ((200 + 110) <= x && x <= (200 + 100 + 110) && 400 <= y && y <= (400 + 40) && EVENT == EVENT_LBUTTONDOWN)
		*clickFlag = 1;
	if ((200) <= x && x <= (200 + 100) && 400 <= y && y <= (400 + 40) && EVENT == EVENT_LBUTTONDOWN)
		*clickFlag = 2;

}