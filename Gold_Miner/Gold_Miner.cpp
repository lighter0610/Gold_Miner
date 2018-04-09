#include "stdafx.h"

struct Point {
	int x;
	int y;
};

struct Mine {
	IMAGE* MinePic;
	IMAGE* MineMaskPic;
	int value;
	int weight;
};

struct Node
{
	Mine mine;
	Point lefttop;
	Node* next;
};

int RandomNum()
{
	static int seed = (int)time(NULL);
	srand(seed);
	seed = rand();
	int num = (int)pow(rand() % 218, 3) / 21;
	return num;
}

void LoadAllImage(IMAGE *pic)
{
	char tool[20], mine[20], miner[20], botton[20];
	strcpy_s(tool, "images/tool1.jpg");
	strcpy_s(mine, "images/mine1.jpg");
	strcpy_s(miner, "images/mining1.jpg");
	strcpy_s(botton, "images/botton1.jpg");
	loadimage(pic, "images/background.jpg");	//pic[0] = 背景
	for (int i = 1; i < 24; i++)
	{
		if (i == 1)
			loadimage(pic + i, "images/Miner_Normal.jpg");	//pic[1] = 正常矿工
		else if (i == 2)
			loadimage(pic + i, "images/Miner_Grab.jpg");		//pic[2] = 向下矿工
		else if (i <= 8)
		{
			loadimage(pic + i, miner);		//pic[3]-pic[8] = 向上矿工
			miner[13]++;
		}
		else if (i <= 12)
		{
			loadimage(pic + i, botton);		//pic[9] 开始 [10] 排行榜 [11] 退出 [12] 暂停
			botton[13]++;
		}
		else if (i <= 15)
		{
			loadimage(pic + i, tool);		//pic[13] 炸药 [14] 大力水 [15] 眼镜
			tool[11]++;
		}
		else if (i == 16)
			loadimage(pic + i, "images/Grabber.jpg");	// pic[16] 钩子
		else if (i == 17)
			loadimage(pic + i, "images/bag.jpg");	//pic[17] 道具包
		else if (i <= 23)
		{
			loadimage(pic + i, mine);	//[18] 小金块 [19] 中金块 [20] 大金块
			mine[11]++;					//[21] 小石头 [22] 大石头 [23] 钻石
		}
	}
}

void MakeMask(IMAGE* pic, IMAGE* mask)		//制作掩码图
{
	for (int i = 0; i < 8; i++)
	{
		mask[i] = pic[i];
		DWORD *pmemPic = GetImageBuffer(pic + i);
		DWORD *pmemMask = GetImageBuffer(mask + i);
		COLORREF maskbk = pmemPic[0];
		int width = (pic + i)->getwidth();
		int height = (pic + i)->getheight();
		for (int j = 0; j < width * height; j++)
		{
			if (pmemPic[j] <= 0x111111 && pmemPic[i] >= 0)
				pmemMask[j] = WHITE;
			else
				pmemMask[j] = BLACK;
		}
	}
}

void SetMine(IMAGE* pic, IMAGE* mask, Mine* mine)
{
	for (int i = 0; i < 7; i++)
	{
		mine[i].MinePic = pic + 17 + i;
		mine[i].MineMaskPic = mask + 1 + i;
		switch (i) {
		case 0:
			mine[i].value = BAGVALUE;
			mine[i].weight = BAGWEIGHT;
			break;
		case 1:
			mine[i].value = SGOLDVALUE;
			mine[i].weight = SGOLDWEIGHT;
			break;
		case 2:
			mine[i].value = MGOLDVALUE;
			mine[i].weight = MGOLDWEIGHT;
			break;
		case 3:
			mine[i].value = LGOLDVALUE;
			mine[i].weight = LGOLDWEIGHT;
			break;
		case 4:
			mine[i].value = SSTONEVALUE;
			mine[i].weight = SSTONEWEIGHT;
			break;
		case 5:
			mine[i].value = LSTONEVALUE;
			mine[i].weight = LSTONEWEIGHT;
			break;
		case 6:
			mine[i].value = DMVALUE;
			mine[i].weight = DMWEIGHT;
			break;
		}
	}
}

