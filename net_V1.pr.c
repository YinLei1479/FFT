/* Process model C form file: net_V1.pr.c */
/* Portions of this file copyright 1986-2008 by OPNET Technologies, Inc. */



/* This variable carries the header into the object file */
const char net_V1_pr_c [] = "MIL_3_Tfile_Hdr_ 145A 30A op_runsim 7 66EA45BC 66EA45BC 1 ray-laptop 28918 0 0 none none 0 0 none 0 0 0 0 0 0 0 0 1bcc 1                                                                                                                                                                                                                                                                                                                                                                                                       ";
#include <string.h>



/* OPNET system definitions */
#include <opnet.h>



/* Header Block */

#include <math.h>

/* Constant Definitions */
#define RX_IN		(0)
#define SRC_IN		(1)
#define TX_OUT		(0)
#define SINK_OUT	(1)
#define TX_OUT_2	(2)
#define RX_IN_2		(2)

#define COLLECT_FINISH	(5000)
#define TIME_OUT_SIGNAL (6000)
#define INTACT_PK_SEND	(7000)

/* Transition Condition Macros */ 
#define FROM_RX_PK			(op_intrpt_type() == OPC_INTRPT_STRM) && (op_intrpt_strm () == RX_IN)
#define FROM_SRC_PK 		(op_intrpt_type() == OPC_INTRPT_STRM) && (op_intrpt_strm () == SRC_IN)
#define FROM_RX2_PK			(op_intrpt_type() == OPC_INTRPT_STRM) && (op_intrpt_strm () == RX_IN_2)

#define INTACT_START 		(op_intrpt_type() == OPC_INTRPT_SELF) && (op_intrpt_code () == INTACT_PK_SEND)
#define COLLECT				(op_intrpt_type() == OPC_INTRPT_SELF) && (op_intrpt_code () == COLLECT_FINISH)
#define TIME_OUT			(op_intrpt_type() == OPC_INTRPT_SELF) && (op_intrpt_code () == TIME_OUT_SIGNAL)

#define m 5201314

/* End of Header Block */

#if !defined (VOSD_NO_FIN)
#undef	BIN
#undef	BOUT
#define	BIN		FIN_LOCAL_FIELD(_op_last_line_passed) = __LINE__ - _op_block_origin;
#define	BOUT	BIN
#define	BINIT	FIN_LOCAL_FIELD(_op_last_line_passed) = 0; _op_block_origin = __LINE__;
#else
#define	BINIT
#endif /* #if !defined (VOSD_NO_FIN) */



/* State variable definitions */
typedef struct
	{
	/* Internal state tracking for FSM */
	FSM_SYS_STATE
	/* State Variables */
	int	                    		my_address                                      ;	/* my node address */
	Objid	                  		my_id                                           ;
	Objid	                  		my_node_id                                      ;
	int	                    		type                                            ;	/* rcv pk type */
	int	                    		interactive_id                                  ;
	int	                    		to_interact_next_hop                            ;	/* the next hop to the interact node */
	int	                    		interact_pk_num                                 ;	/* the Num of the pk */
	int	                    		topo[24][24]                                    ;	/* 0 or 1 for the net topo */
	int	                    		link_interact_pk_num                            ;	/* pk num of interact collection */
	Evhandle	               		evh                                             ;	/* time out self intrpt */
	int	                    		data_num_now                                    ;
	int	                    		A[24][24]                                       ;	/* route select */
	int	                    		dist[24]                                        ;	/* route select */
	int	                    		path[24]                                        ;	/* route select */
	int	                    		mark[24]                                        ;	/* route select */
	int	                    		last_topo[24][24]                               ;
	int	                    		route[5]                                        ;	/* the final route */
	int	                    		route_num                                       ;	/* route hop num */
	int	                    		node_num                                        ;
	int	                    		topo_address[24][3]                             ;
	} net_V1_state;

#define my_address              		op_sv_ptr->my_address
#define my_id                   		op_sv_ptr->my_id
#define my_node_id              		op_sv_ptr->my_node_id
#define type                    		op_sv_ptr->type
#define interactive_id          		op_sv_ptr->interactive_id
#define to_interact_next_hop    		op_sv_ptr->to_interact_next_hop
#define interact_pk_num         		op_sv_ptr->interact_pk_num
#define topo                    		op_sv_ptr->topo
#define link_interact_pk_num    		op_sv_ptr->link_interact_pk_num
#define evh                     		op_sv_ptr->evh
#define data_num_now            		op_sv_ptr->data_num_now
#define A                       		op_sv_ptr->A
#define dist                    		op_sv_ptr->dist
#define path                    		op_sv_ptr->path
#define mark                    		op_sv_ptr->mark
#define last_topo               		op_sv_ptr->last_topo
#define route                   		op_sv_ptr->route
#define route_num               		op_sv_ptr->route_num
#define node_num                		op_sv_ptr->node_num
#define topo_address            		op_sv_ptr->topo_address

