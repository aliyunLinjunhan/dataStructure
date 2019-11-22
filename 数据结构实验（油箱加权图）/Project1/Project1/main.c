#include <stdio.h>
#include <stdlib.h>
#define OK 1
#define ERROR -1
#define OVERFLOW -1
typedef int Status;
#define UNVISITED 0
#define VISITED 1
#define INFINITY 65535   //ȡ�������������ֵ�������� 
typedef char VexType;

/*�����Ȩͼ���ڽ������ȡ�ṹ */
typedef struct {
	VexType *vexs;     /*�������飬VexType�Ƕ������ͣ����û�����*/
	int **arcs;        /*��ϵ���飬������Ȩͼ������ֵ��Ȩֵ����INFINITY*/
	int n, e; 		   /*���嶥����n�ͱ���e*/
	int *tags;         /*��־���飬��������ͼ�ı����б�Ƕ���������*/
}MGraph;

/*���ߵĴ洢�ṹ*/
typedef struct {
	VexType v, w;     //�߻��Ķ˵� 
	int info;       //�߻���Ȩֵ 
}ArcInfo;

/*������еĽڵ�*/
typedef struct LQNode {
	int data;
	struct LQNode *next;
}LQNode, *QueuePtr;

/*�������*/
typedef struct {
	QueuePtr front;
	QueuePtr rear;
}LQueue;


Status InitGraph(MGraph *G, VexType *vexs, int n);
//��ʼ������N���������ޱߵ�ͼ

Status CreatGraph(MGraph *G, VexType *vex, int n, ArcInfo *arcs, int e);
//����n�������e���ߵ�ͼ��vexsΪ������Ϣ��arcsΪ����Ϣ

int LocateVex(MGraph G, VexType v);
//������ȡ�����λ��

VexType GetVex(MGraph G, int k);   //ȡͼG��k�����ֵ��w

Status PutVex(MGraph G, int k, VexType w);    //��ͼG��k���㸳ֵw

int FirstAdjVex(MGraph G, int k);       //��ͼG��k����ĵ�һ���ڽӶ����λ��

int NextAdjVex(MGraph G, int k, int m);   //m����Ϊk���㵽m����ı߻��߻�����ͼ��K���������m�������һ���ڽӶ����λ��

Status AddArc(MGraph G, int k, int m, int info);   //��ͼG������k��m���ߣ���Ȩֵ

Status Remove(MGraph G, int k, int m);    //ɾ��ͼ��k��m����

Status DFS(MGraph G, int k);  //��ͨͼ��������ȱ���

Status DFSTraverse(MGraph G);  //����ͼ�����ȱ���

LQueue* InitQueue();    //����һ���ն���

Status EnQueue(LQueue *Q, int e);  	//�ڶ��еĶ�β����

Status DeQueue(LQueue *Q, int *e);  	//�Ƕ���Q�Ķ�ͷ���Ӳ��ҷ��ص�e

Status BFS(MGraph G, Status(*visit)(int));   //������ȱ��� 

Status InitGraph(MGraph *G, VexType *vexs, int n) {
	/*��ʼ������N���������ޱߵ�ͼ*/
	int i, j;
	if (n < 0 || (n > 0 && vexs == NULL))  return ERROR;
	G->n = n;
	G->e = 0;
	if (0 == n) return OK;
	if (NULL == (G->vexs = (VexType*)malloc(n * sizeof(VexType))))
		return OVERFLOW;
	for (i = 0; i < G->n; i++) G->vexs[i] = vexs[i];
	if (NULL == (G->arcs = (int**)malloc(n * sizeof(int*))))
		return OVERFLOW;
	for (i = 0; i < n; i++) {
		if (NULL == (G->arcs[i] = (int*)malloc(n * sizeof(int))))
			return OVERFLOW;
	}
	if (NULL == (G->tags = (int*)malloc(n * sizeof(int))))  return OVERFLOW;
	for (i = 0; i < n; i++) {
		G->tags[i] = UNVISITED;
		for (j = 0; j < n; j++) {
			G->arcs[i][j] = INFINITY;
		}
	}
	return OK;
}

