#define _CRT_SECURE_NO_WARNINGS
#include <bangtal>
using namespace bangtal;

#include<cstdlib>
#include<ctime>     //�����Լ� �ʱ�ȭ
#include <Windows.h>



ScenePtr scene;
ObjectPtr game_board[16], game_original[16];
ObjectPtr start;

int blank;

TimerPtr timer;
float animation_time = 0.02f;
int mixCount;


int game_index(ObjectPtr piece)     //piece �ε����� ã�Ƴ��� �Լ�
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


void game_move(int index)       //index�� blank�� �ٲ��ִ� ����
{
    auto piece = game_board[index];
    game_board[index] = game_board[blank];
    game_board[index]->locate(scene, index_to_x(index), index_to_y(index));

    game_board[blank] = piece;
    game_board[blank]->locate(scene, index_to_x(blank), index_to_y(blank));
    blank = index;     

}


bool check_move(int index)      //�ε��� üũ �Լ�
{
    //���� ù��° ���� �ƴϸ�  &&  �ε����� ��ũ�� ���� index == blank - 1
    if (blank % 4 > 0 && index == blank - 1)  return true;

    //���� ������ ���� �ƴϸ�  &&  ���ؽ��� ��ũ ������ index == blank + 1
    if (blank % 4 < 3 && index == blank + 1)  return true;

    //�� ������ �ƴϸ�  &&  �ε����� ��ũ ��  index == blank - 4
    if (blank / 4 > 0 && index == blank - 4)  return true;

    //�� ������ ���� �ƴϸ�  &&  �ε����� ��ũ �Ʒ� index == blank + 4
    if (blank / 4 < 3 && index == blank + 4)  return true;
    
    return false;
}


int random_move()
{
    int index = rand() % 16;
    while (check_move(index))       //�ε����� ������ �� �ִµ����� ������. 
    {
        index = rand() % 16;
    }
    return index;
}


void start_game()       //start ��ư ���� �� �ѹ��� 50�� �ݺ� ����
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


void end_game()
{
    game_board[blank]->show();
    start->show();
    showMessage("Completed!");

}

void init_game()
{	
    scene = Scene::create("�����newPuzzle", "Images-2/background.png");


    //���� �ʱ� ���� (���� �׸� ��ġ)
    char path[20];
    for (int i = 0;i < 16;i++)
    {
        sprintf(path, "Images-2/p%d.png", i + 1);
        
        game_board[i] = Object::create(path, scene, index_to_x(i), index_to_y(i));        //��� �迭

        game_board[i]->setOnMouseCallback([&](auto piece, auto x, auto y, auto action)->bool
        {
            //piece�κ��� index�� �˾Ƴ��� ��. 
            int index = game_index(piece);

            if (check_move(index))      //������ �� ������ ���� ����
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


    start = Object::create("Images-2/����.png", scene, 470, 100);
    start->setOnMouseCallback([&](ObjectPtr object, int x, int y, MouseAction action)->bool {       //���� ��ư
        start->hide();
        
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
            timer->set(animation_time);     //Ÿ�̸� �ٽ� ����
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