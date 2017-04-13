//解决台式机问题的代码
//#include "stdio.h"
#ifdef __cplusplus 
extern "C"
#endif 
/*
FILE* __cdecl __iob_func(unsigned i) {
return __acrt_iob_func(i);
}
*/

//#include <graphics.h>
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
//#include <conio.h>
#include <math.h>
#include <time.h>

#include <highgui.h>
#include <opencv2/opencv.hpp>
#include "game_struct.h"
using namespace cv;

#define HEIGHT 600
#define WIDTH 800
#define BORDER 480
#define LEFT 1
#define RIGHT 2


//初始化
void windowInit(); //OpenCV改写
void gameFramwork(); //OpenCV改写
void initPlayer(struct PLAYER *player1, struct PLAYER *player2);
void initItemToZero();
void loadAllImages(); //OpenCV改写
void enterName(struct PLAYER * player1); //OpenCV改写

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
void drawTime(int time);
int drawOver(struct PLAYER *player1, struct PLAYER *player2);
void drawTransparent(Mat image, Mat logo, Mat mask, int x, int y);
void drawImage(Mat image, Mat logo, int x, int y);

//界面函数 //OpenCV改写
void Menu();
clock_t Pause(struct PLAYER *player1, struct PLAYER *player2, int *clickFlag, int time);
void Help();
void Rank();
int returnToMenu(int *clickFlag);
void Exit();
void drawBackground();
void gameStart();

//判断函数
void judgeState(struct ITEM *item, struct PLAYER *player);
int judgeOver(struct PLAYER *player1, struct PLAYER *player2, int time);
void itemMove(struct ITEM *item);
int playerMove(struct PLAYER *player);
int playerMove2(struct PLAYER *player);
int AIMove(struct PLAYER *player, struct ITEM *item);

//其他函数
void playerBubbleSort(struct PLAYER a[], int len);
void Save(struct PLAYER *player);
void showScore();
void rewriteScore(struct PLAYER * pplayer, int lineCount);
int countDown(clock_t startTime, clock_t pauseTime);
enum TYPE randomType();

//OpenCV鼠标回调函数
static void menuOnMouse(int EVENT, int x, int y, int flags, void *userdata);
static void gameOnMouse(int EVENT, int x, int y, int flags, void *userdata);
static void overOnMouse(int EVENT, int x, int y, int flags, void *userdata);
static void returnOnMouse(int EVENT, int x, int y, int flags, void *userdata);

int ItemCount = 0;
const int ItemNumber = 10;//屏幕上物品的总数

Mat allBackground(600, 800, CV_8UC3, Scalar(0, 0, 0)), startBackground(600, 800, CV_8UC3, Scalar(0, 0, 0)), startBackgroundBackUp, overBackground(600, 800, CV_8UC3, Scalar(0, 0, 0)), overBackgroundBackUp, rankBackground(600, 800, CV_8UC3, Scalar(255, 255, 255)), helpBackground(600, 800, CV_8UC3, Scalar(255, 255, 255)), enterBackground(600, 800, CV_8UC3, Scalar(255, 255, 255)), title, title_m;
Mat start_b_h, start_b_n, help_b_h, help_b_n, mode_b_h, mode_b_n, settings_b_h, settings_b_n, rank_b_h, rank_b_n, exit_b_h, exit_b_n, single, single_n, select_, back_b, back_b_m, gstart, gstart_m, gpause, gpause_m;
Mat start_m, help_m, mode_m, settings_m, rank_m, exit_m, single_m, select_m, first;
Mat img_player, img_playerR, img_playerS, img_player2, img_player2R, img_player2S, player_m, playerR_m, playerS_m, img_umbrella, img_bomb, img_cake, img_explode, umbrella_m, bomb_m, cake_m, explode_m;
const String windowName("躲避横祸");

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

