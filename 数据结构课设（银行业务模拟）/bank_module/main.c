#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include<Windows.h>
typedef int Status;
#define OK 1
#define ERROR -1

/*#define MAXTIME 600			//定义银行运作时间（单位：分钟 假设一分钟等于一秒）
#define MAX_TIMEJIANGE 5		//定义最大时间间隔
#define MAX_TIMEJIAOYI 100		//定义最大操作时间  */

typedef struct kehu_event{     
	//   定义客户信息
	int handel_time;   // 事件处理时间
	int money;		//需要处理的钱数目(负数表示取钱，正数表示存钱）
	int No;			//客户的序号
}ElemType;

typedef struct event{
	/*定义事件类型*/
	int index;		/*标志变量，1代表到达事件，其余代表离开事件*/
	int time;		/*事件发生时间*/
	int No;			/*触发该事件的客户序号*/
}event;

typedef struct {	/*定义链表节点*/
	event data;
	struct LNode *next;
}LNode, *Link;

typedef struct {    /*定义链表*/
	Link head, tail;
	int len;
}*LinkList;

LinkList InitLink() {    /*初始化链表操作*/
	LinkList L = (LinkList*)malloc(sizeof(LNode));
	if (L == NULL)   return NULL;
	L->head = NULL;
	L->tail = NULL;
	L->len = 0;
	printf("链表初始化成功！！");
	return L;
};

Status ClearLink(LinkList L) {    /*链表的清空操作*/
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
	printf("链表已经清空！！");
	return OK;
}

Status DestoryLink(LinkList L) {     /*链表的销毁操作*/
	if (L == NULL)   return ERROR;
	ClearLink(L);
	free(L);
	printf("链表销毁完毕！！");
	return OK;
}


Status InsertLink(LinkList L, event e) {    /*链表的尾部插入操作*/
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
	//printf("节点插入成功！！");
	return OK;
}

Status LinkTraverse(LinkList L) {			/*链表的遍历操作*/
	if (L == NULL)  return ERROR;
	Link p = L->head;
	printf("\n遍历结果 :\n");
	printf("     客户序号：         事件触发时间：             事件类型：   ");
	printf("\n------------------------------------------------------------");
	while (p != NULL) {
		printf("\n	     %d			%d", p->data.No, p->data.time);
		if (p->data.index == 1) {
			printf("			到达");
			printf("\n--------------------------------------------------------------");
		}
		else {
			printf("			离开");
			printf("\n-------------------------------------------------------------");
		}
		p = p->next;
	}
	return OK;
}




typedef struct LQNode {				/*定义队列的节点*/
	ElemType data;
	struct LQNode *next;
}LQNode, *QueuePtr;


typedef struct {					/*定义队列*/
	QueuePtr front;
	QueuePtr rear;
	int len;
}LQueue;

LQueue* InitQueue() {				/*构造一个空队列*/
	LQueue *Q = (LQueue *)malloc(sizeof(LQueue));
	if (Q == NULL) return ERROR;
	Q->front = NULL;
	Q->rear = NULL;
	Q->len = 0;
	return Q;
}

