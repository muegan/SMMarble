//
//  main.c
//  SMMarble
//
//  Created by Juyeop Kim on 2023/11/05.
//

#include <time.h>
#include <string.h>
#include "smm_object.h"
#include "smm_database.h"
#include "smm_common.h"

#define BOARDFILEPATH "marbleBoardConfig.txt"
#define FOODFILEPATH "marbleFoodConfig.txt"
#define FESTFILEPATH "marbleFestivalConfig.txt"


//board configuration parameters
static int board_nr;
static int food_nr;
static int festival_nr;

static int player_nr; // static 변수로 게임 보드, 카드구성 등 설정을 저장 




typedef struct player {
		int energy;
		int position;
		char name[MAX_CHARNAME];
		int accumCredit;
		int flag_graduate;
		int flag_exp; // 실험 중 상태: 0
} player_t; //구조체로 플레이어의 정보(에너지, 위치, 이름, 학점)을 저장 

static player_t *cur_player;
//static player_t cur_player[MAX_PLAYER];


static int player_energy[MAX_PLAYER];
static int player_position[MAX_PLAYER];
static char player_name[MAX_PLAYER][MAX_CHARNAME];
//플레이어 인원 수만큼 상태 개수를 설정해줌 

//function prototypes
#if 0
int isGraduated(void); //graduated 플레이어 있는지 확인
void generatePlayers(int n, int initEnergy); // 새 플레이어 생성
void goForward(int player, int step); // 플레이어가 보드 위에서 이동(check graduate)
void printPlayerStatus(void); // 매 턴 시작마다 플레이어의 상태 출력
float calcAverageGrade(int player); // 플레이어의 평균 성적 계산
smmGrade_e takeLecture(int player, char *lectureName, int credit); // 강의 수강(insert a grade of the player)
void* findGrade(int player, char *lectureName); //find the grade from the player's grade history
void printGrades(int player); //print all the grade history of the player
#endif

int isGraduated(void) // 플레이어가 graduate 했는지 체크
{
	int i;
	for (i=0;i<player_nr;i++)// 플레이어 인원 수만큼 반복
	{
		if (cur_player[i].accumCredit >= GRADUATE_CREDIT)
			cur_player[i].flag_graduate = 1; // 졸업 O
	} 
	return cur_player[i].flag_graduate = 0; // 졸업 X
}



void printGrades(int player) // 플레이어의 성적 이력을 출력 
{
	int i;
	void *gradePtr;
	for (i=0;i<smmdb_len(LISTNO_OFFSET_GRADE + player);i++)
	{
		gradePtr = smmdb_getData(LISTNO_OFFSET_GRADE + player, i);
		printf("%s : %i\n", smmObj_getNodeName(gradePtr), smmObj_getNodeGrade(gradePtr));
	}
} 

void printPlayerStatus(void) // 턴 시작마다, 모든 플레이어의 상태 출력 
{
	int i;
	
	printf("===========PLAYER STATUS=============\n");
	for (i=0;i<player_nr;i++)
     {
         printf("%s : credit %i, energy %i, position %i\n", 
                    cur_player[i].name,
                    cur_player[i].accumCredit,
                    cur_player[i].energy,
            		cur_player[i].position);
     }
     printf("===========PLAYER STATUS=============\n\n");
}

void generatePlayers(int n, int initEnergy) // 새 플레이어를 생성 
{
     int i;
     //n time loop
     for (i=0;i<n;i++)
     {
         //input name
         printf("Input player %i's name:", i);
         scanf("%s", cur_player[i].name);
         fflush(stdin);
         
         //set position
         cur_player[i].position = 0;
         
         //set energy
         cur_player[i].energy = initEnergy;
         cur_player[i].accumCredit = 0;
         cur_player[i].flag_graduate = 0;
         cur_player[i].flag_exp = 0;
     }
}


int rolldie(int player) // 주사위를 굴림 
{
    char c;
    printf(" Press any key to roll a die (press g to see grade): ");
    c = getchar();
    fflush(stdin);

    if (c == 'g')
        printGrades(player); // 'g'를 입력한 경우 플레이어 성적을 출력 

    
    return (rand()%MAX_DIE + 1); // 1 ~ MAX_DIE 중 랜덤한 값을 return
}


int randomGrade(void) // 랜덤한 성적을 return
{
	int gradeList[9] = {4.3, 4.0, 3.7, 3.3, 3.0, 2.7, 2.3, 2.0, 1.7};
	
	int randGrade = rand()%9;
	return gradeList[randGrade];
 } 


