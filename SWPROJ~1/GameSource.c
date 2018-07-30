#include<stdio.h>
#include<Windows.h>
#include<time.h>
#include<conio.h>
#include"GameMap.h"
#include"GameMonitor.h"

#pragma warning(disable:4996)

#define LEFT 75
#define RIGHT 77
#define UP 72
#define DOWN 80
#define Zattack 122
#define Xattack 120

typedef struct NPC {
	COORD coordinate; // NPC의 좌표.
	int state; // NPC의 상태.
}NPC;

typedef struct item {
	COORD coordinate;
	int state;
}Item;

COORD PC = { 2,36 };
COORD Door[3] = { { 34,17 },{ 100,36 } ,{ 88,31 } };
Item key[3][3];
Item coin[3][10];
NPC npc[3][4]; // 이차원배열로 첫번째것은 스테이지.
int move[3][4] = { 0 };
NPC Rnpc[3][4];
NPC Cnpc[3][4];
int speed = 50; //속도.
int Ri[3][5] = { 0 }; // for randNpc
int Rcnt[3][5] = { { 4,4,4,4,4 },{ 4,4,4,4,4 },{ 4,4,4,4,4 } }; // for randNpc

int keyCount[3] = { { 1 } ,{ 2 },{ 3 } };
int coinCount[3] = { 3,3,3 };
int stage_num = 0;

int timer;
int timer_a;
int timer_b;
int timeAmt = 1000;
int score = 0;
int npc_time = 0;
int npc_up = 0; //for Movenpc up
int npc_range[3][5] = { 0 };

int find_ladder = 0;
int in_ladder = 0;
int RF = 0;

int cool = 0;
int way = 0;
int crush = 0;
int shw = 0;
COORD coolc;

int clear = 0;

void Set_Cursor(int x, int y)
{
	COORD position = { x, y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), position);
}

COORD Get_CursorPos(void)
{
	COORD point;
	CONSOLE_SCREEN_BUFFER_INFO CurInfo;

	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &CurInfo);
	point.X = CurInfo.dwCursorPosition.X;
	point.Y = CurInfo.dwCursorPosition.Y;

	return point;
}

int DetectCollision0(int X, int Y)
{
	int a;
	a = X / 2;
	if (mapModel[stage_num][Y][a] == 0)
		return 0;
	return 1;
}

int DetectCollision1(int X, int Y)
{
	int a;
	a = X / 2;
	if (mapModel[stage_num][Y][a] == 1)
		return 0;
	return 1;
}

int DetectCollision2(int X, int Y)
{
	int a;
	a = X / 2;
	if (mapModel[stage_num][Y][a] == 2)
		return 0;
	return 1;
}

int DetectCollision3(int X, int Y)
{
	int a;
	a = X / 2;
	if (mapModel[stage_num][Y][a] == 3)
		return 0;
	return 1;
}

int DetectCollision4(int S, int N, int X, int Y)//for key
{
	if (X == key[S][N].coordinate.X && Y == key[S][N].coordinate.Y)
		return 0;
	return 1;
}

int DetectCollision5(int S, int X, int Y)//for Door
{
	if (X == Door[S].X&& Y == Door[S].Y)
		return 0;
	return 1;
}

int TimeCount()  // 타이머 
{
	int i, j;
	Set_Cursor(10, 41);
	COORD curPos = Get_CursorPos();
	timer_a = timer / 10;
	timer_b = timer % 10;
	if (timer > 0)
	{
		printf("Timer : %d.%d ", timer_a, timer_b);
		timer--;

		if (cool > 0)
			cool--;
		if (cool == 0 && shw == 1)
		{
			for (i = 0; i < 3; i++)
			{
				for (j = 0; j < 4; j++)
				{
					npc[i][j].state = 0;
					Rnpc[i][j].state = 0;
					Cnpc[i][j].state = 0;
				}
			}
			shw = 0;
		}
		return 0;
	}
	return 1;
}

void ShowCoolTime()
{

	Set_Cursor(coolc.X, coolc.Y);

	if (cool == 0)
		printf("▶▶▶▶▶▶");
	else if (cool <= 5)
		printf("▶▶▶▶▶▷");
	else if (cool <= 10)
		printf("▶▶▶▶▷▷");
	else if (cool <= 15)
		printf("▶▶▶▷▷▷");
	else if (cool <= 20)
		printf("▶▶▷▷▷▷");
	else if (cool <= 25)
		printf("▶▷▷▷▷▷");
	else
		printf("▷▷▷▷▷▷");

}

void ShowBlock(char mapInfo[40][60])
{
	int x, y;
	COORD curPos = Get_CursorPos();
	for (y = 0; y < 40; y++)
	{
		for (x = 0; x < 60; x++)
		{
			Set_Cursor(curPos.X + (x * 2), curPos.Y + y);
			if (mapInfo[y][x] == 1)
				printf("▒");
			if (mapInfo[y][x] == 2)
				printf("▥");
			//if (mapInfo[y][x] == 3)
			//printf("↓");

		}
	}
}

void ShowMonitor(char mapInfo[40][60]) //시작화면, 게임오버화면 출력
{
	int x, y;
	COORD curPos = Get_CursorPos();
	for (y = 0; y < 40; y++)
	{
		for (x = 0; x < 60; x++)
		{
			Set_Cursor(curPos.X + (x * 2), curPos.Y + y);
			if (mapInfo[y][x] == 0)
				printf(" ");
			if (mapInfo[y][x] == 1)
				printf("※");
			if (mapInfo[y][x] == 3)
				printf("☆");

		}
	}
}

void DeleteBlock()
{
	int x, y;
	Set_Cursor(0, 0);
	COORD curPos = Get_CursorPos();
	for (y = 0; y < 60; y++)
	{
		for (x = 0; x < 60; x++)
		{
			Set_Cursor(curPos.X + (x * 2), curPos.Y + y);
			printf(" ");
		}
	}
}

void NpcCoordinate()// 여기 추가하면 npc의 위치 바뀜.
{
	npc[0][0].coordinate.X = 2;
	npc[0][0].coordinate.Y = 24;
	npc[0][0].state = 0;
	npc[0][1].coordinate.X = 80;
	npc[0][1].coordinate.Y = 30;
	npc[0][1].state = 0;
	npc[0][2].coordinate.X = 68;
	npc[0][2].coordinate.Y = 17;
	npc[0][2].state = 0;

	///////////////////////////////
	Rnpc[0][0].coordinate.X = 30;
	Rnpc[0][0].coordinate.Y = 17;
	Rnpc[0][0].state = 0;
	///////////////////////////////
	npc[1][0].coordinate.X = 12;
	npc[1][0].coordinate.Y = 24;
	npc[1][0].state = 0;
	npc[1][1].coordinate.X = 68;
	npc[1][1].coordinate.Y = 17;
	npc[1][1].state = 0;;
	Rnpc[1][0].coordinate.X = 6;
	Rnpc[1][0].coordinate.Y = 17;
	Rnpc[1][0].state = 0;

	Cnpc[1][0].coordinate.X = 68;
	Cnpc[1][0].coordinate.Y = 9;
	Cnpc[1][0].state = 0;
	Cnpc[1][1].coordinate.X = 100;
	Cnpc[1][1].coordinate.Y = 36;
	Cnpc[1][1].state = 0;
	Cnpc[1][2].coordinate.X = 100;
	Cnpc[1][2].coordinate.Y = 6;
	Cnpc[1][2].state = 0;

	////////////////////////////////
	npc[2][0].coordinate.X = 20;
	npc[2][0].coordinate.Y = 36;
	npc[2][0].state = 0;
	npc[2][1].coordinate.X = 28;
	npc[2][1].coordinate.Y = 17;
	npc[2][1].state = 0;
	npc[2][2].coordinate.X = 68;
	npc[2][2].coordinate.Y = 9;
	npc[2][2].state = 0;
	npc[2][3].coordinate.X = 90;
	npc[2][3].coordinate.Y = 3;
	npc[2][3].state = 0;


	Rnpc[2][0].coordinate.X = 44;
	Rnpc[2][0].coordinate.Y = 23;
	Rnpc[2][0].state = 0;
	Rnpc[2][1].coordinate.X = 92;
	Rnpc[2][1].coordinate.Y = 31;
	Rnpc[2][1].state = 0;
	Rnpc[2][2].coordinate.X = 52;
	Rnpc[2][2].coordinate.Y = 5;
	Rnpc[2][2].state = 0;

	Cnpc[2][0].coordinate.X = 64;
	Cnpc[2][0].coordinate.Y = 9;
	Cnpc[2][0].state = 0;
	Cnpc[2][1].coordinate.X = 94;
	Cnpc[2][1].coordinate.Y = 36;
	Cnpc[2][1].state = 0;
}