void Initialize(IMAGE* pic, IMAGE* mask, Mine* mine)
{
	initgraph(WINDOWWIDTH, WINDOWHEIGHT);
	setbkcolor(RGB(0xff, 0xcf, 0x34));
	cleardevice();
	settextstyle(20, 0, "Times New Roman");
	rectangle(0, 0, BGWIDTH, BGHEIGHT);
	rectangle(BGWIDTH, 0, WINDOWWIDTH, INFOHEIGHT);
	LoadAllImage(pic);
	MakeMask(pic + 16, mask);
	SetMine(pic, mask, mine);
	putimage(0, 0, pic);
}

bool CheckOverlap(Node* NodeL, Node* NodeCheck)
{
	bool flagx = true;
	bool flagy = true;
	bool flag = true;
	int xcheck = NodeCheck->mine.MinePic->getwidth();
	int ycheck = NodeCheck->mine.MinePic->getheight();
	for (Node* NodeP = NodeL->next; NodeP != NULL; NodeP = NodeP->next)
	{
		int width = NodeP->mine.MinePic->getwidth();
		int height = NodeP->mine.MinePic->getheight();
		if (abs(NodeCheck->lefttop.x - NodeP->lefttop.x) < 
			(xcheck > width ? xcheck : width))
			flagx = false;

		if (abs(NodeCheck->lefttop.y - NodeP->lefttop.y) < 
			(ycheck > height ? xcheck : height))
			flagy = false;
	}
	if (flagx == false && flagy == false)
		flag = false;
	return flag;
}

void PutLucidImage(int x, int y, IMAGE* pic, IMAGE* mask)
{
	BeginBatchDraw();
	putimage(x, y, mask, SRCAND);
	putimage(x, y, pic, SRCPAINT);
	FlushBatchDraw();
	EndBatchDraw();
}

bool StartMenu(char *Username, IMAGE *pic, IMAGE* mask)
{
	InputBox(Username, 14, "Enter your name with no more than 14 characters",
		"Gold Miner", "player0");
	{
		putimage(FNPLACEX, BOTTON1PLACE, pic + 9);		//按钮图片
		putimage(FNPLACEX, BOTTON2PLACE, pic + 10);
		putimage(FNPLACEX, BOTTON3PLACE, pic + 11);

		putimage(MINERPLACEX, MINERPLACEY, pic + 1);	//初始矿工
		PutLucidImage(GRPLACEX, GRPLACEY, pic + 16, mask);	//钩子
		setlinecolor(RGB(0, 0, 0));
		line(MINERPLACEX + 42, MINERPLACEY + 92, GRPLACEX + 30, GRPLACEY);
	}
	int TextY = 450;
	{		//输出信息
		outtextxy(FNPLACEX, TEXT1PLACE, "Player Name:");
		outtextxy(TEXTPLACEX, TEXT1PLACE, Username);
		outtextxy(FNPLACEX, TEXT2PLACE, "Score:");
		outtextxy(TEXTPLACEX, TEXT2PLACE, "0");
		outtextxy(FNPLACEX, TEXT3PLACE, "Time Left:");
		outtextxy(TEXTPLACEX, TEXT3PLACE, "00:00");
		outtextxy(FNPLACEX, TEXT4PLACE, "High Score:");
		//TODO highscore 
	}
	while (true)
	{
		FlushMouseMsgBuffer();
		MOUSEMSG m = GetMouseMsg();
		if (m.x >= FNPLACEX && m.x <= FNPLACEX + BOTTONWIDTH)
		{
			if (m.y >= BOTTON1PLACE && m.y <= BOTTON1PLACE + BOTTONHEIGHT)
			{
				if (m.mkLButton == true)
					return true;
			}
			if (m.y >= BOTTON2PLACE && m.y <= BOTTON2PLACE + BOTTONHEIGHT)
			{
				//TODO RANK
			}
			if (m.y >= BOTTON3PLACE && m.y <= BOTTON3PLACE + BOTTONHEIGHT)
			{
				if (m.mkLButton == true)
				{
					if (MessageBox(GetHWnd(), "Are you sure to exit?", "Exit",
						MB_OKCANCEL) == true)
						return false;
				}
			}
		}
		FlushMouseMsgBuffer();
		Sleep(100);
	}
}