//action code when a player stays at a node
void actionNode(int player)
{
	void *boardPtr = smmdb_getData(LISTNO_NODE, cur_player[player].position );
    int type = smmObj_getNodeType( boardPtr );
    char *name = smmObj_getNodeName( boardPtr );
    void *gradePtr;
    
    switch(type)
    {
        case SMMNODE_TYPE_LECTURE:
        {		
        	// 이전에 듣지 않음
        	
			// 충분한에너지(현재 에너지>강의에너지)
        	if(cur_player[player].energy >= smmObj_getNodeEnergy(boardPtr))
        	{
        		printf("%s is too hungry to take the lecture\n", cur_player[player].name);
        		break;
			}
        		
        	// 강의를 수강했을 때, credit은 더하고 energy는 빼줌
            cur_player[player].accumCredit += smmObj_getNodeCredit( boardPtr );
            cur_player[player].energy -= smmObj_getNodeEnergy( boardPtr );
            
            //grade generation
            gradePtr = smmObj_genObject(name, smmObjType_grade, 0, smmObj_getNodeCredit( boardPtr ), 0, randomGrade());
            smmdb_addTail(LISTNO_OFFSET_GRADE + player, gradePtr);
            
            break;
    	}
    	
        case SMMNODE_TYPE_RESTAURANT:
        	// 플레이어 현재 에너지 += 보충 에너지
        	cur_player[player].energy += smmObj_getNodeEnergy(boardPtr);
        	printf("Let's eat in %s and charge %i energies\n\n",smmObj_getNodename(boardPtr), smmObj_getNodeEnergy(boardPtr));
        	
        	break;
        
        case SMMNODE_TYPE_LABORATORY:
        {
			
        	if (cur_player[player].flag_exp==0)//실험 중 상태일 때
        	{
        		printf("Experiment time! (threshold: 4) ");
        		int a = rolldie(player);
        		
        		if (a < 4) // 실험 실패 
        		{
        			printf("Experiment result : %i, fail. %s needs more experiments...", a, cur_player[player].name);
        			cur_player[player].energy -= smmObj_getNodeEnergy(boardPtr);
				}
				else // 실험 성공 
				{
					printf("Experiment result : %i, success! %s can exit this lab!", a, cur_player[player].name);
					cur_player[player].energy -= smmObj_getNodeEnergy(boardPtr);
					cur_player[player].flag_exp=1;
				}
			}
			else
				printf("This is not experiment time.\n\n");
			
        	break;
        	
    	}
    	
        case SMMNODE_TYPE_HOME:
        	// 지나가는 순간 현재 에너지 += 보충 에너지
        	cur_player[player].energy += smmObj_getNodeEnergy(boardPtr);
        	printf("returned to HOME! energy charged by %i\n", smmObj_getNodeEnergy(boardPtr));
        	
        	break;
        	
        case SMMNODE_TYPE_GOTOLAB:
        	//실험실로 이동(실험 중 상태로 전환)
        	printf("This is experiment time! %s goes to the lab.\n\n", cur_player[player].name);
        	cur_player[player].flag_exp=0;
        	
        	break;
        	
        case SMMNODE_TYPE_FOODCHANCE:
       	{
		   // 음식카드 랜덤 선택, 현재 에너지 += 보충 에너지
        	void* foodPtr;
			foodPtr= smmdb_getData(LISTNO_FOODCARD, rand()%smmdblen(LISTNO_FOODCARD));
        	cur_player[player].energy += smmObj_getFoodCharge(foodPtr);
        	printf("%s picks %s and charges %i\n\n", cur_player[player].name, smmObj_getFoodname(foodPtr), smmObj_getFoodChage(foodPtr));
        	
        	break;
    	}
    	
        case SMMNODE_TYPE_FESTIVAL:
        {
        	// 축제카드 랜덤 선택, 미션 수행
        	void* festPtr;
        	char c;
        	
        	festPtr = smmdb_getData(LISTNO_FESTCARD, rand()%smmdb_len(LISTNO_FESTCARD));
        	printf("%s participates to Snow Festival! press any key to pick a festival card: ", cur_player[player].name);
        	c=getchar();
        	fflush(stdin);
        	
			printf("MISSION : %s\n\n", smmObj_getFestivalname(festPtr));
			
			printf("(Press any key when mission is ended.)");
			c=getchar();
			fflush(stdin);
        	
        	break;
    	}
            
        default:
            break;
    }
}

