//���̨ʽ������Ĵ���
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

#define HEIGHT 600
#define WIDTH 800
#define BORDER 480
#define LEFT 1
#define RIGHT 2

//��ʼ��
void gameFramwork();
//struct PLAYER *initPlayer();
void initPlayer(struct PLAYER *player);
//TODO ׼�������Ѷȣ���������
//struct PLAYER *initPlayer();

//�µ�������
void intiItem();
void addItem();
int delItem();
struct ITEM *get_first_item();
struct ITEM *get_next_item();

//��ͼ����
void drawItem(struct ITEM *item);
void coverItem(int x, int y, enum TYPE type);
void drawPlayer(struct PLAYER *player, int direction);
void drawScore(int score);
void drawPlayerName(struct PLAYER *player1);//˫������
void drawPlayerName(struct PLAYER *player1, struct PLAYER *player2);
// TODO �ӵ�ͼ�ƶ�

//���溯��
void Menu();
//void changeSettings();
//void Pause();
//void Help();
//void Rank();
//void Exit();
void drawBackground();
//void startGame();
void singlePlayer();
//void multiPlayer();
//void AI();
//void gameOver();


//�жϺ���
void judgeState(struct ITEM *item, struct PLAYER *player);
enum MOUSE_POSITION judgeMouse();
void itemMove(struct ITEM *item);
int playerMove(struct PLAYER *player);

//��������

enum STATE { //�����״̬
	ON_SCREEN, //����Ļ��
	OFF_SCREEN, //�뿪����Ļ
	EATED // ����ҳԵ�
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
	ON_RANK,
	ON_EXIT,
	HIT_START,
	HIT_HELP,
	HIT_SETTINGS,
	HIT_RANK,
	HIT_EXIT,
	OFF_START
};

struct SPEED { //TODO ��д�ٶ�,���������Ѷ�,���Ը��ٶ�
	int player;
	int cake;
	int umbrella;
	int bomb;
};

struct PLAYER {
	char name[10]; //�������
	int x = 400;
	int y = 480; //��������һ��ȷ������
	int score = 0; //��ҷ���
	int life = 1; //��ҵ���
};

struct Settings {
	int difficult; //�Ѷ�, 1�򵥣�2�еȣ�3����
	int mode; //ģʽ 1Ϊ���ˣ�2Ϊ˫�ˣ�3ΪAI
	int time; //��Ϸʱ��
};
//�µ����ݽṹ
typedef struct item_list {
	struct ITEM *this_item;
	struct item_list *next;
}ItemList;

typedef struct ITEM {
	enum TYPE type; //��������һ����Ʒ
	enum STATE state;
	int x;
	int y;
	//struct ITEM *next;
}Item;

struct SPEED speed = {//��ʼ���ٶ�
	10,//player
	3,//cake
	4,//umbrella
	2//bomb
};

//ȫ�ֱ���
struct item_list item_list_head;
struct item_list *curr_list;
struct Settings settings = { //Ĭ������
	1,
	1,
	120
};
int ItemCount = 0;
const int ItemNumber = 10;//��Ļ����Ʒ������

int main()
{
	initgraph(WIDTH, HEIGHT);
	while (1)
	{
		Menu();
	}
	singlePlayer();

	_getch(); //TODO �ǵ�ɾ��
	closegraph();
}