Status EnQueue(LQueue *Q, ElemType e) {			//在队列的队尾插入e
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
	//是队列Q的队头出队并且返回到e
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
	/*队列的销毁操作*/
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
	/*将E队列插入到Q队列的前面，并将Q队列作为新的队列释放E*/
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


/*-------------------------------------定义需要使用的全局变量--------------------------------------*/

int total ;   							/*一天营业开始时银行拥有的款额为10000(元),并且默认取钱只能*/
//int total_time = 600;					/*营业时间为600(分钟)*/
int max_money;
LQueue *handel_queue = NULL;			/*定义第一个队列用于处理业务*/
LQueue *wait_queue = NULL;				/* 定义第二个队列用于存储等候的客户*/
LinkList event_link = NULL;				/*定义一个事件表*/
int nextTime = 0;						/*定义一个变量作为下一个客户的随机到达时间*/
int nexthandelTime = 0;					/*定义一个变量作为正在排队的下一个客户处理时间*/
int kehu_NO = 1;						/*客户的服务序号*/
ElemType *leave_kehu = NULL;			/*定义一个办理完业务要离开的客户变量，用于记录*/
ElemType *search_kehu = NULL;			/*定义一个用来存储待处理业务的客户变量*/
/*-------------------------------------------------------------------------------------------------*/

ElemType* mymalloc( int max_timejiaoyi) {
	/*动态申请客户节点*/
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
	/*动态释放客户节点*/
	if (e == NULL)	return ERROR;
	free(e);
	return OK;
}
/*
Status suiji_kehu(ElemType *e,int max_timejiaoyi) {
/*随机生成客户信息
mymalloc(e, max_timejiaoyi);
return OK;
}
*/


ElemType* Arrial_event(int i, int max_timejiange, int max_timejiaoyi) {
	/*用来处理新客户到达事件*/
	if (nextTime == i) {
		printf("当前时间是:%d\n", i);
		nextTime = nextTime + (rand() % max_timejiange) + 1;			//随机生成下一名客户到达时间，这里可以设置客户到达时间间隔的大小
		ElemType *p=NULL;
		p = mymalloc(max_timejiaoyi);									//到达前面设置的到达时间时，随机生成刚到达用户的信息，包括客户编号，业务处理时间，业务操作钱的数目

		event *e = (event*)malloc(sizeof(event));		//生成该到达事件的信息节点
		e->index = 1;
		e->No = p->No;
		e->time = i;										
		InsertLink(event_link, *e);						//将该到达事件的信息节点加入事件链表

		if (nexthandelTime < i) {						//如果当前时间已经超过了下一个用户操作时间，则将当前时间和该到达用户的时间相加作为下一个用户操作时间
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
			if (nexthandelTime == 0) {//第一个进入操作的用户情况
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
		if (nexthandelTime == 0  ) {//第一个进入操作的用户情况
			if (-(p->money) <= total) {
				nexthandelTime = p->handel_time + i;
				total = total + p->money;
				EnQueue(handel_queue, *p);
			}
			else {
				EnQueue(wait_queue, *p);
			}

		}
		printf("\n 客户编号：%d 客户操作需要时间：%d  客户需要处理的钱：%d  银行余额：%d\n\n\n", p->No, p->handel_time, p->money, total);
		return p;
	}
}

Status handel_event(int i) {
	/*处理队列事件*/
	if (nexthandelTime == i) {			/*到达下一个客户操作时间时*/

		DeQueue(handel_queue, leave_kehu);				//将处理完的客户出队，并将客户信息存储在leave_kehu变量中
		int last_money = total - leave_kehu->money;		//获取刚离开的客户操作前银行的钱数目
		Leave_event(leave_kehu, i);						//将离开事件的用户信息存入事件列表中
		if (((leave_kehu->money) > 0) && wait_queue->front != NULL) {		
			/*如果刚刚离开的用户是存钱或者还钱，并且 等待队列 不为空*/
			int index = 0;								//设置标记变量，用以标记等待队列已经查看的客户数目
			LQueue *e = InitQueue();					//创造出一个可以被满足需求的等待用户组成的队列
			int linshi_total = total;					//设置一个临时金额存储空间
			while (linshi_total > last_money && wait_queue->front != NULL) {				//如果当前银行拥有的款额比之前多，进入循环
				DeQueue(wait_queue, search_kehu);		//将等待队列的队头用户出队
				if (-(search_kehu->money) <= linshi_total) {	
					/*如果等待用户的需求可以得到满足*/
					EnQueue(e, *search_kehu);			//将可满足用户加入等待用户队列
					linshi_total = linshi_total + search_kehu->money;
				}
				else {
					EnQueue(wait_queue, *search_kehu);
				}
				index++;
				if (index >= wait_queue->len)	break;
			}
			InsertQueue(handel_queue, e);				//将可以满足要求的等待队列并入前面
			//nexthandelTime = nexthandelTime + handel_queue->front->data.handel_time;	//下一个处理用户时间更新

			if (handel_queue->front != NULL) {	
				/*如果操作队列不为空*/
				while(handel_queue->front != NULL  && -(handel_queue->front->data.money) > total)
					/*如果无法满足要求*/
				{ 
					DeQueue(handel_queue, leave_kehu);
					EnQueue(wait_queue, *leave_kehu);
				}
				if (handel_queue->front != NULL) {
					nexthandelTime = handel_queue->front->data.handel_time + nexthandelTime;	//下一位操作客户时间更新
					total = total + handel_queue->front->data.money;			//银行金钱总额更新
				}
			}
		}else {
			/*刚刚离开的用户是取钱或者借钱，或者 等待队列 为空*/
			if (handel_queue->front != NULL) {
				while (handel_queue->front != NULL  && -(handel_queue->front->data.money) > total  )
				{
					/*无法满足的用户进入等待队列*/
					DeQueue(handel_queue, leave_kehu);
					EnQueue(wait_queue, *leave_kehu);
				}
				if (handel_queue->front != NULL) {
					/*处理队列不为空*/
					nexthandelTime = handel_queue->front->data.handel_time + nexthandelTime;
					total = total + handel_queue->front->data.money;
				}

			}
		}
	}
}


Status Leave_event(ElemType *e ,int i) {
	/*用来处理客户离开事件*/
	event *p = (event*)malloc(sizeof(event));
	p->index = 0;
	p->No = e->No;
	p->time = i;
	InsertLink(event_link, *p);
	return OK;
}

double average_StayTime(LinkList e,double final) {
	/*通过事件列表来计算用户平均逗留时间*/
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
	printf("全部用户数: %d \n总共消耗时间: %f \n", shumu, sum);
	printf("客户平均逗留时间： %f 分", sum / shumu);
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
	printf("请输入银行的运营时间（单位：分钟）：");
	scanf_s("%d", &MAXTIME);
	printf("请输入银行的运营金额（单位：元）：");
	scanf_s("%d", &total);
	printf("请输入用户最大交易金额（单位：元）：");
	scanf_s("%d", &max_money);
	printf("请输入俩个到达事件的最大时间间隔：（单位：分钟）：");
	scanf_s("%d", &MAX_TIMEJIANGE);
	printf("请输入用户最大交易时间：（单位：分钟）：");
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