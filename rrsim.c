/*
July 12, 2017
Jared Foulds
CSC 360 Assignment #3

This program is a simulation of a round-robin CPU-scheduler and will
print out the entire schedule of a list of given tasks, each with an 
ID number, arrival time, and the number of ticks required. It will also
print the wait time and turnaround time of each completed task when they
are finished.
*/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "linkedlist.h"
#include <stdbool.h>

#define MAX_BUFFER_LEN 80

taskval_t *event_list = NULL;
int curr_time;
bool increment_time_count = true;
float total_wait_times;
float total_ta_times;
taskval_t *ready_q = NULL;

void print_task(taskval_t *t, void *arg) {
    printf("task %03d: %5d %3.2f %3.2f\n",
        t->id,
        t->arrival_time,
        t->cpu_request,
        t->cpu_used
    );  
}

void increment_count(taskval_t *t, void *arg) {
    int *ip;
    ip = (int *)arg;
    (*ip)++;
}

/*
This function is used for task 2 to print the average wait and turnaround
times to a file which will be processed by my python script to graph the values
*/
void print_average_wait_and_ta_times(int num_events)
{
	FILE *fp1;
	FILE *fp2;
	fp1 = fopen("python_input_wait.txt", "a");
	fp2 = fopen("python_input_ta.txt", "a");
	float num_events_float = (float) num_events;
	float wait_time_average = total_wait_times / num_events_float;
	float ta_time_average = total_ta_times / num_events_float;

	fprintf(fp1,"%6.2f\n", wait_time_average);
	fprintf(fp2,"%6.2f\n", ta_time_average);

	fclose(fp1);
	fclose(fp2);
}
/*
Function to simply print current time instead of having 
multiple print statements throughout program.
*/
void print_curr_time()
{
	printf("[%05d] ", curr_time);
}

void check_event_list(taskval_t *event_curr_node)
{
	if(event_curr_node != NULL){
		if(event_curr_node -> arrival_time <= curr_time){
			if(event_list->next != NULL){
				event_list = remove_front(event_list);
			}
			else{
				event_list = NULL;
			}
			ready_q = add_end(ready_q, event_curr_node);
		}
	}
}
/*
Function to calculate and print turnaround and wait times. 
*/
void print_wait_and_turnaround_time(taskval_t *curr_node)
{	
	float turnaround_time = curr_time - curr_node->arrival_time;
	total_ta_times = total_ta_times + turnaround_time; 

	float wait_time = turnaround_time -  curr_node->cpu_request;
	total_wait_times = total_wait_times + wait_time;

	printf("id=%05d ",curr_node->id); 
	printf("EXIT w=%3.2f ta=%3.2f\n", wait_time, turnaround_time);
}

/*
Function that processes the passed node. It will print the task ID, 
the amount of quantum required, and the amount of quantum used. It
increments the amount of quantum until it reaches the quantum max. 
*/
void process_curr_node(taskval_t *curr_node, int qlen)
{
	int i;
	for(i = 0; i < qlen; i++){
		print_curr_time();
		if(curr_node->cpu_request <= curr_node->cpu_used){
			print_wait_and_turnaround_time(curr_node);
			break;
		}

		printf("id=%05d req=%3.2f used=%3.2f\n", curr_node->id, 
		curr_node->cpu_request, curr_node->cpu_used);
		curr_node->cpu_used = curr_node->cpu_used + 1.0;
		curr_time++;		
	}
}

/*
Function that prints dispatch statements when a new task arrives
*/
void print_dispatches(int dlen)
{
	int i;
	for (i = 0; i < dlen; i ++){
		print_curr_time();
		curr_time ++;
		printf("DISPATCHING\n");
	}
}
/*
The main part of this program. This function works by first checking the event list
and ensuring it's not empty. If it's not empty it adds the first item on the event list to 
the ready queue and enters infinite for loop. It then uses all the functions above to print 
each step of the schedule and will end when the ready queue and event list is empty. 
*/

void run_simulation(int qlen, int dlen) 
{
	taskval_t * ready_curr_node = NULL;
	taskval_t * event_curr_node = NULL;
	taskval_t * temp = NULL;
	curr_time = 0;

	temp = peek_front(event_list);
	if(temp == NULL){
		printf("NOTHING HERE TO RUN\n");
	}
	else{
		temp = peek_front(event_list);
		event_list = remove_front(event_list);
		ready_q = add_end(ready_q, temp);

		for(;;){
			ready_curr_node = peek_front(ready_q);
			event_curr_node = peek_front(event_list);

			if(ready_curr_node == NULL){
				check_event_list(event_curr_node);
				ready_curr_node = peek_front(ready_q);
				if(ready_curr_node == NULL){
					print_curr_time();
					printf("IDLE\n");
				}
			}
			else{
				if(ready_curr_node->arrival_time > curr_time){ //if curr_time > node arrival time, IDLE
					print_curr_time();
					printf("IDLE\n");
				}
				else if(ready_curr_node->arrival_time <= curr_time){ //if curr_time == arrival time, dispatch
					increment_time_count = false;
					print_dispatches(dlen);
					process_curr_node(ready_curr_node, qlen);

					if(ready_curr_node-> cpu_used >= ready_curr_node->cpu_request){//if front of ready q done, remove it 
						temp = peek_front(ready_q);
						ready_q = remove_front(ready_q);
					}
				else{
					check_event_list(event_curr_node);
					ready_q = remove_front(ready_q);
					ready_q = add_end(ready_q, ready_curr_node);
					}
				}
			}

			if(ready_q == NULL && event_list == NULL){
				break;
			}
			if(increment_time_count){
				curr_time ++;
			}
			increment_time_count = true;
		}
	}
}

int main(int argc, char *argv[]) {
    char   input_line[MAX_BUFFER_LEN];
    int    i;
    int    task_num;
    int    task_arrival;
    float  task_cpu;
    int    quantum_length = -1;
    int    dispatch_length = -1;

    taskval_t *temp_task;

    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--quantum") == 0 && i+1 < argc) {
            quantum_length = atoi(argv[i+1]);
        }
        else if (strcmp(argv[i], "--dispatch") == 0 && i+1 < argc) {
            dispatch_length = atoi(argv[i+1]);
        }
    }

    if (quantum_length == -1 || dispatch_length == -1) {
        fprintf(stderr, 
            "usage: %s --quantum <num> --dispatch <num>\n",
            argv[0]);
        exit(1);
    }


    while(fgets(input_line, MAX_BUFFER_LEN, stdin)) {
        sscanf(input_line, "%d %d %f", &task_num, &task_arrival,
            &task_cpu);
        temp_task = new_task();
        temp_task->id = task_num;
        temp_task->arrival_time = task_arrival;
        temp_task->cpu_request = task_cpu;
        temp_task->cpu_used = 0.0;
        event_list = add_end(event_list, temp_task);
    }

#ifdef DEBUG
    int num_events = 0;
    apply(event_list, increment_count, &num_events);
    printf("DEBUG: # of events read into list -- %d\n", num_events);
    printf("DEBUG: value of quantum length -- %d\n", quantum_length);
    printf("DEBUG: value of dispatch length -- %d\n", dispatch_length);
#endif

    run_simulation(quantum_length, dispatch_length);
	//TO USE THIS PROGRAM FOR TASK 2:
	//print_average_wait_and_ta_times(num_events);

    return (0);
}
