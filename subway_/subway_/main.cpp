#define _CRT_SECURE_NO_WARINGINS
#define HASHTABLE_SIZE 20
#define ID_SIZE 10
#define NAME_SIZE 20

#include <iostream>

using namespace std;

//주변 역의 정보를 저장할 구조체
typedef struct Station {
	char ID[ID_SIZE];	//역 ID
	float dist;		//거리
	struct Station *s_next;
}Station;


//역의 정보를 저장할 구조체
typedef struct hashNode {
	char ID[ID_SIZE];	//역 ID
	char name[NAME_SIZE];		//역 이름
	int num_hashNode; //해쉬 노드 개수
	int num_transfer; //주변 역 개수
	struct hashNode *next;
	struct Station *s_next;
}hashNode;

class Subway {
private:
	hashNode * hashTable[HASHTABLE_SIZE];	//해쉬 테이블 

public:
	Subway();
	void init_hashTable();
	void create_hashTable(char _ID[ID_SIZE], char _name[NAME_SIZE], int num_transfer);
	int hashFunction(char _ID[ID_SIZE]);
	void connect_Stations(char _ID[ID_SIZE], char connected_ID[ID_SIZE], float _dist);
	void count_hashNode();
};

/*
	생성자
	Subway data 입력
*/
Subway::Subway() {
	FILE *fp = fopen("subway_data.txt", "r");
	hashNode *temp;
	Station *temp2;
	int i; 
	int num_tokens;				//token의 개수
	int num_transfer;			//주변 역 개수
	int ID_index, dist_index;	//id, 거리의 인덱스
	float _dist;				//거리 저장 변수
	char line[100];				//한줄씩 읽을 변수
	char _ID[ID_SIZE], connected_ID[ID_SIZE];	//역ID, 주변역ID 저장 변수
	char _name[NAME_SIZE];
	char *ptr;		  //tokenize를 위한 ptr
	char *tokens[15]; //tokens을 저장할 포인터 배열

	init_hashTable();	//hashTable 초기화

	//read file
	while (fgets(line, 100, fp) != NULL)
	{
		//tokenize
		/*
			token[0] : 역 ID
			token[1] : 역 이름
			token[2] : 연결된 역 개수
			...
			num_tokens  : data가 A B C D E 이면 num_tokens는 5
		*/
		ptr = strtok(line, " \t\0\n");
		i = 0;
		num_tokens = -1;
		tokens[0] = ptr;
		do {
			if(ptr)
				tokens[i++] = ptr;
			num_tokens++;
		} while (ptr = strtok(NULL, " \t\0\n"));


		strcpy(_ID, tokens[0]);	//_ID 저장
		strcpy(_name, tokens[1]);	//역 이름 저장
		num_transfer = atoi(tokens[2]);	//주변 역 개수 저장
		
		//hashTable 생성
		//create_hashTable(_ID, _name, num_transfer);	//역 ID를 통해 hashTable 생성
		create_hashTable(_ID, _name, num_transfer);

		//주변 역 연결
		for (i = 0; i < num_transfer; i++) {
			ID_index = 3 + i*2;	
			dist_index = 4 + i*2;

			strcpy(connected_ID, tokens[ID_index]);
			_dist = atof(tokens[dist_index]);
			
			connect_Stations(_ID, connected_ID, _dist);
		}
		

	}

	//hashTable 출력
	/*
	for (i = 0; i < HASHTABLE_SIZE; i++) {
		temp = hashTable[i]->next;
		while (temp) {
			cout << "[" <<temp->ID << "]" << " ";
			if (temp) cout << "->";
			temp = temp->next;
		}
		cout << "\n";
	}
	*/

	//역과 그 역의 근접 station 출력
	for (i = 0; i < HASHTABLE_SIZE; i++) {
		temp = hashTable[i]->next;
		while (temp) {
			cout << "역ID / 이름:[" << temp->ID << "/" << temp->name << "] :\t\t" 
				 << "주변역 " << temp->num_transfer << "개 :";
			if (temp->s_next) {
				temp2 = temp->s_next;
				while (temp2) {
					cout << "[" << temp2->ID << "]";
					temp2 = temp2->s_next;
					if (temp2) cout << "->";
				}	
			}
			temp = temp->next;
			cout << "\n";
		}
		
	}

	count_hashNode();	//해쉬노드 개수를 구함
}