void ShowPC(COORD C)
{
	Set_Cursor(C.X, C.Y);
	printf("Ω");
}

void DeleteC(COORD C)  // C 는 캐릭터
{
	Set_Cursor(C.X, C.Y);
	printf(" ");
}

void ShowA(COORD C)
{
	Set_Cursor(C.X, C.Y);
	if (way == 0)
		printf("☞");
	else
		printf("☜");
}

void ShowNPC(int S, int N)//NPC
{
	Set_Cursor(npc[S][N].coordinate.X, npc[S][N].coordinate.Y);
	printf("♠");
}

void ShowRNPC(int S, int N)//NPC
{
	Set_Cursor(Rnpc[S][N].coordinate.X, Rnpc[S][N].coordinate.Y);
	printf("★");
}

void ShowCNPC(int S, int N)//NPC
{
	Set_Cursor(Cnpc[S][N].coordinate.X, Cnpc[S][N].coordinate.Y);
	printf("♬");
}

void MoveNPC(int i, int S, int N)
{
	if (npc[S][N].state == 0)
	{
		if (i) {
			DeleteC(npc[S][N].coordinate);
			if (!DetectCollision2(npc[S][N].coordinate.X, npc[S][N].coordinate.Y))
			{
				Set_Cursor(npc[S][N].coordinate.X, npc[S][N].coordinate.Y);
				printf("▥");
			}
			for (int i = 0; i < S + 1; i++)
			{
				if (!DetectCollision4(S, i, npc[S][N].coordinate.X, npc[S][N].coordinate.Y))
				{
					if (key[S][i].state == 1)
					{
						Set_Cursor(npc[S][N].coordinate.X, npc[S][N].coordinate.Y);
						printf("♀");
					}
				}
			}
			if (!DetectCollision5(S, npc[S][N].coordinate.X, npc[S][N].coordinate.Y))
			{
				if (keyCount[S] == 0 && PC.X != Door[S].X && PC.Y != Door[S].Y)
				{
					Set_Cursor(npc[S][N].coordinate.X, npc[S][N].coordinate.Y);
					printf("∩");
				}
			}
			for (int i = 0; i < S + 1; i++)
			{
				if (!DetectCollision4(S, i, npc[S][N].coordinate.X, npc[S][N].coordinate.Y))
				{
					if (key[S][i].state == 1)
					{
						Set_Cursor(npc[S][N].coordinate.X, npc[S][N].coordinate.Y);
						printf("♀");
					}
				}
			}
			if (!DetectCollision5(S, npc[S][N].coordinate.X, npc[S][N].coordinate.Y))
			{
				if (keyCount[S] == 0 && PC.X != Door[S].X && PC.Y != Door[S].Y)
				{
					Set_Cursor(npc[S][N].coordinate.X, npc[S][N].coordinate.Y);
					printf("∩");
				}
			}
			npc[S][N].coordinate.X -= 2;
			Set_Cursor(npc[S][N].coordinate.X, npc[S][N].coordinate.Y);
			ShowNPC(S, N);
		}
		else
		{
			DeleteC(npc[S][N].coordinate);
			if (!DetectCollision2(npc[S][N].coordinate.X, npc[S][N].coordinate.Y))
			{
				Set_Cursor(npc[S][N].coordinate.X, npc[S][N].coordinate.Y);
				printf("▥");
			}
			for (int i = 0; i < S + 1; i++)
			{
				if (!DetectCollision4(S, i, npc[S][N].coordinate.X, npc[S][N].coordinate.Y))
				{
					if (key[S][i].state == 1)
					{
						Set_Cursor(npc[S][N].coordinate.X, npc[S][N].coordinate.Y);
						printf("♀");
					}
				}
			}
			if (!DetectCollision5(S, npc[S][N].coordinate.X, npc[S][N].coordinate.Y))
			{
				if (keyCount[S] == 0 && PC.X != Door[S].X && PC.Y != Door[S].Y)
				{
					Set_Cursor(npc[S][N].coordinate.X, npc[S][N].coordinate.Y);
					printf("∩");
				}
			}
			npc[S][N].coordinate.X += 2;
			Set_Cursor(npc[S][N].coordinate.X, npc[S][N].coordinate.Y);
			ShowNPC(S, N);
		}
	}
	else
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
		DeleteC(npc[S][N].coordinate);
		Set_Cursor(npc[S][N].coordinate.X, npc[S][N].coordinate.Y);
		ShowNPC(S, N);
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);

	}
}

void up_MoveNPC(int S, int N, int R)
{
	if (npc[S][N].state == 0)
	{
		if (npc_up == 1) {
			DeleteC(npc[S][N].coordinate);
			if (!DetectCollision2(npc[S][N].coordinate.X, npc[S][N].coordinate.Y))
			{
				Set_Cursor(npc[S][N].coordinate.X, npc[S][N].coordinate.Y);
				printf("▥");
			}
			for (int i = 0; i < S + 1; i++)
			{
				if (!DetectCollision4(S, i, npc[S][N].coordinate.X, npc[S][N].coordinate.Y))
				{
					if (key[S][i].state == 1)
					{
						Set_Cursor(npc[S][N].coordinate.X, npc[S][N].coordinate.Y);
						printf("♀");
					}
				}
			}
			if (!DetectCollision5(S, npc[S][N].coordinate.X, npc[S][N].coordinate.Y))
			{
				if (keyCount[S] == 0 && PC.X != Door[S].X && PC.Y != Door[S].Y)
				{
					Set_Cursor(npc[S][N].coordinate.X, npc[S][N].coordinate.Y);
					printf("∩");
				}
			}
			npc[S][N].coordinate.Y--;
			Set_Cursor(npc[S][N].coordinate.X, npc[S][N].coordinate.Y);
			ShowNPC(S, N);
			if (mapModel[S][npc[S][N].coordinate.Y][npc[S][N].coordinate.X / 2] == 0)
			{
				npc_up = 0;
			}
			return;
		}
		else if (npc_up == -1)
		{
			DeleteC(npc[S][N].coordinate);
			if (!DetectCollision2(npc[S][N].coordinate.X, npc[S][N].coordinate.Y))
			{
				Set_Cursor(npc[S][N].coordinate.X, npc[S][N].coordinate.Y);
				printf("▥");
			}
			for (int i = 0; i < S + 1; i++)
			{
				if (!DetectCollision4(S, i, npc[S][N].coordinate.X, npc[S][N].coordinate.Y))
				{
					if (key[S][i].state == 1)
					{
						Set_Cursor(npc[S][N].coordinate.X, npc[S][N].coordinate.Y);
						printf("♀");
					}
				}
			}
			if (!DetectCollision5(S, npc[S][N].coordinate.X, npc[S][N].coordinate.Y))
			{
				if (keyCount[S] == 0 && PC.X != Door[S].X && PC.Y != Door[S].Y)
				{
					Set_Cursor(npc[S][N].coordinate.X, npc[S][N].coordinate.Y);
					printf("∩");
				}
			}
			npc[S][N].coordinate.Y++;
			Set_Cursor(npc[S][N].coordinate.X, npc[S][N].coordinate.Y);
			ShowNPC(S, N);
			if (mapModel[S][npc[S][N].coordinate.Y + 1][npc[S][N].coordinate.X / 2] == 1)
			{
				npc_up = 0;
			}
			return;
		}
		else if (npc_up == 0)
		{
			DeleteC(npc[S][N].coordinate);
			if (!DetectCollision2(npc[S][N].coordinate.X, npc[S][N].coordinate.Y))
			{
				Set_Cursor(npc[S][N].coordinate.X, npc[S][N].coordinate.Y);
				printf("▥");
			}
			for (int i = 0; i < S + 1; i++)
			{
				if (!DetectCollision4(S, i, npc[S][N].coordinate.X, npc[S][N].coordinate.Y))
				{
					if (key[S][i].state == 1)
					{
						Set_Cursor(npc[S][N].coordinate.X, npc[S][N].coordinate.Y);
						printf("♀");
					}
				}
			}
			if (!DetectCollision5(S, npc[S][N].coordinate.X, npc[S][N].coordinate.Y))
			{
				if (keyCount[S] == 0 && PC.X != Door[S].X && PC.Y != Door[S].Y)
				{
					Set_Cursor(npc[S][N].coordinate.X, npc[S][N].coordinate.Y);
					printf("∩");
				}
			}
			if (npc_range[S][N] % (R * 2) < R)
			{
				npc[S][N].coordinate.X += 2;
			}
			else
			{
				npc[S][N].coordinate.X -= 2;
			}
			Set_Cursor(npc[S][N].coordinate.X, npc[S][N].coordinate.Y);
			ShowNPC(S, N);
			npc_range[S][N]++;
			if (npc_range[S][N] / R % 2 == 0 && mapModel[S][npc[S][N].coordinate.Y][npc[S][N].coordinate.X / 2] == 2)
			{
				npc_up = 1;
			}
			else if (npc_range[S][N] / R % 2 == 1 && mapModel[S][npc[S][N].coordinate.Y + 1][npc[S][N].coordinate.X / 2] == 2)
			{
				npc_up = -1;
			}
			return;
		}
	}

	else
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 13);
		DeleteC(npc[S][N].coordinate);
		Set_Cursor(npc[S][N].coordinate.X, npc[S][N].coordinate.Y);
		ShowNPC(S, N);
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
	}
}