Status CreatGraph(MGraph *G, VexType *vex, int n, ArcInfo *arcs, int e) {
	//����n�������e���ߵ�ͼ��vexsΪ������Ϣ��arcsΪ����Ϣ
	int i, j, k;
	if (InitGraph(G, vex, n) != OK)  return ERROR;
	G->e = e;
	for (k = 0; k < G->e; k++) {
		i = LocateVex(*G, arcs[k].v);
		j = LocateVex(*G, arcs[k].w);
		if (i < 0 || j < 0)  return ERROR;
		G->arcs[i][j] = arcs[k].info;
	}
	return OK;
}

int LocateVex(MGraph G, VexType v) {
	//���ڻ�ȡ�����λ��
	int i;
	for (i = 0; i < G.n; i++) {
		if (G.vexs[i] == v)  return i;
	}
	return ERROR;
}

VexType GetVex(MGraph G, int k) {
	//ȡͼG��k�����ֵ��w
	if (k<0 || k>G.e)          return;
	return  G.vexs[k];
}

Status PutVex(MGraph G, int k, VexType w) {
	//��ͼG��k���㸳ֵw
	if (k<0 || k>G.e)          return ERROR;
	G.vexs[k] = w;
	return  OK;
}

int FirstAdjVex(MGraph G, int k) {
	int i;
	if (k < 0 || k >= G.n)		return ERROR;
	for (i = 0; i <G.n; i++) {
		if (G.arcs[k][i] != INFINITY) return i;
	}
	return ERROR;
}

int NextAdjVex(MGraph G, int k, int m) {
	m = m + 1;
	// //m����Ϊk���㵽m����ı߻��߻�����ͼ��K���������m�������һ���ڽӶ����λ��
	if (k < 0 || k >= G.n || m<0 || m >= G.n || m <= k)		return ERROR;
	for (; m<G.n; m++)
	{
		if (G.arcs[k][m] != INFINITY) return m;
	}
	return ERROR;
}

Status AddArc(MGraph G, int k, int m, int info) {
	//��ͼG������k��m���ߣ���Ȩֵ
	if (k < 0 || k >= G.n || m < 0 || m >= G.n || m <= k)		return ERROR;
	if (G.arcs[k][m] == INFINITY)
	{
		G.arcs[k][m] = info;
	}
	else
		return ERROR;
}

Status Remove(MGraph G, int k, int m) {
	//ɾ��ͼ��k��m����
	if (k < 0 || k >= G.n || m < 0 || m >= G.n || m <= k)		return ERROR;
	if (G.arcs[k][m] != INFINITY)
	{
		G.arcs[k][m] = INFINITY;
	}
	else
		return ERROR;
}

Status DFS(MGraph G, int k) {
	//��ͨͼ��������ȱ���
	int i;
	if (k < 0 || k >= G.n) return ERROR;
	printf("%c", G.vexs[k]);
	G.tags[k] = VISITED;
	for (i = FirstAdjVex(G, k); i >= 0; i = NextAdjVex(G, k, i)) {
		if (UNVISITED == G.tags[i])
			if (ERROR == DFS(G, i))
				return ERROR;
	}
	return OK;
}

Status DFSTraverse(MGraph G) {
	//����ͼ�����ȱ���
	int i = 0;
	for (i = 0; i < G.n; i++)  G.tags[i] = UNVISITED;
	for (i = 0; i < G.n; i++) {
		if (UNVISITED == G.tags[i])
			if (ERROR == DFS(G, i))   return ERROR;
	}
	return OK;
}

LQueue* InitQueue() {
	//����һ���ն���
	LQueue *Q = (LQueue *)malloc(sizeof(LQueue));
	if (Q == NULL) return ERROR;
	Q->front = NULL;
	Q->rear = NULL;
	return Q;
}

Status EnQueue(LQueue *Q, int e) {
	//�ڶ��еĶ�β����e
	LQNode *p;
	p = (LQNode*)malloc(sizeof(LQNode));
	if (p == NULL)  return ERROR;
	p->data = e;
	p->next = NULL;
	if (NULL == Q->front) {
		Q->front = p;
		Q->rear = p;
	}
	else
	{
		Q->rear->next = p;
		Q->rear = p;
	}
	return OK;
}

