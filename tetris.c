#include "tetris.h"

static struct sigaction act, oact;

int main(){
	int exit=0;

	initscr();
	noecho();
	keypad(stdscr, TRUE);	

	srand((unsigned int)time(NULL));

	while(!exit){
		clear();
		switch(menu()){
		case MENU_PLAY: play(); break;
		case MENU_EXIT: exit=1; break;
		default: break;
		}
	}

	endwin();
	system("clear");
	return 0;
}

void InitTetris(){
	int i,j;

	for(j=0;j<HEIGHT;j++)
		for(i=0;i<WIDTH;i++)
			field[j][i]=0;

	nextBlock[0]=rand()%7;
	nextBlock[1]=rand()%7;
        nextBlock[2]=rand()%7; // 1주차 과제2 구현
	blockRotate=0;
	blockY=-1;
	blockX=WIDTH/2-2;
	score=0;	
	gameOver=0;
	timed_out=0;

	DrawOutline();
	DrawField();
	DrawBlock(blockY,blockX,nextBlock[0],blockRotate,' ');
	DrawNextBlock(nextBlock);
	PrintScore(score);
}

void DrawOutline(){	
	int i,j;
	/* 블럭이 떨어지는 공간의 태두리를 그린다.*/
	DrawBox(0,0,HEIGHT,WIDTH);

	/* next block을 보여주는 공간의 태두리를 그린다.*/
	move(2,WIDTH+10);
	printw("NEXT BLOCK");
	DrawBox(3,WIDTH+10,4,8);
        move(9, WIDTH+10);
        DrawBox(10,WIDTH+10,4,8); // 1주차 과제2 구현

	/* score를 보여주는 공간의 태두리를 그린다.*/
	move(17,WIDTH+10);
	printw("SCORE");
	DrawBox(18,WIDTH+10,1,8);
}

int GetCommand(){
	int command;
	command = wgetch(stdscr);
	switch(command){
	case KEY_UP:
		break;
	case KEY_DOWN:
		break;
	case KEY_LEFT:
		break;
	case KEY_RIGHT:
		break;
	case ' ':	/* space key*/
		/*fall block*/
		break;
	case 'q':
	case 'Q':
		command = QUIT;
		break;
	default:
		command = NOTHING;
		break;
	}
	return command;
}

int ProcessCommand(int command){
	int ret=1;
	int drawFlag=0;
	switch(command){
	case QUIT:
		ret = QUIT;
		break;
	case KEY_UP:
		if((drawFlag = CheckToMove(field,nextBlock[0],(blockRotate+1)%4,blockY,blockX)))
			blockRotate=(blockRotate+1)%4;
		break;
	case KEY_DOWN:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY+1,blockX)))
			blockY++;
		break;
	case KEY_RIGHT:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY,blockX+1)))
			blockX++;
		break;
	case KEY_LEFT:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY,blockX-1)))
			blockX--;
		break;
	default:
		break;
	}
	if(drawFlag) DrawChange(field,command,nextBlock[0],blockRotate,blockY,blockX);
	return ret;	
}