void goForward(int player, int step) // 플레이어를 보드 위에서 이동
{
    void *boardPtr;
    cur_player[player].position += step; // 플레이어 위치 + 전진 수 
    boardPtr = smmdb_getData(LISTNO_NODE, cur_player[player].position ); // 이동 완료 위치 노드 정보 저장 
     
    printf("%s go to node %i (name: %s)\n", 
                cur_player[player].name, cur_player[player].position,
                smmObj_getNodeName(boardPtr));
}


int main(int argc, const char * argv[]) {
    
    FILE* fp;
    char name[MAX_CHARNAME];
    int type;
    int credit;
    int energy;
    int i;
    int initEnergy;
    int turn=0;
    
    board_nr = 0;
    food_nr = 0;
    festival_nr = 0;
    
    srand(time(NULL));
    
    
    //1. import parameters ---------------------------------------------------------------------------------
    //1-1. boardConfig 
    if ((fp = fopen(BOARDFILEPATH,"r")) == NULL)
    {
        printf("[ERROR] failed to open %s. This file should be in the same directory of SMMarble.exe.\n", BOARDFILEPATH);
        getchar();
        return -1;
    }
    
    printf("Reading board component......\n");
    while ((fscanf(fp, "%s %i %i %i", name, &type, &credit, &energy))==4) //read a node parameter set
    {
        //store the parameter set
        //(char* name, smmObjType_e objType, int type, int credit, int energy, smmObjGrade_e grade)
        void *boardObj = smmObj_genObject(name, smmObjType_board, type, credit, energy, 0);
        smmdb_addTail(LISTNO_NODE, boardObj);
        
        if (type == SMMNODE_TYPE_HOME)
           initEnergy = energy;
        board_nr++;
    }
    fclose(fp);
    printf("Total number of board nodes : %i\n", board_nr);
    
    
    for (i = 0;i<board_nr;i++)
    {
        void *boardObj = smmdb_getData(LISTNO_NODE, i);
        
        printf("node %i : %s, %i(%s), credit %i, energy %i\n", 
                     i, smmObj_getNodeName(boardObj), 
                     smmObj_getNodeType(boardObj), smmObj_getTypeName(smmObj_getNodeType(boardObj)),
                     smmObj_getNodeCredit(boardObj), smmObj_getNodeEnergy(boardObj));
    }
    //printf("(%s)", smmObj_getTypeName(SMMNODE_TYPE_LECTURE));
    

    //2. food card config 
    if ((fp = fopen(FOODFILEPATH,"r")) == NULL)
    {
        printf("[ERROR] failed to open %s. This file should be in the same directory of SMMarble.exe.\n", FOODFILEPATH);
        return -1;
    }
    
    printf("\n\nReading food card component......\n");
    while ((fscanf(fp, " %s %i", name, &energy))==2) //read a food parameter set
    {
        //store the parameter set
    }
    fclose(fp);
    printf("Total number of food cards : %i\n", food_nr);
    
    
    
    //3. festival card config 
    if ((fp = fopen(FESTFILEPATH,"r")) == NULL)
    {
        printf("[ERROR] failed to open %s. This file should be in the same directory of SMMarble.exe.\n", FESTFILEPATH);
        return -1;
    }
    
    printf("\n\nReading festival card component......\n");
    while ((fscanf(fp, "%s", name))==1) //read a festival card string
    {
        //store the parameter set
    }
    fclose(fp);
    printf("Total number of festival cards : %i\n", festival_nr);
    
    
    //2. Player configuration ---------------------------------------------------------------------------------
    
    do
    {
        //input player number to player_nr
        printf("input player no.:");
        scanf("%d", &player_nr);
        fflush(stdin);
    }
    while (player_nr < 0 || player_nr > MAX_PLAYER);
    cur_player = (player_t*)malloc(player_nr*sizeof(player_t));
    generatePlayers(player_nr, initEnergy);
	
	
    
    
    
    
    //3. SM Marble game starts ---------------------------------------------------------------------------------
    while (isGraduated()==0) //is anybody graduated?
    {
        int die_result;
        
        
        //4-1. initial printing
        printPlayerStatus();
        
        //4-2. die rolling (if not in experiment)
        die_result = rolldie(turn);
        printf("die result : %i\n", die_result);
        
        //4-3. go forward
        goForward(turn, die_result);
        printf("player position : %i\n", cur_player[turn].position);

		//4-4. take action at the destination node of the board
        actionNode(turn);
        
        //4-5. next turn
        turn = (turn + 1)%player_nr;
    }
    
    
    free(cur_player);
    system("PAUSE");
    return 0;
}
