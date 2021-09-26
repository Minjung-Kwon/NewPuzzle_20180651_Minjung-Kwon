#define _CRT_SECURE_NO_WARNINGS
#include <bangtal>
using namespace bangtal;

#include<cstdlib>
#include<ctime>     //랜덤함수 초기화
#include <Windows.h>



ScenePtr scene;
ObjectPtr game_board[16], game_original[16];
ObjectPtr start;

int blank;

TimerPtr timer;
float animation_time = 0.02f;
int mixCount;





void show_clock()       //현재 게임 진행 시간 보여주는 타이머 clock. 시간이 지나면  Fail 메시지 출력
{

    auto clock = Timer::create(100.f);
    showTimer(clock);

    start->show();
    clock->start();

    clock->setOnTimerCallback([&](TimerPtr clock)->bool {
        showMessage("FAIL");

        start->show();      //게임이 fail되면 다시 시작하는 버튼

        return true;
    });
}



void end_button()       //게임 중 재시작 버튼
{

    auto endButton = Object::create("Images-2/end.png", scene, 1100, 50);

    endButton->setOnMouseCallback([&](ObjectPtr object, int x, int y, MouseAction action)->bool {
        endGame();
        return true;
    });

}




int game_index(ObjectPtr piece)     //piece 인덱스를 찾아내는 함수
{
    for (int i = 0; i < 16;i++)
        if (game_board[i] == piece) return i;
    return -1;      //INVALID PIECE
}


int index_to_x(int index)
{
    return 275 + 180 * (index % 4);
}


int index_to_y(int index)
{
     return 540 - 180 * (index / 4);
}


void game_move(int index)       //index와 blank를 바꿔주는 역할
{
    auto piece = game_board[index];
    game_board[index] = game_board[blank];
    game_board[index]->locate(scene, index_to_x(index), index_to_y(index));

    game_board[blank] = piece;
    game_board[blank]->locate(scene, index_to_x(blank), index_to_y(blank));
    blank = index;     

}


bool check_move(int index)      //인덱스 체크 함수
{
    //제일 첫번째 열이 아니면  &&  인덱스가 블랭크의 왼쪽 index == blank - 1
    if (blank % 4 > 0 && index == blank - 1)  return true;

    //제일 마지막 열이 아니면  &&  인텍스가 블랭크 오른쪽 index == blank + 1
    if (blank % 4 < 3 && index == blank + 1)  return true;

    //맨 윗줄이 아니면  &&  인덱스가 블랭크 위  index == blank - 4
    if (blank / 4 > 0 && index == blank - 4)  return true;

    //맨 마지막 줄이 아니면  &&  인데스가 블랭크 아래 index == blank + 4
    if (blank / 4 < 3 && index == blank + 4)  return true;
    
    return false;
}


int random_move()
{
    int index = rand() % 16;
    while (check_move(index))       //인덱스가 움직일 수 있는데까지 움직임. 
    {
        index = rand() % 16;
    }
    return index;
}


void start_game()       //start 버튼 누를 때 한번에 50번 반복 섞기
{
    mixCount = 50;
    timer->set(animation_time);
    timer->start();


    blank = 15;
    game_board[blank]->hide();



    start->hide();
}


bool check_end()
{
    for (int i = 0; i < 16;i++)
    {
        if (game_board[i] != game_original[i])  return false;
    }
    return true;
}


void end_game()     //게임성공
{
    game_board[blank]->show();
    start->show();
    showMessage("Completed!");

}

void init_game()
{	
    scene = Scene::create("춘식이newPuzzle", "Images-2/background.png");


    //퍼즐 초기 나열 (원래 그림 배치)
    char path[20];
    for (int i = 0;i < 16;i++)
    {
        sprintf(path, "Images-2/p%d.png", i + 1);
        
        game_board[i] = Object::create(path, scene, index_to_x(i), index_to_y(i));        //블록 배열

        game_board[i]->setOnMouseCallback([&](auto piece, auto x, auto y, auto action)->bool
        {
            //piece로부터 index를 알아내야 함. 
            int index = game_index(piece);

            if (check_move(index))      //움직일 수 있으면 무브 실행
            {
                game_move(index);

                if (check_end())
                {
                    end_game();
                }
            }
            
            return true;
        });
        game_original[i] = game_board[i];

    }


    start = Object::create("Images-2/시작.png", scene, 470, 200);
    start->setOnMouseCallback([&](ObjectPtr object, int x, int y, MouseAction action)->bool {       //시작 버튼
        start->hide();

        show_clock();       //시작 버튼 누르면 타이머 실행
        end_button();       //게임 중간에 끝내는 버튼

        auto clockback = Object::create("Images-2/시계투명.png", scene, 565, 670);
  

        start_game();


        return true;
    });

    timer = Timer::create(animation_time);
    timer->setOnTimerCallback([&](auto)->bool
    {
        game_move(random_move());

        mixCount--;
        if (mixCount > 0)
        {        
            timer->set(animation_time);     //타이머 다시 실행
            timer->start();
        }
        return 0;
    });

    
	startGame(scene);


}



int main()
{
    srand((unsigned int)time(NULL));
    
    setGameOption(GameOption::GAME_OPTION_INVENTORY_BUTTON, false);
	setGameOption(GameOption::GAME_OPTION_MESSAGE_BOX_BUTTON, false);
	
    init_game();

	
	return 0;
}