void gameFramwork()
{
	//界面初始化
	line(startBackground, Point(0, BORDER), Point(WIDTH, BORDER), Scalar(255, 255, 255));

	//开始按钮
	drawTransparent(startBackground, gpause, gpause_m, 30, BORDER + 40);

	//结束按钮
	Rect end(150, BORDER + 40, 70, 40);
	rectangle(startBackground, Point(150, BORDER + 40), Point(150 + 70, BORDER + 80), Scalar(255, 255, 255));
	putText(startBackground, String("Menu"), Point(150 + 10, BORDER + 80 - 15), CV_FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));

	//时间
	Rect time(270, BORDER + 40, 70, 40);
	rectangle(startBackground, Point(270, BORDER + 40), Point(270 + 100, BORDER + 80 + 20), Scalar(255, 255, 255));
	putText(startBackground, String("Time Left: "), Point(270 + 10, BORDER + 80 - 20), CV_FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));

	//分数
	//玩家1
	Rect scoreTextAera(WIDTH - 300, BORDER + 40 - 20, 70, 20);
	Rect playerNameAera(WIDTH - 300, BORDER + 80 - 20, 70, 20);
	rectangle(startBackground, Point(WIDTH - 300, BORDER + 40 - 20), Point(WIDTH - 180 + 25, BORDER + 80 - 20), Scalar(255, 255, 255));
	rectangle(startBackground, Point(WIDTH - 300, BORDER + 80 - 20), Point(WIDTH - 180 + 25, BORDER + 120 - 20), Scalar(255, 255, 255));

	putText(startBackground, String("Score:"), Point(WIDTH - 300 + 5, BORDER + 80 - 20 - 10), CV_FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
	putText(startBackground, String("Player1:"), Point(WIDTH - 300 + 5, BORDER + 120 - 20 - 10), CV_FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));

	//玩家2
	if (settings.mode == 2)
	{
		rectangle(startBackground, Point(WIDTH - 180 + 25, BORDER + 40 - 20), Point(WIDTH - 60 + 40 + 10, BORDER + 80 - 20), Scalar(255, 255, 255));
		rectangle(startBackground, Point(WIDTH - 180 + 25, BORDER + 80 - 20), Point(WIDTH - 60 + 40 + 10, BORDER + 120 - 20), Scalar(255, 255, 255));

		putText(startBackground, String("Score:"), Point(WIDTH - 180 + 30, BORDER + 80 - 20 - 10), CV_FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
		putText(startBackground, String("Player2:"), Point(WIDTH - 180 + 30, BORDER + 120 - 20 - 10), CV_FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
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
		item = get_next_item();
	}
}

void loadAllImages()
{
	startBackground = (600, 800, CV_8UC3, Scalar(0, 0, 0));
	overBackground = imread("img/over.bmp");
	overBackgroundBackUp = imread("img/over.bmp");
	helpBackground = imread("img/help.jpg");
	title = imread("img/title.bmp");
	title_m = imread("img/title_m.bmp");
	//rankBackground = imread("img/rankbackground.bmp");
	startBackgroundBackUp = imread("img/bkg.png");
	start_b_h = imread("img/buttons/start_h.bmp");
	start_b_n = imread("img/buttons/start_n.bmp");
	start_m = imread("img/buttons/start_m.bmp");
	help_b_h = imread("img/buttons/help_h.bmp");
	help_b_n = imread("img/buttons/help_n.bmp");
	help_m = imread("img/buttons/help_m.bmp");
	mode_b_h = imread("img/buttons/mode_h.bmp");
	mode_b_n = imread("img/buttons/mode_n.bmp");
	mode_m = imread("img/buttons/mode_m.bmp");
	settings_b_h = imread("img/buttons/settings_h.bmp");
	settings_b_n = imread("img/buttons/settings_n.bmp");
	settings_m = imread("img/buttons/settings_m.bmp");
	rank_b_h = imread("img/buttons/rank_h.bmp");
	rank_b_n = imread("img/buttons/rank_n.bmp");
	rank_m = imread("img/buttons/rank_m.bmp");
	exit_b_h = imread("img/buttons/exit_h.bmp");
	exit_b_n = imread("img/buttons/exit_n.bmp");
	exit_m = imread("img/buttons/exit_m.bmp");
	back_b = imread("img/buttons/back.bmp");
	back_b_m = imread("img/buttons/back_m.bmp");
	gstart = imread("img/buttons/gstart.bmp");
	gstart_m = imread("img/buttons/gstart_m.bmp");
	gpause = imread("img/buttons/gpause.bmp");
	gpause_m = imread("img/buttons/gstart_m.bmp");

	select_ = imread("img/buttons/select.bmp");
	select_m = imread("img/buttons/select_m.bmp");
	first = imread("img/first.jpg");

	img_player = imread("img/main/player.bmp");
	img_playerR = imread("img/main/playerR.bmp");
	img_playerS = imread("img/main/playerS.bmp");
	img_player2 = imread("img/main/player2.bmp");
	img_player2R = imread("img/main/player2R.bmp");
	img_player2S = imread("img/main/player2S.bmp");
	player_m = imread("img/main/player_m.bmp");
	playerR_m = imread("img/main/playerR_m.bmp");
	playerS_m = imread("img/main/playerS_m.bmp");

	img_bomb = imread("img/main/bomb.bmp");
	img_cake = imread("img/main/cake.bmp");
	img_umbrella = imread("img/main/umbrella.bmp");
	img_explode = imread("img/main/explode.bmp");
	umbrella_m = imread("img/main/umbrella_m.bmp");
	bomb_m = imread("img/main/bomb_m.bmp");
	cake_m = imread("img/main/cake_m.bmp");
	explode_m = imread("img/main/explode_m.bmp");
}

void enterName(struct PLAYER * player1)
{
	int i = 0;
	char c = 0;
	char name[13] = { '\0' };
	int charCount = 0;
	
	for (i = 0; i < 10; i++)
	{
		putText(enterBackground, String("Enter Name: (No more than 9 words)"), Point(30, 70 + 100), CV_FONT_HERSHEY_PLAIN, 2.5, Scalar(0, 0, 0));
		line(enterBackground, Point(100, 260), Point(300, 260), Scalar(0, 0, 0));
		putText(enterBackground, String("Press ENTER to start the game."), Point(50, 300), CV_FONT_HERSHEY_PLAIN, 2, Scalar(0, 0, 0));
		imshow(windowName, enterBackground);
		enterBackground = (600, 800, CV_8UC3, Scalar(255, 255, 255));
		c = waitKey(0);
		if (c == 13)//回车
		{
			name[i] = '\0';
			strcpy(player1->name, name);
			return;
		}
		if (c == 8)//退格
		{
			if (i == 0) //健壮性
			{
				i--;
				continue;
			}
			name[--i] = '\0';
			putText(enterBackground, (String)name, Point(100, 260), CV_FONT_HERSHEY_PLAIN, 2, Scalar(0, 0, 0));
			imshow(windowName, enterBackground);
			i--;
			continue;
		}
		name[i] = c;
		putText(enterBackground, (String)name, Point(100, 260), CV_FONT_HERSHEY_PLAIN, 2, Scalar(0, 0, 0));
	}
	name[i] = '\0';
	strcpy(player1->name, name);
	return;
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
	newItem->type = randomType();
	newItem->state = ON_SCREEN;
	newItem->x = 200 + rand() % 400;
	newItem->y = 0;
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
	curr_list = curr_list->next;
	return curr_list->this_item;
}

//画图函数
void drawItem(struct ITEM *item)
{
	switch (item->type)
	{
	case UMBRELLA:
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
	if (type == BOMB)
		drawTransparent(startBackground, img_explode, explode_m, x, y);
}

void drawPlayer(struct PLAYER *player, int direction)
{
	switch (direction)
	{
	case LEFT:	drawTransparent(startBackground, img_player, player_m, (int)player->x - 20, (int)player->y - 20);	break;
	case RIGHT: drawTransparent(startBackground, img_playerR, playerR_m, (int)player->x - 20, (int)player->y - 20); break;
	default:	drawTransparent(startBackground, img_playerS, playerS_m, (int)player->x - 20, (int)player->y - 20); break;
	}
}

void drawPlayer2(struct PLAYER *player, int direction)
{
	switch (direction)
	{
	case LEFT:	drawTransparent(startBackground, img_player2, player_m, (int)player->x - 20, (int)player->y - 20);	break;
	case RIGHT: drawTransparent(startBackground, img_player2R, playerR_m, (int)player->x - 20, (int)player->y - 20); break;
	default:	drawTransparent(startBackground, img_player2S, playerS_m, (int)player->x - 20, (int)player->y - 20); break;
	}
}

void drawScore(int score, int score2)
{
	char scoreNumber[10];
	sprintf(scoreNumber, "%d", score);
	putText(startBackground, scoreNumber, Point(WIDTH - 240 + 10, BORDER + 80 - 20 - 10), CV_FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
	//玩家2
	if (settings.mode == 2)
	{
		char scoreNumber2[10];
		sprintf(scoreNumber2, "%d", score2);
		putText(startBackground, scoreNumber2, Point(WIDTH - 120 + 10 + 20, BORDER + 80 - 20 - 10), CV_FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
	}
}

void drawPlayerName(struct PLAYER *player, struct PLAYER *player2)
{
	//玩家1或AI
	putText(startBackground, player->name, Point(WIDTH - 230, BORDER + 120 - 20 - 10), CV_FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));

	//玩家2	
	if (settings.mode == 2)
		putText(startBackground, player->name, Point(WIDTH - 120 + 40, BORDER + 120 - 20 - 10), CV_FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
}

void drawTime(int time)
{
	char a[5] = { '\n' };
	sprintf(a, "%d", time);
	if(time <= 10)
		putText(startBackground, a, Point(270 + 40, BORDER + 40 + 50), CV_FONT_HERSHEY_PLAIN, 1.5, Scalar(0, 0, 255), 2);
	else
		putText(startBackground, a, Point(270 + 40, BORDER + 40 + 50), CV_FONT_HERSHEY_PLAIN, 1.1, Scalar(255, 255, 255));
}

int drawOver(struct PLAYER *player1, struct PLAYER *player2)
{
	int clickFlag = 0;
	
	char score[30] = { '\0' };
	char score2[30] = { '\0' };
	if(settings.mode != 2)
		sprintf(score, "Your score is: %d", player1->score);
	else
	{
		sprintf(score, "Player1's score is: %d", player1->score);
		sprintf(score2, "Player2's score is: %d", player2->score);
	}

	overBackground = (600, 800, CV_8UC3, Scalar(0, 0, 0));
	drawImage(overBackground, overBackgroundBackUp, 0, 0);
	while (1)
	{
		setMouseCallback(windowName, overOnMouse, &clickFlag);
		putText(overBackground, (String)score, Point(325, 320), CV_FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
		if(settings.mode == 2)
			putText(overBackground, (String)score2, Point(325, 345), CV_FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
		putText(overBackground, String("Save Score"), Point(200, 400 + 40), CV_FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
		putText(overBackground, String("Show Rank"), Point(200, 400 + 40 + 40), CV_FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
		putText(overBackground, String("Return To Menu"), Point(200 + 110, 400 + 40), CV_FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));

		if (clickFlag == 1)
			return 1;
		if (clickFlag == 2)
			Save(player1);
		if (clickFlag == 3)
		{
			setMouseCallback(windowName, NULL, NULL);
			Rank();
		}
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
		drawTransparent(startBackground, title, title_m, 200, 25);
		//开始
		Rect start(350, 120, 100, 50);
		drawTransparent(startBackground, start_b_h, start_m, 250, 120);

		//帮助
		Rect help(350, 180, 100, 50);
		drawTransparent(startBackground, help_b_h, help_m, 250, 120 + 70);

		//设置/模式
		drawTransparent(startBackground, settings_b_h, settings_m, 250, 120 + 70 + 70);

		switch (settings.difficult)
		{
		case 1:drawTransparent(startBackground, select_, select_m, 400 - 280 - 100, 120 + 70 + 70 + 70 + 10); break;
		case 2:drawTransparent(startBackground, select_, select_m, 400 - 170 - 100, 120 + 70 + 70 + 70 + 10); break;
		case 3:drawTransparent(startBackground, select_, select_m, 400 - 60 - 100, 120 + 70 + 70 + 70 + 10); break;
		}
		switch (settings.mode)
		{
		case 1:drawTransparent(startBackground, select_, select_m, 460, 120 + 70 + 70 + 70 + 10); break;
		case 2:drawTransparent(startBackground, select_, select_m, 570, 120 + 70 + 70 + 70 + 10); break;
		case 3:drawTransparent(startBackground, select_, select_m, 680, 120 + 70 + 70 + 70 + 10); break;
		}

		//单人
		Rect singleplayer(460, 120 + 70 + 70 + 70 + 10, 100, 50);
		rectangle(startBackground, singleplayer, Scalar(255, 255, 255));
		putText(startBackground, String("Single"), Point(460 + 25, 120 + 70 + 70 + 70 + 10 + 30), CV_FONT_HERSHEY_PLAIN, 1.2, Scalar(255, 255, 255));
		//双人
		Rect multiplayer(570, 120 + 70 + 70 + 70 + 10, 100, 50);
		rectangle(startBackground, multiplayer, Scalar(255, 255, 255));
		putText(startBackground, String("Multiplayer"), Point(570, 120 + 70 + 70 + 70 + 10 + 30), CV_FONT_HERSHEY_PLAIN, 1.1, Scalar(255, 255, 255));

		//AI
		Rect ai(680, 120 + 70 + 70 + 70 + 10, 100, 50);
		rectangle(startBackground, ai, Scalar(255, 255, 255));
		putText(startBackground, String("AI"), Point(680 + 40, 120 + 70 + 70 + 70 + 10 + 30), CV_FONT_HERSHEY_PLAIN, 1.2, Scalar(255, 255, 255));

		//困难
		Rect difficult(400 - 60 - 100, 120 + 70 + 70 + 70 + 10, 100, 50);
		rectangle(startBackground, difficult, Scalar(255, 255, 255));
		putText(startBackground, String("Difficult"), Point(400 - 60 - 100, 120 + 70 + 70 + 70 + 10 + 30), CV_FONT_HERSHEY_PLAIN, 1.2, Scalar(255, 255, 255));

		//中等
		Rect medium(400 - 170 - 100, 120 + 70 + 70 + 70 + 10, 100, 50);
		rectangle(startBackground, medium, Scalar(255, 255, 255));
		putText(startBackground, String("Medium"), Point(400 - 170 - 100, 120 + 70 + 70 + 70 + 10 + 30), CV_FONT_HERSHEY_PLAIN, 1.1, Scalar(255, 255, 255));

		//简单
		Rect easy(400 - 280 - 100, 120 + 70 + 70 + 70 + 10, 100, 50);
		rectangle(startBackground, easy, Scalar(255, 255, 255));
		putText(startBackground, String("Easy"), Point(400 - 280 - 100, 120 + 70 + 70 + 70 + 10 + 30), CV_FONT_HERSHEY_PLAIN, 1.2, Scalar(255, 255, 255));

		//排名
		drawTransparent(startBackground, rank_b_h, rank_m, 250, 120 + 70 + 70 + 70 + 70);

		//退出
		drawTransparent(startBackground, exit_b_h, exit_m, 250, 120 + 70 + 70 + 70 + 70 + 70);
		switch (p.y)
		{
		case 120:							drawTransparent(startBackground, start_b_n, start_m, 250, 120); break;
		case 120 + 70:						drawTransparent(startBackground, help_b_n, help_m, 250, 120 + 70); break;
		case 120 + 70 + 70:					drawTransparent(startBackground, settings_b_n, settings_m, 250, 120 + 70 + 70); break;
		case 120 + 70 + 70 + 70 + 70:		drawTransparent(startBackground, rank_b_n, rank_m, 250, 120 + 70 + 70 + 70 + 70); break;
		case 120 + 70 + 70 + 70 + 70 + 70:	drawTransparent(startBackground, exit_b_n, exit_m, 250, 120 + 70 + 70 + 70 + 70 + 70); break;
		//case 0: break;
		}

		setMouseCallback(windowName, menuOnMouse, &p);
		imshow(windowName, startBackground);
		waitKey(1);

	}

}

clock_t Pause(struct PLAYER *player1, struct PLAYER *player2, int *clickFlag, int time)
{
	clock_t pauseStartTime = clock();
	while (*clickFlag == 1)
	{
		drawBackground();
		gameFramwork();
		drawTransparent(startBackground, gstart, gstart_m, 30, BORDER + 40);
		drawPlayer(player1, 3);
		if(settings.mode == 2)
			drawPlayer2(player2, 3);
		drawPlayerName(player1, player2);
		drawScore(player1->score, player2->score);
		drawTime(time);
		imshow(windowName, startBackground);
		waitKey(33);
	}
	clock_t pauseEndTime = clock();

	return (pauseEndTime - pauseStartTime);
}

void Help()
{
	int clickFlag = 0;
	setMouseCallback(windowName, returnOnMouse, &clickFlag);
	while (1)
	{
		drawTransparent(helpBackground, back_b, back_b_m, 340, HEIGHT - 70);
		imshow(windowName, helpBackground);
		waitKey(33);
		if (clickFlag == 1)
			return;
	}
}

void Rank()
{
	rankBackground = (600, 800, CV_8UC3, Scalar(255, 255, 255));
	int clickFlag = 0;
	setMouseCallback(windowName, returnOnMouse, &clickFlag);
	while (1)
	{
		rectangle(rankBackground, Rect(0, 0, 800, 100), Scalar(204, 153, 0), -1);
		putText(rankBackground, String("Ranking"), Point(10, 70), CV_FONT_HERSHEY_TRIPLEX, 2.5, Scalar(255, 255, 255));
		showScore();
		drawTransparent(rankBackground, back_b, back_b_m, 340, HEIGHT - 70);
		if (clickFlag == 1)
			return;
	}
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
	startBackground = (600, 800, CV_8UC3, Scalar(0, 0, 0)); //将startBackground重置为背景图，相当于画了一遍背景
	drawImage(startBackground, startBackgroundBackUp, 0, 0);
}

void gameStart()
{
	// 数据初始化
	srand((unsigned int)time(NULL));

	struct PLAYER playerOne;
	struct PLAYER *player1 = &playerOne;
	struct PLAYER playerTwo;
	struct PLAYER *player2 = &playerTwo;
	struct ITEM *item;
	int direction = 0;
	int direction2 = 0;
	initPlayer(player1, player2);
	initItemToZero();

	clock_t startTime = clock(); //游戏开始的时间
	clock_t pauseTime = 0; //暂停所经过的时间

	int clickFlag = 0;
	setMouseCallback(windowName, gameOnMouse, &clickFlag);

	switch (settings.mode)
	{
	case 1:	enterName(player1); break;
	case 2: {strcpy(player1->name, "Player1"); strcpy(player2->name, "Player2");}break;
	case 3:	strcpy(player1->name, "AI"); break;
	}
	while (1)
	{

		if(rand() % 20 == 1 && ItemNumber - ItemCount)
			addItem();

		//画背景和框架
		drawBackground();
		gameFramwork();

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
			if (returnToMenu(&clickFlag))
				return;
			pauseTime += Pause(player1, player2, &clickFlag, countDown(startTime, pauseTime));

			judgeState(item, player1);
			if (judgeOver(player1, player2, countDown(startTime, pauseTime)))
				return;
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
					direction = AIMove(player1, item);
				drawItem(item);
			}
			item = get_next_item();
		}
		if (settings.mode == 3)
			drawPlayer(player1, direction);
		drawPlayerName(player1, player2);
		drawScore(player1->score, player2->score);
		drawTime(countDown(startTime, pauseTime));

		imshow(windowName, startBackground);
		waitKey(1);
	}
}

//判断函数
void judgeState(struct ITEM *item, struct PLAYER *player)
{
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
			player->life = 0; 
	}
}

int judgeOver(struct PLAYER *player1, struct PLAYER *player2, int time)
{
	if ((settings.mode != 3 && (player1->life == 0 || player2->life == 0)) || time == 0)
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
	//char c = cvWaitKey(1);
	int key = 0;
	if (/*c == 'a'*/GetAsyncKeyState(0x41))
	{
		player->x -= speed.player;
		if (player->x <= 0 + speed.player + 10 || player->x >= WIDTH - speed.player - 40) //边界检测
			player->x += speed.player;
		return LEFT;
	}
	if (/*c == 'd'*/GetAsyncKeyState(0x44))
	{
		player->x += speed.player;
		if (player->x <= 0 + speed.player + 10 || player->x >= WIDTH - speed.player - 40)
			player->x -= speed.player;
		return RIGHT;
	}
	return 0;
}

int playerMove2(struct PLAYER *player)
{
	int key = 0;
	char c = cvWaitKey(1);
	if (/*c == 'j'*/GetAsyncKeyState(0x4A))
	{
		player->x -= speed.player;
		if (player->x <= 0 + speed.player + 10 || player->x >= WIDTH - speed.player - 40) //边界检测
			player->x += speed.player;
		return LEFT;
	}
	if (/*c == 'l'*/GetAsyncKeyState(0x4C))
	{
		player->x += speed.player;
		if (player->x <= 0 + speed.player + 10 || player->x >= WIDTH - speed.player - 40)
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
				if (player->x <= 0 + speed.player + 10 || player->x >= WIDTH - speed.player - 40) //边界检测
					player->x += speed.player;
				return LEFT;
			}

			if (((player->x - 20) >= item->x) && ((player->x - 20) <= (item->x + 20)))
			{
				player->x += speed.player;
				if (player->x <= 0 + speed.player + 10 || player->x >= WIDTH - speed.player - 40)
					player->x -= speed.player;
				return RIGHT;
			}
		}
		if (item->type == CAKE)
		{
			if (((item->x - 20) - (player->x + 20)) > 0)
			{
				player->x += speed.player;
				if (player->x <= 0 + speed.player + 10 || player->x >= WIDTH - speed.player - 40) //边界检测
					player->x -= speed.player;
				return RIGHT;
			}
			if (((player->x - 20) - (item->x + 20)) > 0)
			{
				player->x -= speed.player;
				if (player->x <= 0 + speed.player + 10 || player->x >= WIDTH - speed.player - 40)
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
	//if ((fp = fopen("data", "ab+")) != NULL)
	{
		putText(overBackground, String("Save Failed."), Point(50, 50), CV_FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
		return;
	}
	if (player->name[0] == '\0')
		strcpy(player->name, "N/A");

	fwrite(player, sizeof(struct PLAYER), 1, fp);
	fclose(fp);
	putText(overBackground, String("Saved Successfully."), Point(50, 50), CV_FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
	imshow(windowName, overBackground);
	waitKey(33);
	//showScore();
}

void showScore()
{
	int const rankNumber = 10;
	int lineCount = 0;
	char rankLine[15][50] = { '\0' };
	struct PLAYER *pplayer = NULL;
	FILE *fp = NULL;

	pplayer = (struct PLAYER *)malloc(sizeof(struct PLAYER)*(rankNumber + 1));

	if (fopen_s(&fp, "data", "ab+"))
	//if ((fp = fopen("data", "ab+")) != NULL)
	{
		putText(rankBackground, String("File Reading Failed."), Point(50, 50), CV_FONT_HERSHEY_PLAIN, 1, Scalar(0, 0, 0));
		imshow(windowName, rankBackground);
		waitKey(33);
		return;
	}

	for (int i = 0; i < rankNumber + 1; i++, lineCount++)
	{
		fread(&pplayer[i], sizeof(struct PLAYER), 1, fp);
		if (feof(fp))
			break;
	}

	playerBubbleSort(pplayer, lineCount);
	if (lineCount == 11)
		lineCount = 10;
	for (int i = 0; i < lineCount; i++)
	{
		sprintf(rankLine[i], "%-2d: Name: %-10s, Score: %d", i + 1, pplayer[i].name, pplayer[i].score);
		putText(rankBackground, (String)rankLine[i], Point(230, 150 + i * 40), CV_FONT_HERSHEY_PLAIN, 1.2, Scalar(0, 0, 0));
	}
	fclose(fp);
	
	rewriteScore(pplayer, lineCount);

	imshow(windowName, rankBackground);
	waitKey(33);
	free(pplayer);
	pplayer = NULL;
	
}

void rewriteScore(struct PLAYER * pplayer, int lineCount)
{
	FILE *fp = NULL;
	if (fopen_s(&fp, "data", "wb"))
	//if ((fp = fopen("data", "ab+")) != NULL)
	{
		putText(overBackground, String("File Rewrite Failed."), Point(50, 50), CV_FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
		imshow(windowName, overBackground);
		waitKey(33);
		return;
	}

	for (int i = 0; i < lineCount; i++)
		fwrite(&pplayer[i], sizeof(struct PLAYER), 1, fp);

	fclose(fp);
}

int countDown(clock_t startTime, clock_t pauseTime)
{
	clock_t nowTime = clock();
	int passedTime = (int)(nowTime - startTime - pauseTime) / CLOCKS_PER_SEC;
	return settings.time - passedTime;
}

enum TYPE randomType()
{
	int randomNumber = rand() % 100;
	if (settings.difficult == 1)
	{
		if (randomNumber <= 10)
			return BOMB;
		else if (randomNumber <= 50)
			return UMBRELLA;
		else
			return CAKE;
	}
	if (settings.difficult == 2)
	{
		if (randomNumber <= 20)
			return BOMB;
		else if (randomNumber <= 50)
			return UMBRELLA;
		else
			return CAKE;
	}
	if (settings.difficult == 3)
	{
		if (randomNumber <= 40)
			return BOMB;
		else if (randomNumber <= 70)
			return UMBRELLA;
		else
			return CAKE;
	}
	return CAKE;
}

//OpenCV鼠标回调函数
static void menuOnMouse(int EVENT, int x, int y, int flags, void *userdata)
{
	Point *p = (Point *)userdata;

	if (250 <= x && x <= 250 + 300 && 120 <= y && y <= 120 + 71) //开始
	{
		p->x = 250;
		p->y = 120;
		if (EVENT == EVENT_LBUTTONDOWN)
		{
			setMouseCallback(windowName, NULL, NULL);
			gameStart();
		}
	}
	else if (250 <= x && x <= 250 + 300 && 120 + 70 <= y && y <= 120 + 70 + 71) //帮助
	{
		p->x = 250;
		p->y = 120 + 70;
		if (EVENT == EVENT_LBUTTONDOWN)
		{
			setMouseCallback(windowName, NULL, NULL);
			Help();
		}
	}
	else if (250 <= x && x <= 250 + 300 && 120 + 70 + 70 <= y && y <= 120 + 70 + 70 + 71) //设置
	{
		p->x = 250;
		p->y = 120 + 70 + 70;
	}
	else if (460 <= x && x <= 460 + 100 && 120 + 70 + 70 + 70 + 10 <= y && y <= 120 + 70 + 70 + 70 + 10 + 50) //单人
	{
		p->x = 460;
		p->y = 120 + 70 + 70 + 70 + 10;
		if (EVENT == EVENT_LBUTTONDOWN)
			settings.mode = 1;
	}
	else if (570 <= x && x <= 570 + 100 && 120 + 70 + 70 + 70 + 10 <= y && y <= 120 + 70 + 70 + 70 + 10 + 50) //双人
	{
		p->x = 570;
		p->y = 120 + 70 + 70 + 70 + 10;
		if (EVENT == EVENT_LBUTTONDOWN)
			settings.mode = 2;
	}
	else if (680 <= x && x <= 680 + 100 && 120 + 70 + 70 + 70 + 10 <= y && y <= 120 + 70 + 70 + 70 + 10 + 50) //AI
	{
		p->x = 680;
		p->y = 120 + 70 + 70 + 70 + 10;
		if (EVENT == EVENT_LBUTTONDOWN)
			settings.mode = 3;
	}
	else if (400 - 60 - 100 <= x && x <= 400 - 60 - 100 + 100 && 120 + 70 + 70 + 70 + 10 <= y && y <= 120 + 70 + 70 + 70 + 10 + 50) //困难
	{
		p->x = 400 - 60 - 100;
		p->y = 120 + 70 + 70 + 70 + 10;
		if (EVENT == EVENT_LBUTTONDOWN)
			settings.difficult = 3;
	}
	else if (400 - 170 - 100 <= x && x <= 400 - 170 - 100 + 100 && 120 + 70 + 70 + 70 + 10 <= y && y <= 120 + 70 + 70 + 70 + 10 + 50) //中等
	{
		p->x = 400 - 170 - 100;
		p->y = 120 + 70 + 70 + 70 + 10;
		if (EVENT == EVENT_LBUTTONDOWN)
			settings.difficult = 2;
	}
	else if (400 - 280 - 100 <= x && x <= 400 - 280 - 100 + 100 && 120 + 70 + 70 + 70 + 10 <= y && y <= 120 + 70 + 70 + 70 + 10 + 50) //简单
	{
		p->x = 400 - 280 - 100;
		p->y = 120 + 70 + 70 + 70 + 10;
		if (EVENT == EVENT_LBUTTONDOWN)
			settings.difficult = 1;
	}
	else if (250 <= x && x <= 250 + 300 && 120 + 70 + 70 + 70 + 70 <= y && y <= 120 + 70 + 70 + 70 + 70 + 71) //排名
	{
		p->x = 250;
		p->y = 120 + 70 + 70 + 70 + 70;
		if (EVENT == EVENT_LBUTTONDOWN)
		{
			setMouseCallback(windowName, NULL, NULL);
			Rank();
		}
	}
	else if (250 <= x && x <= 250 + 300 && 120 + 70 + 70 + 70 + 70 + 70 <= y && y <= 120 + 70 + 70 + 70 + 70 + 70 + 71) //退出
	{
		p->x = 250;
		p->y = 120 + 70 + 70 + 70 + 70 + 70;
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
	if (200 + 110 <= x && x <= (200 + 110 + 130) && 400 <= y && y <= (400 + 40) && EVENT == EVENT_LBUTTONDOWN) //返回
		*clickFlag = 1;
	if (200 <= x && x <= (200 + 100) && 400 <= y && y <= (400 + 40) && EVENT == EVENT_LBUTTONDOWN) // 保存
		*clickFlag = 2;
	if (200 <= x && x <= (200 + 100) && 400 + 60 <= y && y <= (400 + 40 + 40) && EVENT == EVENT_LBUTTONDOWN) // 显示排行榜
		*clickFlag = 3;
}

static void returnOnMouse(int EVENT, int x, int y, int flags, void *userdata)
{
	int *clickFlag = (int *)userdata;
	if (340 <= x && x <= 340 + 120 && HEIGHT - 70 <= y && y <= HEIGHT - 70 + 59 && EVENT == EVENT_LBUTTONDOWN) //返回
		*clickFlag = 1;
}
