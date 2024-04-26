#include<conio.h>
#include<stdio.h>
#include<graphics.h>
#include<mmsystem.h>
#pragma comment(lib,"winmm.lib")
#include<time.h>
#include <stdlib.h>
#include <windows.h>
#include<stdio.h>
#include <graphics.h>
#include <conio.h>
#include<vector>

#define MAPW 1080
#define MAPH 720
#define win_width 1012
#define win_height 396
#define OBSTACLE_COUNT 100
#define WIN_SCORE 50

IMAGE welcome;
IMAGE choose;
IMAGE bk1, road;
MOUSEMSG msg;
IMAGE characterImg[12];
IMAGE dise[12];
IMAGE bgdise[4];
IMAGE Down;
IMAGE obstacle[4];
IMAGE obstacledise[4];
IMAGE wall[4];
IMAGE walldise[4];
IMAGE characterDownImg;
IMAGE Downdise[2];
IMAGE characterjump;
IMAGE Jumpdise;
IMAGE backgroundImg[3];//背景图片
IMAGE gameover;	//游戏结束图片
using namespace std;//声明命名空间

int characterX;//玩家的X坐标
int characterY;//玩家的Y坐标
int ObstacleX, ObstacleY;		//障碍物的X，Y坐标
int hit = 0;
int backgroundX[3];//背景图片的x坐标
int bgSpeed[3] = { 1,2,8 };  
int characterIndex;//玩家奔跑时的图片帧序号
int maxJumpHeight;//跳跃的最高高度
int characterJumpOff=13;	//跳跃每帧上升的高度
int heroBlood = 99;
int score = 0;		//分数

bool update;//是否是要马上刷新
bool characterJump;//表示玩家正在跳跃
bool characterDown;
bool intersect;


typedef struct
{
	int hour;
	int minute;
	int second;
}gametime;

void DisplayTime(int t)
{
	gametime MYTIME;
	t /= 1000;
	MYTIME.hour = t / 3600;
	t %= 3600;
	MYTIME.minute = t / 60;
	MYTIME.second = t % 60;
	TCHAR buf[100];
	_sntprintf_s(buf, sizeof(buf),_T( "已跑：%02d:%02d:%02d"), MYTIME.hour, MYTIME.minute, MYTIME.second);
	outtextxy(30, 20, buf);
}



void DrawWelcome()
{
	loadimage(&welcome, _T("img\\welcome.jpg"));
	putimage(0, 0, &welcome);
	setbkmode(TRANSPARENT);
	mciSendString(_T("welcome.mp3 "), NULL, 0, NULL);
	mciSendString(_T("point.mp3 "), NULL, 0, NULL);
	mciSendString(_T("play welcome.mp3 repeat"), NULL, 0, NULL); //播放背景音乐
	rectangle(10, 680, 80, 700);
	outtextxy(20, 680, _T("帮助"));

	while (1)
	{
		if (MouseHit())
		{
			msg = GetMouseMsg();
			if (msg.uMsg == WM_LBUTTONDOWN)
			{
				mciSendString(_T("play point.mp3 "), NULL, 0, NULL);
				RECT r1 = { 300,570,600,645 };
				RECT r2 = { 680,570,985,645 };
				RECT r3 = { 10,680,80,700 };
				if (msg.x < r2.right && msg.x > r2.left && msg.y < r2.bottom && msg.y > r2.top)
				{
					break;
				}
				else if (msg.x < r1.right && msg.x > r1.left && msg.y < r1.bottom && msg.y > r1.top)
				{
					MessageBoxA(GetHWnd(), _T("很遗憾，你的微信好友都充钱而提前润了，你只能和QQ好友玩"), _T("太遗憾了"), 0);
					continue;

				}
				else if (msg.x < r3.right && msg.x > r3.left && msg.y < r3.bottom && msg.y > r3.top)
				{
					MessageBoxA(GetHWnd(), _T("W跳跃，S下蹲，躲避障碍物，到达终点。你最多只能3次碰到障碍物"), _T("帮助"), 0);
					continue;
				}
			}
		}
	}

}
//初始界面选项