void RMoveNPC(int Fx, int Lx, int S, int N) //랜덤 NPC의 패턴은 3가지 이며 지정된 좌표 (Fx,Lx)(처음위치 마지막 위치)를 왔다갔다 한다.
{
	if (Rnpc[S][N].state == 0)
	{
		if (Rcnt[S][N] == 4) {
			Ri[S][N] = rand() % 3; //Ri 는 랜덤하게 움직이는 패턴의 종류.
			Rcnt[S][N] = 0;
		}

		if (Rnpc[S][N].coordinate.X > Lx) // RNPC 의 위치좌표. 제한범위.
		{
			DeleteC(Rnpc[S][N].coordinate);
			if (!DetectCollision2(Rnpc[S][N].coordinate.X, Rnpc[S][N].coordinate.Y))
			{
				Set_Cursor(Rnpc[S][N].coordinate.X, Rnpc[S][N].coordinate.Y);
				printf("▥");
			}
			for (int i = 0; i < S + 1; i++)
			{
				if (!DetectCollision4(S, i, Rnpc[S][N].coordinate.X, Rnpc[S][N].coordinate.Y))
				{
					if (key[S][i].state == 1)
					{
						Set_Cursor(Rnpc[S][N].coordinate.X, Rnpc[S][N].coordinate.Y);
						printf("♀");
					}
				}
			}
			if (!DetectCollision5(S, npc[S][N].coordinate.X, npc[S][N].coordinate.Y))
			{
				if (keyCount[S] == 0 && PC.X != Door[S].X && PC.Y != Door[S].Y)
				{
					Set_Cursor(npc[S][N].coordinate.X, npc[S][N].coordinate.Y);
					printf("∩");
				}
			}
			Rnpc[S][N].coordinate.X -= 2;
			Set_Cursor(Rnpc[S][N].coordinate.X, Rnpc[S][N].coordinate.Y);
			Rcnt[S][N] = 1;
			Ri[S][N] = 0;
			ShowRNPC(S, N);

			return;
		}
		else if (Rnpc[S][N].coordinate.X < Fx)
		{
			DeleteC(Rnpc[S][N].coordinate);

			if (!DetectCollision2(Rnpc[S][N].coordinate.X, Rnpc[S][N].coordinate.Y))
			{
				Set_Cursor(Rnpc[S][N].coordinate.X, Rnpc[S][N].coordinate.Y);
				printf("▥");
			}
			for (int i = 0; i < S + 1; i++)
			{
				if (!DetectCollision4(S, i, Rnpc[S][N].coordinate.X, Rnpc[S][N].coordinate.Y))
				{
					if (key[S][i].state == 1)
					{
						Set_Cursor(Rnpc[S][N].coordinate.X, Rnpc[S][N].coordinate.Y);
						printf("♀");
					}
				}
			}
			if (!DetectCollision5(S, npc[S][N].coordinate.X, npc[S][N].coordinate.Y))
			{
				if (keyCount[S] == 0 && PC.X != Door[S].X && PC.Y != Door[S].Y)
				{
					Set_Cursor(npc[S][N].coordinate.X, npc[S][N].coordinate.Y);
					printf("∩");
				}
			}
			Rnpc[S][N].coordinate.X += 2;
			Set_Cursor(Rnpc[S][N].coordinate.X, Rnpc[S][N].coordinate.Y);
			Rcnt[S][N] = 1;
			Ri[S][N] = 1;
			ShowRNPC(S, N);

			return;
		}
		if (Ri[S][N] == 0) {
			DeleteC(Rnpc[S][N].coordinate);
			if (!DetectCollision2(Rnpc[S][N].coordinate.X, Rnpc[S][N].coordinate.Y))
			{
				Set_Cursor(Rnpc[S][N].coordinate.X, Rnpc[S][N].coordinate.Y);
				printf("▥");
			}
			for (int i = 0; i < S + 1; i++)
			{
				if (!DetectCollision4(S, i, Rnpc[S][N].coordinate.X, Rnpc[S][N].coordinate.Y))
				{
					if (key[S][i].state == 1)
					{
						Set_Cursor(Rnpc[S][N].coordinate.X, Rnpc[S][N].coordinate.Y);
						printf("♀");
					}
				}
			}
			if (!DetectCollision5(S, npc[S][N].coordinate.X, npc[S][N].coordinate.Y))
			{
				if (keyCount[S] == 0 && PC.X != Door[S].X && PC.Y != Door[S].Y)
				{
					Set_Cursor(npc[S][N].coordinate.X, npc[S][N].coordinate.Y);
					printf("∩");
				}
			}
			Rnpc[S][N].coordinate.X -= 2;
			Set_Cursor(Rnpc[S][N].coordinate.X, Rnpc[S][N].coordinate.Y);
			ShowRNPC(S, N);
			Rcnt[S][N]++;

		}
		else if (Ri[S][N] == 1)
		{
			DeleteC(Rnpc[S][N].coordinate);
			if (!DetectCollision2(Rnpc[S][N].coordinate.X, Rnpc[S][N].coordinate.Y))
			{
				Set_Cursor(Rnpc[S][N].coordinate.X, Rnpc[S][N].coordinate.Y);
				printf("▥");
			}
			for (int i = 0; i < S + 1; i++)
			{
				if (!DetectCollision4(S, i, Rnpc[S][N].coordinate.X, Rnpc[S][N].coordinate.Y))
				{
					if (key[S][i].state == 1)
					{
						Set_Cursor(Rnpc[S][N].coordinate.X, Rnpc[S][N].coordinate.Y);
						printf("♀");
					}
				}
			}
			if (!DetectCollision5(S, npc[S][N].coordinate.X, npc[S][N].coordinate.Y))
			{
				if (keyCount[S] == 0 && PC.X != Door[S].X && PC.Y != Door[S].Y)
				{
					Set_Cursor(npc[S][N].coordinate.X, npc[S][N].coordinate.Y);
					printf("∩");
				}
			}
			Rnpc[S][N].coordinate.X += 2;
			Set_Cursor(Rnpc[S][N].coordinate.X, Rnpc[S][N].coordinate.Y);
			ShowRNPC(S, N);
			Rcnt[S][N]++;
		}
		else if (Ri[S][N] == 2)
		{
			if (Rcnt[S][N] < 2)
			{
				DeleteC(Rnpc[S][N].coordinate);
				if (!DetectCollision2(Rnpc[S][N].coordinate.X, Rnpc[S][N].coordinate.Y))
				{
					Set_Cursor(Rnpc[S][N].coordinate.X, Rnpc[S][N].coordinate.Y);
					printf("▥");
				}
				for (int i = 0; i < S + 1; i++)
				{
					if (!DetectCollision4(S, i, Rnpc[S][N].coordinate.X, Rnpc[S][N].coordinate.Y))
					{
						if (key[S][i].state == 1)
						{
							Set_Cursor(Rnpc[S][N].coordinate.X, Rnpc[S][N].coordinate.Y);
							printf("♀");
						}
					}
				}
				if (!DetectCollision5(S, npc[S][N].coordinate.X, npc[S][N].coordinate.Y))
				{
					if (keyCount[S] == 0 && PC.X != Door[S].X && PC.Y != Door[S].Y)
					{
						Set_Cursor(npc[S][N].coordinate.X, npc[S][N].coordinate.Y);
						printf("∩");
					}
				}
				Rnpc[S][N].coordinate.X += 2;
				Set_Cursor(Rnpc[S][N].coordinate.X, Rnpc[S][N].coordinate.Y);
				ShowRNPC(S, N);
				Rcnt[S][N]++;
			}
			else
			{
				DeleteC(Rnpc[S][N].coordinate);
				if (!DetectCollision2(Rnpc[S][N].coordinate.X, Rnpc[S][N].coordinate.Y))
				{
					Set_Cursor(Rnpc[S][N].coordinate.X, Rnpc[S][N].coordinate.Y);
					printf("▥");
				}
				for (int i = 0; i < S + 1; i++)
				{
					if (!DetectCollision4(S, i, Rnpc[S][N].coordinate.X, Rnpc[S][N].coordinate.Y))
					{
						if (key[S][i].state == 1)
						{
							Set_Cursor(Rnpc[S][N].coordinate.X, Rnpc[S][N].coordinate.Y);
							printf("♀");
						}
					}
				}
				if (!DetectCollision5(S, npc[S][N].coordinate.X, npc[S][N].coordinate.Y))
				{
					if (keyCount[S] == 0 && PC.X != Door[S].X && PC.Y != Door[S].Y)
					{
						Set_Cursor(npc[S][N].coordinate.X, npc[S][N].coordinate.Y);
						printf("∩");
					}
				}
				Rnpc[S][N].coordinate.X -= 2;
				Set_Cursor(Rnpc[S][N].coordinate.X, Rnpc[S][N].coordinate.Y);
				ShowRNPC(S, N);
				Rcnt[S][N]++;
			}
		}
	}
	else
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
		DeleteC(Rnpc[S][N].coordinate);
		Set_Cursor(Rnpc[S][N].coordinate.X, Rnpc[S][N].coordinate.Y);
		ShowRNPC(S, N);
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
	}
}