/* These macro definitions will define a local variable called	*/
/* "op_sv_ptr" in each function containing a FIN statement.	*/
/* This variable points to the state variable data structure,	*/
/* and can be used from a C debugger to display their values.	*/
#undef FIN_PREAMBLE_DEC
#undef FIN_PREAMBLE_CODE
#define FIN_PREAMBLE_DEC	net_V1_state *op_sv_ptr;
#define FIN_PREAMBLE_CODE	\
		op_sv_ptr = ((net_V1_state *)(OP_SIM_CONTEXT_PTR->_op_mod_state_ptr));


/* Function Block */

#if !defined (VOSD_NO_FIN)
enum { _op_block_origin = __LINE__ + 2};
#endif

static void Initial(void)
	{
	int i;
	FIN(Initial(void));
	for(i=0;i<24;i++)
		{
		dist[i] = m;
		path[i] = -1;
		mark[i] = 0;
		}
	FOUT;
	}

static int Min(void)
	{
	int l=0,i;
	FIN(Min(void));
	while (mark[l]!=0)
		l++;
	for(i=0;i<24;i++)
		if (mark[i] == 0 && dist[l] > dist[i])
			l = i;
	FRET(l);
	}

static void Update(int start, int last)
	{
	int number = 0, k, i;
	FIN(Update(int start, int last));
	k = start;
	dist[k] = 0;
	number++;
	while (number <= 24)
		{
		for(i=0;i<24;i++)
			{
			if(mark[i] == 0)
				{
				if (dist[i] > dist[k] + A[k][i])
					{
					dist[i] = dist[k] + A[k][i];
					path[i] = k;
					}
				}
			}
		k = Min();
		number++;
		mark[k] = 1;
		}
	FOUT;
	}
	
static void Output(int start, int last)
	{
	int k, i;
	int res[20], n = 0;
	FIN(Output(int start, int last));
	res[n++] = last;
	k = last;
	printf("%d--->%d\n",start,last);
	while (k != start)
	{
		res[n++] = path[k];
		k = path[k];
	}
	printf("\n path:");
	for (i = n - 1; i >= 0; i--)
	{
		if (i == n - 1)
			printf("%d", res[i]);
		else
			printf("->%d", res[i]);
	}
	for(i=0;i<5;i++)
		route[i]=res[i];
	route_num=n;
	FOUT;
}	
		

/* End of Function Block */

/* Undefine optional tracing in FIN/FOUT/FRET */
/* The FSM has its own tracing code and the other */
/* functions should not have any tracing.		  */
#undef FIN_TRACING
#define FIN_TRACING

#undef FOUTRET_TRACING
#define FOUTRET_TRACING

#if defined (__cplusplus)
extern "C" {
#endif
	void net_V1 (OP_SIM_CONTEXT_ARG_OPT);
	VosT_Obtype _op_net_V1_init (int * init_block_ptr);
	void _op_net_V1_diag (OP_SIM_CONTEXT_ARG_OPT);
	void _op_net_V1_terminate (OP_SIM_CONTEXT_ARG_OPT);
	VosT_Address _op_net_V1_alloc (VosT_Obtype, int);
	void _op_net_V1_svar (void *, const char *, void **);


#if defined (__cplusplus)
} /* end of 'extern "C"' */
#endif




/* Process model interrupt handling procedure */


