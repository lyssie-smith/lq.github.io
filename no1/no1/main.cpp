/*
*1.导入素材
*2.实现最开始的游戏场景
*3.实现游戏顶部的工具栏
*4.实现工具栏中的植物卡牌
*/

#include <stdio.h>
#include <easyx.h>
#include "tools.h"
#include "time.h"
#define Win_Width 900
#define Win_Height 600

enum {Pea, Sunflower, Number};

IMAGE Background;
IMAGE Blank;
IMAGE Plants[Number];
IMAGE* Dynamic_Plants[Number][20];
IMAGE Sunshine[29];

int Card_X, Card_Y,Select;

struct plant {
	int Type;
	int Frame;
	int X;
	int Y;
};

struct plant Map[3][9];

struct Sunshine {
	int X;
	int Y;
	int Frame;
	int DestY;
	bool Used;
	int Timer;
};

struct Sunshine SunshinePool[10];

int SunshineNum = sizeof(SunshinePool) / sizeof(SunshinePool[0]);

//判断图片是否存在
bool File_Exist(const char* ch) {
	FILE* fp = fopen(ch, "r");
	if (fp == NULL) {
		return false;
	}
	else {
		fclose(fp);
		return true;
	}
}

//初始化游戏界面
void Game_Init() {
	//创建窗口
	initgraph(Win_Width, Win_Height, EX_SHOWCONSOLE);
	//导入背景
	loadimage(&Background,"./res/bg.jpg");
	//导入工具栏
	loadimage(&Blank,"./res/bar5.png");
	//导入植物卡牌
	char Plant_Name[64]; //创建一个字符数组，用于存储卡牌所在的路径
	for (int i = 0; i < Number; i++) {
		sprintf_s(Plant_Name, sizeof(Plant_Name), "./res/Cards/card_%d.png", i + 1);
		loadimage(&Plants[i], Plant_Name);
	}
	memset(Dynamic_Plants, NULL, sizeof(Dynamic_Plants));//使指针都指向空
	memset(Map, 0, sizeof(Map));//使指针都指向空
	memset(SunshinePool, 0, sizeof(SunshinePool));
	srand(time(NULL));
	//导入动态植物
	for (int i = 0; i < Number; i++) {
		for (int j = 0; j < 20; j++) {
			sprintf_s(Plant_Name, sizeof(Plant_Name), "./res/zhiwu/%d/%d.png", i , j + 1);
			if (File_Exist(Plant_Name)) {
				Dynamic_Plants[i][j] = new IMAGE;//动态申请内存
				loadimage(Dynamic_Plants[i][j], Plant_Name);
				//printf("%d %d\n", i, j);
			}
			else {
				break;
			}
		}
	}
	for (int i = 0; i < 29; i++) {
		sprintf_s(Plant_Name, sizeof(Plant_Name), "./res/sunshine/%d.png", i + 1);
			loadimage(&Sunshine[i], Plant_Name);
	}
	
}

void Creat_Sunshine() {
	static int count = 0;
	static int pre = 80;
	count++;
	if (count > pre) {
		int i;
		count = 0;
		pre = 80 + rand() % 80;
		for (i = 0; i < SunshineNum && SunshinePool[i].Used; i++);
		if (i > SunshineNum) return;
		SunshinePool[i].Used = 1;
		SunshinePool[i].X = 200 + rand() % (900 - 300);       //300~900
		SunshinePool[i].Y = 60;
		SunshinePool[i].DestY = 180 + (rand() % 4) * 90;
		SunshinePool[i].Frame = 0;
		SunshinePool[i].Timer = 0;
	}
}

