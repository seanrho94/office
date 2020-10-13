struct queue_node {
    struct employee *emp;
    struct queue_node *next; 
};

struct queue { 
	struct queue_node *front; // remove pt
    struct queue_node *rear;  // insert pt
}; 

struct queue* create_queue() { 
	struct queue* q = (struct queue*)malloc(sizeof(struct queue));
	q->front = NULL; 
    q->rear = NULL; 
	return q; 
}

struct queue_node *create_queue_node(struct employee *emp) { 
	struct queue_node* temp = (struct queue_node*)malloc(sizeof(struct queue_node)); 
	temp->emp = emp; 
	temp->next = NULL; 
	return temp; 
} 

void enqueue(struct queue* q, struct employee* emp);
struct employee* dequeue(struct queue* q);
void destroy_queue(struct queue *q);
