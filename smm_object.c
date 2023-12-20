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
       "����",
       "�Ĵ�",
       "�����",
       "��",
       "����Ƿ��̵�",
       "��������",
       "�����ð�"
};

char* smmObj_getTypeName(int type)
{
      return (char*)smmNodeName[type];
}



//����ü ���� ����
typedef struct smmObject {
       char name[MAX_CHARNAME];
       smmObjType_e objType; 
       int type;
       int credit;
       int energy;
       smmObjGrade_e grade;
} smmObject_t;

//����ü ���� ���� 
static smmObject_t smm_node[MAX_NODE];
static int smmObj_noNode = 0;

//���� �Լ� ���� 
//object generation(parameter�� object�� ����) 
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

//3. ���� �Լ� ���� 
char* smmObj_getNodeName(void* obj) // object�� �̸��� return
{
    smmObject_t* ptr = (smmObject_t*)obj;
    
    return ptr->name;
}

int smmObj_getNodeType(int node_nr) // object�� type�� return 
{
	smmObject_t* ptr = (smmObject_t*)obj;
	
    return ptr->type;
}

int smmObj_getNodeCredit(int node_nr) // object�� credit�� return 
{
	smmObject_t* ptr = (smmObject_t*)obj;
	
    return ptr->credit;
}

int smmObj_getNodeEnergy(int node_nr) // object�� energy�� return 
{
	smmObject_t* ptr = (smmObject_t*)obj;
	
    return ptr->energy;
}