void DrawChoose()
{
	loadimage(&choose, _T("img\\choose.png"));
	putimage(0, 0, &choose);
	while (1)
	{
		if (MouseHit())
		{
			msg = GetMouseMsg();
			if (msg.uMsg == WM_LBUTTONDOWN)
			{
				mciSendString(_T("play point.mp3 "), NULL, 0, NULL);
				RECT r1 = { 380,594,640,698 };
				RECT r2 = { 712,594,973,698 };
				RECT r3 = { 10,680,80,700 };
				if (msg.x < r2.right && msg.x > r2.left && msg.y < r2.bottom && msg.y > r2.top)
				{
					break;
				}
				else if (msg.x < r1.right && msg.x > r1.left && msg.y < r1.bottom && msg.y > r1.top)
				{
					MessageBoxA(GetHWnd(), _T("不充钱还想选我？"), _T("哼，用心创造快乐，没钱____"), 0);
					outtextxy(20, 680, _T("充值"));
					continue;

				}
				else if (msg.x < r3.right && msg.x > r3.left && msg.y < r3.bottom && msg.y > r3.top)
				{
					MessageBoxA(GetHWnd(), _T("你害真想充钱啊,这是单机游戏啊喂"), _T("吃惊"), 0);
					continue;
				}
				
			}
		}
	}
}

//人物选择
void DrawBackground()
{
	mciSendString(_T("stop welcome.mp3 "), NULL, 0, NULL);
	mciSendString(_T("play.mp3 "), NULL, 0, NULL);
	mciSendString(_T("play play.mp3 repeat"), NULL, 0, NULL);
	loadimage(&bk1, _T("bk1.jpg"));
	loadimage(&gameover, _T("img\\gameover.png"));
	putimage(0, 0, &bk1);
	setbkmode(TRANSPARENT);
	putimage(0, 560, &road);

}
//背景绘制

void init()
{

	//创建游戏窗口
	initgraph(win_width, win_height);
	char name[64];
	int i = 0, j = 0;
	loadimage(&backgroundImg[0], _T("img\\background\\bg001.png"));
	loadimage(&backgroundImg[1], _T("img\\background\\bg002.png"));
	loadimage(&backgroundImg[2], _T("img\\background\\bg003.png"));
	loadimage(&bgdise[2], _T("img\\dise\\bgdise002.png"));
	loadimage(&bgdise[3], _T("img\\dise\\bgdise003.png"));
	backgroundX[0] = 0; backgroundX[1] = 0; backgroundX[2] = 0;
	//加载游戏角色的图片帧素材
	loadimage(&characterImg[i++], _T("img\\character\\c1.png"));
	loadimage(&characterImg[i++], _T("img\\character\\c2.png"));
	loadimage(&characterImg[i++], _T("img\\character\\c3.png"));
	loadimage(&characterImg[i++], _T("img\\character\\c4.png"));
	loadimage(&characterImg[i++], _T("img\\character\\c5.png"));
	loadimage(&characterImg[i++], _T("img\\character\\c6.png"));
	loadimage(&characterImg[i++], _T("img\\character\\c7.png"));
	loadimage(&characterImg[i++], _T("img\\character\\c8.png"));
	loadimage(&characterImg[i++], _T("img\\character\\c9.png"));
	loadimage(&characterImg[i++], _T("img\\character\\c10.png"));
	loadimage(&characterImg[i++], _T("img\\character\\c11.png"));
	loadimage(&characterImg[i++], _T("img\\character\\c12.png"));
	loadimage(&characterDownImg, _T("img\\character\\down1.png"));
	loadimage(&characterjump, _T("img\\character\\jump1.png"));
	loadimage(&Jumpdise, _T("img\\character\\jumpdise1.png"));
	loadimage(&Downdise[0], _T("img\\character\\downdise1.png"));
	loadimage(&Downdise[1], _T("img\\character\\downdise2.png"));

	loadimage(&dise[j++], _T("img\\dise\\dise1.png"));
	loadimage(&dise[j++], _T("img\\dise\\dise2.png"));
	loadimage(&dise[j++], _T("img\\dise\\dise3.png"));
	loadimage(&dise[j++], _T("img\\dise\\dise4.png"));
	loadimage(&dise[j++], _T("img\\dise\\dise5.png"));
	loadimage(&dise[j++], _T("img\\dise\\dise6.png"));
	loadimage(&dise[j++], _T("img\\dise\\dise7.png"));
	loadimage(&dise[j++], _T("img\\dise\\dise8.png"));
	loadimage(&dise[j++], _T("img\\dise\\dise9.png"));
	loadimage(&dise[j++], _T("img\\dise\\dise10.png"));
	loadimage(&dise[j++], _T("img\\dise\\dise11.png"));
	loadimage(&dise[j++], _T("img\\dise\\dise12.png"));			
	
	//角色图片

	loadimage(&obstacle[0], _T("img\\obstacle\\h1.png"), 63, 260, true);
	loadimage(&obstacle[1], _T("img\\obstacle\\h2.png"), 63, 260, true);
	loadimage(&obstacle[2], _T("img\\obstacle\\h3.png"), 63, 260, true);
	loadimage(&obstacle[3], _T("img\\obstacle\\h4.png"), 63, 260, true);
	loadimage(&obstacledise[0], _T("img\\dise\\h1dise.png"), 63, 260, true);
	loadimage(&obstacledise[1], _T("img\\dise\\h2dise.png"), 63, 260, true);
	loadimage(&obstacledise[2], _T("img\\dise\\h3dise.png"), 63, 260, true);
	loadimage(&obstacledise[3], _T("img\\dise\\h4dise.png"), 63, 260, true);

	loadimage(&wall[0], _T("img\\obstacle\\w1.png") );
	loadimage(&wall[1], _T("img\\obstacle\\w2.png"));
	loadimage(&wall[2], _T("img\\obstacle\\w3.png") );
	loadimage(&wall[3], _T("img\\obstacle\\w4.png") );
	loadimage(&walldise[0], _T("img\\dise\\w1dise.png") );
	loadimage(&walldise[1], _T("img\\dise\\w2dise.png") );
	loadimage(&walldise[2], _T("img\\dise\\w3dise.png") );
	loadimage(&walldise[3], _T("img\\dise\\w4dise.png") );


	mciSendString(_T("jump.mp3 "), NULL, 0, NULL);
	mciSendString(_T("down.mp3 "), NULL, 0, NULL);
	mciSendString(_T("speedup.mp3 "), NULL, 0, NULL);
	mciSendString(_T("gameover.mp3 "), NULL, 0, NULL);
	mciSendString(_T("hurt1.mp3 "), NULL, 0, NULL);
	mciSendString(_T("hurt2.mp3 "), NULL, 0, NULL);
	mciSendString(_T("brave.mp3 "), NULL, 0, NULL);			//加载音效

	//障碍物图片

	ObstacleX = win_width;
	ObstacleY = 15;

	characterX = win_width * 0.2 - characterImg[0].getwidth() * 0.5;
	characterY = 345 - characterImg[0].getheight();
	characterIndex = 0;
	characterJump = false;
	maxJumpHeight = 380 - characterImg[0].getheight() - 120;
	update = true;
	
}

