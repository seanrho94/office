#include "office.h"
#include "queue.h"

// Queue implementation
void enqueue(struct queue* q, struct employee* emp) { 
	// Create a new node 
	struct queue_node* temp = create_queue_node(emp); 

	// A new node is placed at front and rear when a queue is empty
	if (q->rear == NULL) { 
		q->front = temp; 
    	q->rear = temp; 
		return; 
	} 
	
	q->rear->next = temp; // set rear node
	q->rear = temp;  // set queue's rear pt
}

struct employee* dequeue(struct queue* q) { 
	// If queue is empty, return NULL. 
	if (q->front == NULL){
		return NULL; 
  	}

	// Keep the front node to free it later.
	struct queue_node* temp_node = q->front; 
	// Keep the front node (in empyloee) to return it later.
	struct employee* emp = temp_node->emp;
	// Change front to its next.
	q->front = q->front->next; 

	// If front becomes NULL, then change rear also as NULL 
	if (q->front == NULL){
		q->rear = NULL; 
  	}
	
	// Free the node.
	free(temp_node);
	
	return emp;
}

// Destroy queue
void destroy_queue(struct queue *q) {
	// Dequeue and free every nodes in the queue until it becomes empty.
    while(q->front != NULL) {
        dequeue(q);
    }

    free(q);
}

/**
 * Places an employee within the office, if the supervisor field is NULL
 *  it is assumed the employee will be placed under the next employee that is
 * not superivising any other employee (top-down, left-to-right).
 * 
 * If the supervisor is not NULL, it will be added to the supervisor's subordinates list
 *  of employees (make sure the supervisor exists in hierarchy).
 * if the office or employee are null, the function not do anything.
 */
void office_employee_place(struct office* off, struct employee* supervisor, struct employee* emp) {
	// if the office or employee are null, the function not do anything.
	if(off == NULL || emp == NULL){
		return ;
	}
	
	// Place a boss when the department head is NULL in the office.
	if(off != NULL && off->department_head ==  NULL) {
		// Allocate memory for the department head in the office.
		off->department_head = (struct employee*)malloc(sizeof(struct employee));
		// Copy the address of employee into the department head.
		memcpy(off->department_head, emp, sizeof(struct employee));
		// Allocate memory for the name of the department head.
		off->department_head->name = malloc(sizeof(char) * 40);
		// String copy the employee's name into the department head's name.
		strcpy(off->department_head->name, emp->name);
		return ;
	}
	
	struct queue* q = create_queue(); // Create a queue to traverse the office tree.
    struct employee* temp_node = off->department_head; // start from boss(root)
	
	//If supervisor is not given
	if (supervisor == NULL){
		// Add employee to someone has NO subordinates
	    while (temp_node != NULL) { 
			if(temp_node->n_subordinates == 0) {
				// Found the supervisor person
    			temp_node->n_subordinates++;
				// allocate memory for the employee
    			temp_node->subordinates = (struct employee*)malloc(sizeof(struct employee) * temp_node->n_subordinates);
				// Copy the employee to the first item of subordinates
    			memcpy(&temp_node->subordinates[0], emp, sizeof(struct employee));
				// allocate memory for the name of first subordinates. 
				temp_node->subordinates[0].name = malloc(sizeof(char) * 40);
				// Copy the employee's name into  the subordinate's name.
				strcpy(temp_node->subordinates[0].name, emp->name);
				// Set employee's supervisor.
				temp_node->subordinates[0].supervisor = temp_node;
				break;
			}
			else if(temp_node->n_subordinates > 0) {
				for(int i = 0; i < temp_node->n_subordinates; i++){
					// Enqueue the employee's subordinates.
					enqueue(q, temp_node->subordinates + i);
				}	
			}
			// Dequeue employee and make it temp_node
        	temp_node = dequeue(q); 
    	} 
	// If supervisor is given	
	}else{
		while (temp_node != NULL) {
			// Check if the supervisor is exist in the tree.
			if (temp_node == supervisor) {
				// If the employee does not have subordinates
				if(temp_node->n_subordinates == 0){
    				temp_node->n_subordinates++;
    				temp_node->subordinates = (struct employee*)malloc(sizeof(struct employee) * temp_node->n_subordinates);
    				memcpy(&temp_node->subordinates[0], emp, sizeof(struct employee));
					temp_node->subordinates[0].name = malloc(sizeof(char) * 40);
					strcpy(temp_node->subordinates[0].name, emp->name);
					temp_node->subordinates[0].supervisor = supervisor;
					break;
				// If the employee has subordinates	
				}else if(temp_node->n_subordinates > 0) {
    				temp_node->n_subordinates++;
    				temp_node->subordinates = (struct employee*)realloc(temp_node->subordinates, sizeof(struct employee) * temp_node->n_subordinates);
    				memcpy(&temp_node->subordinates[temp_node->n_subordinates - 1], emp, sizeof(struct employee));
					temp_node->subordinates[temp_node->n_subordinates - 1].name = malloc(sizeof(char) * 40);
					strcpy(temp_node->subordinates[temp_node->n_subordinates - 1].name, emp->name);
					temp_node->subordinates[temp_node->n_subordinates - 1].supervisor = supervisor;
					break;
				}
			}else {
				for(int i = 0; i < temp_node->n_subordinates; i++){
				  	enqueue(q, temp_node->subordinates + i);
				}					
			}
			// Dequeue node and make it temp_node
			temp_node = dequeue(q);	
		}
	}
	
	// Free queue and queue nodes.
    destroy_queue(q);

}