void CnpcRight(int S, int N)
{
	int Nx = Cnpc[S][N].coordinate.X;
	int Nx2 = (Cnpc[S][N].coordinate.X) / 2;
	int Ny = Cnpc[S][N].coordinate.Y;

	if ((mapModel[S][Ny][Nx2 + 1] == 0 || mapModel[S][Ny][Nx2 + 1] == 2) && mapModel[S][Ny + 1][Nx2 + 1] != 3)
	{
		DeleteC(Cnpc[S][N].coordinate);
		if (!DetectCollision2(Cnpc[S][N].coordinate.X, Cnpc[S][N].coordinate.Y))
		{
			Set_Cursor(Cnpc[S][N].coordinate.X, Cnpc[S][N].coordinate.Y);
			printf("▥");
		}
		for (int i = 0; i < S + 1; i++)
		{
			if (!DetectCollision4(S, i, Cnpc[S][N].coordinate.X, Cnpc[S][N].coordinate.Y))
			{
				if (key[S][i].state == 1)
				{
					Set_Cursor(Cnpc[S][N].coordinate.X, Cnpc[S][N].coordinate.Y);
					printf("♀");
				}
			}
		}
		if (!DetectCollision5(S, npc[S][N].coordinate.X, npc[S][N].coordinate.Y))
		{
			if (keyCount[S] == 0 && PC.X != Door[S].X && PC.Y != Door[S].Y)
			{
				Set_Cursor(npc[S][N].coordinate.X, npc[S][N].coordinate.Y);
				printf("∩");
			}
		}

		Cnpc[S][N].coordinate.X += 2;
		Set_Cursor(Cnpc[S][N].coordinate.X, Cnpc[S][N].coordinate.Y);
		ShowCNPC(S, N);

	}
	else
	{
		RF = 0;
	}
}

void CnpcLeft(int S, int N)
{
	int Nx = Cnpc[S][N].coordinate.X;
	int Nx2 = (Cnpc[S][N].coordinate.X) / 2;
	int Ny = Cnpc[S][N].coordinate.Y;

	if ((mapModel[S][Ny][Nx2 - 1] == 0 || mapModel[S][Ny][Nx2 - 1] == 2) && mapModel[S][Ny + 1][Nx2 - 1] != 3)
	{
		DeleteC(Cnpc[S][N].coordinate);
		if (!DetectCollision2(Cnpc[S][N].coordinate.X, Cnpc[S][N].coordinate.Y))
		{
			Set_Cursor(Cnpc[S][N].coordinate.X, Cnpc[S][N].coordinate.Y);
			printf("▥");
		}
		for (int i = 0; i < S + 1; i++)
		{
			if (!DetectCollision4(S, i, Cnpc[S][N].coordinate.X, Cnpc[S][N].coordinate.Y))
			{
				if (key[S][i].state == 1)
				{
					Set_Cursor(Cnpc[S][N].coordinate.X, Cnpc[S][N].coordinate.Y);
					printf("♀");
				}
			}
		}
		if (!DetectCollision5(S, npc[S][N].coordinate.X, npc[S][N].coordinate.Y))
		{
			if (keyCount[S] == 0 && PC.X != Door[S].X && PC.Y != Door[S].Y)
			{
				Set_Cursor(npc[S][N].coordinate.X, npc[S][N].coordinate.Y);
				printf("∩");
			}
		}
		Cnpc[S][N].coordinate.X -= 2;
		Set_Cursor(Cnpc[S][N].coordinate.X, Cnpc[S][N].coordinate.Y);
		ShowCNPC(S, N);

	}
	else
	{
		RF = 0;
	}
}

