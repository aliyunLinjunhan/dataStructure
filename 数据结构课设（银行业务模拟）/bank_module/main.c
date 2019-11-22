#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include<Windows.h>
typedef int Status;
#define OK 1
#define ERROR -1

/*#define MAXTIME 600			//������������ʱ�䣨��λ������ ����һ���ӵ���һ�룩
#define MAX_TIMEJIANGE 5		//�������ʱ����
#define MAX_TIMEJIAOYI 100		//����������ʱ��  */

typedef struct kehu_event{     
	//   ����ͻ���Ϣ
	int handel_time;   // �¼�����ʱ��
	int money;		//��Ҫ�����Ǯ��Ŀ(������ʾȡǮ��������ʾ��Ǯ��
	int No;			//�ͻ������
}ElemType;

typedef struct event{
	/*�����¼�����*/
	int index;		/*��־������1�������¼�����������뿪�¼�*/
	int time;		/*�¼�����ʱ��*/
	int No;			/*�������¼��Ŀͻ����*/
}event;

typedef struct {	/*��������ڵ�*/
	event data;
	struct LNode *next;
}LNode, *Link;

typedef struct {    /*��������*/
	Link head, tail;
	int len;
}*LinkList;

LinkList InitLink() {    /*��ʼ���������*/
	LinkList L = (LinkList*)malloc(sizeof(LNode));
	if (L == NULL)   return NULL;
	L->head = NULL;
	L->tail = NULL;
	L->len = 0;
	printf("�����ʼ���ɹ�����");
	return L;
};

Status ClearLink(LinkList L) {    /*�������ղ���*/
	if (L == NULL)  return ERROR;
	Link p = L->head;
	Link k = L->head->next;
	while (p != L->tail)
	{
		free(p);
		p = k;
		if(k!=L->tail)   k = k->next;
	}
	free(p);
	L->head = NULL;
	L->tail = NULL;
	L->len = 0;
	printf("�����Ѿ���գ���");
	return OK;
}

Status DestoryLink(LinkList L) {     /*��������ٲ���*/
	if (L == NULL)   return ERROR;
	ClearLink(L);
	free(L);
	printf("����������ϣ���");
	return OK;
}


Status InsertLink(LinkList L, event e) {    /*�����β���������*/
	if (L == NULL)   return ERROR;
	Link p = (Link)malloc(sizeof(LNode));
	if (p == NULL)  return ERROR;
	p->data = e;
	p->next = NULL;
	if (L->head == NULL) {
		L->head = p;
		L->tail = p;
		L->len = 1;
	}else{
		L->tail->next = p;
		L->tail = p;
		L->len++;
	}
	//printf("�ڵ����ɹ�����");
	return OK;
}

Status LinkTraverse(LinkList L) {			/*����ı�������*/
	if (L == NULL)  return ERROR;
	Link p = L->head;
	printf("\n������� :\n");
	printf("     �ͻ���ţ�         �¼�����ʱ�䣺             �¼����ͣ�   ");
	printf("\n------------------------------------------------------------");
	while (p != NULL) {
		printf("\n	     %d			%d", p->data.No, p->data.time);
		if (p->data.index == 1) {
			printf("			����");
			printf("\n--------------------------------------------------------------");
		}
		else {
			printf("			�뿪");
			printf("\n-------------------------------------------------------------");
		}
		p = p->next;
	}
	return OK;
}




typedef struct LQNode {				/*������еĽڵ�*/
	ElemType data;
	struct LQNode *next;
}LQNode, *QueuePtr;


typedef struct {					/*�������*/
	QueuePtr front;
	QueuePtr rear;
	int len;
}LQueue;

LQueue* InitQueue() {				/*����һ���ն���*/
	LQueue *Q = (LQueue *)malloc(sizeof(LQueue));
	if (Q == NULL) return ERROR;
	Q->front = NULL;
	Q->rear = NULL;
	Q->len = 0;
	return Q;
}

