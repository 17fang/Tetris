// TETRIS.cpp: 定义控制台应用程序的入口点。
//
#include "stdafx.h"
#include <windows.h>
#include <iostream>
#include <time.h>

#define area 30
#define ROW 20
#define COL 15
HWND hwnd;
int score = 0; //分数
int level = 1; //难度
int newleft, newtop;  //方块当前位置
int width, height; //方块的宽和高
bool isPause = 0;    //暂停
UINT timeID = 0;    //计时器

static byte *block = NULL;
byte position[ROW][COL] = { 0 };

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

//void table(HDC hdc); //地图
void refresh(HDC hdc);
void Collision(HDC hdc);
void Left(HDC hdc);
void Right(HDC hdc);
void Down(HDC hdc);
void Turn(HDC hdc);
void ClearLine(HDC hdc);
bool product();
bool bottom(HDC hdc);

int main() {
	std::cout << "WELCOME TO MY TETRIS!" << std::endl;
	std::cout << "Please choose the level:(1~4) ";
	std::cin >> level;

	HINSTANCE hInstance = GetModuleHandle(NULL);

	TCHAR GameName[] = TEXT("tetris");
	WNDCLASS tc; //包含一个窗口的全部信息
	tc.style = CS_HREDRAW | CS_VREDRAW;
	tc.lpfnWndProc = WndProc;
	tc.cbClsExtra = 0;
	tc.cbWndExtra = 0;
	tc.hInstance = hInstance;
	tc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	tc.hCursor = LoadCursor(NULL, IDC_ARROW);
	tc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	tc.lpszMenuName = NULL;
	tc.lpszClassName = GameName;

	if (!RegisterClass(&tc))
	{
		printf("error");
		return 0;
	}
	hwnd = CreateWindow(
		GameName,
		TEXT("My Tetris"),
		WS_OVERLAPPEDWINDOW,
		0, 0, 0, 0,
		NULL,
		NULL,
		hInstance,
		NULL);
	ShowWindow(hwnd, SW_SHOW); //指定窗口显示状态
	UpdateWindow(hwnd); //更新窗口客户区
	MSG message; //message结构体
	while (GetMessage(&message, NULL, 0, 0))
	{
		TranslateMessage(&message);
		DispatchMessage(&message);
	} //消息检测循环
	return message.wParam;
}

void Collision(HDC hdc) {
	if (NULL == block) return;
	if (bottom(hdc)) {
		ClearLine(hdc);
		product();
	}
	newtop++;
	refresh(hdc);
};

void Down(HDC hdc) {
	if (NULL == block) return;
	if (bottom(hdc)) {
		ClearLine(hdc);
		product();
	}
	newtop++;
	refresh(hdc);
};

void Left(HDC hdc) {
	int x, y;
	if (NULL == block) return;
	if (newleft == 0) return;
	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			if (*(block + y * width + x)) {
				if (position[newtop + y][newleft + x - 1]) return;
				break;
			}
		}
	}
	newleft--;
	refresh(hdc);
};

void Right(HDC hdc) {
	int x, y;
	if (NULL == block) return;
	if (newleft + width == COL) return;
	for (y = 0; y < height; y++) {
		for (x = width - 1; x >= 0; x--)
		{
			if (*(block + y * width + x))
			{
				if (position[newtop + y][newleft + x + 1]) return;
				break;
			}
		}
	}
	newleft++;
	refresh(hdc);
};

void Turn(HDC hdc) {
	int x, y;
	byte * temp = NULL;
	if (NULL == block) return;
	temp = (byte *)malloc(sizeof(byte) * width * height);
	for (x = 0; x < width; x++) {
		for (y = 0; y < height; y++) {
			*(temp + x * height + y) = *(block + (height - y - 1) * width + x);
		}
	}
	int turn_newtop = newtop - (width - height) / 2;
	int turn_newleft = newleft - (height - width) / 2;

	int max_len = max(width, height);
	if (turn_newtop + max_len - 1 >= ROW || turn_newleft<0 || turn_newleft + max_len - 1 >= COL) {
		free(temp);  //释放内存
		return;
	}
	for (x = 0; x < max_len; x++) {
		for (y = 0; y < max_len; y++) {
			if (position[turn_newtop + x][turn_newleft + y]) {
				free(temp);  //释放内存
				return;
			}
		}
	}
	for (x = 0; x < height; x++) {
		for (y = 0; y < width; y++) {
			*(block + x * width + y) = *(temp + x * width + y);
		}
	}
	newtop = turn_newtop;
	newleft = turn_newleft;
	//交换
	x = width;
	width = height;
	height = x;

	free(temp);  //释放内存
	refresh(hdc);
};