Status DeQueue(LQueue *Q, int *e) {
	//�Ƕ���Q�Ķ�ͷ���Ӳ��ҷ��ص�e
	LQNode *p;
	if (NULL == Q->front) return ERROR;
	p = Q->front;
	*e = p->data;
	Q->front = p->next;
	if (Q->rear == p)  Q->rear = NULL;
	free(p);
	return OK;
}

Status BFS(MGraph G) {
	//������ȱ��� 
	int i, j, k;
	int *p_k = &k;
	LQueue *Q = InitQueue();
	for (i = 0; i < G.n; i++)   G.tags[i] = UNVISITED;
	for (i = 0; i < G.n; i++) {
		if (UNVISITED == G.tags[i]) {
			printf("%c", G.vexs[i]);
			G.tags[i] = VISITED;
			EnQueue(Q, i);
			while (DeQueue(Q, p_k) == OK)
			{
				for (j = FirstAdjVex(G, k); j >= 0; j = NextAdjVex(G, k, j))
					if (UNVISITED == G.tags[j] && j >= 0) {
						printf("%c", G.vexs[j]);
						G.tags[j] = VISITED;
						EnQueue(Q, j);
					}
			}
		}
	}
	return OK;
}

/* run this program using the console pauser or add your own getch, system("pause") or input loop */

void main() {
	printf("�����Ȩͼ���ԣ�\n");
	MGraph M;
	MGraph *p_M = &M;
	VexType vexs[5] = { 'A','C','R','F','E' };
	int n = 5;
	int k = 3;;
	ArcInfo *p = (ArcInfo*)malloc(3 * sizeof(ArcInfo));
	p[0].v = 'A';
	p[0].w = 'R';
	p[0].info = 32;
	p[1].w = 'F';
	p[1].v = 'A';
	p[1].info = 34;
	p[2].v = 'R';
	p[2].w = 'E';
	p[2].info = 45;
	VexType w = 'c';
	CreatGraph(p_M, vexs, n, p, 3);
	printf("ͼ�Ķ������£�\n");
	for (int s = 0; s < M.n; s++) {
		printf("\t\t%c", M.vexs[s]);
	}
	printf("\nͼ�Ĺ�ϵ���£�\n");
	int h, s;
	for (h = 0; h < M.n; h++) {
		printf("%c��", M.vexs[h]);
		for (s = 0; s < M.n; s++)
		{
			printf("%d\t\t", M.arcs[h][s]);
		}
		printf("\n");
	}

	printf("\n��ȡָ��λ�ö���Ԫ�صĲ��ԣ�\n λ��1��Ԫ���ǣ�");
	w = GetVex(M, 1);
	printf("%c\n", w);
	printf("\n��ָ��Ԫ�ط���ָ��λ����ԣ���k�ŵ�o��λ��:\n");
	PutVex(M, 0, 'K');
	for (int s = 0; s < M.n; s++) {
		printf("\t\t%c", M.vexs[s]);
	}
	printf("\n��ȡָ��λ��ĵ�һ���ڽӵ�Ԫ�ز��ԣ�0��λ��ĵ�һ���ڽӵ㣩��\n");
	int i = FirstAdjVex(M, 0);
	printf("%c\t��һ���ڽӵ��ǣ�", M.vexs[i]);
	i = NextAdjVex(M, 0, 2);
	printf("%c\n", M.vexs[i]);

	printf("��1��2֮���������ߣ�����Ȩ20:\n");
	AddArc(M, 1, 2, 20);
	for (h = 0; h < M.n; h++) {
		printf("%c��", M.vexs[h]);
		for (s = 0; s < M.n; s++)
		{
			printf("%d\t\t", M.arcs[h][s]);
		}
		printf("\n");
	}
	printf("������ȱ������:\n");
	DFSTraverse(M, 0);
	printf("\n");
	printf("������ȱ������:\n");
	BFS(M);
	printf("\n\n");

	printf("�Ƴ�2�ź�4��֮������ߣ�\n");
	Remove(M, 2, 4);
	for (h = 0; h < M.n; h++) {
		printf("%c��", M.vexs[h]);
		for (s = 0; s < M.n; s++)
		{
			printf("%d\t\t", M.arcs[h][s]);
		}
		printf("\n");
	}
	system("pause");
}