/**
 * Fires an employee, removing from the office
 * If employee is null, nothing should occur
 * If the employee does not supervise anyone, they will just be removed
 * If the employee is supervising other employees, the first member of that 
 *  team will replace him.
 */
void office_fire_employee(struct employee* employee) {
	// Do nothing if emplyee is NULL
	if(employee == NULL){
		return;
	}
	
	if(employee != NULL){
		// If employee does not have subordinates, then just free it.
		if(employee->n_subordinates == 0){
			free(employee->name);
			employee->supervisor->n_subordinates--;
			free(employee);
			
		// If employee has subordinates, delete the emplyee and replace with its first subordiantes.	
		}else{
			employee->n_subordinates--;
			strcpy(employee->name, employee->subordinates[0].name);
			free(employee->subordinates[0].name);
			free(employee->subordinates);
		}
	}
}

/**
 * Retrieves the first encounter where the employee's name is matched to one in the office
 * If the employee does not exist, it must return NULL
 * if office or name are NULL, your function must do nothing
 */ 
struct employee* office_get_first_employee_with_name(struct office* office,
  const char* name) {
	struct queue* q = create_queue(); 
    struct employee* temp_node = office->department_head;
	int is_equal_name = 0;
    
	while (temp_node != NULL) { 	
	    is_equal_name = strcmp(temp_node->name, name);
		if(is_equal_name == 0){
			destroy_queue(q);
			return temp_node;
			break;
		}
		
		int i = 0;
		while(temp_node->subordinates != NULL && i < temp_node->n_subordinates){
			struct employee *temp_emp = &temp_node->subordinates[i];
			enqueue(q, temp_emp);
			i++;
		}
     
        temp_node = dequeue(q); 
    } 
	if(is_equal_name != 0){
		destroy_queue(q);
		return NULL;
	}
}

/**
 * Retrieves the last encounter where the employee's name is matched to one in the office
 * If the employee does not exist, it must return NULL
 * if office or name are NULL, your function must do nothing
 */ 
struct employee* office_get_last_employee_with_name(struct office* office,
  const char* name) {
	struct queue* q = create_queue(); 
    struct employee* temp_node = office->department_head;
	// This is the last encounter where the employee's name is matched.
	struct employee* temp_node_get_last = NULL;
	int is_equal_name = 0;
    
	while (temp_node != NULL) { 	
	    is_equal_name = strcmp(temp_node->name, name);
		if(is_equal_name == 0){
			temp_node_get_last = temp_node;
		}
		
		int i = 0;
		while(temp_node->subordinates != NULL && i < temp_node->n_subordinates){
			struct employee *temp_emp = &temp_node->subordinates[i];
			enqueue(q, temp_emp);
			i++;
		}
     
        temp_node = dequeue(q); 
    } 
	
	destroy_queue(q);
	return temp_node_get_last;
	
}

/**
 * This function will need to retrieve all employees at a level.
 * A level is defined as distance away from the boss. For example, all 
 * subordinates of the boss are 1 level away, subordinates of the boss's subordinates
 * are 2 levels away.
 * 
 * if office, n_employees or emplys are NULL, your function must do nothing
 * You will need to provide an allocation for emplys and specify the
 * correct number of employees found in your query.
 */
