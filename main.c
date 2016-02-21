#define STAT "/proc/net/dev"
#define MAX 255
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

struct iface_stat{ 
	char iface_name[MAX];
	long long iface_data_recv;
	long long iface_data_send;
	struct iface_stat *next;
	} ;
typedef struct iface_stat iface_stat_t; //declare iface_stat_t type data structure.
void get_iface_name(iface_stat_t *iface_stat);
int number_of_interface(void);
void get_iface_data(iface_stat_t *iface_stat);
void io_error(void);

int main(void) {
	
	int n_iface=number_of_interface(); // get number of interfaces
	long long iface_odata_recv[n_iface];
	for(int i=0 ; i<n_iface ; i++){
		iface_odata_recv[i]=0;
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
		printf("%s : %lld Kbytes %lld Kbps\n",iface_stat->iface_name,(iface_stat->iface_data_recv/1024) , ((iface_stat->iface_data_recv - iface_odata_recv[i])/1024));
		iface_odata_recv[i]=iface_stat->iface_data_recv;
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
		tmp = strstr(buffer , ":");
		int u_index=-1;
			if(tmp) {
				u_index=tmp-buffer;
			}
		int l_index=u_index;
		if(u_index >0){
			while( l_index >=0) {
				if(buffer[l_index] != ' '){
					l_index--;
				}
				else {
					break;
				}
			}
			l_index++;
			strncpy(iface_stat->iface_name,buffer+l_index,(u_index-l_index));
			iface_stat=iface_stat->next;
		}
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
		int l_index=-1;
			if(tmp) {
				l_index=tmp-buffer;
			}
			l_index++;
		while( l_index >0) {
			if(buffer[l_index] == ' '){
				l_index++;
			}
			else {
				break;
			}
		}
		int u_index=l_index;
		while(u_index>0){
			if(buffer[u_index]!=' '){
				u_index++;
				}
			else {
				break;
				}
		}
			strncpy(iface_tmp_data,buffer+l_index,(u_index-l_index));
			iface_stat->iface_data_recv=atoll(iface_tmp_data);
			iface_stat=iface_stat->next;
		}
		fclose(statfp);
	}
void io_error(void){
	perror("Error opening stat file\n");
	}
