#define _CRT_SECURE_NO_WARINGINS
#define HASHTABLE_SIZE 20
#define ID_SIZE 10
#define NAME_SIZE 20

#include <iostream>

using namespace std;

//�ֺ� ���� ������ ������ ����ü
typedef struct Station {
	char ID[ID_SIZE];	//�� ID
	float dist;		//�Ÿ�
	struct Station *s_next;
}Station;


//���� ������ ������ ����ü
typedef struct hashNode {
	char ID[ID_SIZE];	//�� ID
	char name[NAME_SIZE];		//�� �̸�
	int num_hashNode; //�ؽ� ��� ����
	int num_transfer; //�ֺ� �� ����
	struct hashNode *next;
	struct Station *s_next;
}hashNode;

class Subway {
private:
	hashNode * hashTable[HASHTABLE_SIZE];	//�ؽ� ���̺� 

public:
	Subway();
	void init_hashTable();
	void create_hashTable(char _ID[ID_SIZE], char _name[NAME_SIZE], int num_transfer);
	int hashFunction(char _ID[ID_SIZE]);
	void connect_Stations(char _ID[ID_SIZE], char connected_ID[ID_SIZE], float _dist);
	void count_hashNode();
};

/*
	������
	Subway data �Է�
*/
Subway::Subway() {
	FILE *fp = fopen("subway_data.txt", "r");
	hashNode *temp;
	Station *temp2;
	int i; 
	int num_tokens;				//token�� ����
	int num_transfer;			//�ֺ� �� ����
	int ID_index, dist_index;	//id, �Ÿ��� �ε���
	float _dist;				//�Ÿ� ���� ����
	char line[100];				//���پ� ���� ����
	char _ID[ID_SIZE], connected_ID[ID_SIZE];	//��ID, �ֺ���ID ���� ����
	char _name[NAME_SIZE];
	char *ptr;		  //tokenize�� ���� ptr
	char *tokens[15]; //tokens�� ������ ������ �迭

	init_hashTable();	//hashTable �ʱ�ȭ

	//read file
	while (fgets(line, 100, fp) != NULL)
	{
		//tokenize
		/*
			token[0] : �� ID
			token[1] : �� �̸�
			token[2] : ����� �� ����
			...
			num_tokens  : data�� A B C D E �̸� num_tokens�� 5
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


		strcpy(_ID, tokens[0]);	//_ID ����
		strcpy(_name, tokens[1]);	//�� �̸� ����
		num_transfer = atoi(tokens[2]);	//�ֺ� �� ���� ����
		
		//hashTable ����
		//create_hashTable(_ID, _name, num_transfer);	//�� ID�� ���� hashTable ����
		create_hashTable(_ID, _name, num_transfer);

		//�ֺ� �� ����
		for (i = 0; i < num_transfer; i++) {
			ID_index = 3 + i*2;	
			dist_index = 4 + i*2;

			strcpy(connected_ID, tokens[ID_index]);
			_dist = atof(tokens[dist_index]);
			
			connect_Stations(_ID, connected_ID, _dist);
		}
		

	}

	//hashTable ���
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

	//���� �� ���� ���� station ���
	for (i = 0; i < HASHTABLE_SIZE; i++) {
		temp = hashTable[i]->next;
		while (temp) {
			cout << "��ID / �̸�:[" << temp->ID << "/" << temp->name << "] :\t\t" 
				 << "�ֺ��� " << temp->num_transfer << "�� :";
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

	count_hashNode();	//�ؽ���� ������ ����
}



/*
	hashTable �ʱ�ȭ �Լ�
*/
void Subway::init_hashTable() {
	for (int i = 0; i < HASHTABLE_SIZE; i++) {
		hashTable[i] = (hashNode*)malloc(sizeof(hashNode));
		hashTable[i]->next = NULL;
		hashTable[i]->s_next = NULL;
	}
}



/*
	hashTable�� �����ϴ� �Լ�
	�Ǿ��� ��� (ex. hashtable[hashIndex])���� ���� num_hashNode�� ������ ������
	hashTable[hashIndex]->next ���� ���� ID�� �����Ѵ�.
*/
void Subway::create_hashTable(char _ID[ID_SIZE], char _name[NAME_SIZE], int num_transfer) {
	int hashIndex = hashFunction(_ID);
	
	hashNode *newNode;
	hashNode *temp;

	//newNode �Ҵ�, �ʱ�ȭ
	newNode = (hashNode*)malloc(sizeof(hashNode));
	strcpy(newNode->ID, _ID);
	strcpy(newNode->name, _name);
	newNode->num_transfer = num_transfer;
	newNode->next = NULL;
	newNode->s_next = NULL;

	
	if (this->hashTable[hashIndex]->next == NULL)	//��������� �Ǿտ� ���� 
		this->hashTable[hashIndex]->next = newNode;
	else {											//�ƴϸ� ������ ����� �ڿ� ����
		temp = this->hashTable[hashIndex]->next;
		while (temp->next)
			temp = temp->next;
		temp->next = newNode;
	}

	
}

/*	
	���� ID�� �޾Ƽ� hashing�� �ε����� ����
*/
int Subway::hashFunction(char _ID[ID_SIZE]) {
	int i, length, sum = 0;
	length = strlen(_ID);
	for (i = 0; i < length; i++) 
		sum += _ID[i];
	return sum % HASHTABLE_SIZE;
}

/*
	hashTable�� ����� ���� ���ؼ�
	Station����ü�� �̿��Ͽ� �ֺ� ���� �����Ѵ�.
*/
void Subway::connect_Stations(char _ID[ID_SIZE], char connected_ID[ID_SIZE],float _dist) {
	int hashIndex = hashFunction(_ID);

	hashNode *temp;
	Station *newNode, *temp2;

	//newNode ���� �� �����Է�
	newNode = (Station*)malloc(sizeof(Station));
	strcpy(newNode->ID, connected_ID);
	newNode->dist = _dist;
	newNode->s_next = NULL;

	//������ �Ǵ� ���� ������ hashtable���� ã�´�.(temp�� ����Ű�� �Ѵ�.)
	temp = this->hashTable[hashIndex]->next;
	while (temp) {
		if (!strcmp(temp->ID,_ID))
			break;
		temp = temp->next;
	}

	//�ֺ����� ������ ������ �տ� ����
	if (temp->s_next == NULL) {
		temp->s_next = newNode;
	}

	//������ �� �ڿ� ����
	else {
		temp2 = temp->s_next;
		while (temp2->s_next)
			temp2 = temp2->s_next;
		temp2->s_next = newNode;
	}
	
}

/*
	hashTable���� �� ó�� ���(hashTable[hashIndex])�� ����Ǵ�
	hashNode�� ������ ���Ѵ�.
	�ش� �ε����� ��� ��尡 �پ��ִ��� �� �� �ִ�.
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