void jump()
{
	characterJump = true;
	update = true;
}
void down() {
	characterDown = true;
	update = true;
	characterIndex = 0;
}
int jumpflag = 0;
int delay[2] = { 8,30 };		//下蹲延迟
void fly()
{
	for (int i = 0; i < 3; i++)
	{
		backgroundX[i] -= bgSpeed[i];
		if (backgroundX[i] < -win_width) {
			backgroundX[i] = 0;
		}
	}
	//实现跳跃
	if (characterJump)
	{
		if (jumpflag == 0)
		{
			characterY -= characterJumpOff;
			if (characterY < 20) jumpflag = 1;
		}
		else if (jumpflag == 1)
		{
			characterY += characterJumpOff;
			if (characterY == 345 - characterImg[0].getheight())
			{
				characterJump = false;
				jumpflag = 0;
			}
			else if (characterY >345 - characterImg[0].getheight())
			{
				characterY = 345 - characterImg[0].getheight();
				characterJump = false;
				jumpflag = 0;
			}
		}
	}
	else if (characterDown)
	{
		static int count = 0;
		count++;
		if (count >= delay[characterIndex]) 
		{
			count = 0;
			characterIndex++;
			if (characterIndex >= 2)
			{
				characterIndex = 0;
				characterDown = false;
			}
		}
	}
	else
	{  //不跳跃
		characterIndex = (characterIndex + 1) % 12;
	}
}


void updateBackground()
{
	setbkmode(TRANSPARENT);
	putimage(backgroundX[0], 0, &backgroundImg[0]);
	putimage(backgroundX[1], 119, &bgdise[2],SRCAND);
	putimage(backgroundX[1], 119, &backgroundImg[1], SRCPAINT);
	putimage(backgroundX[2], 330, &bgdise[3], SRCAND);
	putimage(backgroundX[2], 330, &backgroundImg[2],SRCPAINT);
}
void keyEvent()
{
	char ch;
	if (_kbhit())//如果有按键，kbhit()返回true
	{
		ch = _getch();//getch()不需要按下回车即可直接读取
		if ((ch == 'w'||ch=='W' )&& characterDown == false)
		{
			jump();
			mciSendString(_T("play jump.mp3 "), NULL, 0, NULL);
		}
		else if ((ch == 's'||ch=='S') && characterJump == false) {
			down();
			mciSendString(_T("play down.mp3 "), NULL, 0, NULL);
		}

	}
}