void ClearLine(HDC hdc) {
	int x, y, z;
	int line = 0;
	bool FULL;
	for (x = ROW - 1; x >= 0; x--) {
		FULL = true;
		for (y = 0; y < COL; y++) {
			if (!position[x][y]) {
				FULL = false;
				break;
			}
		}
		if (FULL) {
			for (y = 0; y < COL; y++) {
				position[x][y] = 0;
			}
			//方块下移
			for (z = x - 1; z >= 0; z--) {
				for (y = 0; y < COL; y++) {
					position[z + 1][y] = position[z][y];
				}
			}
			x++;
			line++;
		}
	}
	if (score == 10000) {
		return;
	}
	//消除一行加100分
	for (int i = line; i > 0; i--) {
		score += 100;
	}
	system("cls"); //清屏
	std::cout << "socre: " << score << ", " << "level: " << level << std::endl;
};


bool product() {
	int category;
	if (block) {
		free(block);
		block = NULL;
	}
	category = rand() % 8;

	switch (category) {
	case 0:  //三角
		width = 3;
		height = 2;
		block = (byte *)malloc(sizeof(byte) * width * height);
		*(block + 0) = 0; *(block + 1) = 1;
		*(block + 2) = 0; *(block + 3) = 1;
		*(block + 4) = 1; *(block + 5) = 1;
		newtop = 0 - height;
		newleft = (COL - width) / 2;
		break;
	case 1:  //水平条
		width = 4;
		height = 1;
		block = (byte *)malloc(sizeof(byte) * width * height);
		*(block + 0) = 1;
		*(block + 1) = 1;
		*(block + 2) = 1;
		*(block + 3) = 1;
		newtop = 0 - height;
		newleft = (COL - width) / 2;
		break;
	case 2:  //同形
		width = 3;
		height = 2;
		block = (byte *)malloc(sizeof(byte) * width * height);
		*(block + 0) = 1;
		*(block + 1) = 1;
		*(block + 2) = 1;
		*(block + 3) = 1;
		*(block + 4) = 0;
		*(block + 5) = 1;
		newtop = 0 - height;
		newleft = (COL - width) / 2;
		break;
	case 3:  //横折
		width = 3;
		height = 2;
		block = (byte *)malloc(sizeof(byte) * width * height);
		*(block + 0) = 1;
		*(block + 1) = 0;
		*(block + 2) = 0;
		*(block + 3) = 1;
		*(block + 4) = 1;
		*(block + 5) = 1;
		newtop = 0 - height;
		newleft = (COL - width) / 2;
		break;
	case 4:  //闪电
		width = 3;
		height = 2;
		block = (byte *)malloc(sizeof(byte) * width * height);
		*(block + 0) = 1;
		*(block + 1) = 1;
		*(block + 2) = 0;
		*(block + 3) = 0;
		*(block + 4) = 1;
		*(block + 5) = 1;
		newtop = 0 - height;
		newleft = (COL - width) / 2;
		break;
	case 5:  //三饼
		width = 3;
		height = 3;
		block = (byte *)malloc(sizeof(byte) * width * height);
		*(block + 0) = 0;
		*(block + 1) = 0;
		*(block + 2) = 1;
		*(block + 3) = 0;
		*(block + 4) = 1;
		*(block + 5) = 0;
		*(block + 6) = 1;
		*(block + 7) = 0;
		*(block + 8) = 0;
		newtop = 0 - height;
		newleft = (COL - width) / 2;
		break;
	case 6:  //折块
		width = 2;
		height = 2;
		block = (byte *)malloc(sizeof(byte) * width * height);
		*(block + 0) = 1;
		*(block + 1) = 1;
		*(block + 2) = 0;
		*(block + 3) = 1;
		newtop = 0 - height;
		newleft = (COL - width) / 2;
	case 7:  //石头
		width = 2;
		height = 2;
		block = (byte *)malloc(sizeof(byte) * width * height);
		*(block + 0) = 1;
		*(block + 1) = 1;
		*(block + 2) = 1;
		*(block + 3) = 1;
		newtop = 0 - height;
		newleft = (COL - width) / 2;
		break;
	}
	return 0;
	//return block != NULL;
};