void Menu()
{
	BeginBatchDraw();
	//cleardevice();
	setlinecolor(WHITE);
	setbkmode(TRANSPARENT);
	//��ʼ
	RECT start = { 350, 120, 350 + 100, 120 + 50 };
	rectangle(350, 120, 350 + 100, 120 + 50);
	drawtext(_T("��ʼ��Ϸ"), &start, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
	//����

	//����
	//����
	//�˳�

	setfillcolor(CYAN);
	//FlushBatchDraw();
	EndBatchDraw();
	BeginBatchDraw();
	switch (judgeMouse())
	{
	case ON_START: fillrectangle(350, 120, 350 + 100, 120 + 50);  break;
	case HIT_START: singlePlayer(); break;
	case OFF_START:clearrectangle(350, 120, 350 + 100, 120 + 50); break;
	}

	EndBatchDraw();
}

enum MOUSE_POSITION judgeMouse()
{
	MOUSEMSG mouse = GetMouseMsg();
	if ((350 <= mouse.x && mouse.x <= (350 + 100)) && (120 <= mouse.y && mouse.y <= (120 + 50)))
		if (mouse.mkLButton)//�������
			return HIT_START;
		else
			return ON_START;
	else
		return OFF_START;

	return NOTHING;
}

void gameFramwork()
{
	// TODO Ӧ���������ٽ���
	//�����ʼ��
	//setbkmode(TRANSPARENT);//�������ֱ���͸��
	line(0, BORDER, WIDTH, BORDER);
	//��ʼ��ť
	RECT start = { 30, BORDER + 40, 100, BORDER + 80 };
	rectangle(30, BORDER + 40, 100, BORDER + 80);
	drawtext(_T("��ʼ"), &start, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
	//������ť
	RECT end = { 150, BORDER + 40, 220, BORDER + 80 };
	rectangle(150, BORDER + 40, 220, BORDER + 80);
	drawtext(_T("����"), &end, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
	//����
	RECT scoreTextAera = { WIDTH - 250, BORDER + 40, WIDTH - 180, BORDER + 80 };
	RECT scoreNumberAera = { WIDTH - 200, BORDER + 40, WIDTH - 180, BORDER + 80 };
	RECT playerNameAera = { WIDTH - 250, BORDER + 80, WIDTH - 180, BORDER + 120 };
	rectangle(WIDTH - 250, BORDER + 40, WIDTH - 80, BORDER + 80);
	rectangle(WIDTH - 250, BORDER + 80, WIDTH - 80, BORDER + 120);

	drawtext(_T("������"), &scoreTextAera, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
	drawtext(_T("���ƣ� "), &playerNameAera, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
}

void intiItem()
{
	item_list_head.next = NULL;
	item_list_head.this_item = NULL;
	curr_list = NULL;
}

void addItem()//��new��add�����ϳ�һ��������
{
	//�ȴ���С�ڵ�
	Item *newItem = (Item *)malloc(sizeof(Item));
	//srand((unsigned int)time(0));
	int randomNumber = rand() % 3;
	// TODO ����д������randomType()�����ֵΪenum���Ӹ���
	switch (randomNumber)
	{
	case 0:newItem->type = UMBRELLA; break;
	case 1:newItem->type = CAKE; break;
	case 2:newItem->type = BOMB; break;
	}
	////
	newItem->state = ON_SCREEN;
	newItem->x = rand() % 800;
	newItem->y = -(rand() % 550);
	//	newItem->next = NULL;
	//������ڵ�
	struct item_list *list_tail = &item_list_head;
	while (list_tail->next != NULL)
		list_tail = list_tail->next;
	ItemList *newList = (ItemList *)malloc(sizeof(ItemList));
	newList->next = NULL;
	newList->this_item = newItem;
	list_tail->next = newList;
	ItemCount++;
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

void drawItem(struct ITEM *item)
{
	switch (item->type)
	{
	case UMBRELLA:
		//setlinecolor(BLACK);
		//circle(item->x, item->y - speed.umbrella, 20);// ����ǰһ��
		setlinecolor(WHITE);
		circle(item->x, item->y, 20);
		break;
	case CAKE:
		/*setlinecolor(BLACK);
		circle(item->x, item->y - speed.cake, 20);*/
		setlinecolor(GREEN);
		circle(item->x, item->y, 20);
		break;
	case BOMB:
		/*setlinecolor(BLACK);
		circle(item->x, item->y - speed.bomb, 20);*/
		setlinecolor(RED);
		circle(item->x, item->y, 20);

		break;
	}
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
			player->life = 0;
	}
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

void coverItem(int x, int y, enum TYPE type)
{
	switch (type)
	{
	case UMBRELLA:
		setlinecolor(BLACK);
		circle(x, y, 20);// ����ǰһ��
		break;
	case CAKE:
		setlinecolor(BLACK);
		circle(x, y, 20);// ����ǰһ��
		break;
	case BOMB:
		setlinecolor(BLACK);
		circle(x, y, 20);
		break;
	}
}

void initPlayer(struct PLAYER *player)
{
	player->x = 400;
	player->y = 459;
	player->life = 1;
	player->score = 0;
}

void drawPlayer(struct PLAYER *player, int direction)
{
	int flag = 0;
	switch (direction)
	{
	case LEFT: flag = -1; break;
	case RIGHT: flag = 1; break;
	}
	//setlinecolor(BLACK);
	//circle(player->x - (speed.player)*flag, player->y, 20);// ����ǰһ��
	setlinecolor(WHITE);
	circle(player->x, player->y, 20);
}

int playerMove(struct PLAYER *player)
{
	int key = 0;
	if (player->x <= 0 || player->x >= WIDTH)
	{

	}
	if (GetAsyncKeyState(0x41) || GetAsyncKeyState(VK_LEFT))
	{
		player->x -= speed.player;
		if (player->x <= 0 + speed.player || player->x >= WIDTH - speed.player) //�߽���
			player->x += speed.player;
		return LEFT;
	}
	if (GetAsyncKeyState(0x44) || GetAsyncKeyState(VK_RIGHT))
	{
		player->x += speed.player;
		if (player->x <= 0 + speed.player || player->x >= WIDTH - speed.player)
			player->x -= speed.player;
		return RIGHT;
	}
	return 0;
}

void drawScore(int score)
{
	//TODO �Ż�score ������
	char scoreNumber[10];
	sprintf_s(scoreNumber, "%d", score);
	RECT scoreNumberAera = { WIDTH - 200, BORDER + 40, WIDTH - 80, BORDER + 80 };
	setlinecolor(WHITE);
	drawtext((LPCTSTR)scoreNumber, &scoreNumberAera, (DT_CENTER | DT_SINGLELINE | DT_VCENTER));
}

void drawPlayerName(struct PLAYER *player)
{
	RECT playerNameAera = { WIDTH - 200, BORDER + 80, WIDTH - 80, BORDER + 120 };
	drawtext((LPCTSTR)(player->name), &playerNameAera, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
}

void drawPlayerName(struct PLAYER *player1, struct PLAYER *player2)
{

}

void drawBackground()
{
	IMAGE background;
	loadimage(&background, _T("img\\bkg.jpg"));
	putimage(0, 0, &background);
}

void singlePlayer()
{
	cleardevice();
	// ���ݳ�ʼ��
	srand((unsigned int)time(0));
	struct PLAYER player;
	struct PLAYER *player1 = &player;
	struct ITEM *item;
	initPlayer(player1);
	int direction = 0;

	InputBox((LPTSTR)&(player1->name), 10, (LPCTSTR)"���������֣�");

	while (1)
	{
		for (int i = ItemNumber - ItemCount; i > 0; i--)
			addItem();

		BeginBatchDraw();

		//�������Ϳ��
		drawBackground();
		gameFramwork();

		//��Ҷ���
		direction = playerMove(player1);
		drawPlayer(player1, direction);

		//���嶯��
		item = get_first_item();
		while (item != NULL)
		{
			judgeState(item, player1);
			//����item����ʱ�����ݣ���Ϊɾ��֮����޷���ȡitem���޷���ͼ����ɾ��item��
			int temp_x = item->x;
			int temp_y = item->y;
			enum TYPE temp_type = item->type;

			if (delItem())
				coverItem(temp_x, temp_y, temp_type);
			else {
				itemMove(item);
				drawItem(item);
			}
			//Sleep(1); //���ú��������
			item = get_next_item();
		}

		drawPlayerName(player1);
		drawScore(player1->score);

		EndBatchDraw();
	}
}