void CnpcUP(int S, int N) {
	int Nx = Cnpc[S][N].coordinate.X;
	int Nx2 = (Cnpc[S][N].coordinate.X) / 2;
	int Ny = Cnpc[S][N].coordinate.Y;

	if ((mapModel[S][Ny - 1][Nx2] == 2 || mapModel[S][Ny - 1][Nx2] == 0) && mapModel[S][Ny][Nx2] == 2)
	{
		DeleteC(Cnpc[S][N].coordinate);
		if (!DetectCollision2(Cnpc[S][N].coordinate.X, Cnpc[S][N].coordinate.Y))
		{
			Set_Cursor(Cnpc[S][N].coordinate.X, Cnpc[S][N].coordinate.Y);
			printf("▥");
		}
		for (int i = 0; i < S + 1; i++)
		{
			if (!DetectCollision4(S, i, Cnpc[S][N].coordinate.X, Cnpc[S][N].coordinate.Y))
			{
				if (key[S][i].state == 1)
				{
					Set_Cursor(Cnpc[S][N].coordinate.X, Cnpc[S][N].coordinate.Y);
					printf("♀");
				}
			}
		}
		if (!DetectCollision5(S, npc[S][N].coordinate.X, npc[S][N].coordinate.Y))
		{
			if (keyCount[S] == 0 && PC.X != Door[S].X && PC.Y != Door[S].Y)
			{
				Set_Cursor(npc[S][N].coordinate.X, npc[S][N].coordinate.Y);
				printf("∩");
			}
		}
		Cnpc[S][N].coordinate.Y--;
		Set_Cursor(Cnpc[S][N].coordinate.X, Cnpc[S][N].coordinate.Y);
		ShowCNPC(S, N);
	}
	else
	{
		RF = 0;
	}
}

void CnpcDown(int S, int N) {
	int Nx = Cnpc[S][N].coordinate.X;
	int Nx2 = (Cnpc[S][N].coordinate.X) / 2;
	int Ny = Cnpc[S][N].coordinate.Y;

	if ((mapModel[S][Ny][Nx2] == 0 || mapModel[S][Ny][Nx2] == 2) && mapModel[S][Ny + 1][Nx2] == 2)
	{

		DeleteC(Cnpc[S][N].coordinate);
		if (!DetectCollision2(Cnpc[S][N].coordinate.X, Cnpc[S][N].coordinate.Y))
		{
			Set_Cursor(Cnpc[S][N].coordinate.X, Cnpc[S][N].coordinate.Y);
			printf("▥");
		}
		for (int i = 0; i < S + 1; i++)
		{
			if (!DetectCollision4(S, i, Cnpc[S][N].coordinate.X, Cnpc[S][N].coordinate.Y))
			{
				if (key[S][i].state == 1)
				{
					Set_Cursor(Cnpc[S][N].coordinate.X, Cnpc[S][N].coordinate.Y);
					printf("♀");
				}
			}
		}
		if (!DetectCollision5(S, npc[S][N].coordinate.X, npc[S][N].coordinate.Y))
		{
			if (keyCount[S] == 0 && PC.X != Door[S].X && PC.Y != Door[S].Y)
			{
				Set_Cursor(npc[S][N].coordinate.X, npc[S][N].coordinate.Y);
				printf("∩");
			}
		}
		Cnpc[S][N].coordinate.Y++;
		Set_Cursor(Cnpc[S][N].coordinate.X, Cnpc[S][N].coordinate.Y);
		ShowCNPC(S, N);

	}
	else
	{
		RF = 0;
	}
}

void CCMoveNPC(int S, int N, int X1, int X2, int Y1, int Y2) {

	int Nx = Cnpc[S][N].coordinate.X;
	int Nx2 = (Cnpc[S][N].coordinate.X) / 2;
	int Ny = Cnpc[S][N].coordinate.Y;
	int i = 1;
	if (Cnpc[S][N].state == 0)
	{
		if (PC.X / 2 < X1 || PC.X / 2 > X2 || PC.Y < Y1 || PC.Y >Y2)
		{
			find_ladder = 0;
			RF = 0;
			ShowCNPC(S, N);
			return;
		}

		if (find_ladder != 0)
		{
			if (find_ladder > 0)
			{
				CnpcRight(S, N);
				find_ladder--;
				if (find_ladder == 0)
				{
					if (mapModel[S][Ny + 1][Nx2 + 1] == 2)
						RF = 1;
					else
						RF = -1;
				}

				return;
			}
			else
			{
				CnpcLeft(S, N);
				find_ladder++;
				if (find_ladder == 0)
				{
					if (mapModel[S][Ny + 1][Nx2 - 1] == 2)
						RF = 1;
					else
						RF = -1;
				}
				return;
			}
		}

		if (in_ladder != 0)
		{
			if (in_ladder > 0)
			{
				CnpcUP(S, N);
				if (mapModel[S][Ny - 1][Nx2] == 0)
				{
					in_ladder = 0;
				}
				return;
			}
			else
			{
				CnpcDown(S, N);
				if (mapModel[S][Ny + 2][Nx2] == 1)
				{
					in_ladder = 0;
				}
				return;
			}
		}

		if (PC.X < Nx) // 왼쪽에 있을때 피씨가
		{
			if ((mapModel[S][Ny][Nx2 - 1] == 0 || mapModel[S][Ny][Nx2 - 1] == 2) && mapModel[S][Ny + 1][Nx2 - 1] != 3 && RF == 0)
			{
				CnpcLeft(S, N);
				return;
			}
			if (PC.Y > Ny)
			{
				if (RF == 1)
				{
					CnpcDown(S, N);
					if (mapModel[S][Ny + 2][Nx2] == 1)
						RF = 0;
					return;
				}
				else if (RF == -1)
				{
					CnpcUP(S, N);
					if (mapModel[S][Ny - 1] == 0)
						RF = 0;
					return;
				}
				if (mapModel[S][Ny + 1][Nx2] == 2)
				{
					CnpcDown(S, N);
					in_ladder--;
					return;
				}

				while (1)
				{
					if (mapModel[S][Ny + 1][Nx2 + i] == 2)
					{
						find_ladder += i;
					}

					if (find_ladder > 0)
					{
						CnpcRight(S, N);
						find_ladder--;
						return;
					}

					i++;
				}

			}
			else if (PC.Y == Ny)
			{
				if (RF == 1)
				{
					CnpcDown(S, N);
					if (mapModel[S][Ny + 2][Nx2] == 1)
						RF = 0;
					return;
				}
				if (RF == -1)
				{
					CnpcUP(S, N);
					if (mapModel[S][Ny - 1] == 0)
						RF = 0;
					return;
				}
				while (1)
				{
					if (mapModel[S][Ny + 1][Nx2 + i] == 2)
					{
						find_ladder += i;
						CnpcRight(S, N);
						find_ladder--;
						return;
					}
					if (mapModel[S][Ny + 1][Nx2 - i] == 2)
					{
						find_ladder -= i;
						CnpcLeft(S, N);
						find_ladder++;
						return;
					}
					if (mapModel[S][Ny][Nx2 + i] == 2)
					{
						find_ladder += i;
						CnpcRight(S, N);
						find_ladder--;
						return;
					}
					if (mapModel[S][Ny][Nx2 - i] == 2)
					{
						find_ladder -= i;
						CnpcLeft(S, N);
						find_ladder++;
						return;
					}
					i++;
				}
			}
			else
			{

				if (mapModel[S][Ny][Nx2] == 2)
				{
					CnpcUP(S, N);
					in_ladder++;
					return;
				}

				while (1)
				{

					if (mapModel[S][Ny][Nx2 + i] == 2)
					{
						find_ladder += i;
					}

					if (find_ladder > 0)
					{
						CnpcRight(S, N);
						find_ladder--;
						return;
					}

					i++;
				}
			}
		}
		else if (PC.X == Nx) //피씨와 엔피씨가 같은 엑스일때
		{

			if (PC.Y > Ny)
			{
				if (mapModel[S][Ny + 1][Nx2] == 2)
				{
					CnpcDown(S, N);
					in_ladder--;
					return;
				}

				while (1)
				{
					if (mapModel[S][Ny + 1][Nx2 + i] == 2)
					{
						find_ladder += i;
						CnpcRight(S, N);
						find_ladder--;
						return;
					}
					if (mapModel[S][Ny + 1][Nx2 - i] == 2)
					{
						find_ladder -= i;
						CnpcLeft(S, N);
						find_ladder++;
						return;
					}
					i++;
				}
			}
			else
			{
				if (mapModel[S][Ny][Nx2] == 2)
				{
					CnpcUP(S, N);
					in_ladder++;
					return;
				}
				while (1)
				{
					if (mapModel[S][Ny + 1][Nx2 + i] == 2)
					{
						find_ladder += i;
						CnpcRight(S, N);
						find_ladder--;
						return;
					}
					if (mapModel[S][Ny + 1][Nx2 - i] == 2)
					{
						find_ladder -= i;
						CnpcLeft(S, N);
						find_ladder++;
						return;
					}

					i++;
				}
			}
		}
		else //피씨가 엔피씨 오른쪽에 있을때
		{
			if ((mapModel[S][Ny][Nx2 + 1] == 0 || mapModel[S][Ny][Nx2 + 1] == 2) && mapModel[S][Ny + 1][Nx2 + 1] != 3 && RF == 0)
			{
				CnpcRight(S, N);
				return;
			}
			if (PC.Y > Ny)
			{
				if (RF == 1)
				{
					CnpcDown(S, N);
					if (mapModel[S][Ny + 2][Nx2] == 1)
						RF = 0;
					return;
				}
				if (RF == -1)
				{
					CnpcUP(S, N);
					if (mapModel[S][Ny - 1] == 0)
						RF = 0;
					return;
				}

				if (mapModel[S][Ny + 1][Nx2] == 2)
				{
					CnpcDown(S, N);
					in_ladder--;
					return;
				}

				while (1)
				{
					if (mapModel[S][Ny + 1][Nx2 - i] == 2)
					{
						find_ladder -= i;
					}

					if (find_ladder < 0)
					{
						CnpcLeft(S, N);
						find_ladder++;
						return;
					}

					i++;
				}

			}
			else if (PC.Y == Ny)
			{
				if (RF == 1)
				{
					CnpcDown(S, N);
					if (mapModel[S][Ny + 2][Nx2] == 1)
						RF = 0;
					return;
				}
				if (RF == -1)
				{
					CnpcUP(S, N);
					if (mapModel[S][Ny - 1] == 0)
						RF = 0;
					return;
				}
				while (1)
				{
					if (mapModel[S][Ny + 1][Nx2 - i] == 2)
					{
						find_ladder -= i;
						CnpcLeft(S, N);
						find_ladder++;
						return;
					}

					if (mapModel[S][Ny + 1][Nx2 + i] == 2)
					{
						find_ladder += i;
						CnpcRight(S, N);
						find_ladder--;
						return;
					}

					if (mapModel[S][Ny][Nx2 + i] == 2)
					{
						find_ladder -= i;
						CnpcLeft(S, N);
						find_ladder++;
						return;
					}

					if (mapModel[S][Ny][Nx2 + i] == 2)
					{
						find_ladder += i;
						CnpcRight(S, N);
						find_ladder--;
						return;
					}

					i++;
				}
			}
			else
			{
				if (RF == 1)
				{
					CnpcDown(S, N);
					if (mapModel[S][Ny + 2][Nx2] == 1)
						RF = 0;
					return;
				}
				if (RF == -1)
				{
					CnpcUP(S, N);
					if (mapModel[S][Ny - 1] == 0)
						RF = 0;
					return;
				}
				if (mapModel[S][Ny][Nx2] == 2)
				{
					CnpcUP(S, N);
					in_ladder++;
					return;
				}

				while (1)
				{

					if (mapModel[S][Ny][Nx2 - i] == 2)
					{
						find_ladder -= i;
					}

					if (find_ladder < 0)
					{
						CnpcLeft(S, N);
						find_ladder++;
						return;
					}
					i++;
				}
			}
		}
	}
	else
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
		DeleteC(Cnpc[S][N].coordinate);
		Set_Cursor(Cnpc[S][N].coordinate.X, Cnpc[S][N].coordinate.Y);
		ShowCNPC(S, N);
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
	}
}