void office_get_employees_at_level(struct office* office, size_t level,
  struct employee** emplys, size_t* n_employees){
	struct queue* q = create_queue();
    struct employee* temp_node = office->department_head;
	size_t level_count = 1;
	size_t current_count = 0;
	size_t level_idx = 0;
	*n_employees = 0;
	enqueue(q, temp_node);
	
	while(q->front != NULL){
		while(level_count > 0){
			temp_node = dequeue(q);
		
			int i = 0;
			while(temp_node->subordinates != NULL && i < temp_node->n_subordinates){
				struct employee *temp_emp = &temp_node->subordinates[i];
				enqueue(q, temp_emp);
				i++;
				current_count++;
			}
			level_count--;
			
			if(level_idx == level){
				// subordinates are just processed. Now, process current employee.
				struct employee *emplys_ptr = *emplys; // Prepare temporary employs table for the entrire emplys table, it can be used for malloc/realloc
				size_t n_employees_new = *n_employees;
				n_employees_new++;
				*n_employees = n_employees_new;

				if (emplys_ptr == NULL) {
					emplys_ptr = malloc(sizeof(struct employee) * n_employees_new);
				}
				else {
					emplys_ptr = realloc(emplys_ptr, sizeof(struct employee) * n_employees_new);
				}

				// copy current employee to emplys table
				memcpy(&emplys_ptr[n_employees_new - 1], temp_node, sizeof(struct employee));

				// copy current employee's name to emplys table
				emplys_ptr[n_employees_new - 1].name = malloc(sizeof(char) * 40);
				strcpy(emplys_ptr[n_employees_new - 1].name, temp_node->name);
				// Update memory map for emplys table
				*emplys = emplys_ptr;
			}
		}
		
		level_idx++;
		level_count = current_count;
		current_count = 0;
	}
	destroy_queue(q);
	
}

static void office_get_employees_by_name_inner(struct employee* emp, const char* name, 
	struct employee** emplys,
  	size_t* n_employees) {

	// current emp has subordinates, process each subordinates
	for (int subordinate_idx = 0; subordinate_idx < emp->n_subordinates; subordinate_idx++) {
		office_get_employees_by_name_inner(&emp->subordinates[subordinate_idx], name, emplys, n_employees);
	}
	
	int is_equal_name = strcmp(name, emp->name);
	if(is_equal_name == 0){
		// subordinates are just processed. Now, process current employee.
		struct employee *emplys_ptr = *emplys; // Prepare temporary employs table for the entrire emplys table, it can be used for malloc/realloc
		size_t n_employees_new = *n_employees;
		n_employees_new++;
		*n_employees = n_employees_new;

		if (emplys_ptr == NULL) {
			emplys_ptr = malloc(sizeof(struct employee) * n_employees_new);
		}
		else {
			emplys_ptr = realloc(emplys_ptr, sizeof(struct employee) * n_employees_new);
		}

		// copy current employee to emplys table
		memcpy(&emplys_ptr[n_employees_new - 1], emp, sizeof(struct employee));

		// copy current employee's name to emplys table
		emplys_ptr[n_employees_new - 1].name = malloc(sizeof(char) * 40);
		strcpy(emplys_ptr[n_employees_new - 1].name, emp->name);
		// Update memory map for emplys table
		*emplys = emplys_ptr;
	}
}

/**
 * Will retrieve a list of employees that match the name given
 * If office, name, emplys or n_employees is NULL, this function should do
 * nothing
 * if office, n_employees, name or emplys are NULL, your function must do
 * nothing. 
 * You will need to provide an allocation to emplys and specify the
 * correct number of employees found in your query.
 */
void office_get_employees_by_name(struct office* office, const char* name,
  struct employee** emplys, size_t* n_employees) {
	
	struct employee *head = office->department_head;
	*n_employees = 0;

	if (head == NULL) {
		return;
	}

	office_get_employees_by_name_inner(head, name, emplys, n_employees);
}