Status EnQueue(LQueue *Q, ElemType e) {			//�ڶ��еĶ�β����e
	LQNode *p;
	p = (LQNode*)malloc(sizeof(LQNode));
	if (p == NULL)  return ERROR;
	p->data = e;
	p->next = NULL;
	if ( NULL == Q->front) {
		Q->front = p;
		Q->rear = p;
		Q->len = 1;
	}
	else
	{
		Q->rear->next = p;
		Q->rear = p;
		Q->len++;
	}
	return OK;
}

Status DeQueue(LQueue *Q, ElemType *e) {
	//�Ƕ���Q�Ķ�ͷ���Ӳ��ҷ��ص�e
	LQNode *p;
	if (NULL == Q->front) return ERROR;
	p = Q->front;
	*e = p->data;
	Q->front = p->next;
	if (Q->rear == p)  Q->rear = NULL;
	free(p);
	Q->len--;
	return OK;
}

Status DestoryQueue(LQueue *Q) {
	/*���е����ٲ���*/
	if (Q == NULL)	return ERROR;
	QueuePtr p=NULL, q=NULL;
	p = Q->front;



	while (p != Q->rear) {
		q = p->next;
		myfree(p);
		p = q;
	}
	free(Q->rear);
	return OK;
}


Status InsertQueue(LQueue *Q, LQueue *E) {
	/*��E���в��뵽Q���е�ǰ�棬����Q������Ϊ�µĶ����ͷ�E*/
	if ( E == NULL || E->front == NULL)	return ERROR;
	if (Q == NULL || Q->front == NULL || Q->rear == NULL) {
		Q->front = E->front;
		Q->rear = E->rear;
	}else
	{
		E->rear->next = Q->front;
		Q->front = E->front;
	}
	return OK;
}


/*-------------------------------------������Ҫʹ�õ�ȫ�ֱ���--------------------------------------*/

int total ;   							/*һ��Ӫҵ��ʼʱ����ӵ�еĿ��Ϊ10000(Ԫ),����Ĭ��ȡǮֻ��*/
//int total_time = 600;					/*Ӫҵʱ��Ϊ600(����)*/
int max_money;
LQueue *handel_queue = NULL;			/*�����һ���������ڴ���ҵ��*/
LQueue *wait_queue = NULL;				/* ����ڶ����������ڴ洢�Ⱥ�Ŀͻ�*/
LinkList event_link = NULL;				/*����һ���¼���*/
int nextTime = 0;						/*����һ��������Ϊ��һ���ͻ����������ʱ��*/
int nexthandelTime = 0;					/*����һ��������Ϊ�����Ŷӵ���һ���ͻ�����ʱ��*/
int kehu_NO = 1;						/*�ͻ��ķ������*/
ElemType *leave_kehu = NULL;			/*����һ��������ҵ��Ҫ�뿪�Ŀͻ����������ڼ�¼*/
ElemType *search_kehu = NULL;			/*����һ�������洢������ҵ��Ŀͻ�����*/
/*-------------------------------------------------------------------------------------------------*/

ElemType* mymalloc( int max_timejiaoyi) {
	/*��̬����ͻ��ڵ�*/
	ElemType *e;
	e = (ElemType*)malloc(sizeof(ElemType));
	if (e == NULL)		return ;
	e->handel_time = (rand() % max_timejiaoyi) + 1;
	e->money = (rand() % (2 * max_money)) - max_money;
	e->No = kehu_NO;
	kehu_NO++;
	return e;
}

Status myfree(ElemType *e) {
	/*��̬�ͷſͻ��ڵ�*/
	if (e == NULL)	return ERROR;
	free(e);
	return OK;
}
/*
Status suiji_kehu(ElemType *e,int max_timejiaoyi) {
/*������ɿͻ���Ϣ
mymalloc(e, max_timejiaoyi);
return OK;
}
*/


