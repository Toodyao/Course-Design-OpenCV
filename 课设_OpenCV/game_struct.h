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
enum STATE { //ÎïÌåµÄ×´Ì¬
	ON_SCREEN, //ÔÚÆÁÄ»ÉÏ
	OFF_SCREEN, //Àë¿ªÁËÆÁÄ»
	EATED // ±»Íæ¼Ò³Ôµô
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


struct SPEED { //TODO ÌîÐ´ËÙ¶È,ºóÆÚÉèÖÃÄÑ¶È,¿ÉÒÔ¸ÄËÙ¶È
	double player;
	double cake;
	double umbrella;
	double bomb;
};

struct SPEED speed = {//³õÊ¼»¯ËÙ¶È
	5,//player
	1,//cake
	4,//umbrella
	3//bomb
};

struct PLAYER {
	char name[10];
	double x = 400;
	double y = 480; //ºá×ø±êÊÇÒ»¸öÈ·¶¨µÄÊý
	int score = 0; //Íæ¼Ò·ÖÊý
	int life = 1; //Íæ¼ÒµÄÃü
};

struct Settings {
	int difficult; //ÄÑ¶È, 1¼òµ¥£¬2ÖÐµÈ£¬3À§ÄÑ
	int mode; //Ä£Ê½ 1Îªµ¥ÈË£¬2ÎªË«ÈË£¬3ÎªAI
	int time; //ÓÎÏ·Ê±¼ä
};
//ÐÂµÄÊý¾Ý½á¹¹
typedef struct item_list {
	struct ITEM *this_item;
	struct item_list *next;
}ItemList;

typedef struct ITEM {
	enum TYPE type; //¾ßÌåÊÇÄÄÒ»ÖÖÎïÆ·
	enum STATE state;
	double x;
	double y;
	//struct ITEM *next;
}Item;


//È«¾Ö±äÁ¿
//struct SPEED speed = {//³õÊ¼»¯ËÙ¶È
//	0.5,//player
//	0.11,//cake
//	0.14,//umbrella
//	0.19//bomb
//};

struct item_list item_list_head;
struct item_list *curr_list;

struct Settings settings = { //Ä¬ÈÏÉèÖÃ
	1,
	1,
	120
};
#endif