static void office_get_employees_postorder_inner(struct employee* emp, 
	struct employee** emplys,
  	size_t* n_employees) {

	// current emp has subordinates, process each subordinates
	for (int subordinate_idx = 0; subordinate_idx < emp->n_subordinates; subordinate_idx++) {
		office_get_employees_postorder_inner(&emp->subordinates[subordinate_idx], emplys, n_employees);
	}

	// subordinates are just processed. Now, process current employee.
	struct employee *emplys_ptr = *emplys; // Prepare temporary employs table for the entrire emplys table, it can be used for malloc/realloc
	size_t n_employees_new = *n_employees;
	n_employees_new++;
	*n_employees = n_employees_new;

	if (emplys_ptr == NULL) {
		emplys_ptr = malloc(sizeof(struct employee) * n_employees_new);
	}
	else {
		emplys_ptr = realloc(emplys_ptr, sizeof(struct employee) * n_employees_new);
	}

	// copy current employee to emplys table
	memcpy(&emplys_ptr[n_employees_new - 1], emp, sizeof(struct employee));

	// copy current employee's name to emplys table
	emplys_ptr[n_employees_new - 1].name = malloc(sizeof(char) * 40);
	strcpy(emplys_ptr[n_employees_new - 1].name, emp->name);
	// Update memory map for emplys table
	*emplys = emplys_ptr;
}
/**
 * You will traverse the office and retrieve employees using a postorder traversal
 * If off, emplys or n_employees is NULL, this function should do nothing
 *
 * You will need to provide an allocation to emplys and specify the
 * correct number of employees found in your query.
 */
void office_get_employees_postorder(struct office* off, 
  struct employee** emplys,
  size_t* n_employees) {
	
	struct employee *head = off->department_head;
	*n_employees = 0;

	if (head == NULL) {
		return;
	}

	office_get_employees_postorder_inner(head, emplys, n_employees);
}

// Destroys every individual employees in the office.
static void destroy_emp(struct employee* emp) {

	// current emp has subordinates, process each subordinates
	for (int subordinate_idx = 0; subordinate_idx < emp->n_subordinates; subordinate_idx++) {
		destroy_emp(&emp->subordinates[subordinate_idx]);
	}
	free(emp->name);
	if(emp->n_subordinates > 0){
		free(emp->subordinates);
	}
	
	if(emp->supervisor == NULL){
		free(emp);
	}
}

/**
 * The office disbands
 * (You will need to free all memory associated with employees attached to
 *   the office and the office itself)
 */
void office_disband(struct office* office) {
	struct employee *head = office->department_head;

	if (head == NULL) {
		free(office);
		return;
	}

	destroy_emp(head);
	free(office);
}

// Dump office tree by level order traversal.
void printLevelOrder(struct employee* emp) { 
    struct queue* q = create_queue(); 
    struct employee* temp_node = emp;
    while (temp_node != NULL) { 
        printf("%p: emp->name: %s, n_subordinates: %d, ", temp_node, temp_node->name, (int)temp_node->n_subordinates); 
		
		if(temp_node->supervisor == NULL){
			printf("boss\n");
		}
		
		if(temp_node->supervisor != NULL){
			printf("supervisor: %s\n", temp_node->supervisor->name);
		}
		
		int i = 0;
		while(temp_node->subordinates != NULL && i < temp_node->n_subordinates){
			struct employee *temp_emp = &temp_node->subordinates[i];
			enqueue(q, temp_emp);
			printf("%s 's subordinats[%d]: %s\n", temp_node->name, i, temp_emp->name);
			i++;
		}
    	
		printf("\n");
     
        temp_node = dequeue(q); 
    } 
	destroy_queue(q);
} 