int CreateList(Node** NodeL, Mine* mine)
{
	int valuesum = 0;
	int minenum = 0;
	int Wrong = 0;
	Node *NodeP, *NodeS;
	*NodeL = (Node*)malloc(sizeof(Node));
	NodeP = *NodeL;
	while (valuesum < 1500) 
	{
		NodeS = (Node*)malloc(sizeof(Node));
		NodeS->mine = mine[RandomNum() % 7];
		NodeS->lefttop.x = RandomNum() % (BGWIDTH - NodeS->mine.MinePic->getwidth() - 20);
		NodeS->lefttop.y = RandomNum() % (BGHEIGHT - MINESPACEY - NodeS->mine.MinePic->getheight()
									- GRABHEIGHT) + MINESPACEY + GRABHEIGHT + 10;
		NodeS->next = NULL;
		while (minenum > 0 && CheckOverlap(*NodeL, NodeS) == false)
		{
			NodeS->lefttop.x = RandomNum() % (BGWIDTH - NodeS->mine.MinePic->getwidth() - 20);
			NodeS->lefttop.y = RandomNum() % (BGHEIGHT - MINESPACEY - GRABHEIGHT
				- NodeS->mine.MinePic->getheight()) + MINESPACEY + GRABHEIGHT + 10;
			Wrong++;
			if (Wrong >= 20)
				break;
		}
		if (Wrong >= 20)
			break;
		NodeP->next = NodeS;
		NodeP = NodeS;
		valuesum += NodeP->mine.value;
		minenum++;
	}
	return valuesum;
}

void OutputTime(time_t* Lefttime)
{
	static time_t Starttime = GetTickCount();
	time_t Passtime = GetTickCount() - Starttime;
	Passtime = GetTickCount() - Starttime;
	if (Passtime >= 1000)
	{
		*Lefttime -= Passtime / 1000;
		Passtime %= 1000;
		Starttime = GetTickCount();
		if (*Lefttime >= 0)
		{
			char Time[20];
			sprintf(Time, "%02d:%02d", (int)*Lefttime / 60, (int)*Lefttime % 60);
			clearrectangle(TEXTPLACEX, TEXT3PLACE, TEXTPLACEX + 30, TEXT3PLACE + 20);
			outtextxy(TEXTPLACEX, TEXT3PLACE, Time);
		}
	}
}

void MoveMiner(int flag, IMAGE* pic, IMAGE* mask, Point Miner, Point Grabber, time_t* Lefttime)
{
	IMAGE tempsave;
	OutputTime(Lefttime);
	BeginBatchDraw();
	clearrectangle(Miner.x, Miner.y, Miner.x + MINERWIDTH + 5, Miner.y + MINERHEIGHT + GRABHEIGHT);
	SetWorkingImage(pic);
	getimage(&tempsave, Miner.x - 5, Miner.y - 5, MINERWIDTH + 11, MINERHEIGHT + GRABHEIGHT + 10);
	SetWorkingImage();
	putimage(Miner.x - 5, Miner.y - 5, &tempsave);
	putimage(Miner.x + flag * 5, Miner.y, pic + 1);
	PutLucidImage(Grabber.x + flag * 5, Grabber.y, pic + 16, mask);
	line(Miner.x + 42 + flag * 5, Miner.y + 92, Grabber.x + 30 + flag * 5, Grabber.y);
	FlushBatchDraw();
	EndBatchDraw();
}