void CsameX(int S, int N)
{
	if (Cnpc[S][N].state == 0)
	{
		if (mapModel[S][Cnpc[S][N].coordinate.Y][Cnpc[S][N].coordinate.X / 2 - 1] == 0 || mapModel[S][Cnpc[S][N].coordinate.Y][Cnpc[S][N].coordinate.X / 2 + 1] == 0 || mapModel[S][Cnpc[S][N].coordinate.Y][Cnpc[S][N].coordinate.X / 2 - 1] == 2 && mapModel[S][Cnpc[S][N].coordinate.Y][Cnpc[S][N].coordinate.X / 2 + 1] == 2)
		{
			if (PC.Y == Cnpc[S][N].coordinate.Y)
			{

				if (PC.X < Cnpc[S][N].coordinate.X)
					CnpcLeft(S, N);
				else
					CnpcRight(S, N);
			}
		}
		ShowCNPC(S, N);
	}
	else
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
		DeleteC(Cnpc[S][N].coordinate);
		Set_Cursor(Cnpc[S][N].coordinate.X, Cnpc[S][N].coordinate.Y);
		ShowCNPC(S, N);
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
	}
}

int MoveForNPC(int i, int j, int range) { //NPC움직임의 범위지정
	int a;
	if (move[i][j] % (range * 2) < range)
	{
		for (a = 0; a < 4; a++)
		{
			if (npc[i][a].state == 0)
				move[i][a]++;
		}

		return 1;
	}
	else
	{
		for (a = 0; a < 4; a++)
		{
			if (npc[i][a].state == 0)
				move[i][a]++;
		}
		return 0;
	}
}

void stageNPC(int S) { // 첫번째 스테이지.
	if (S == 0)
	{
		RMoveNPC(4, 30, S, 0); // 랜덤엔피씨

		if (MoveForNPC(S, 0, 46)) // 일반엔피씨.
			MoveNPC(0, S, 0);
		else
			MoveNPC(1, S, 0);

		if (MoveForNPC(S, 1, 20))
			MoveNPC(0, S, 1);
		else
			MoveNPC(1, S, 1);

		if (MoveForNPC(S, 2, 60))
			MoveNPC(0, S, 2);
		else
			MoveNPC(1, S, 2);
	}
	else if (S == 1)
	{
		RMoveNPC(4, 34, S, 0);
		up_MoveNPC(S, 1, 10);
		if (npc_time % 2)
			CCMoveNPC(S, 0, 20, 59, 0, 24);
		CsameX(S, 1);
		if (MoveForNPC(S, 0, 20)) // 일반엔피씨.
			MoveNPC(0, S, 0);
		else
			MoveNPC(1, S, 0);
	}
	else if (S == 2)
	{
		if (MoveForNPC(S, 0, 40)) // 일반엔피씨.
			MoveNPC(0, S, 0);
		else
			MoveNPC(1, S, 0);

		if (MoveForNPC(S, 1, 40)) // 일반엔피씨.
			MoveNPC(0, S, 1);
		else
			MoveNPC(1, S, 1);

		if (MoveForNPC(S, 3, 30)) // 일반엔피씨.
			MoveNPC(0, S, 3);
		else
			MoveNPC(1, S, 3);

		up_MoveNPC(S, 2, 10);

		RMoveNPC(24, 70, S, 0);
		RMoveNPC(100, 108, S, 1);
		RMoveNPC(40, 80, S, 2);

		if (npc_time % 3)
			CCMoveNPC(S, 0, 0, 120, 0, 50);
		CsameX(S, 1);
	}

}

void ShowDoor(int S) {
	Set_Cursor(Door[S].X, Door[S].Y);
	printf("∩");
}

void ShowKey(int S, int N) {
	Set_Cursor(key[S][N].coordinate.X, key[S][N].coordinate.Y);
	printf("♀");
}

void ShowCoin(int S, int N) {
	Set_Cursor(coin[S][N].coordinate.X, coin[S][N].coordinate.Y);
	printf("ⓒ");
}