int main(){
	// Allocate memory for office.
	struct office* off = malloc(sizeof(struct office));
	off->department_head = NULL;
	
	// Create and initialise employees
	struct employee emp1 = { .name = "1", .supervisor = NULL, .subordinates = NULL, .n_subordinates = 0  } ;
	struct employee emp2 = { .name = "2", .supervisor = NULL, .subordinates = NULL, .n_subordinates = 0  } ;
	struct employee emp3 = { .name = "3", .supervisor = NULL, .subordinates = NULL, .n_subordinates = 0  } ;	
	struct employee emp4 = { .name = "4", .supervisor = NULL, .subordinates = NULL, .n_subordinates = 0  } ;
	struct employee emp5 = { .name = "5", .supervisor = NULL, .subordinates = NULL, .n_subordinates = 0  } ;
	struct employee emp6 = { .name = "6", .supervisor = NULL, .subordinates = NULL, .n_subordinates = 0  } ;
	struct employee emp7 = { .name = "7", .supervisor = NULL, .subordinates = NULL, .n_subordinates = 0  } ;
	struct employee emp8 = { .name = "8", .supervisor = NULL, .subordinates = NULL, .n_subordinates = 0  } ;
	struct employee emp9 = { .name = "9", .supervisor = NULL, .subordinates = NULL, .n_subordinates = 0  } ;
	struct employee emp10 = { .name = "10", .supervisor = NULL, .subordinates = NULL, .n_subordinates = 0  } ;
	struct employee emp11 = { .name = "11", .supervisor = NULL, .subordinates = NULL, .n_subordinates = 0  } ;
	struct employee emp12 = { .name = "12", .supervisor = NULL, .subordinates = NULL, .n_subordinates = 0  } ;
	struct employee emp13 = { .name = "13", .supervisor = NULL, .subordinates = NULL, .n_subordinates = 0  } ;
	struct employee emp14 = { .name = "14", .supervisor = NULL, .subordinates = NULL, .n_subordinates = 0  } ;

	//Place employees into office.
	//off->department_head
  	office_employee_place(off, NULL, &emp1);
	//&off->department_head->subordinates[0]
	office_employee_place(off, NULL, &emp2);
	//&off->department_head->subordinates[1]
	office_employee_place(off, off->department_head, &emp3);
	//&off->department_head->subordinates[2]
	office_employee_place(off, off->department_head, &emp4);
	//&off->department_head->subordinates[3]
	office_employee_place(off, off->department_head, &emp5);
	//&off->department_head->subordinates[0].subordinates[0]
	office_employee_place(off, &off->department_head->subordinates[0], &emp6);
	//&off->department_head->subordinates[0].subordinates[1]
	office_employee_place(off, &off->department_head->subordinates[0], &emp7);
	//&off->department_head->subordinates[0].subordinates[2]
	office_employee_place(off, &off->department_head->subordinates[0], &emp8);
	//&off->department_head->subordinates[1].subordinates[0]
	office_employee_place(off, &off->department_head->subordinates[1], &emp9);
	//&off->department_head->subordinates[1].subordinates[1]
	office_employee_place(off, &off->department_head->subordinates[1], &emp10);
	//&off->department_head->subordinates[2].subordinates[0]
	office_employee_place(off, &off->department_head->subordinates[2], &emp11);
	//&off->department_head->subordinates[2].subordinates[1]
	office_employee_place(off, &off->department_head->subordinates[2], &emp12);
	//&off->department_head->subordinates[1].subordinates[0].subordinates[0]
	office_employee_place(off, &off->department_head->subordinates[1].subordinates[0], &emp13);
	//&off->department_head->subordinates[1].subordinates[1].subordinates[0]
	office_employee_place(off, &off->department_head->subordinates[1].subordinates[1], &emp14);

	printLevelOrder(off->department_head);

	// Fire employee with name "9".
	office_fire_employee(&off->department_head->subordinates[1].subordinates[0]);

	printLevelOrder(off->department_head);

	struct employee* emplys = NULL;
	size_t n_emps = 0;
	// Testing for postorder traversal in the office tree. This returns the total number of employees in the office tree.
	office_get_employees_postorder(off, &emplys, &n_emps);
	printf("The current numbers of employees in the office are %ld\n", n_emps);
	// Find total number of employees at the specific level of the office tree.
	office_get_employees_at_level(off, 2, &emplys, &n_emps);
	printf("There are %ld employees in 1st level of the office tree\n", n_emps);

	/*Further testing for office_get_employees_at_level function. Prints every employees' name at the specific level.
	for (int empl_idx = 0; empl_idx < n_emps; empl_idx++) {
		struct employee* emp = (struct employee*)&emplys[empl_idx];
		printf("emp[%d]: %p: %s\n", empl_idx, emp, emp->name);
		//printf("[%s]\n", emp->name);
	}
	printf("\n");
	*/

	// Testing for office_get_first_employee_with_name and office_get_last_employee_with_name.
	const char* name = "1";
	struct employee* emp_first = office_get_first_employee_with_name(off, name);
	struct employee* emp_last = office_get_last_employee_with_name(off, name);
	printf("emp_first_encounter: %s\n", emp_first->name);
	printf("emp_last_encounter: %s\n", emp_last->name);

	// Testing for office_get_employees_by_name.
	const char* target_name = "1";
	struct employee* emplys1 = NULL;
	size_t n_emps1 = 0;
	office_get_employees_by_name(off, target_name, &emplys1, &n_emps1);
	printf("There are %ld employees matched with the name %s.\n", n_emps1, target_name);

	office_disband(off);
}