/*
	hashTable 초기화 함수
*/
void Subway::init_hashTable() {
	for (int i = 0; i < HASHTABLE_SIZE; i++) {
		hashTable[i] = (hashNode*)malloc(sizeof(hashNode));
		hashTable[i]->next = NULL;
		hashTable[i]->s_next = NULL;
	}
}



/*
	hashTable을 생성하는 함수
	맨앞의 노드 (ex. hashtable[hashIndex])에는 오직 num_hashNode의 정보만 가지며
	hashTable[hashIndex]->next 부터 역의 ID를 저장한다.
*/
void Subway::create_hashTable(char _ID[ID_SIZE], char _name[NAME_SIZE], int num_transfer) {
	int hashIndex = hashFunction(_ID);
	
	hashNode *newNode;
	hashNode *temp;

	//newNode 할당, 초기화
	newNode = (hashNode*)malloc(sizeof(hashNode));
	strcpy(newNode->ID, _ID);
	strcpy(newNode->name, _name);
	newNode->num_transfer = num_transfer;
	newNode->next = NULL;
	newNode->s_next = NULL;

	
	if (this->hashTable[hashIndex]->next == NULL)	//비어있으면 맨앞에 연결 
		this->hashTable[hashIndex]->next = newNode;
	else {											//아니면 마지막 노드의 뒤에 연결
		temp = this->hashTable[hashIndex]->next;
		while (temp->next)
			temp = temp->next;
		temp->next = newNode;
	}

	
}

/*	
	역의 ID를 받아서 hashing한 인덱스를 리턴
*/
int Subway::hashFunction(char _ID[ID_SIZE]) {
	int i, length, sum = 0;
	length = strlen(_ID);
	for (i = 0; i < length; i++) 
		sum += _ID[i];
	return sum % HASHTABLE_SIZE;
}

/*
	hashTable에 저장된 역에 대해서
	Station구조체를 이용하여 주변 역을 연결한다.
*/
void Subway::connect_Stations(char _ID[ID_SIZE], char connected_ID[ID_SIZE],float _dist) {
	int hashIndex = hashFunction(_ID);

	hashNode *temp;
	Station *newNode, *temp2;

	//newNode 생성 및 정보입력
	newNode = (Station*)malloc(sizeof(Station));
	strcpy(newNode->ID, connected_ID);
	newNode->dist = _dist;
	newNode->s_next = NULL;

	//기준이 되는 역의 정보를 hashtable에서 찾는다.(temp가 가리키게 한다.)
	temp = this->hashTable[hashIndex]->next;
	while (temp) {
		if (!strcmp(temp->ID,_ID))
			break;
		temp = temp->next;
	}

	//주변역의 정보가 없으면 앞에 연결
	if (temp->s_next == NULL) {
		temp->s_next = newNode;
	}

	//있으면 그 뒤에 연결
	else {
		temp2 = temp->s_next;
		while (temp2->s_next)
			temp2 = temp2->s_next;
		temp2->s_next = newNode;
	}
	
}

/*
	hashTable에서 맨 처음 노드(hashTable[hashIndex])에 저장되는
	hashNode의 개수를 구한다.
	해당 인덱스에 몇개의 노드가 붙어있는지 알 수 있다.
*/
void Subway::count_hashNode() {
	int i;
	int count = 0;
	hashNode *temp;

	for (i = 0; i < HASHTABLE_SIZE; i++) {
		count = 0;
		temp = hashTable[i]->next;
		while (temp) {
			count++;
			temp = temp->next;
		}
		hashTable[i]->num_hashNode = count;
	}
}

int main() {
	Subway A;
	printf("test\n");

	return 0;
}