void getCoin(int S, int N)
{
	int i;

	if (PC.X == coin[S][N].coordinate.X&& PC.Y == coin[S][N].coordinate.Y&&coin[S][N].state != 0)
	{
		coin[S][N].state = 0;
		coinCount[S] --;
		score += 100;
	}

}

int getKey(int S) {
	int i;
	for (i = 0; i < S + 1; i++)
	{
		if (PC.X == key[S][i].coordinate.X && PC.Y == key[S][i].coordinate.Y&&key[S][i].state != 0)
		{
			key[S][i].state = 0;
			keyCount[S]--;
			Set_Cursor(PC.X, PC.Y);
			DeleteC(key[S][i].coordinate);
			return 1;
		}

	}
	if (keyCount[S] == 0 && PC.X == Door[S].X && PC.Y == Door[S].Y)
	{
		Set_Cursor(PC.X, PC.Y);
		for (i = 0; i < 3; i++)
		{
			DeleteC(npc[S][i].coordinate);
		}
		return 2;
	}
	else
		return 0;

}

void ItemCoordinate()
{
	if (stage_num == 0)
	{
		key[0][0].coordinate.X = 74;
		key[0][0].coordinate.Y = 9;
		key[0][0].state = 1;
		ShowKey(0, 0);
		coin[0][0].coordinate.X = 26;
		coin[0][0].coordinate.Y = 5;
		coin[0][0].state = 1;
		coin[0][1].coordinate.X = 90;
		coin[0][1].coordinate.Y = 17;
		coin[0][1].state = 1;
		coin[0][2].coordinate.X = 84;
		coin[0][2].coordinate.Y = 30;
		coin[0][2].state = 1;
		ShowCoin(0, 0);
		ShowCoin(0, 1);
		ShowCoin(0, 2);
	}
	////////////////////////////////////////
	else if (stage_num == 1)
	{
		key[1][0].coordinate.X = 74;
		key[1][0].coordinate.Y = 9;
		key[1][0].state = 1;
		ShowKey(1, 0);
		key[1][1].coordinate.X = 2;
		key[1][1].coordinate.Y = 5;
		key[1][1].state = 1;
		ShowKey(1, 1);
		coin[1][0].coordinate.X = 50;
		coin[1][0].coordinate.Y = 13;
		coin[1][0].state = 1;
		coin[1][1].coordinate.X = 100;
		coin[1][1].coordinate.Y = 17;
		coin[1][1].state = 1;
		coin[1][2].coordinate.X = 84;
		coin[1][2].coordinate.Y = 30;
		coin[1][2].state = 1;

		ShowCoin(1, 0);
		ShowCoin(1, 1);
		ShowCoin(1, 2);

	}
	else if (stage_num == 2)
	{
		key[2][0].coordinate.X = 88;
		key[2][0].coordinate.Y = 3;
		key[2][0].state = 1;
		ShowKey(2, 0);
		key[2][1].coordinate.X = 52;
		key[2][1].coordinate.Y = 27;
		key[2][1].state = 1;
		ShowKey(2, 1);
		key[2][2].coordinate.X = 78;
		key[2][2].coordinate.Y = 5;
		key[2][2].state = 1;
		ShowKey(2, 2);
		coin[2][0].coordinate.X = 76;
		coin[2][0].coordinate.Y = 5;
		coin[2][0].state = 1;
		coin[2][1].coordinate.X = 86;
		coin[2][1].coordinate.Y = 3;
		coin[2][1].state = 1;
		coin[2][2].coordinate.X = 56;
		coin[2][2].coordinate.Y = 27;
		coin[2][2].state = 1;

		ShowCoin(2, 0);
		ShowCoin(2, 1);
		ShowCoin(2, 2);
	}
}

void stageItem(int S) {

	int ok = getKey(S);
	for (int k = 0; k < 3; k++)
	{
		getCoin(S, k);
		if (coin[S][k].state != 0)
			ShowCoin(S, k);
	}

	if (ok > 0) // 첫번째 키에 대한 검사.
	{
		ShowPC(PC);

		if (keyCount[S] == 0)
		{
			ShowDoor(S);
		}

		if (ok == 2) //다음스테이지로 넘어가기위한 것!
		{
			score += (timer_a * 10) + (timer_b * 10);
			clear++;//최종 클리어를 하기위한 것
			if (clear == 3)
				return;
			stage_num++;
			system("cls"); //cmd 날려버리는 함수.
			Set_Cursor(50, 20);
			printf("your score : %d ", score);
			getchar();
			system("cls");
			Set_Cursor(0, 0);
			ShowBlock(mapModel[stage_num]);
			cool = 0;
			timer = timeAmt;
			PC.X = 2;
			PC.Y = 36;
			ShowPC(PC);
			ItemCoordinate();
		}

	}
}

int NPCcrush(int S)
{
	int i, j;

	for (j = 0; j < 4; j++)
	{
		if ((PC.X == npc[S][j].coordinate.X) && (PC.Y == npc[S][j].coordinate.Y) && (npc[S][j].state == 0))
			return 1;
		if ((PC.X == Rnpc[S][j].coordinate.X) && (PC.Y == Rnpc[S][j].coordinate.Y) && (Rnpc[S][j].state == 0))
			return 1;
		if ((PC.X == Cnpc[S][j].coordinate.X) && (PC.Y == Cnpc[S][j].coordinate.Y) && (Cnpc[S][j].state == 0))
			return 1;
	}

	return 0;
}

int Drop()
{
	int w = 0;
	if (!DetectCollision3(PC.X, PC.Y + 1))
	{
		while (DetectCollision1(PC.X, PC.Y + 1))
		{
			DeleteC(PC);
			//if (w>0&&w<5)
			//printf("↓");
			PC.Y += 1;
			Set_Cursor(PC.X, PC.Y);
			ShowPC(PC);
			Sleep(100);
			w++;
		}
		return 1;
	}
	return 0;
}

int ShiftRight()
{
	if (!DetectCollision1(PC.X + 2, PC.Y))
	{
		return 0;
	}

	if (!DetectCollision2(PC.X, PC.Y + 1) && !DetectCollision2(PC.X, PC.Y))
	{
		if (!DetectCollision0(PC.X + 2, PC.Y) && !DetectCollision1(PC.X + 2, PC.Y + 1))
		{
			DeleteC(PC);
			Set_Cursor(PC.X, PC.Y);
			printf("▥");
			PC.X += 2;
			Set_Cursor(PC.X, PC.Y);
			ShowPC(PC);
			return 1;
		}
		return 0;
	}

	if (!DetectCollision2(PC.X, PC.Y))
	{
		DeleteC(PC);
		Set_Cursor(PC.X, PC.Y);
		printf("▥");
		PC.X += 2;
		Set_Cursor(PC.X, PC.Y);
		ShowPC(PC);
		return 1;
	}

	DeleteC(PC);
	PC.X += 2;
	Set_Cursor(PC.X, PC.Y);
	ShowPC(PC);
	return 1;
}

int ShiftDown()
{

	if ((!DetectCollision2(PC.X, PC.Y + 1)) && (!DetectCollision2(PC.X, PC.Y)))
	{
		DeleteC(PC);
		Set_Cursor(PC.X, PC.Y);
		printf("▥");
		PC.Y += 1;
		Set_Cursor(PC.X, PC.Y);
		ShowPC(PC);
		return 1;
	}

	else if (!DetectCollision2(PC.X, PC.Y + 1))
	{
		DeleteC(PC);
		PC.Y += 1;
		Set_Cursor(0, 0);
		ShowBlock(mapModel[stage_num]);
		Set_Cursor(PC.X, PC.Y);
		ShowPC(PC);
		return 1;
	}

	return 0;
}

