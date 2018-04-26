#include <stdio.h>
#include <stdlib.h>
struct linked_list_node{
	struct linked_list_node *next;
	void *data;
};
typedef struct linked_list_node *Linked_List_Node;

struct linked_list{
	Linked_List_Node head;
	int size;
};
typedef struct linked_list *Linked_List;
/*
Linked_List create_linked_list();
void add_toHead(Linked_List list,void *data);
void add_toTail(Linked_List list,void *data);
int get_size(Linked_List list);
Linked_List_Node get(Linked_List list,int index);
int insert_node(Linked_List list,void *data,int index);
void view_list(Linked_List list);
void *remove_from_list(Linked_List list,int index);
void clear_list(Linked_List list,void (*cleaner)(void *dt));
void *search(int *index,Linked_List list,void *data,int (*compare)(void *dt1,void *dt2));*/

Linked_List create_linked_list(){
	Linked_List result = (Linked_List)malloc(sizeof(struct linked_list));
	result->head = NULL;
	result->size = 0;
	return result;
}
void add_toHead(Linked_List list,void *data){
	//adds a node to head of link list
	Linked_List_Node temp = (Linked_List_Node)malloc(sizeof(struct linked_list_node));
	temp->data = data;
	temp->next = list->head;
	list->head = temp;
	list->size = list->size + 1;
}
void add_toTail(Linked_List list,void *data){
	//adds a node to tail of link list
	Linked_List_Node temp = (Linked_List_Node)malloc(sizeof(struct linked_list_node));
	Linked_List_Node temp2 = list->head;
	temp->data=data;
	temp->next=NULL;
	if(list->head == NULL){
		list->head = temp;
	}
	else{
		while(1){
			if(temp2->next==NULL){
				temp2->next = temp;
				break;
			}
			temp2=temp2->next;
		}
	}
	list->size = list->size + 1;
}
int get_size(Linked_List list){
	return list->size;
}
Linked_List_Node get(Linked_List list,int index){
	//returns specific node from given list
	Linked_List_Node temp = list->head;
	if(index<0){
		return NULL;
	}
	for(int i=0;i!=index;i++){//loop stops if i==index ,this updates traverser, given index times
		if(temp == NULL){
			return NULL;//if wanted index is not exist return NULL
		}
		temp = temp->next;
	}
	return temp;
}
int insert_node(Linked_List list,void *data,int index){
	//this function inserts a node specific position in linked list ,returns 0 or -1
	Linked_List_Node temp;
	if(index > get_size(list) || index < 0){
		return -1;//if given index gretar than size of list return -1
	}
	Linked_List_Node element = (Linked_List_Node)malloc(sizeof(struct linked_list_node));
	element->data = data;
	element->next = NULL;

	if(index == 0){//if node is going to be added to head
		element->next = list->head;
		list->head = element;//update headptr
	}
	else{
		temp = get(list,index-1);//get previous element
		element->next = temp->next;//make element's next, previous' next
		temp->next = element;//make previous' next's element
	}
	list->size = list->size + 1;
	return 0;
}
void view_list(Linked_List list){
	//views list with pointers
	Linked_List_Node temp = list->head;
	while(temp!=NULL){
		printf("%p ",temp->data);
		printf("\n");
		temp=temp->next;
	}
	printf("\n");
}
void *remove_from_list(Linked_List list,int index){
	//removes and returns specific position,
	//returns null there is not such an element
	Linked_List_Node returnPtr = NULL;
	Linked_List_Node temp = list->head;
	if(index==0){
		if(list->head == NULL){//empty list
			return NULL;
		}
		list->head = list->head->next;
		returnPtr = temp;
	}
	else{
		temp = get(list,index-1);
		if(temp == NULL || temp->next == NULL){
			return NULL;
		}
		returnPtr = temp->next;
		temp->next = temp->next->next;
	}
	void *temp2 = returnPtr->data;
	free(returnPtr);
	list->size = list->size - 1;
	return temp;
}
void clear_list(Linked_List list,void (*cleaner)(void *dt)){//clear list
	Linked_List_Node temp = NULL;
	Linked_List_Node iterator = list->head;
	while(iterator != NULL){
		temp = iterator;
		iterator = iterator->next;
		if(temp->data != NULL){
			cleaner(temp->data);
		}
		free(temp);
	}
	free(list);
}
void *search(int *index,Linked_List list,void *data,int (*compare)(void *dt1,void *dt2)){
	//general purpose sequantial search algorithm, requires comperator function
	*index=0;
	Linked_List_Node iterator = list->head;
	while(iterator != NULL){
		if(compare(data,iterator->data)){
			return iterator->data;
		}
		(*index)++;
		iterator = iterator->next;
	}
	*index = -1;
	return NULL;
}