bool PutDown(Point Miner, Point* Grabber, IMAGE* pic, IMAGE* mask, time_t* Lefttime)
{
	bool getflag = false;
	int pixel1, pixel2;
	IMAGE tempsave;
	DWORD* buffer = GetImageBuffer();
	OutputTime(Lefttime);
	clearrectangle(Miner.x, Miner.y, Miner.x + MINERWIDTH, Miner.y + MINERHEIGHT);
	putimage(Miner.x, Miner.y, pic + 2);
	line(Miner.x + 42, Miner.y + 92, Grabber->x + 30, Grabber->y);
	clearrectangle(Grabber->x, Grabber->y, Grabber->x + GRABWIDTH, Grabber->y + GRABHEIGHT);
	SetWorkingImage(pic);
	getimage(&tempsave, Grabber->x, Grabber->y, GRABWIDTH + 1, GRABHEIGHT + 1);
	SetWorkingImage();
	putimage(Grabber->x, Grabber->y, &tempsave);
	getimage(&tempsave, Grabber->x, MINESPACEY, GRABWIDTH + 2, BGHEIGHT - MINESPACEY);
	PutLucidImage(Grabber->x, Grabber->y, pic + 16, mask);
	for (Grabber->y; Grabber->y < WINDOWHEIGHT - GRABHEIGHT; Grabber->y += 5)
	{
		OutputTime(Lefttime);
		pixel1 = (Grabber->y + 24) * WINDOWWIDTH + Grabber->x + (GRABWIDTH / 2);
		pixel2 = pixel1 + WINDOWWIDTH;
		BeginBatchDraw();
		clearrectangle(Grabber->x, Grabber->y, Grabber->x + GRABWIDTH, Grabber->y + GRABHEIGHT);
		putimage(Grabber->x, MINESPACEY, &tempsave);
		PutLucidImage(Grabber->x, Grabber->y, pic + 16, mask);
		line(Miner.x + 42, Miner.y + 92, Grabber->x + 30, Grabber->y);
		FlushBatchDraw();
		EndBatchDraw();
		if (abs(GetRValue(buffer[pixel1])) <= 20
			&& abs(GetGValue(buffer[pixel1])) <= 20
			&& abs(GetBValue(buffer[pixel1])) <= 20)	//TODO
		{
			getflag = true;
			break;
		}
		Sleep(10);
	}
	return getflag;
}

int PutUp(Point Miner, Point* Grabber, Node** MineListH, bool get, int* score,
	IMAGE* pic, IMAGE* mask, time_t* Lefttime)
{
	IMAGE tempsave;
	Node *NodeP, *NodeS;
	int ItemNum = 0;
	OutputTime(Lefttime);
	if (get)
	{
		for (NodeP = *MineListH; NodeP != NULL; NodeP = NodeP->next)
		{
			if (NodeP->lefttop.x < Grabber->x + GRABWIDTH && NodeP->lefttop.x +
				NodeP->mine.MinePic->getwidth() > Grabber->x)
			{
				if (NodeP->lefttop.y - Grabber->y < 20)
					break;
			}
		}
		for (Grabber->y; Grabber->y > MINESPACEY; Grabber->y -= 5)
		{

		}
	}
	else
	{
		clearrectangle(Miner.x, Miner.y, Miner.x + MINERWIDTH, Miner.y + MINERHEIGHT);
		putimage(Miner.x, Miner.y, pic + 1);
		line(Miner.x + 42, Miner.y + 92, Grabber->x + 30, MINESPACEY);
		clearrectangle(Grabber->x, Grabber->y, Grabber->x + GRABWIDTH, Grabber->y + GRABHEIGHT);
		SetWorkingImage(pic);
		getimage(&tempsave, Grabber->x, Grabber->y, GRABWIDTH + 1, GRABHEIGHT + 1);
		SetWorkingImage();
		putimage(Grabber->x, Grabber->y, &tempsave);
		SetWorkingImage(pic);
		getimage(&tempsave, Grabber->x, MINESPACEY, GRABWIDTH + 1, Grabber->y);
		SetWorkingImage();
		PutLucidImage(Grabber->x, Grabber->y, pic + 16, mask);
		for (Grabber->y; Grabber->y > MINESPACEY; Grabber->y -= 5)
		{
			BeginBatchDraw();
			OutputTime(Lefttime);
			clearrectangle(Grabber->x, Grabber->y, Grabber->x + GRABWIDTH, Grabber->y + GRABHEIGHT);
			putimage(Grabber->x, MINESPACEY, &tempsave);
			line(Miner.x + 42, Miner.y + 92, Grabber->x + 30, Grabber->y);
			PutLucidImage(Grabber->x, Grabber->y, pic + 16, mask);
			FlushBatchDraw();
			EndBatchDraw();
			Sleep(10);
		}
	}
	return ItemNum;
}

