//
//  smm_node.c
//  SMMarble
//
//  Created by Juyeop Kim on 2023/11/05.
//

#include "smm_common.h"
#include "smm_object.h"
#include <string.h>

#define MAX_NODETYPE    7
#define MAX_GRADE       9
#define MAX_NODE        100


static char smmNodeName[SMMNODE_TYPE_MAX][MAX_CHARNAME] = {
       "강의",
       "식당",
       "실험실",
       "집",
       "실험실로이동",
       "음식찬스",
       "축제시간"
};

char* smmObj_getTypeName(int type)
{
      return (char*)smmNodeName[type];
}



//구조체 형식 정의
typedef struct smmObject {
       char name[MAX_CHARNAME];
       smmObjType_e objType; 
       int type;
       int credit;
       int energy;
       smmObjGrade_e grade;
} smmObject_t;

//구조체 변수 정의 
static smmObject_t smm_node[MAX_NODE];
static int smmObj_noNode = 0;

//관련 함수 변경 
//object generation(parameter로 object를 생성) 
void* smmObj_genObject(char* name, smmObjType_e objType, int type, int credit, int energy, smmObjGrade_e grade)
{    
    smmObject_t* ptr;
    
    ptr = (smmObject_t*)malloc(sizeof(smmObject_t));
    
    strcpy(ptr->name, name);
    ptr->objType = objType;
    ptr->type = type;
    ptr->credit = credit;
    ptr->energy = energy;
    ptr->grade = grade;
    
    return ptr;
}

//3. 관련 함수 변경 
char* smmObj_getNodeName(void* obj) // object의 이름을 return
{
    smmObject_t* ptr = (smmObject_t*)obj;
    
    return ptr->name;
}

int smmObj_getNodeType(int node_nr) // object의 type을 return 
{
	smmObject_t* ptr = (smmObject_t*)obj;
	
    return ptr->type;
}

int smmObj_getNodeCredit(int node_nr) // object의 credit울 return 
{
	smmObject_t* ptr = (smmObject_t*)obj;
	
    return ptr->credit;
}

int smmObj_getNodeEnergy(int node_nr) // object의 energy를 return 
{
	smmObject_t* ptr = (smmObject_t*)obj;
	
    return ptr->energy;
}