void
net_V1 (OP_SIM_CONTEXT_ARG_OPT)
	{
#if !defined (VOSD_NO_FIN)
	int _op_block_origin = 0;
#endif
	FIN_MT (net_V1 ());

		{
		/* Temporary Variables */
		Packet* pkptr;
		int source;
		int i,j;
		int data_dest,data_type;
		
		/* End of Temporary Variables */


		FSM_ENTER ("net_V1")

		FSM_BLOCK_SWITCH
			{
			/*---------------------------------------------------------*/
			/** state (init) enter executives **/
			FSM_STATE_ENTER_FORCED_NOLABEL (0, "init", "net_V1 [init enter execs]")
				FSM_PROFILE_SECTION_IN ("net_V1 [init enter execs]", state0_enter_exec)
				{
				//initial begin
				my_id = op_id_self();
				my_node_id = op_topo_parent (my_id);
				
				op_ima_obj_attr_get(my_node_id,"Address",&my_address);
				
				interactive_id=4;
				to_interact_next_hop=0xFF;
				interact_pk_num=0;
				link_interact_pk_num=0;
				data_num_now=-1;
				node_num=13;
				
				for(i=0;i<24;i++)
					for(j=0;j<24;j++)
						{
						topo[i][j]=0;
						last_topo[i][j]=0;
						}
				
				for(i=0;i<24;i++)
					{
					topo_address[i][0]=0;
					topo_address[i][1]=0;
					topo_address[i][2]=0;
					}
				
				printf("$$$$$$$$$$$$net over$$$$$$$$$$$$$\n");
				
				
				}
				FSM_PROFILE_SECTION_OUT (state0_enter_exec)

			/** state (init) exit executives **/
			FSM_STATE_EXIT_FORCED (0, "init", "net_V1 [init exit execs]")


			/** state (init) transition processing **/
			FSM_TRANSIT_FORCE (1, state1_enter_exec, ;, "default", "", "init", "idle", "tr_0", "net_V1 [init -> idle : default / ]")
				/*---------------------------------------------------------*/



			/** state (idle) enter executives **/
			FSM_STATE_ENTER_UNFORCED (1, "idle", state1_enter_exec, "net_V1 [idle enter execs]")

			/** blocking after enter executives of unforced state. **/
			FSM_EXIT (3,"net_V1")


			/** state (idle) exit executives **/
			FSM_STATE_EXIT_UNFORCED (1, "idle", "net_V1 [idle exit execs]")


			/** state (idle) transition processing **/
			FSM_PROFILE_SECTION_IN ("net_V1 [idle trans conditions]", state1_trans_conds)
			FSM_INIT_COND (FROM_RX_PK)
			FSM_TEST_COND (FROM_SRC_PK)
			FSM_TEST_COND (COLLECT || TIME_OUT)
			FSM_TEST_COND (FROM_RX2_PK)
			FSM_DFLT_COND
			FSM_TEST_LOGIC ("idle")
			FSM_PROFILE_SECTION_OUT (state1_trans_conds)

			FSM_TRANSIT_SWITCH
				{
				FSM_CASE_TRANSIT (0, 2, state2_enter_exec, ;, "FROM_RX_PK", "", "idle", "rx_rcv", "tr_3", "net_V1 [idle -> rx_rcv : FROM_RX_PK / ]")
				FSM_CASE_TRANSIT (1, 3, state3_enter_exec, ;, "FROM_SRC_PK", "", "idle", "src_rcv", "tr_6", "net_V1 [idle -> src_rcv : FROM_SRC_PK / ]")
				FSM_CASE_TRANSIT (2, 4, state4_enter_exec, ;, "COLLECT || TIME_OUT", "", "idle", "link_state_feedback", "tr_10", "net_V1 [idle -> link_state_feedback : COLLECT || TIME_OUT / ]")
				FSM_CASE_TRANSIT (3, 5, state5_enter_exec, ;, "FROM_RX2_PK", "", "idle", "rx2_rcv", "tr_12", "net_V1 [idle -> rx2_rcv : FROM_RX2_PK / ]")
				FSM_CASE_TRANSIT (4, 1, state1_enter_exec, ;, "default", "", "idle", "idle", "tr_1", "net_V1 [idle -> idle : default / ]")
				}
				/*---------------------------------------------------------*/



			/** state (rx_rcv) enter executives **/
			FSM_STATE_ENTER_FORCED (2, "rx_rcv", state2_enter_exec, "net_V1 [rx_rcv enter execs]")
				FSM_PROFILE_SECTION_IN ("net_V1 [rx_rcv enter execs]", state2_enter_exec)
				{
				int pk_interact_num;
				int nei_num;
				int nei_id[8];
				int nei_longitude;
				int nei_latitude;
				int nei_height;
				Ici* net_ici;
				
				
				for(i=0;i<8;i++) nei_id[i]=0xFF;
				
				//收到不同类型包的行为
				pkptr=op_pk_get(op_intrpt_strm());
				op_pk_nfd_get (pkptr, "TYPE", &type);
				
				
				if(type==0x01)//向交互节点发送的上报帧
					{
					op_pk_nfd_get (pkptr, "Source",&source);
					if(source==my_address && my_address!=interactive_id)
						{
						net_ici = (Ici*)op_intrpt_ici();
						op_ici_attr_get_int32(net_ici, "mac_to_net_ici",&to_interact_next_hop);
						op_ici_destroy(net_ici);
						op_pk_nfd_set(pkptr,"Next_Hop",to_interact_next_hop);
						op_pk_nfd_set(pkptr,"Num",interact_pk_num);
						op_pk_send(pkptr,TX_OUT);
						printf("%d not intact node,ready to send tdma,next hop:%d\n",my_address,to_interact_next_hop);
						interact_pk_num+=1;
						if(interact_pk_num==16) interact_pk_num=0;//Num
						}
					else if(source==my_address && my_address==interactive_id)
						{
						//记录信息
						op_pk_nfd_get (pkptr, "Nei_num",&nei_num);
						op_pk_nfd_get (pkptr, "Nei_address_0", &nei_id[0]);
						op_pk_nfd_get (pkptr, "Nei_address_1", &nei_id[1]);
						op_pk_nfd_get (pkptr, "Nei_address_2", &nei_id[2]);
						op_pk_nfd_get (pkptr, "Nei_address_3", &nei_id[3]);
						op_pk_nfd_get (pkptr, "Nei_address_4", &nei_id[4]);
						op_pk_nfd_get (pkptr, "Nei_address_5", &nei_id[5]);
						op_pk_nfd_get (pkptr, "Nei_address_6", &nei_id[6]);
						op_pk_nfd_get (pkptr, "Nei_address_7", &nei_id[7]);
						op_pk_nfd_get(pkptr,"Longitude", &nei_longitude);
						op_pk_nfd_get(pkptr,"Latitude", &nei_latitude);
						op_pk_nfd_get(pkptr,"Height", &nei_height);
						
						for(i=0;i<nei_num;i++)
							{
							topo[source][nei_id[i]]=1;
							}
						topo_address[source][0]=nei_longitude;
						topo_address[source][1]=nei_latitude;
						topo_address[source][2]=nei_height;
						
						op_pk_destroy(pkptr);
						//开启定时器
						evh=op_intrpt_schedule_self(op_sim_time()+100,TIME_OUT_SIGNAL);////can change!!!
						}
					else//jieshou daode
						{
						op_pk_nfd_get (pkptr, "Num", &pk_interact_num);
						if(my_address!=interactive_id)
							{
							printf("***********\n");
							op_pk_nfd_set (pkptr, "Next_Hop", to_interact_next_hop);
							op_pk_send(pkptr,TX_OUT);
							}
						else/////我是交互节点
							{
							link_interact_pk_num++;
							op_pk_nfd_get (pkptr, "Source",&source);
							op_pk_nfd_get (pkptr, "Nei_num",&nei_num);
							op_pk_nfd_get (pkptr, "Nei_address_0", &nei_id[0]);
							op_pk_nfd_get (pkptr, "Nei_address_1", &nei_id[1]);
							op_pk_nfd_get (pkptr, "Nei_address_2", &nei_id[2]);
							op_pk_nfd_get (pkptr, "Nei_address_3", &nei_id[3]);
							op_pk_nfd_get (pkptr, "Nei_address_4", &nei_id[4]);
							op_pk_nfd_get (pkptr, "Nei_address_5", &nei_id[5]);
							op_pk_nfd_get (pkptr, "Nei_address_6", &nei_id[6]);
							op_pk_nfd_get (pkptr, "Nei_address_7", &nei_id[7]);
							op_pk_nfd_get (pkptr, "Longitude", &nei_longitude);
							op_pk_nfd_get (pkptr, "Latitude", &nei_latitude);
							op_pk_nfd_get (pkptr, "Height", &nei_height);
								
							op_pk_destroy(pkptr);
							printf("intact node get pk:%d\n",source);
							for(i=0;i<nei_num;i++)
								{
								topo[source][nei_id[i]]=1;
								}
							topo_address[source][0]=nei_longitude;
							topo_address[source][1]=nei_latitude;
							topo_address[source][2]=nei_height;
							printf("now have:%d\n",link_interact_pk_num);
								
							if(link_interact_pk_num==node_num-1)
								{
								printf("收集完毕！！！\n");
								op_ev_cancel(evh);
								op_intrpt_schedule_self(op_sim_time(),COLLECT_FINISH);
								}
							}	
						}
					}
				
				if(type==0x12)//D_REP
					{
					op_pk_send(pkptr,TX_OUT_2);
					}
				
				
				
				}
				FSM_PROFILE_SECTION_OUT (state2_enter_exec)

			/** state (rx_rcv) exit executives **/
			FSM_STATE_EXIT_FORCED (2, "rx_rcv", "net_V1 [rx_rcv exit execs]")


			/** state (rx_rcv) transition processing **/
			FSM_TRANSIT_FORCE (1, state1_enter_exec, ;, "default", "", "rx_rcv", "idle", "tr_5", "net_V1 [rx_rcv -> idle : default / ]")
				/*---------------------------------------------------------*/



			/** state (src_rcv) enter executives **/
			FSM_STATE_ENTER_FORCED (3, "src_rcv", state3_enter_exec, "net_V1 [src_rcv enter execs]")

			/** state (src_rcv) exit executives **/
			FSM_STATE_EXIT_FORCED (3, "src_rcv", "net_V1 [src_rcv exit execs]")


			/** state (src_rcv) transition processing **/
			FSM_TRANSIT_FORCE (1, state1_enter_exec, ;, "default", "", "src_rcv", "idle", "tr_7", "net_V1 [src_rcv -> idle : default / ]")
				/*---------------------------------------------------------*/



			/** state (link_state_feedback) enter executives **/
			FSM_STATE_ENTER_FORCED (4, "link_state_feedback", state4_enter_exec, "net_V1 [link_state_feedback enter execs]")
				FSM_PROFILE_SECTION_IN ("net_V1 [link_state_feedback enter execs]", state4_enter_exec)
				{
				//packup
				int z=0;
				int num_elements=9;
				int a_topo[276];
				int int_array[9];
				unsigned char bit_sequence[35] = {0};
				
				printf("ready to pk up to ground\n");
				printf("time:%f\n",op_sim_time());
				for(i=0;i<node_num;i++)
					{
					for(j=0;j<node_num;j++)
						printf("%d   ",topo[i][j]);
					printf("\n");
					}
				
				
				pkptr=op_pk_create_fmt("link_maintain");
				op_pk_nfd_set(pkptr,"TYPE",0x10);
				op_pk_nfd_set(pkptr,"Num",interact_pk_num);
				op_pk_nfd_set(pkptr,"None",0);
				
				for(i=0;i<23;i++)
					{
					for(j=i+1;j<24;j++)
						{
						if(topo[i][j]==1 || topo[j][i]==1)
				            a_topo[z]=1;
				        else
				            a_topo[z]=0;
						z++;
						}
					}
				for(i=0;i<276;i++)
					{
					if(a_topo[i]==1)
				        bit_sequence[i / 8] |= (1 << (i % 8));
				    else
				        bit_sequence[i / 8] &= ~(1 << (i % 8));
					}
				for(i=0;i<num_elements;i++)
					{
					int_array[i]=0;
					for(j=0;j<4;j++)
				        int_array[i] |= (bit_sequence[i * 4 + j] << (8 * (3- j)));
					}
				
				op_pk_nfd_set(pkptr,"Net_Topo1",int_array[0]);
				op_pk_nfd_set(pkptr,"Net_Topo2",int_array[1]);
				op_pk_nfd_set(pkptr,"Net_Topo3",int_array[2]);
				op_pk_nfd_set(pkptr,"Net_Topo4",int_array[3]);
				op_pk_nfd_set(pkptr,"Net_Topo5",int_array[4]);
				op_pk_nfd_set(pkptr,"Net_Topo6",int_array[5]);
				op_pk_nfd_set(pkptr,"Net_Topo7",int_array[6]);
				op_pk_nfd_set(pkptr,"Net_Topo8",int_array[7]);
				op_pk_nfd_set(pkptr,"Net_Topo9",int_array[8]);
				
				op_pk_nfd_set(pkptr,"Nei_longtitude0",topo_address[0][0]);
				op_pk_nfd_set(pkptr,"Nei_longtitude1",topo_address[1][0]);
				op_pk_nfd_set(pkptr,"Nei_longtitude2",topo_address[2][0]);
				op_pk_nfd_set(pkptr,"Nei_longtitude3",topo_address[3][0]);
				op_pk_nfd_set(pkptr,"Nei_longtitude4",topo_address[4][0]);
				op_pk_nfd_set(pkptr,"Nei_longtitude5",topo_address[5][0]);
				op_pk_nfd_set(pkptr,"Nei_longtitude6",topo_address[6][0]);
				op_pk_nfd_set(pkptr,"Nei_longtitude7",topo_address[7][0]);
				op_pk_nfd_set(pkptr,"Nei_longtitude8",topo_address[8][0]);
				op_pk_nfd_set(pkptr,"Nei_longtitude9",topo_address[9][0]);
				op_pk_nfd_set(pkptr,"Nei_longtitude10",topo_address[10][0]);
				op_pk_nfd_set(pkptr,"Nei_longtitude11",topo_address[11][0]);
				op_pk_nfd_set(pkptr,"Nei_longtitude12",topo_address[12][0]);
				op_pk_nfd_set(pkptr,"Nei_longtitude13",topo_address[13][0]);
				op_pk_nfd_set(pkptr,"Nei_longtitude14",topo_address[14][0]);
				op_pk_nfd_set(pkptr,"Nei_longtitude15",topo_address[15][0]);
				op_pk_nfd_set(pkptr,"Nei_longtitude16",topo_address[16][0]);
				op_pk_nfd_set(pkptr,"Nei_longtitude17",topo_address[17][0]);
				op_pk_nfd_set(pkptr,"Nei_longtitude18",topo_address[18][0]);
				op_pk_nfd_set(pkptr,"Nei_longtitude19",topo_address[19][0]);
				op_pk_nfd_set(pkptr,"Nei_longtitude20",topo_address[20][0]);
				op_pk_nfd_set(pkptr,"Nei_longtitude21",topo_address[21][0]);
				op_pk_nfd_set(pkptr,"Nei_longtitude22",topo_address[22][0]);
				op_pk_nfd_set(pkptr,"Nei_longtitude23",topo_address[23][0]);
				
				op_pk_nfd_set(pkptr,"Nei_latitude0",topo_address[0][1]);
				op_pk_nfd_set(pkptr,"Nei_latitude1",topo_address[1][1]);
				op_pk_nfd_set(pkptr,"Nei_latitude2",topo_address[2][1]);
				op_pk_nfd_set(pkptr,"Nei_latitude3",topo_address[3][1]);
				op_pk_nfd_set(pkptr,"Nei_latitude4",topo_address[4][1]);
				op_pk_nfd_set(pkptr,"Nei_latitude5",topo_address[5][1]);
				op_pk_nfd_set(pkptr,"Nei_latitude6",topo_address[6][1]);
				op_pk_nfd_set(pkptr,"Nei_latitude7",topo_address[7][1]);
				op_pk_nfd_set(pkptr,"Nei_latitude8",topo_address[8][1]);
				op_pk_nfd_set(pkptr,"Nei_latitude9",topo_address[9][1]);
				op_pk_nfd_set(pkptr,"Nei_latitude10",topo_address[10][1]);
				op_pk_nfd_set(pkptr,"Nei_latitude11",topo_address[11][1]);
				op_pk_nfd_set(pkptr,"Nei_latitude12",topo_address[12][1]);
				op_pk_nfd_set(pkptr,"Nei_latitude13",topo_address[13][1]);
				op_pk_nfd_set(pkptr,"Nei_latitude14",topo_address[14][1]);
				op_pk_nfd_set(pkptr,"Nei_latitude15",topo_address[15][1]);
				op_pk_nfd_set(pkptr,"Nei_latitude16",topo_address[16][1]);
				op_pk_nfd_set(pkptr,"Nei_latitude17",topo_address[17][1]);
				op_pk_nfd_set(pkptr,"Nei_latitude18",topo_address[18][1]);
				op_pk_nfd_set(pkptr,"Nei_latitude19",topo_address[19][1]);
				op_pk_nfd_set(pkptr,"Nei_latitude20",topo_address[20][1]);
				op_pk_nfd_set(pkptr,"Nei_latitude21",topo_address[21][1]);
				op_pk_nfd_set(pkptr,"Nei_latitude22",topo_address[22][1]);
				op_pk_nfd_set(pkptr,"Nei_latitude23",topo_address[23][1]);
				
				op_pk_nfd_set(pkptr,"Nei_height0",topo_address[0][2]);
				op_pk_nfd_set(pkptr,"Nei_height1",topo_address[1][2]);
				op_pk_nfd_set(pkptr,"Nei_height2",topo_address[2][2]);
				op_pk_nfd_set(pkptr,"Nei_height3",topo_address[3][2]);
				op_pk_nfd_set(pkptr,"Nei_height4",topo_address[4][2]);
				op_pk_nfd_set(pkptr,"Nei_height5",topo_address[5][2]);
				op_pk_nfd_set(pkptr,"Nei_height6",topo_address[6][2]);
				op_pk_nfd_set(pkptr,"Nei_height7",topo_address[7][2]);
				op_pk_nfd_set(pkptr,"Nei_height8",topo_address[8][2]);
				op_pk_nfd_set(pkptr,"Nei_height9",topo_address[9][2]);
				op_pk_nfd_set(pkptr,"Nei_height10",topo_address[10][2]);
				op_pk_nfd_set(pkptr,"Nei_height11",topo_address[11][2]);
				op_pk_nfd_set(pkptr,"Nei_height12",topo_address[12][2]);
				op_pk_nfd_set(pkptr,"Nei_height13",topo_address[13][2]);
				op_pk_nfd_set(pkptr,"Nei_height14",topo_address[14][2]);
				op_pk_nfd_set(pkptr,"Nei_height15",topo_address[15][2]);
				op_pk_nfd_set(pkptr,"Nei_height16",topo_address[16][2]);
				op_pk_nfd_set(pkptr,"Nei_height17",topo_address[17][2]);
				op_pk_nfd_set(pkptr,"Nei_height18",topo_address[18][2]);
				op_pk_nfd_set(pkptr,"Nei_height19",topo_address[19][2]);
				op_pk_nfd_set(pkptr,"Nei_height20",topo_address[20][2]);
				op_pk_nfd_set(pkptr,"Nei_height21",topo_address[21][2]);
				op_pk_nfd_set(pkptr,"Nei_height22",topo_address[22][2]);
				op_pk_nfd_set(pkptr,"Nei_height23",topo_address[23][2]);
				
				op_pk_print(pkptr);
				op_pk_send(pkptr,TX_OUT_2);
				
				//ready for next circle
				interact_pk_num+=1;
				if(interact_pk_num==16) interact_pk_num=0;//Num
				
				link_interact_pk_num=0;//pk collection
				
				for(i=0;i<24;i++)
					for(j=0;j<24;j++) //topo
						{
						topo[i][j]=0;
						}
				}
				FSM_PROFILE_SECTION_OUT (state4_enter_exec)

			/** state (link_state_feedback) exit executives **/
			FSM_STATE_EXIT_FORCED (4, "link_state_feedback", "net_V1 [link_state_feedback exit execs]")


			/** state (link_state_feedback) transition processing **/
			FSM_TRANSIT_FORCE (1, state1_enter_exec, ;, "default", "", "link_state_feedback", "idle", "tr_11", "net_V1 [link_state_feedback -> idle : default / ]")
				/*---------------------------------------------------------*/



			/** state (rx2_rcv) enter executives **/
			FSM_STATE_ENTER_FORCED (5, "rx2_rcv", state5_enter_exec, "net_V1 [rx2_rcv enter execs]")
				FSM_PROFILE_SECTION_IN ("net_V1 [rx2_rcv enter execs]", state5_enter_exec)
				{
				int a_topo[276];
				int num_elements=9;
				int int_array[9];
				unsigned char bit_sequence[35];
				int z;
				int num;
				
				pkptr =  op_pk_get (op_intrpt_strm());
				op_pk_nfd_get (pkptr, "TYPE", &type);
				
				if(type==0x12)//D_REP
					{
					//need nei_num;nei8(send_to_mac0)
					op_pk_send(pkptr,TX_OUT);
					}
				
				if(type==0x14)//Cross
					{
					op_pk_nfd_get(pkptr,"Interact_ID",&interactive_id);
					op_pk_nfd_get(pkptr,"Num",&num);
					
					z=0;
					op_pk_nfd_get(pkptr,"Net_Topo1",&int_array[0]);
					op_pk_nfd_get(pkptr,"Net_Topo2",&int_array[1]);
					op_pk_nfd_get(pkptr,"Net_Topo3",&int_array[2]);
					op_pk_nfd_get(pkptr,"Net_Topo4",&int_array[3]);
					op_pk_nfd_get(pkptr,"Net_Topo5",&int_array[4]);
					op_pk_nfd_get(pkptr,"Net_Topo6",&int_array[5]);
					op_pk_nfd_get(pkptr,"Net_Topo7",&int_array[6]);
					op_pk_nfd_get(pkptr,"Net_Topo8",&int_array[7]);
					op_pk_nfd_get(pkptr,"Net_Topo9",&int_array[8]);
					for(i = 0; i < num_elements; i++)
						for(j = 0; j <4; j++)
							bit_sequence[i * 4 + j] = (int_array[i] >> (8 * (3 - j))) & 0xFF;
					for (i=0;i<276;i++)
				        a_topo[i] = (bit_sequence[i / 8] >> (i % 8)) & 1;
					//组织topo
					for(i=0;i<24;i++)
						for(j=0;j<24;j++) topo[i][j]=0;
					for(i=0;i<23;i++)
						for(j=i+1;j<24;j++)
							{
							topo[i][j]=a_topo[z];
							topo[j][i]=a_topo[z];
							z++;
							}
					printf("new intact node get the topo:\n");
					for(i=0;i<13;i++)///////////////////////test for 13
					{
					for(j=0;j<13;j++)
						printf("%d   ",topo[i][j]);
					printf("\n");
					}
					op_pk_destroy(pkptr);
					
					//yinggai fasong 0x01
					pkptr=op_pk_create_fmt("limk_maintain_f");
					op_pk_nfd_set(pkptr,"SEND",my_address);
					op_pk_nfd_set(pkptr,"TYPE",1);
					op_pk_nfd_set(pkptr,"Intact_node",interactive_id);
					op_pk_nfd_set(pkptr,"Num",num);
					op_pk_send(pkptr,TX_OUT);
					}
				
				if(type==0x15)//appointment_req_G
					{
					op_pk_nfd_get(pkptr,"DEST_ID",&data_dest);
					op_pk_nfd_get(pkptr,"Num",&data_num_now);
					op_pk_nfd_get(pkptr,"Data_Type",&data_type);
					//lujing guihua
					for(i=0;i<24;i++)
						for(j=0;j<24;j++)
							{
							if(topo[i][j]==0) A[i][j]=m;
							else A[i][j]=1;
							}
					Initial();
					Update(interactive_id, data_dest);
					Output(interactive_id, data_dest);
					printf("route:");
					for(i=0;i<route_num;i++)
						printf("%d ",route[i]);
					//shixi guihua
					
					
					//dabao fasong
					}
				}
				FSM_PROFILE_SECTION_OUT (state5_enter_exec)

			/** state (rx2_rcv) exit executives **/
			FSM_STATE_EXIT_FORCED (5, "rx2_rcv", "net_V1 [rx2_rcv exit execs]")


			/** state (rx2_rcv) transition processing **/
			FSM_TRANSIT_FORCE (1, state1_enter_exec, ;, "default", "", "rx2_rcv", "idle", "tr_13", "net_V1 [rx2_rcv -> idle : default / ]")
				/*---------------------------------------------------------*/



			}


		FSM_EXIT (0,"net_V1")
		}
	}