void updateCharacter()
{
	if (characterDown)
	{
		int y = 345 - characterDownImg.getheight();
		putimage(characterX, y, &Downdise[characterIndex], SRCAND);
		putimage(characterX, y, &characterDownImg, SRCPAINT);
		
	}
	else if (characterJump==true)
	{
		putimage(characterX, characterY, &Jumpdise, SRCAND);
		putimage(characterX, characterY, &characterjump,SRCPAINT);
	}
	else
	{
		putimage(characterX, characterY, &dise[characterIndex], SRCAND);
		putimage(characterX, characterY, &characterImg[characterIndex], SRCPAINT);
	}
	Sleep(30);
}


int wcount = rand() % 3 , hcount = rand() % 3;	//墙和柱编号
int obstacleflag=rand()%2;	//决定墙还是柱子：1为柱子，二为墙
int ObstacleSpeed=8;			//障碍物移动速度
void Obstacle()
{
		srand(time(0));
		if (obstacleflag == 1)
		{
			ObstacleY = 15;
			putimage(ObstacleX, ObstacleY, &obstacledise[hcount], SRCAND);
			putimage(ObstacleX, ObstacleY, &obstacle[hcount], SRCPAINT);
		}
		else if(obstacleflag==0)
		{
			ObstacleY = 345 - wall[0].getheight();
			putimage(ObstacleX, ObstacleY, &walldise[wcount], SRCAND);
			putimage(ObstacleX, ObstacleY, &wall[wcount], SRCPAINT);
		}
		ObstacleX -= ObstacleSpeed;
		if (ObstacleX < -80)
		{
			ObstacleX = win_width;
			obstacleflag = rand() % 2;
			wcount = rand() % 4;
			hcount = rand() % 4;
		}
}

void hurt()
{
	int characterheight = 105;
	if (ObstacleX <= -15) hit = 0;
	if (hit)	//如果以及碰撞过就直接返回，不再扣血
		return;
	if (obstacleflag == 0)
	{
		if ((characterX + 73 >= ObstacleX && characterX + 123 <= ObstacleX + 80) && (characterY + characterheight > ObstacleY))
		{
			hit++;
			heroBlood -= 33;
			mciSendString(_T("play hurt1.mp3"), NULL, 0, NULL);
		}
	}

	if (obstacleflag == 1)
	{
		characterheight = 150;
		if (characterX + 123 >= ObstacleX && characterX <= ObstacleX + 63 && characterY <= ObstacleY + 260)
		{
			if (characterDown)	return;
			else
			{
				hit++;
				heroBlood -= 33;
				mciSendString(_T("play hurt2.mp3"), NULL, 0, NULL);
			}
		}
	}
}



int add = 0;
void addscore(int timediff)
{
	int time = timediff / 1000;
	if (bgSpeed[2] < 48)
	{
		if (ObstacleX <= -60) add = 0;
	}
	else if (bgSpeed[2] >= 48)
	{
		if (ObstacleX <= -15) add = 0;
	}
	if (add)	//一个障碍物不重复加分
		return;
	if (obstacleflag == 0)
	{
		if ((characterX + 73 >= ObstacleX && characterX + 123 <= ObstacleX + 80) && hit == 0)
		{
			if (time < 15)	score += 10;
			else if (time >= 15 && time < 30) score += 15;
			else if (time >= 30 && time < 90) score += 30;
			else if (time >= 90 < 600) score += 60;
			else if (time >= 600) score += 1;
			add = 1;
		}
	}
	else if (obstacleflag == 1)
	{
		if ((characterX + 123 >= ObstacleX && characterX <= ObstacleX + 63) && hit == 0)
		{
			if (time < 15)	score += 10;
			else if (time >= 15 && time < 30) score += 15;
			else if (time >= 30 && time < 90) score += 30;
			else if (time >= 90 < 600) score += 60;
			else if (time >= 600) score += 120;
			add = 1;
		}
	}
	
}

