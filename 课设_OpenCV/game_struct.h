#ifndef GAME_STRUCT_H
#define GAME_STRUCT_H

#define HEIGHT 600
#define WIDTH 800
#define BORDER 480
#define LEFT 1
#define RIGHT 2

enum TYPE {
	UMBRELLA,
	CAKE,
	BOMB
};
enum STATE { //物体的状态
	ON_SCREEN, //在屏幕上
	OFF_SCREEN, //离开了屏幕
	EATED // 被玩家吃掉
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


struct SPEED {
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

struct item_list item_list_head;
struct item_list *curr_list;

struct SPEED speed = {//初始化速度
	5,//player
	3,//cake
	5,//umbrella
	1.5//bomb
};

struct Settings settings = { //默认设置
	1,
	1,
	60
};
#endif