bool bottom(HDC hdc) {
	int x, y;
	int i, j;
	if (NULL == block) return 0;
	if (ROW == newtop + height) {
		//固定方块
		for (i = 0; i < height; i++) {
			for (j = 0; j < width; j++) {
				if (*(block + i * width + j)) {
					position[newtop + i][newleft + j] = 1;
				}
			}
		}
		return true;
	}
	for (y = height - 1; y >= 0; y--) {
		for (x = 0; x < width; x++) {
			if (*(block + y * width + x)) {
				if (newtop + y + 1 < 0) {
					return false;
				}
				if (position[newtop + y + 1][newleft + x]) {
					//判断游戏结束
					if (newtop <= 0) {
						if (time) {
							KillTimer(hwnd, 1);
							timeID = 0;
						}
						MessageBox(hwnd, TEXT("YOU FAILED!"), TEXT("gameover"), MB_OK);
						SendMessage(hwnd, WM_CLOSE, 0, 0);
					}
					for (i = 0; i < height; i++) {
						for (j = 0; j < width; j++) {
							if (*(block + i * width + j)) {
								position[newtop + i][newleft + j] = 1;
							}
						}
					}
					return true;
				}
			}
		}
	}
	return 0;
};

void refresh(HDC hdc) {
	int x, y;
	RECT rect;
	HBRUSH BLOCK = (HBRUSH)GetStockObject(BLACK_BRUSH),
		BLANK = (HBRUSH)GetStockObject(WHITE_BRUSH);
	if (NULL == block) return;
	for (y = 0; y < ROW; y++) {
		for (x = 0; x < COL; x++) {
			rect.top = y * area + 2;
			rect.bottom = (y + 1) * area - 2;
			rect.left = x * area + 2;
			rect.right = (x + 1) * area - 2;
			if (position[y][x]) {
				FillRect(hdc, &rect, BLOCK);
			}
			else {
				FillRect(hdc, &rect, BLANK);
			}
		}
	}
	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			if (*(block + y * width + x)) {
				rect.top = (y + newtop) * area + 2;
				rect.bottom = (y + newtop + 1) * area - 2;
				rect.left = (x + newleft) * area + 2;
				rect.right = (x + newleft + 1) * area - 2;
				FillRect(hdc, &rect, BLOCK);
			}
		}
	}
};

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	switch (message)
	{
	case WM_CREATE:
		MoveWindow(hwnd, 400, 10, area * COL + 15, area * ROW + 40, FALSE);
		srand(time(NULL));
		product();
		timeID = SetTimer(hwnd, 1, 300 - level * 40, NULL);
		return 0;
	case WM_TIMER:
		hdc = GetDC(hwnd);
		Down(hdc);
		ReleaseDC(hwnd, hdc);
		return 0;
	case WM_KEYDOWN:
		hdc = GetDC(hwnd);
		switch (wParam)
		{
		case VK_LEFT:       //左移
			if (!isPause) Left(hdc);
			break;
		case VK_RIGHT:       //右移
			if (!isPause) Right(hdc);
			break;
		case VK_UP:        //转向
			if (!isPause) Turn(hdc);
			break;
		case VK_DOWN:       //加速
			if (!isPause) Down(hdc);
			break;
		case VK_SPACE:  //暂停
			isPause = !isPause;
			if (isPause)
			{
				if (timeID) KillTimer(hwnd, 1);
				timeID = 0;
			}
			else
			{
				timeID = SetTimer(hwnd, 1, 300 - level * 40, FALSE);
			}
			break;
		}
		ReleaseDC(hwnd, hdc);
		return 0;
	case WM_DESTROY:
		if (block)
			free(block);
		if (timeID)
			KillTimer(hwnd, 1);
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}