void
_op_net_V1_diag (OP_SIM_CONTEXT_ARG_OPT)
	{
	/* No Diagnostic Block */
	}




void
_op_net_V1_terminate (OP_SIM_CONTEXT_ARG_OPT)
	{

	FIN_MT (_op_net_V1_terminate ())


	/* No Termination Block */

	Vos_Poolmem_Dealloc (op_sv_ptr);

	FOUT
	}


/* Undefine shortcuts to state variables to avoid */
/* syntax error in direct access to fields of */
/* local variable prs_ptr in _op_net_V1_svar function. */
#undef my_address
#undef my_id
#undef my_node_id
#undef type
#undef interactive_id
#undef to_interact_next_hop
#undef interact_pk_num
#undef topo
#undef link_interact_pk_num
#undef evh
#undef data_num_now
#undef A
#undef dist
#undef path
#undef mark
#undef last_topo
#undef route
#undef route_num
#undef node_num
#undef topo_address

#undef FIN_PREAMBLE_DEC
#undef FIN_PREAMBLE_CODE

#define FIN_PREAMBLE_DEC
#define FIN_PREAMBLE_CODE

VosT_Obtype
_op_net_V1_init (int * init_block_ptr)
	{
	VosT_Obtype obtype = OPC_NIL;
	FIN_MT (_op_net_V1_init (init_block_ptr))

	obtype = Vos_Define_Object_Prstate ("proc state vars (net_V1)",
		sizeof (net_V1_state));
	*init_block_ptr = 0;

	FRET (obtype)
	}

