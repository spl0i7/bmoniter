#ifndef BMON_H_   
#define BMON_H_
struct iface_stat{ 
	char iface_name[MAX];
	long long iface_data_recv;
	long long iface_data_send;
	struct iface_stat *next;
	};
struct buffer_index{
	int l_index;
	int u_index;
	};

typedef struct buffer_index index_t;	//declare index_t type data structure.	
typedef struct iface_stat iface_stat_t; //declare iface_stat_t type data structure.
void get_iface_name(iface_stat_t *iface_stat);
int number_of_interface(void);
void get_iface_data(iface_stat_t *iface_stat);
void calculate_iface_name(index_t *tmp_index , char *buffer, char *tmp);
void calculate_iface_data(index_t *tmp_index , char *buffer, int test_case);
void io_error(void);


#endif