ElemType* Arrial_event(int i, int max_timejiange, int max_timejiaoyi) {
	/*���������¿ͻ������¼�*/
	if (nextTime == i) {
		printf("��ǰʱ����:%d\n", i);
		nextTime = nextTime + (rand() % max_timejiange) + 1;			//���������һ���ͻ�����ʱ�䣬����������ÿͻ�����ʱ�����Ĵ�С
		ElemType *p=NULL;
		p = mymalloc(max_timejiaoyi);									//����ǰ�����õĵ���ʱ��ʱ��������ɸյ����û�����Ϣ�������ͻ���ţ�ҵ����ʱ�䣬ҵ�����Ǯ����Ŀ

		event *e = (event*)malloc(sizeof(event));		//���ɸõ����¼�����Ϣ�ڵ�
		e->index = 1;
		e->No = p->No;
		e->time = i;										
		InsertLink(event_link, *e);						//���õ����¼�����Ϣ�ڵ�����¼�����

		if (nexthandelTime < i) {						//�����ǰʱ���Ѿ���������һ���û�����ʱ�䣬�򽫵�ǰʱ��͸õ����û���ʱ�������Ϊ��һ���û�����ʱ��
			if (-(p->money) <= total) {
				nexthandelTime = p->handel_time + i;
				total = total + p->money;
				EnQueue(handel_queue, *p);
			}
			else {
				EnQueue(wait_queue, *p);
			}
		}
		else {
			if (nexthandelTime == 0) {//��һ������������û����
				if (-(p->money) <= total) {
					nexthandelTime = p->handel_time + i;
					total = total + p->money;
					EnQueue(handel_queue, *p);
				}
				else {
					EnQueue(wait_queue, *p);
				}

			}else 					EnQueue(handel_queue, *p);
		}
		if (nexthandelTime == 0  ) {//��һ������������û����
			if (-(p->money) <= total) {
				nexthandelTime = p->handel_time + i;
				total = total + p->money;
				EnQueue(handel_queue, *p);
			}
			else {
				EnQueue(wait_queue, *p);
			}

		}
		printf("\n �ͻ���ţ�%d �ͻ�������Ҫʱ�䣺%d  �ͻ���Ҫ�����Ǯ��%d  ������%d\n\n\n", p->No, p->handel_time, p->money, total);
		return p;
	}
}

Status handel_event(int i) {
	/*��������¼�*/
	if (nexthandelTime == i) {			/*������һ���ͻ�����ʱ��ʱ*/

		DeQueue(handel_queue, leave_kehu);				//��������Ŀͻ����ӣ������ͻ���Ϣ�洢��leave_kehu������
		int last_money = total - leave_kehu->money;		//��ȡ���뿪�Ŀͻ�����ǰ���е�Ǯ��Ŀ
		Leave_event(leave_kehu, i);						//���뿪�¼����û���Ϣ�����¼��б���
		if (((leave_kehu->money) > 0) && wait_queue->front != NULL) {		
			/*����ո��뿪���û��Ǵ�Ǯ���߻�Ǯ������ �ȴ����� ��Ϊ��*/
			int index = 0;								//���ñ�Ǳ��������Ա�ǵȴ������Ѿ��鿴�Ŀͻ���Ŀ
			LQueue *e = InitQueue();					//�����һ�����Ա���������ĵȴ��û���ɵĶ���
			int linshi_total = total;					//����һ����ʱ���洢�ռ�
			while (linshi_total > last_money && wait_queue->front != NULL) {				//�����ǰ����ӵ�еĿ���֮ǰ�࣬����ѭ��
				DeQueue(wait_queue, search_kehu);		//���ȴ����еĶ�ͷ�û�����
				if (-(search_kehu->money) <= linshi_total) {	
					/*����ȴ��û���������Եõ�����*/
					EnQueue(e, *search_kehu);			//���������û�����ȴ��û�����
					linshi_total = linshi_total + search_kehu->money;
				}
				else {
					EnQueue(wait_queue, *search_kehu);
				}
				index++;
				if (index >= wait_queue->len)	break;
			}
			InsertQueue(handel_queue, e);				//����������Ҫ��ĵȴ����в���ǰ��
			//nexthandelTime = nexthandelTime + handel_queue->front->data.handel_time;	//��һ�������û�ʱ�����

			if (handel_queue->front != NULL) {	
				/*����������в�Ϊ��*/
				while(handel_queue->front != NULL  && -(handel_queue->front->data.money) > total)
					/*����޷�����Ҫ��*/
				{ 
					DeQueue(handel_queue, leave_kehu);
					EnQueue(wait_queue, *leave_kehu);
				}
				if (handel_queue->front != NULL) {
					nexthandelTime = handel_queue->front->data.handel_time + nexthandelTime;	//��һλ�����ͻ�ʱ�����
					total = total + handel_queue->front->data.money;			//���н�Ǯ�ܶ����
				}
			}
		}else {
			/*�ո��뿪���û���ȡǮ���߽�Ǯ������ �ȴ����� Ϊ��*/
			if (handel_queue->front != NULL) {
				while (handel_queue->front != NULL  && -(handel_queue->front->data.money) > total  )
				{
					/*�޷�������û�����ȴ�����*/
					DeQueue(handel_queue, leave_kehu);
					EnQueue(wait_queue, *leave_kehu);
				}
				if (handel_queue->front != NULL) {
					/*������в�Ϊ��*/
					nexthandelTime = handel_queue->front->data.handel_time + nexthandelTime;
					total = total + handel_queue->front->data.money;
				}

			}
		}
	}
}


