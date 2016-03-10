#define STAT "/proc/net/dev"
#define MAX 255
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include "bmon.h"
int main(void) {
	
	int n_iface=number_of_interface(); // get number of interfaces
	long long iface_odata_recv[n_iface];
	long long iface_odata_send[n_iface];
	for(int i=0 ; i<n_iface ; i++){
		iface_odata_recv[i]=0;
		iface_odata_send[i]=0;
		}
	iface_stat_t *iface_stat , *head;
	head=NULL;
	/*Initialize our linked list*/
	while(1>0){
	for(int i=0;i<n_iface;i++){
		iface_stat=(iface_stat_t *)malloc(sizeof(iface_stat_t));
		iface_stat->next=head;
		head=iface_stat;
		}
	get_iface_name(iface_stat);
	printf("Got %d interfaces\n",n_iface);
	get_iface_data(iface_stat);
	head=iface_stat;
	printf("\033[2J\033[1;1H"); // clear screen output
	for(int i=0;i<n_iface;i++){
		if((iface_stat->iface_data_recv - iface_odata_recv[i])<0){
			iface_odata_recv[i]=iface_stat->iface_data_recv;
			}
		if((iface_stat->iface_data_send - iface_odata_send[i])<0){
			iface_odata_send[i]=iface_stat->iface_data_send;
			}	
			
		//printf("Interface \t Received \t DownSpeed \t");
		printf("%s : %lld KB | %lld KBps | %lld KB | %lld KBps \n",iface_stat->iface_name,((iface_stat->iface_data_recv)/1024) , 
		((iface_stat->iface_data_recv - iface_odata_recv[i])/1024) , ((iface_stat->iface_data_send)/1024) ,
		(iface_stat->iface_data_send - iface_odata_send[i])/1024);
		iface_odata_recv[i]=iface_stat->iface_data_recv;
		iface_odata_send[i]=iface_stat->iface_data_send;
		iface_stat=iface_stat->next;
	}
	
	/*dellocate Linked List */
	iface_stat=head;
	iface_stat_t *tmp;
	tmp=iface_stat;
	for(int i=0;i<n_iface;i++){
		head=tmp->next;
		free(tmp);
		tmp=head;
		}
		
	sleep(1);
	
	}
	return 0;
	}
	
int number_of_interface(void) {
	FILE *statfp;
	statfp=fopen(STAT,"r");
	if(statfp==NULL){
		io_error();
		exit(1);
	}
	int n_iface=0;
	char buffer[MAX];
	fgets(buffer,MAX,statfp); /*Ignore first two lines*/
	fgets(buffer,MAX,statfp);
	while(fgets(buffer,MAX,statfp)){
			n_iface++;
		}
	fclose(statfp);
	return(n_iface);
	}
	
void get_iface_name(iface_stat_t *iface_stat){
	FILE *statfp;
	statfp=fopen(STAT,"r");
	if(statfp==NULL){
		io_error();
		exit(1);
	}
	char buffer[MAX];
	fgets(buffer,MAX,statfp); /*Ignore first two lines*/
	fgets(buffer,MAX,statfp);
	while(fgets(buffer,MAX,statfp)){
		char *tmp;
		tmp = strstr(buffer , ":"); // return pointer to ":"
		index_t tmp_index;
		calculate_iface_name(&tmp_index , buffer  , tmp);
		strncpy(iface_stat->iface_name,buffer+tmp_index.l_index,(tmp_index.u_index-tmp_index.l_index));
		iface_stat=iface_stat->next;
		}
	fclose(statfp);
}

void get_iface_data(iface_stat_t *iface_stat){
	FILE *statfp;
	statfp=fopen(STAT,"r");
	if(statfp==NULL){
		io_error();
		exit(1);
	}
	char buffer[MAX];
	char iface_tmp_data[MAX];
	fgets(buffer,MAX,statfp); /*Ignore first two lines*/
	fgets(buffer,MAX,statfp);
	while(fgets(buffer,MAX,statfp)){
		char *tmp;
		tmp = strstr(buffer , ":");
		index_t tmp_index;
		
		calculate_iface_name(&tmp_index , buffer , tmp);
		tmp_index.l_index=tmp_index.u_index;
		calculate_iface_data(&tmp_index , buffer , 1);
		memset(iface_tmp_data , 0 , 255);
		strncpy(iface_tmp_data,buffer+tmp_index.l_index,(tmp_index.u_index-tmp_index.l_index)); //put in received data
		iface_stat->iface_data_recv=atoll(iface_tmp_data);
		
		/*let's get sent data*/
		tmp_index.l_index=tmp_index.u_index;
		calculate_iface_data(&tmp_index , buffer , 8);
		memset(iface_tmp_data , 0 , 255);
		strncpy(iface_tmp_data,buffer+tmp_index.l_index,(tmp_index.u_index-tmp_index.l_index)); //put in received data
		iface_stat->iface_data_send=atoll(iface_tmp_data);
		
		iface_stat=iface_stat->next;
		}
		fclose(statfp);
	}
void io_error(void){
	// Something bad happened
	perror("Error opening stat file\n");
	}
void calculate_iface_name(index_t *tmp_index , char *buffer, char *tmp){ //calculation goes here
	tmp_index->u_index=-1;
			if(tmp) {
				tmp_index->u_index=tmp-buffer;
			}
			else {
				exit(1);
				}
		tmp_index->l_index=tmp_index->u_index;
		if(tmp_index->u_index >0){
			while( tmp_index->l_index >=0) {
				if(buffer[tmp_index->l_index] != ' '){
					tmp_index->l_index--;
				}
				else {
					break;
				}
			}
			tmp_index->l_index++;
	
	}
}

void calculate_iface_data(index_t *tmp_index , char *buffer, int test_case){
	while(test_case > 0) {
		tmp_index->l_index=tmp_index->u_index;
		(tmp_index->l_index)++;
	while( tmp_index->l_index >0) {
			if(buffer[tmp_index->l_index] == ' '){
				(tmp_index->l_index)++;
			}
			else {
				break;
			}
		}
		tmp_index->u_index=tmp_index->l_index;
		while(tmp_index->u_index>0){
			if(buffer[tmp_index->u_index]!=' '){
				tmp_index->u_index++;
				}
			else {
				break;
				}
		}
		test_case--;
	}
}