void Update_Sunshine() {
	for (int i = 0; i < SunshineNum; i++) {
		if (SunshinePool[i].Used == 1) {
			if (SunshinePool[i].Timer == 0) {
				SunshinePool[i].Y += 3;
			}
			SunshinePool[i].Frame = (SunshinePool[i].Frame + 1) % 29;
			if (SunshinePool[i].Y > SunshinePool[i].DestY) {
				SunshinePool[i].Timer++;
				if (SunshinePool[i].Timer == 75) {
					SunshinePool[i].Timer = 0;
					SunshinePool[i].Used = 0;
				}
			}
		}	
	}
}
//鼠标操作实现
void User_Click() {
	ExMessage msg;
	static int Status = 0;//1：鼠标移动状态
	if (peekmessage(&msg)) {
		if (msg.message == WM_LBUTTONDOWN && Status == 0) {
			if (msg.x > 338 && msg.x < 338 + Number * 65 && msg.y < 96) {
				int Index = (msg.x - 338) / 65;
				Select = Index + 1;
				Status = 1;
			}
		}
		else if (msg.message == WM_MOUSEMOVE && Status == 1) {
			//定位鼠标坐标
			Card_X = msg.x;
			Card_Y = msg.y;
		}
		else if (msg.message == WM_LBUTTONDOWN && Status == 1) {
			if (msg.y > 179 && msg.y < 489 && msg.x > 256) {
				int row = (msg.y - 179) / 103;
				int col = (msg.x - 256) / 82;
				if (Map[row][col].Type == 0) {
					Map[row][col].Type = Select;
					Map[row][col].Frame = 0;
				}
			}
			Status = 0;
			Select = 0;
			Card_X = -20;
			Card_Y = -20;
		}
	}
}

//加载或更新游戏界面
void Update_Window() {
	BeginBatchDraw();
	putimage(0, 0, &Background);
	putimagePNG(255, 0, &Blank);
	for (int i = 0; i < Number; i++) {
		putimage(338 + i * 65, 6, &Plants[i]);
	}
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 9; j++) {
			if (Map[i][j].Type > 0) {
				int planttype = Map[i][j].Type - 1;
				int index = Map[i][j].Frame;
				putimagePNG(256 + j * 82, 179 + i * 103, Dynamic_Plants[planttype][index]);
				//Sleep(2);
				//printf("%d %d\n", planttype, index);
			}
		}
	}
	if (Select) {
		IMAGE* img = Dynamic_Plants[Select - 1][0];
		putimagePNG(Card_X - img->getwidth() / 2, Card_Y - img->getheight() / 2, img);
	}
	for (int i = 0; i < SunshineNum; i++) {
			//printf("%d", SunshinePool[i].Used);
			if (SunshinePool[i].Used > 0) {
				putimagePNG(SunshinePool[i].X, SunshinePool[i].Y, &Sunshine[SunshinePool[i].Frame]);
			}
	}
			
	EndBatchDraw();
}

void Update_Game() {
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 9; j++) {
			if (Map[i][j].Type > 0) {
				Map[i][j].Frame++;
				int Type = Map[i][j].Type - 1;
				int Frame = Map[i][j].Frame;
				if (Dynamic_Plants[Type][Frame] == NULL) {
					//printf("%d", Frame);
					Map[i][j].Frame = 0;
				}
				//printf("%d\n", Map[i][j].Frame);
			}
		}
	}

	Creat_Sunshine();
	Update_Sunshine();
}

void Start_UI() {
	IMAGE MENU;
	IMAGE MENU1;
	IMAGE MENU2;

	loadimage(&MENU, "./res/menu.png");
	loadimage(&MENU1, "./res/menu1.png");
	loadimage(&MENU2, "./res/menu2.png");
	int flag = 0;
	while (1) {
		BeginBatchDraw();

		putimage(0, 0, &MENU);
		putimagePNG(475, 125, flag ? &MENU2 : &MENU1);
		ExMessage msg;
		if (peekmessage(&msg)) {
			//printf("1");
			if (msg.x > 475 && msg.x < 450 + 310 && msg.y > 125 && msg.y < 125 + 130) {
				flag = 1;
				if (msg.message == WM_LBUTTONDOWN) {
					return;
				}
			}
			else {
				flag = 0;
			}
		}
		EndBatchDraw();
	}
}




int main() {
	
	Game_Init();
	Start_UI();
	int Timer = 0;
	bool flag = true;
	while (1) {
		User_Click();
		Timer += getDelay();
		if (Timer > 100) {
			flag = true;
			Timer = 0;
		}
		if (flag == true) {
			flag = false;
			Update_Game();
		}
		Update_Window();
	}

	return 0;
}