void DrawField(){
	int i,j;
	for(j=0;j<HEIGHT;j++){
		move(j+1,1);
		for(i=0;i<WIDTH;i++){
			if(field[j][i]==1){
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(".");
		}
	}
}


void PrintScore(int score){
	move(19,WIDTH+11);
	printw("%8d",score); // 1주차 과제2 구현
}

void DrawNextBlock(int *nextBlock){
	int i, j;
	for( i = 0; i < 4; i++ ){
		move(4+i,WIDTH+13);
		for( j = 0; j < 4; j++ ){
			if( block[nextBlock[1]][0][i][j] == 1 ){
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(" ");
		}
	}
        for( i = 0; i < 4; i++)
        {
                move(10 + i, WIDTH + 13);
                for( j = 0; j < 4; j++)
                {
                        if( block[nextBlock[2]][0][i][j] == 1)
                        {
                                attron(A_REVERSE);
                                printw(" ");
                                attroff(A_REVERSE);
                        }
                        else printw(" ");
                }
        }                                                       // 1주차 과제2 구현
}

void DrawBlock(int y, int x, int blockID,int blockRotate,char tile){
	int i,j;
	for(i=0;i<4;i++)
		for(j=0;j<4;j++){
			if(block[blockID][blockRotate][i][j]==1 && i+y>=0){
				move(i+y+1,j+x+1);
				if(tile != '.'){
                                        attron(A_REVERSE);
                                }                               //추가 수정
				printw("%c",tile);
                                if(tile != '.'){
				        attroff(A_REVERSE);
                                }                               //지나간 자리는 배경색 반전을 해 주지 않음.
			}
		}

	move(HEIGHT,WIDTH+10);
}

void DrawBox(int y,int x, int height, int width){
	int i,j;
	move(y,x);
	addch(ACS_ULCORNER);
	for(i=0;i<width;i++)
		addch(ACS_HLINE);
	addch(ACS_URCORNER);
	for(j=0;j<height;j++){
		move(y+j+1,x);
		addch(ACS_VLINE);
		move(y+j+1,x+width+1);
		addch(ACS_VLINE);
	}
	move(y+j+1,x);
	addch(ACS_LLCORNER);
	for(i=0;i<width;i++)
		addch(ACS_HLINE);
	addch(ACS_LRCORNER);
}

void play(){
	int command;
	clear();
	act.sa_handler = BlockDown;
	sigaction(SIGALRM,&act,&oact);
	InitTetris();
	do{
		if(timed_out==0){
			alarm(1);
			timed_out=1;
		}

		command = GetCommand();
		if(ProcessCommand(command)==QUIT){
			alarm(0);
			DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
			move(HEIGHT/2,WIDTH/2-4);
			printw("Good-bye!!");
			refresh();
			getch();

			return;
		}
	}while(!gameOver);

	alarm(0);
	getch();
	DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
	move(HEIGHT/2,WIDTH/2-4);
	printw("GameOver!!");
	refresh();
	getch();
	newRank(score);
}

char menu(){
	printw("1. play\n");
	printw("2. rank\n");
	printw("3. recommended play\n");
	printw("4. exit\n");
	return wgetch(stdscr);
}

/////////////////////////첫주차 실습에서 구현해야 할 함수/////////////////////////

int CheckToMove(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int blockY, int blockX){
	// user code
        int i, j;
        for(j = 0; j < 4; j++)
        {
                for(i = 0; i < 4; i++)
                {
                        if(block[currentBlock][blockRotate][j][i] == 1)
                        {
                                if(((blockX + i) < 0) || (blockX + i >= WIDTH))
                                        return 0;
                                else if(((blockY + j) < 0) || (blockY + j) >= HEIGHT)
                                        return 0;
                                else if(f[blockY + j][blockX + i] == 1)
                                        return 0;
                        }
                }
        }
        return 1;
}

void DrawChange(char f[HEIGHT][WIDTH],int command,int currentBlock,int blockRotate, int blockY, int blockX){
	// user code
        
        switch (command)
        {
                case KEY_UP:                
                        DrawBlockWithFeatures(blockY, blockX, currentBlock, (blockRotate + 3) % 4, '.'); //DrawBlock함수 정의 일부 수정
                        break;
                case KEY_RIGHT:
                        DrawBlockWithFeatures(blockY, blockX - 1, currentBlock, blockRotate, '.');
                        break;
                case KEY_LEFT:
                        DrawBlockWithFeatures(blockY, blockX + 1, currentBlock, blockRotate, '.');
                        break;
                case KEY_DOWN:
                        DrawBlockWithFeatures(blockY - 1, blockX, currentBlock, blockRotate, '.');
                        break;
        }
        DrawBlockWithFeatures(blockY, blockX, currentBlock, blockRotate, ' ');

	//1. 이전 블록 정보를 찾는다. ProcessCommand의 switch문을 참조할 것
	//2. 이전 블록 정보를 지운다. DrawBlock함수 참조할 것.
	//3. 새로운 블록 정보를 그린다. 
}

void BlockDown(int sig){
	// user code
        if(CheckToMove(field, nextBlock[0], blockRotate, blockY+1, blockX))
        {
               blockY++;
                DrawChange(field, KEY_DOWN, nextBlock[0], blockRotate, blockY, blockX); 
        }
        else
        {
                if(blockY == -1)
                {
                        gameOver = 1;
                }               
                score += AddBlockToField(field, nextBlock[0], blockRotate, blockY, blockX); // 1주차 과제3 구현
                score += DeleteLine(field);
                PrintScore(score); // 점수 계산 및 출력

                nextBlock[0] = nextBlock[1];
                nextBlock[1] = nextBlock[2];
                nextBlock[2] = rand() % 7;
                DrawNextBlock(nextBlock); // 다음 블록 초기화 및 출력, 1주차 과제2 구현

                blockY = -1;
                blockX = WIDTH / 2 - 2;
                blockRotate = 0; // 블록 위치 및 회전수 초기화
                
                DrawField();
        }
        timed_out = 0;

	//강의자료 p26-27의 플로우차트를 참고한다.
}

int AddBlockToField(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int blockY, int blockX){
	// user code

        int i, j, touched = 0;
        for(j = 0; j < 4; j++)
        {
                for(i = 0; i < 4; i++)
                {
                        if(block[currentBlock][blockRotate][j][i] == 1) // currentBlock[j][i] == 1
                        {
                                field[blockY+j][blockX+i] = 1;
                                if((field[blockY+j+1][blockX+i] == 1) || (blockY+j+1 == HEIGHT))
                                {
                                        touched++;                              // 1주차 과제3 구현
                                }
                        }
                }
        }
	//Block이 추가된 영역의 필드값을 바꾼다.
        return touched * 10; // 1주차 과제3 구현
}

int DeleteLine(char f[HEIGHT][WIDTH]){
	// user code

        int cnt = 0, result = 1, i, j, k, l;

        for(j = 0; j < HEIGHT; j++)
        {
                result = 1;
                for(i = 0; i < WIDTH; i++)
                {
                        if(f[j][i] == 0)
                        {
                                result *= 0;
                                break;
                        }
                }
                if(result == 1)
                {
                        cnt+=1;
                        for(k = j-1; k >= 0; k--) // 밑에서부터 올라와야 함
                        {
                                for(l = 0; l < WIDTH; l++)
                                {
                                        field[k+1][l] = field[k][l];
                                }
                        }
                }
        }
        return 100*cnt*cnt;

	//1. 필드를 탐색하여, 꽉 찬 구간이 있는지 탐색한다.
	//2. 꽉 찬 구간이 있으면 해당 구간을 지운다. 즉, 해당 구간으로 필드값을 한칸씩 내린다.
}

///////////////////////////////////////////////////////////////////////////

void DrawShadow(int y, int x, int blockID,int blockRotate, char tile){
	// user code
        int offset = 0;
        while(CheckToMove(field, blockID, blockRotate, y+offset+1, x))
        {
                offset++;
        }
        DrawBlock(y+offset, x, blockID, blockRotate, tile);
}                                                                       //1주차 과제1 구현

void DrawBlockWithFeatures(int y, int x, int blockID, int blockRotate, char tile){
        if(tile == '.'){
                DrawShadow(y, x, blockID, blockRotate, tile);
                DrawBlock(y, x, blockID, blockRotate, tile);
        }
        else{
                DrawShadow(y, x, blockID, blockRotate, '/');
                DrawBlock(y, x, blockID, blockRotate, ' ');
        }
}                                                                       //1주차 과제1 구현

void createRankList(){
	// user code
}

void rank(){
	// user code
}

void writeRankFile(){
	// user code
}

void newRank(int score){
	// user code
}

void DrawRecommend(int y, int x, int blockID,int blockRotate){
	// user code
}

int recommend(RecNode *root){
	int max=0; // 미리 보이는 블럭의 추천 배치까지 고려했을 때 얻을 수 있는 최대 점수

	// user code

	return max;
}

void recommendedPlay(){
	// user code
}