VosT_Address
_op_net_V1_alloc (VosT_Obtype obtype, int init_block)
	{
#if !defined (VOSD_NO_FIN)
	int _op_block_origin = 0;
#endif
	net_V1_state * ptr;
	FIN_MT (_op_net_V1_alloc (obtype))

	ptr = (net_V1_state *)Vos_Alloc_Object (obtype);
	if (ptr != OPC_NIL)
		{
		ptr->_op_current_block = init_block;
#if defined (OPD_ALLOW_ODB)
		ptr->_op_current_state = "net_V1 [init enter execs]";
#endif
		}
	FRET ((VosT_Address)ptr)
	}



void
_op_net_V1_svar (void * gen_ptr, const char * var_name, void ** var_p_ptr)
	{
	net_V1_state		*prs_ptr;

	FIN_MT (_op_net_V1_svar (gen_ptr, var_name, var_p_ptr))

	if (var_name == OPC_NIL)
		{
		*var_p_ptr = (void *)OPC_NIL;
		FOUT
		}
	prs_ptr = (net_V1_state *)gen_ptr;

	if (strcmp ("my_address" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->my_address);
		FOUT
		}
	if (strcmp ("my_id" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->my_id);
		FOUT
		}
	if (strcmp ("my_node_id" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->my_node_id);
		FOUT
		}
	if (strcmp ("type" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->type);
		FOUT
		}
	if (strcmp ("interactive_id" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->interactive_id);
		FOUT
		}
	if (strcmp ("to_interact_next_hop" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->to_interact_next_hop);
		FOUT
		}
	if (strcmp ("interact_pk_num" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->interact_pk_num);
		FOUT
		}
	if (strcmp ("topo" , var_name) == 0)
		{
		*var_p_ptr = (void *) (prs_ptr->topo);
		FOUT
		}
	if (strcmp ("link_interact_pk_num" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->link_interact_pk_num);
		FOUT
		}
	if (strcmp ("evh" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->evh);
		FOUT
		}
	if (strcmp ("data_num_now" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->data_num_now);
		FOUT
		}
	if (strcmp ("A" , var_name) == 0)
		{
		*var_p_ptr = (void *) (prs_ptr->A);
		FOUT
		}
	if (strcmp ("dist" , var_name) == 0)
		{
		*var_p_ptr = (void *) (prs_ptr->dist);
		FOUT
		}
	if (strcmp ("path" , var_name) == 0)
		{
		*var_p_ptr = (void *) (prs_ptr->path);
		FOUT
		}
	if (strcmp ("mark" , var_name) == 0)
		{
		*var_p_ptr = (void *) (prs_ptr->mark);
		FOUT
		}
	if (strcmp ("last_topo" , var_name) == 0)
		{
		*var_p_ptr = (void *) (prs_ptr->last_topo);
		FOUT
		}
	if (strcmp ("route" , var_name) == 0)
		{
		*var_p_ptr = (void *) (prs_ptr->route);
		FOUT
		}
	if (strcmp ("route_num" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->route_num);
		FOUT
		}
	if (strcmp ("node_num" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->node_num);
		FOUT
		}
	if (strcmp ("topo_address" , var_name) == 0)
		{
		*var_p_ptr = (void *) (prs_ptr->topo_address);
		FOUT
		}
	*var_p_ptr = (void *)OPC_NIL;

	FOUT
	}