const TCHAR str[10][2] = { _T("0"), _T("1"), _T("2"), _T("3"), _T("4"), _T("5"), _T("6"), _T("7"), _T("8"), _T("9") };		//编号对应数字，方便outtextxy
void DrawScore(int score)
{
	mciSendString(_T("stop play.mp3 "), NULL, 0, NULL);
	mciSendString(_T("stop brave.mp3 "), NULL, 0, NULL);
	mciSendString(_T("play gameover.mp3 "), NULL, 0, NULL);
	initgraph(1080, 720);
	putimage(0, 0, &gameover);
	setbkmode(TRANSPARENT);
	int x = 440;
	int y = 500;

	settextcolor(RGB(255, 255, 255));
	settextstyle(30, 0, _T("宋体"));
	outtextxy(x, y, _T("得分："));
	x += 200;
	if (score == 0)
	{
		outtextxy(x, y, str[score]);
	}
	while (score > 0)
	{
		outtextxy(x, y, str[score % 10]);
		score = score / 10;
		x -= 16;
	}
	outtextxy(440, 550, _T("按任意键结束游戏"));
}
void DrawBloodAndScore(int heroBlood,int score)
{
	int bloodx = 800;
	int bloody = 20,scorey=20;
	int scorex = 400;
	settextstyle(20, 0, _T("黑体"));
	outtextxy(bloodx - 110, bloody, _T("剩余血量:"));
	outtextxy(scorex - 100, scorey, _T("得分："));
	while (heroBlood > 0)
	{
		outtextxy(bloodx, bloody, str[heroBlood % 10]);
		heroBlood /= 10;
		bloodx -= 16;
	}
	while (score >0)
	{
		outtextxy(scorex, scorey, str[score % 10]);
		score /= 10;
		scorex -= 16;
	}
}
void changespeed(int timediff)
{
	int i;
	if (timediff / 1000 == 15 || timediff / 1000 == 30 || timediff / 1000 ==90 || timediff / 1000 ==600 )
	{
		mciSendString(_T("play speedup.mp3 "), NULL, 0, NULL);
	}
	if (timediff / 1000 == 90)
	{
		mciSendString(_T("stop play.mp3 "), NULL, 0, NULL);
		mciSendString(_T("stop play.mp3 "), NULL, 0, NULL);
		mciSendString(_T("play speedup.mp3 "), NULL, 0, NULL);
	}
	if (timediff / 1000 == 90)
	{
		mciSendString(_T("play brave.mp3 repeat"), NULL, 0, NULL);
	}

	if (timediff / 1000 > 15 && timediff / 1000 <=30)
	{
		bgSpeed[0] = 2;
		bgSpeed[1] = 4;
		bgSpeed[2] = 16;		//背景速度
		ObstacleSpeed = 16;		//障碍物速度
		characterJumpOff = 26;	//跳跃速度
		delay[1] = 15;	//下蹲延迟
	}
	else if (timediff/1000 > 30&&timediff/1000<=90)
	{
		bgSpeed[0] = 4;
		bgSpeed[1] = 8;
		bgSpeed[2] = 32;		//背景速度
		ObstacleSpeed = 32;		//障碍物速度
		characterJumpOff = 52;	//跳跃速度
		delay[1] = 7;	//下蹲延迟
	}
	else if (timediff / 1000 > 90&&timediff/1000<=600)
	{
		bgSpeed[0] = 6;
		bgSpeed[1] = 12;
		bgSpeed[2] =48;		//背景速度
		ObstacleSpeed = 48;		//障碍物速度
		characterJumpOff = 65;	//跳跃速度
		delay[1] = 4;	//下蹲延迟
	}
	else if (timediff / 1000 > 600)
	{
		bgSpeed[0] =8;
		bgSpeed[1] = 16;
		bgSpeed[2] = 64;		//背景速度
		ObstacleSpeed = 64;		//障碍物速度
		characterJumpOff = 104;	//跳跃速度
		delay[1] = 3;	//下蹲延迟
	}
}
clock_t start;

DWORD WINAPI Time(LPVOID lpvoid)
{
	int timediff = 0;
	clock_t  end;
	start = clock();

	while (1)
	{
		setbkmode(TRANSPARENT);
		settextcolor(RGB(0, 100, 205));
		settextstyle(20, 0, _T("黑体"));
		BeginBatchDraw();
		DisplayTime(timediff);
		DrawBloodAndScore(heroBlood,score);
		FlushBatchDraw();
		EndBatchDraw();
		Sleep(1);
		end = clock();
		timediff = end - start;
		if (heroBlood == 0)
		{
			cleardevice();
			return 0;
		}

	}
}

int main()
{
	int timediff = 0;
	clock_t  end;
	srand(time(0));
	initgraph(MAPW, MAPH);
	DrawWelcome();
	DrawChoose();
	DrawBackground();
	init();
	start = clock();
	CreateThread(NULL, NULL, Time, NULL, NULL, NULL);
	
	while (1)
	{

		keyEvent();
		update = true;
		if (update)
		{
			update = false;
			BeginBatchDraw();
			updateBackground();
			Obstacle();
			updateCharacter();
			EndBatchDraw();
			fly();
			hurt();
			end = clock();
			timediff = end - start;
			addscore(timediff);		//加分
			changespeed(timediff);	//变速
			
			if (heroBlood == 0) break;
		}
	}
	
	
	DrawScore(score);
	system("pause");
	_getch();
	return 0;


}

