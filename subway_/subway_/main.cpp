#define _USE_CRT_NO_WARINGINS
#define HASHTABLE_SIZE 20

#include <iostream>

using namespace std;


typedef struct Station {
	char ID[10];	//역 ID
	float dist;		//거리
	struct Station *s_next;
}Station;

typedef struct hashNode {
	char ID[10];	//역 ID
//	char name[15];		//역 이름
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
	void create_hashTable(char _ID[10], int num_transfer);
	int hashFunction(char _ID[10]);
	void connect_Stations(char _ID[10], char connected_ID[10], float _dist);
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
	int num_tokens; //token의 개수
	int num_transfer;	//주변 역 개수
	int ID_index, dist_index;	//id, 거리의 인덱스
	float _dist;
	char line[100];	//한줄씩 읽을 변수
	char _ID[10], connected_ID[10];
	char *ptr; //tokenize를 위한 ptr
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
		//strcpy(_name, tokens[1]);	//역 이름 저장
		num_transfer = atoi(tokens[2]);	//주변 역 개수 저장
		
		//hashTable 생성
		//create_hashTable(_ID, _name, num_transfer);	//역 ID를 통해 hashTable 생성
		create_hashTable(_ID, num_transfer);

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

	//근접 station 출력
	for (i = 0; i < HASHTABLE_SIZE; i++) {
		temp = hashTable[i]->next;
		while (temp) {
			cout << "[H" << temp->ID << "]";
			if (temp->s_next) {
				cout << "->";
				temp2 = temp->s_next;
				while (temp2) {
					cout << "[S" << temp2->ID << "]";
					temp2 = temp2->s_next;
					if (temp2) cout << "->";
				}	
			}
			temp = temp->next;
			cout << "\n";
		}
		
	}
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
void Subway::create_hashTable(char _ID[10], int num_transfer) {
	int hashIndex = hashFunction(_ID);
	
	hashNode *newNode;
	hashNode *temp;

	//newNode 할당, 초기화
	newNode = (hashNode*)malloc(sizeof(hashNode));
	strcpy(newNode->ID, _ID);
	//strcpy(newNode->name, _name);
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
int Subway::hashFunction(char _ID[10]) {
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
void Subway::connect_Stations(char _ID[10], char connected_ID[10],float _dist) {
	int hashIndex = hashFunction(_ID);

	hashNode *temp;
	Station *newNode, *temp2;

	newNode = (Station*)malloc(sizeof(Station));
	strcpy(newNode->ID, connected_ID);
	newNode->dist = _dist;
	newNode->s_next = NULL;

	temp = this->hashTable[hashIndex]->next;
	while (temp) {
		if (!strcmp(temp->ID,_ID))
			break;
		temp = temp->next;
	}

	if (temp->s_next == NULL) {
		temp->s_next = newNode;
	}
	else {
		temp2 = temp->s_next;
		while (temp2->s_next)
			temp2 = temp2->s_next;
		temp2->s_next = newNode;
	}
	
}

int main() {
	Subway A;
	printf("test\n");

	return 0;
}