int ShiftLeft()
{
	if (!DetectCollision1(PC.X - 2, PC.Y))
	{
		return 0;
	}

	if (!DetectCollision2(PC.X, PC.Y + 1) && !DetectCollision2(PC.X, PC.Y))
	{
		if (!DetectCollision0(PC.X - 2, PC.Y) && !DetectCollision1(PC.X - 2, PC.Y + 1))
		{
			DeleteC(PC);
			Set_Cursor(PC.X, PC.Y);
			printf("▥");
			PC.X -= 2;
			Set_Cursor(PC.X, PC.Y);
			ShowPC(PC);
			return 1;
		}
		return 0;
	}

	if (!DetectCollision2(PC.X, PC.Y))
	{
		DeleteC(PC);
		Set_Cursor(PC.X, PC.Y);
		printf("▥");
		PC.X -= 2;
		Set_Cursor(PC.X, PC.Y);
		ShowPC(PC);
		return 1;
	}
	DeleteC(PC);
	PC.X -= 2;
	Set_Cursor(PC.X, PC.Y);
	ShowPC(PC);
	return 1;
}

int ShiftUP()
{

	if (!DetectCollision2(PC.X, PC.Y))
	{
		DeleteC(PC);
		Set_Cursor(PC.X, PC.Y);
		printf("▥");
		PC.Y -= 1;
		Set_Cursor(PC.X, PC.Y);
		ShowPC(PC);
		return 1;
	}

	return 0;
}

int Shotcrush1(COORD A)
{
	int i, j;
	for (i = 0; i < 3; i++)
	{
		for (j = 0; j < 4; j++)
		{
			if ((A.X == npc[i][j].coordinate.X) && (A.Y == npc[i][j].coordinate.Y) && (npc[i][j].state == 0))
			{
				npc[i][j].state = 1;
				shw = 1;
				return 1;
			}
			if ((A.X == Rnpc[i][j].coordinate.X) && (A.Y == Rnpc[i][j].coordinate.Y) && (Rnpc[i][j].state == 0))
			{
				Rnpc[i][j].state = 1;
				shw = 1;
				return 1;
			}
		}
	}
	return 0;
}

int Shotcrush2(COORD A)
{
	int i, j;
	for (i = 0; i < 3; i++)
	{
		for (j = 0; j < 4; j++)
		{
			if ((A.X == Cnpc[i][j].coordinate.X) && (A.Y == Cnpc[i][j].coordinate.Y) && (Cnpc[i][j].state == 0))
			{
				Cnpc[i][j].state = 1;
				shw = 1;
				return 1;
			}
		}
	}
	return 0;
}

void shotZ()
{

	COORD A = PC;
	int i = 0;

	if (cool != 0)
		return;

	//사다리 위에서 발사 x
	if (DetectCollision1(PC.X + 2, PC.Y + 1) && !DetectCollision2(PC.X, PC.Y))
		return;


	if (DetectCollision1(PC.X - 2, PC.Y + 1) && !DetectCollision2(PC.X, PC.Y))
		return;


	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
	cool = 30;

	if (way == 0)
	{

		while (i < 4)
		{
			if (!DetectCollision1(A.X + 2, A.Y))
				break;
			A.X += 2;
			Set_Cursor(A.X, A.Y);
			ShowA(A);
			if (i < 3)
				Sleep(10);
			else
				Sleep(speed);
			DeleteC(A);
			Shotcrush1(A);
			i++;
			if (!DetectCollision2(A.X, A.Y))
			{
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
				Set_Cursor(A.X, A.Y);
				printf("▥");
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
			}
		}
	}
	else if (way == 1)
	{
		while (i < 4)
		{
			if (!DetectCollision1(A.X - 2, A.Y))
				break;

			A.X -= 2;
			Set_Cursor(A.X, A.Y);
			ShowA(A);
			if (i < 3)
				Sleep(10);
			else
				Sleep(speed);
			DeleteC(A);
			Shotcrush1(A);
			i++;

			if (!DetectCollision2(A.X, A.Y))
			{
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
				Set_Cursor(A.X, A.Y);
				printf("▥");
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
			}

		}
	}
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
}

void shotX()
{

	COORD A = PC;
	int i = 0;

	if (cool != 0)
		return;

	//사다리 위에서 발사 x
	if (DetectCollision1(PC.X + 2, PC.Y + 1) && !DetectCollision2(PC.X, PC.Y))
		return;


	if (DetectCollision1(PC.X - 2, PC.Y + 1) && !DetectCollision2(PC.X, PC.Y))
		return;


	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 13);
	cool = 30;
	if (way == 0)
	{
		while (i < 4)
		{
			if (!DetectCollision1(A.X + 2, A.Y))
				break;
			A.X += 2;
			Set_Cursor(A.X, A.Y);
			ShowA(A);
			if (i < 3)
				Sleep(10);
			else
				Sleep(speed);

			DeleteC(A);
			Shotcrush2(A);
			i++;
			if (!DetectCollision2(A.X, A.Y))
			{
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
				Set_Cursor(A.X, A.Y);
				printf("▥");
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 13);
			}
		}
	}
	else if (way == 1)
	{

		while (i < 4)
		{
			if (!DetectCollision1(A.X - 2, A.Y))
				break;
			A.X -= 2;
			Set_Cursor(A.X, A.Y);
			ShowA(A);
			if (i < 3)
				Sleep(10);
			else
				Sleep(speed);

			DeleteC(A);
			Shotcrush2(A);
			i++;
			if (!DetectCollision2(A.X, A.Y))
			{
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
				Set_Cursor(A.X, A.Y);
				printf("▥");
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 13);
			}
		}
	}
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
}

void RemoveCursor(void)
{
	CONSOLE_CURSOR_INFO curInfo;
	GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
	curInfo.bVisible = 0;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
}

void ProcessKeyInput()
{
	int key, i;
	for (i = 0; i < 5; i++)
	{
		if (_kbhit() != 0)
		{
			key = _getch();
			switch (key)
			{
			case LEFT: { ShiftLeft(); way = 1; } break;
			case RIGHT: { ShiftRight(); way = 0; } break;
			case UP: ShiftUP(); break;
			case DOWN: ShiftDown(); break;
			case Zattack:shotZ(); break;
			case Xattack:shotX(); break;
			}
		}
		npc_time++;
		Sleep(speed / 5);
	}

}

void main()
{
	RemoveCursor();
	system("mode con cols=120 lines=45");
	system("title PRISON");
	Set_Cursor(0, 0);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
	ShowMonitor(Monitor[0]);
	getchar();
	timer = timeAmt;


	DeleteBlock();
	Set_Cursor(0, 0);
	ShowBlock(mapModel[stage_num]);
	NpcCoordinate(); // NPC의 정보를 불러오는 함수.
	ItemCoordinate(); // Item의 정보를 불러오는 함수.
	ShowPC(PC);
	while (clear<3)
	{
		Set_Cursor(30, 41);
		printf("COOL : ");
		coolc = Get_CursorPos();
		ShowCoolTime();
		Set_Cursor(55, 41);
		printf("SCORE : %d", score);
		Set_Cursor(70, 41);
		if (stage_num == 0)
			printf("Coin : %4.2d", coinCount[0]);
		else if (stage_num == 1)
			printf("Coin : %4.2d", coinCount[1]);
		else if (stage_num == 2)
			printf("Coin : %4.2d", coinCount[2]);
		if (NPCcrush(stage_num))
		{
			break;
		}
		if (Drop())
		{
			break;
		}
		if (TimeCount())
		{
			break;
		}
		ProcessKeyInput();
		stageNPC(stage_num); //스테이지.관련 엔피씨의 움직임.
		stageItem(stage_num); // 스테이지 관련 아아템.
	}

	if (clear == 3)
	{
		system("cls");
		Set_Cursor(0, 0);
		ShowMonitor(Monitor[2]);
		Set_Cursor(45, 41);
		printf("your score : %d ", score);
		getchar();
	}
	else
	{
		system("cls");
		Set_Cursor(0, 0);
		ShowMonitor(Monitor[1]);
		Set_Cursor(45, 41);
		printf("your score : %d ", score);
		getchar();
	}
}