Status Leave_event(ElemType *e ,int i) {
	/*��������ͻ��뿪�¼�*/
	event *p = (event*)malloc(sizeof(event));
	p->index = 0;
	p->No = e->No;
	p->time = i;
	InsertLink(event_link, *p);
	return OK;
}

double average_StayTime(LinkList e,double final) {
	/*ͨ���¼��б��������û�ƽ������ʱ��*/
	double sum=0, a, b;
	int shumu=0;
	LNode *p = e->head;
	for (int i = 1; i <= e->len; i++) {
		while(p!=NULL) {
			if (p->data.No == i) {
				if (p->data.index == 1) {
					a = p->data.time;
					shumu = shumu + 1;
				}
				if (p->data.index == 0) b = p->data.time;
				
			}
			p = p->next;
		}
		p = e->head;
		if (b == -1.0) b = final;
		sum = sum + b - a;
		b = -1.0;
	}
	printf("ȫ���û���: %d \n�ܹ�����ʱ��: %f \n", shumu, sum);
	printf("�ͻ�ƽ������ʱ�䣺 %f ��", sum / shumu);
	return sum / shumu;

}

int main(void) {
	leave_kehu = (ElemType*)malloc(sizeof(ElemType));
	search_kehu = (ElemType*)malloc(sizeof(ElemType));
	handel_queue = InitQueue();
	wait_queue = InitQueue();
	event_link = InitLink();
	srand((unsigned int)time(NULL));
	int MAXTIME, MAX_TIMEJIANGE, MAX_TIMEJIAOYI;
	printf("���������е���Ӫʱ�䣨��λ�����ӣ���");
	scanf_s("%d", &MAXTIME);
	printf("���������е���Ӫ����λ��Ԫ����");
	scanf_s("%d", &total);
	printf("�������û�����׽���λ��Ԫ����");
	scanf_s("%d", &max_money);
	printf("���������������¼������ʱ����������λ�����ӣ���");
	scanf_s("%d", &MAX_TIMEJIANGE);
	printf("�������û������ʱ�䣺����λ�����ӣ���");
	scanf_s("%d", &MAX_TIMEJIAOYI);
	//ElemType *newKehu = (ElemType*)malloc(sizeof(ElemType));
	for (int i = 0; i < MAXTIME; i++)
	{	
		
		Arrial_event(i, MAX_TIMEJIANGE, MAX_TIMEJIAOYI);
		handel_event(i);
		Sleep(50);
	}
	LinkTraverse(event_link);
	printf("\n");
	average_StayTime(event_link, MAXTIME-1);
	DestoryQueue(handel_queue);
	DestoryQueue(wait_queue);
	system("pause");
	return 1;
}