int MainGame(IMAGE* pic, IMAGE* mask, Mine* mine)
{
	int Score = 0, ItemNum;
	Node *MineListH, *MineListP;
	time_t time = 10;
	time_t* Lefttime = &time;
	putimage(FNPLACEX, BOTTON1PLACE, pic + 12);
	OutputTime(Lefttime);
	int ValueSum = CreateList(&MineListH, mine);
	Point Miner, Grabber;
	Miner.x = MINERPLACEX;
	Miner.y = MINERPLACEY;
	Grabber.x = GRPLACEX;
	Grabber.y = GRPLACEY;
	for (MineListP = MineListH->next; MineListP != NULL; MineListP = MineListP->next)
	{
		PutLucidImage(MineListP->lefttop.x, MineListP->lefttop.y,
			MineListP->mine.MinePic, MineListP->mine.MineMaskPic);
	}

	while (true)
	{
		if (*Lefttime <= 0)
			break;
		OutputTime(Lefttime);
		if (MouseHit() == true)
		{ 
			MOUSEMSG mmsg = GetMouseMsg();
			if (mmsg.x >= FNPLACEX && mmsg.x <= FNPLACEX + BOTTONWIDTH)
			{
				if (mmsg.y >= BOTTON1PLACE && mmsg.y <= BOTTON1PLACE + BOTTONHEIGHT)
				{
					if (mmsg.mkLButton == true)
					{
						IMAGE saveground;
						getimage(&saveground, 0, 0, 800, 768);
						clearrectangle(0, 0, 800, 768);
						putimage(FNPLACEX, BOTTON1PLACE, pic + 9);
						time_t Pausetime = GetTickCount();
						system("pause");
						while (true)
						{
							MOUSEMSG pmsg = GetMouseMsg();
							if (pmsg.x >= FNPLACEX && pmsg.x <= FNPLACEX + BOTTONWIDTH)
							{
								if (pmsg.y >= BOTTON1PLACE && pmsg.y <= BOTTON1PLACE + BOTTONHEIGHT)
								{
									if (pmsg.mkLButton == true)
									{
										time_t Continuetime = GetTickCount();
										Lefttime += (Continuetime - Pausetime) / 1000;
										break;
									}
								}
							}
						}
						putimage(FNPLACEX, BOTTON1PLACE, pic + 12);
						putimage(0, 0, &saveground);
					//	TODO PAUSE
					}
				}
				else if (mmsg.y >= BOTTON2PLACE && mmsg.y <= BOTTON2PLACE + BOTTONHEIGHT)
				{
					//TODO: HIGHSCORE
				}
				else if (mmsg.y >= BOTTON3PLACE && mmsg.y <= BOTTON3PLACE + BOTTONHEIGHT)
				{
					if (mmsg.mkLButton == true)
					{
						if (MessageBox(GetHWnd(), "Are you sure to exit?", "Exit",
							MB_OKCANCEL) == true)
							return -1;
					}
				}
			}
		}
		if (_kbhit() != 0)
		{
			char DirectionKey1, DirectionKey2;
			DirectionKey1 = _getch();
			if (DirectionKey1 == -32)
			{
				DirectionKey2 = _getch();
				if (DirectionKey2 == 77)
				{
					if (BGWIDTH - Miner.x - MINERWIDTH >= 10)
					{
						MoveMiner(1, pic, mask, Miner, Grabber, Lefttime);
						Miner.x += 5;
						Grabber.x += 5;
					}
				}
				else if (DirectionKey2 == 75)
				{
					if (Miner.x >= 5)
					{
						MoveMiner(-1, pic, mask, Miner, Grabber, Lefttime);
						Miner.x -= 5;
						Grabber.x -= 5;
					}
				}
				else if (DirectionKey2 == 80)
				{
					bool getflag = PutDown(Miner, &Grabber, pic, mask, Lefttime);
					ItemNum = PutUp(Miner, &Grabber, &MineListH, getflag, &Score, pic, mask, Lefttime);
				}
			}
			Sleep(10);
		}
	}
	return Score;
}

int main()
{	
	IMAGE Pic[24];
	IMAGE MaskPic[8];
	Mine mine[10];
	char UserName[15];
	Initialize(Pic, MaskPic, mine);
	if (StartMenu(UserName, Pic, MaskPic) == true)
	{
		Sleep(10);
		MainGame(Pic, MaskPic, mine);
	}
	closegraph();

    return 0;
}