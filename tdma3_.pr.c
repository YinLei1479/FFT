/* Process model C form file: tdma3_.pr.c */
/* Portions of this file copyright 1986-2008 by OPNET Technologies, Inc. */



/* This variable carries the header into the object file */
const char tdma3__pr_c [] = "MIL_3_Tfile_Hdr_ 145A 30A op_runsim 7 66EA469F 66EA469F 1 ray-laptop 28918 0 0 none none 0 0 none 0 0 0 0 0 0 0 0 1bcc 1                                                                                                                                                                                                                                                                                                                                                                                                       ";
#include <string.h>



/* OPNET system definitions */
#include <opnet.h>



/* Header Block */

#include <math.h>

/* Constant Definitions */
#define RX_IN_STRM		(1)
#define SRC_IN_STRM		(0)
#define TX_OUT_STRM		(1)
#define SINK_OUT_STRM	(0)

#define EPSILON  		(1e-10)  /* rounding error factor */
#define TDMA_COMPLETE	(-10)
#define FRAME_BEGIN		(2000)
#define INTACT_BEGIN	(4000)



/* Transition Condition Macros */ 
#define NET_BUILD		(op_intrpt_type() == OPC_INTRPT_SELF) && (op_intrpt_code () == 11)
#define NET_IN			(op_intrpt_type() == OPC_INTRPT_SELF) && (op_intrpt_code () == 1111)
#define RX_STRM			(op_intrpt_type() == OPC_INTRPT_STRM) && (op_intrpt_strm() == RX_IN_STRM)
#define FROM_RX			(current_intrpt_type == OPC_INTRPT_STRM) && (op_intrpt_strm () == RX_IN_STRM)
#define FROM_SRC 		(current_intrpt_type == OPC_INTRPT_STRM) && (op_intrpt_strm () == SRC_IN_STRM) 
#define TRANSMITTING	(op_stat_local_read (0) == 1.0) 
#define SLOT 			(current_intrpt_type == OPC_INTRPT_SELF) && (op_intrpt_code () == 0)
#define MY_SLOT 		(current_intrpt_type == OPC_INTRPT_SELF) && (op_intrpt_code () == 3000)
#define NO_DATA 		((op_subq_empty (0)) && (op_subq_empty (1)) && (op_subq_empty (2)))

#define	SELF_INTRPT_SCHLD	(intrpt_flag == 1)

/* Global Variables */
int		tdma_pk_sent;
int		tdma_pk_rcvd;
int		tdma_bits_sent;
int		tdma_bits_rcvd;
int		tdma_setup;
int		tdma_id;


static void QWZ_create(void);
static void QWZ_rcv(Packet* pkptr);


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
	int	                    		my_offset                                       ;
	double	                 		slot_length                                     ;
	double	                 		tx_data_rate                                    ;
	int	                    		intrpt_flag                                     ;
	int	                    		num_pk_sent                                     ;
	int	                    		num_pk_rcvd                                     ;
	int	                    		num_bits_sent                                   ;
	int	                    		num_bits_rcvd                                   ;
	Stathandle	             		num_pk_sent_stat                                ;
	Stathandle	             		global_pk_sent_stat                             ;
	Stathandle	             		num_pk_rcvd_stat                                ;
	Stathandle	             		global_pk_rcvd_stat                             ;
	Objid	                  		my_node_id                                      ;
	Objid	                  		my_id                                           ;
	Stathandle	             		num_bits_sent_stat                              ;
	Stathandle	             		global_bits_sent_stat                           ;
	Stathandle	             		num_bits_rcvd_stat                              ;
	Stathandle	             		global_bits_rcvd_stat                           ;
	Stathandle	             		bits_sec_rcvd_stat                              ;
	Stathandle	             		bits_sec_sent_stat                              ;
	Stathandle	             		pk_sec_rcvd_stat                                ;
	Stathandle	             		pk_sec_sent_stat                                ;
	Stathandle	             		global_bits_sec_rcvd_stat                       ;
	Stathandle	             		global_bits_sec_sent_stat                       ;
	Stathandle	             		global_pk_sec_rcvd_stat                         ;
	Stathandle	             		global_pk_sec_sent_stat                         ;
	int	                    		my_address                                      ;
	int	                    		type                                            ;
	int	                    		my_neighbor[24]                                 ;	/* neighbor address */
	int	                    		nei_count                                       ;	/* number of neighbor */
	int	                    		my_two_nei_count_sum                            ;	/* two hop neighbor number */
	int	                    		my_two_nei[24]                                  ;	/* two hop neighbor address */
	int	                    		is_my_slot                                      ;	/* 0 or 1 for is my slot ? */
	int	                    		my_slot_end                                     ;
	int	                    		interactive_id                                  ;	/* interactive node address */
	Evhandle	               		evh1                                            ;	/* initiai wait self intrpt */
	int	                    		WAIT                                            ;	/* need still to wait */
	int	                    		num_slots                                       ;	/* all slot number */
	int	                    		my_node_state                                   ;
	int	                    		my_clock_level                                  ;
	int	                    		a_frame_record[8][7]                            ;	/* record in a frame                                 */
	                        		                                                	/* node_id + nei_offset + CL + Location + node_state */
	int	                    		INTACT_FLAG                                     ;	/* interact intrpt is coming */
	int	                    		a_frame_record_last[8][7]                       ;	/* neighbor in last frame */
	int	                    		nei_num_last                                    ;
	int	                    		node_num                                        ;
	double	                 		Longitude                                       ;
	double	                 		Latitude                                        ;
	double	                 		Height                                          ;
	int	                    		time[24]                                        ;
	} tdma3__state;

#define my_offset               		op_sv_ptr->my_offset
#define slot_length             		op_sv_ptr->slot_length
#define tx_data_rate            		op_sv_ptr->tx_data_rate
#define intrpt_flag             		op_sv_ptr->intrpt_flag
#define num_pk_sent             		op_sv_ptr->num_pk_sent
#define num_pk_rcvd             		op_sv_ptr->num_pk_rcvd
#define num_bits_sent           		op_sv_ptr->num_bits_sent
#define num_bits_rcvd           		op_sv_ptr->num_bits_rcvd
#define num_pk_sent_stat        		op_sv_ptr->num_pk_sent_stat
#define global_pk_sent_stat     		op_sv_ptr->global_pk_sent_stat
#define num_pk_rcvd_stat        		op_sv_ptr->num_pk_rcvd_stat
#define global_pk_rcvd_stat     		op_sv_ptr->global_pk_rcvd_stat
#define my_node_id              		op_sv_ptr->my_node_id
#define my_id                   		op_sv_ptr->my_id
#define num_bits_sent_stat      		op_sv_ptr->num_bits_sent_stat
#define global_bits_sent_stat   		op_sv_ptr->global_bits_sent_stat
#define num_bits_rcvd_stat      		op_sv_ptr->num_bits_rcvd_stat
#define global_bits_rcvd_stat   		op_sv_ptr->global_bits_rcvd_stat
#define bits_sec_rcvd_stat      		op_sv_ptr->bits_sec_rcvd_stat
#define bits_sec_sent_stat      		op_sv_ptr->bits_sec_sent_stat
#define pk_sec_rcvd_stat        		op_sv_ptr->pk_sec_rcvd_stat
#define pk_sec_sent_stat        		op_sv_ptr->pk_sec_sent_stat
#define global_bits_sec_rcvd_stat		op_sv_ptr->global_bits_sec_rcvd_stat
#define global_bits_sec_sent_stat		op_sv_ptr->global_bits_sec_sent_stat
#define global_pk_sec_rcvd_stat 		op_sv_ptr->global_pk_sec_rcvd_stat
#define global_pk_sec_sent_stat 		op_sv_ptr->global_pk_sec_sent_stat
#define my_address              		op_sv_ptr->my_address
#define type                    		op_sv_ptr->type
#define my_neighbor             		op_sv_ptr->my_neighbor
#define nei_count               		op_sv_ptr->nei_count
#define my_two_nei_count_sum    		op_sv_ptr->my_two_nei_count_sum
#define my_two_nei              		op_sv_ptr->my_two_nei
#define is_my_slot              		op_sv_ptr->is_my_slot
#define my_slot_end             		op_sv_ptr->my_slot_end
#define interactive_id          		op_sv_ptr->interactive_id
#define evh1                    		op_sv_ptr->evh1
#define WAIT                    		op_sv_ptr->WAIT
#define num_slots               		op_sv_ptr->num_slots
#define my_node_state           		op_sv_ptr->my_node_state
#define my_clock_level          		op_sv_ptr->my_clock_level
#define a_frame_record          		op_sv_ptr->a_frame_record
#define INTACT_FLAG             		op_sv_ptr->INTACT_FLAG
#define a_frame_record_last     		op_sv_ptr->a_frame_record_last
#define nei_num_last            		op_sv_ptr->nei_num_last
#define node_num                		op_sv_ptr->node_num
#define Longitude               		op_sv_ptr->Longitude
#define Latitude                		op_sv_ptr->Latitude
#define Height                  		op_sv_ptr->Height
#define time                    		op_sv_ptr->time

/* These macro definitions will define a local variable called	*/
/* "op_sv_ptr" in each function containing a FIN statement.	*/
/* This variable points to the state variable data structure,	*/
/* and can be used from a C debugger to display their values.	*/
#undef FIN_PREAMBLE_DEC
#undef FIN_PREAMBLE_CODE
#define FIN_PREAMBLE_DEC	tdma3__state *op_sv_ptr;
#define FIN_PREAMBLE_CODE	\
		op_sv_ptr = ((tdma3__state *)(OP_SIM_CONTEXT_PTR->_op_mod_state_ptr));


/* Function Block */

#if !defined (VOSD_NO_FIN)
enum { _op_block_origin = __LINE__ + 2};
#endif

static void QWZ_create(void)//QWZ_CREAT
	{
	Packet* pkptr;
	int min;
	int intact_next;
	int i;
	
	FIN(QWZ_create())

	pkptr=op_pk_create_fmt("QWZ");

	op_pk_nfd_set (pkptr, "TYPE", 0x00);
	op_pk_nfd_set (pkptr, "node_state", my_node_state);
	op_pk_nfd_set (pkptr, "Nei_num", nei_count);
	op_pk_nfd_set (pkptr, "Slot", my_offset);
	op_pk_nfd_set (pkptr, "Nei_address_0",a_frame_record[0][0]);
	op_pk_nfd_set (pkptr, "Nei_address_1",a_frame_record[1][0]);
	op_pk_nfd_set (pkptr, "Nei_address_2",a_frame_record[2][0]);
	op_pk_nfd_set (pkptr, "Nei_address_3",a_frame_record[3][0]);
	op_pk_nfd_set (pkptr, "Nei_address_4",a_frame_record[4][0]);
	op_pk_nfd_set (pkptr, "Nei_address_5",a_frame_record[5][0]);
	op_pk_nfd_set (pkptr, "Nei_address_6",a_frame_record[6][0]);
	op_pk_nfd_set (pkptr, "Nei_address_7",a_frame_record[7][0]);
	op_pk_nfd_set (pkptr, "Longitude",Longitude);
	op_pk_nfd_set (pkptr, "Latitude",Latitude);
	op_pk_nfd_set (pkptr, "Height",Height);
	
	min=0xFF;
	for(i=0;i<8;i++)
		{
		if(a_frame_record[i][0]!=0xFF && a_frame_record[i][2]<min)
			{
			min=a_frame_record[i][2];
			intact_next=a_frame_record[i][0];
			}
		}
	if(my_clock_level>min) my_clock_level=min+1;
	
	op_pk_nfd_set (pkptr, "Clock_level", my_clock_level);
	
	op_subq_pk_insert (1, pkptr, OPC_QPOS_TAIL);
	
	FOUT
	}



static void QWZ_rcv(Packet* pkptr)//QWZ_RCV
	{
	int nei_id;
	int nei_slot;
	int nei_CL;
	int nei_state;
	int two_nei_num;
	int two_nei_id[8];
	int nei_longitude;
	int nei_latitude;
	int nei_height;
	int i;
	
	FIN(QWZ_rcv(Packet* pkptr))
	
	op_pk_nfd_get (pkptr, "SEND", &nei_id);
	op_pk_nfd_get (pkptr, "Slot", &nei_slot);
	op_pk_nfd_get (pkptr, "node_state", &nei_state);
	op_pk_nfd_get (pkptr, "Clock_level", &nei_CL);
	op_pk_nfd_get (pkptr, "Longitude", &nei_longitude);
	op_pk_nfd_get (pkptr, "Latitude", &nei_latitude);
	op_pk_nfd_get (pkptr, "Height", &nei_height);
	
	
	//record
	a_frame_record[nei_count][0]=nei_id;
	a_frame_record[nei_count][1]=nei_slot;
	a_frame_record[nei_count][2]=nei_CL;
	a_frame_record[nei_count][3]=nei_longitude;
	a_frame_record[nei_count][4]=nei_latitude;
	a_frame_record[nei_count][5]=nei_height;
	a_frame_record[nei_count][6]=nei_state;
	nei_count++;
				
	//neighbor			
	op_pk_nfd_get (pkptr, "Nei_num", &two_nei_num);
	op_pk_nfd_get (pkptr, "Nei_address_0", &two_nei_id[0]);
	op_pk_nfd_get (pkptr, "Nei_address_1", &two_nei_id[1]);
	op_pk_nfd_get (pkptr, "Nei_address_2", &two_nei_id[2]);
	op_pk_nfd_get (pkptr, "Nei_address_3", &two_nei_id[3]);
	op_pk_nfd_get (pkptr, "Nei_address_4", &two_nei_id[4]);
	op_pk_nfd_get (pkptr, "Nei_address_5", &two_nei_id[5]);
	op_pk_nfd_get (pkptr, "Nei_address_6", &two_nei_id[6]);
	op_pk_nfd_get (pkptr, "Nei_address_7", &two_nei_id[7]);
	for(i=0;i<two_nei_num;i++)
		my_two_nei[two_nei_id[i]]=2;
	
	//节点状态更改
	for(i=0;i<two_nei_num;i++)
		{
		if(two_nei_id[i]==my_address && my_node_state==1)
			my_node_state=2;
		}
	//begin node
	if(my_node_state==1)
		{
		my_offset=((nei_slot+(my_address-nei_id))%node_num+node_num)%node_num;
		time[my_offset]=1;
		}
	
	op_pk_destroy(pkptr);
	
	FOUT
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
	void tdma3_ (OP_SIM_CONTEXT_ARG_OPT);
	VosT_Obtype _op_tdma3__init (int * init_block_ptr);
	void _op_tdma3__diag (OP_SIM_CONTEXT_ARG_OPT);
	void _op_tdma3__terminate (OP_SIM_CONTEXT_ARG_OPT);
	VosT_Address _op_tdma3__alloc (VosT_Obtype, int);
	void _op_tdma3__svar (void *, const char *, void **);


#if defined (__cplusplus)
} /* end of 'extern "C"' */
#endif




/* Process model interrupt handling procedure */


void
tdma3_ (OP_SIM_CONTEXT_ARG_OPT)
	{
#if !defined (VOSD_NO_FIN)
	int _op_block_origin = 0;
#endif
	FIN_MT (tdma3_ ());

		{
		/* Temporary Variables */
		Packet*	pkptr;
		
		Objid 	tx_id, comp_id, tx_ch_id; 
		
		//double  floor();
		//double  fmod();
		
		int	used_slots;
		int	current_offset;
		int	next_offset;
		
		
		double	current_time;
		double	time_left_in_slot;
		double	pk_len;
		double	pk_time;
		
		double	my_next_slot_time;
		
		int		current_intrpt_type;
		
		int i,j;
		int nei_temp[8];
		/* End of Temporary Variables */


		FSM_ENTER ("tdma3_")

		FSM_BLOCK_SWITCH
			{
			/*---------------------------------------------------------*/
			/** state (init) enter executives **/
			FSM_STATE_ENTER_FORCED_NOLABEL (0, "init", "tdma3_ [init enter execs]")
				FSM_PROFILE_SECTION_IN ("tdma3_ [init enter execs]", state0_enter_exec)
				{
				/*important setting*/
				op_ima_sim_attr_get (OPC_IMA_DOUBLE, "Slot Length", &slot_length);
				interactive_id=4;
				node_num=13;
				
				/*initial begin*/
				my_id = op_id_self();
				my_node_id = op_topo_parent (my_id);
				num_slots = node_num;
				op_ima_obj_attr_get(my_node_id,"Address",&my_address);
				if(my_address==interactive_id) my_offset=0;
				if(my_address==interactive_id) my_clock_level=0; else my_clock_level=7;
				if(my_address==interactive_id) my_node_state=7; else my_node_state=1;
				nei_count=0;
				nei_num_last=0;
				is_my_slot=0;
				for(i=0;i<8;i++)
					{
					for(j=0;j<7;j++)
						{
						a_frame_record[i][j]=0xFF;
						a_frame_record_last[i][j]=0xFF;
						}
					}
				for(i=0;i<24;i++) my_two_nei[i]=0;
				for(i=0;i<24;i++) time[i]=0;
				if(my_address==interactive_id) time[0]=1;
				Longitude=0;
				Latitude=0;
				Height=0;
				
				/* Determine the data rate for the transmitter */
				tx_id =  op_topo_assoc (my_id, OPC_TOPO_ASSOC_OUT, OPC_OBJTYPE_RATX, 0); 
				comp_id = op_topo_child (tx_id, OPC_OBJTYPE_COMP, 0);
				tx_ch_id = op_topo_child (comp_id, OPC_OBJTYPE_RATXCH, 0);
				op_ima_obj_attr_get (tx_ch_id, "data rate", &tx_data_rate); 
				
				
				/* Schedule interupt to complete initialization in the exit execs */
				printf("%%%%%%%%%%% %d tdma init is over%%%%%%%%%%%%%\n",my_address);
				op_intrpt_schedule_self (op_sim_time(), 0);//slot
				
				}
				FSM_PROFILE_SECTION_OUT (state0_enter_exec)

			/** state (init) exit executives **/
			FSM_STATE_EXIT_FORCED (0, "init", "tdma3_ [init exit execs]")


			/** state (init) transition processing **/
			FSM_TRANSIT_FORCE (1, state1_enter_exec, ;, "default", "", "init", "idle", "tr_91", "tdma3_ [init -> idle : default / ]")
				/*---------------------------------------------------------*/



			/** state (idle) enter executives **/
			FSM_STATE_ENTER_UNFORCED (1, "idle", state1_enter_exec, "tdma3_ [idle enter execs]")

			/** blocking after enter executives of unforced state. **/
			FSM_EXIT (3,"tdma3_")


			/** state (idle) exit executives **/
			FSM_STATE_EXIT_UNFORCED (1, "idle", "tdma3_ [idle exit execs]")
				FSM_PROFILE_SECTION_IN ("tdma3_ [idle exit execs]", state1_exit_exec)
				{
				current_intrpt_type = op_intrpt_type ();
				}
				FSM_PROFILE_SECTION_OUT (state1_exit_exec)


			/** state (idle) transition processing **/
			FSM_PROFILE_SECTION_IN ("tdma3_ [idle trans conditions]", state1_trans_conds)
			FSM_INIT_COND (FROM_RX)
			FSM_TEST_COND (FROM_SRC)
			FSM_TEST_COND (SLOT )
			FSM_DFLT_COND
			FSM_TEST_LOGIC ("idle")
			FSM_PROFILE_SECTION_OUT (state1_trans_conds)

			FSM_TRANSIT_SWITCH
				{
				FSM_CASE_TRANSIT (0, 2, state2_enter_exec, ;, "FROM_RX", "", "idle", "fr_rx", "tr_13", "tdma3_ [idle -> fr_rx : FROM_RX / ]")
				FSM_CASE_TRANSIT (1, 3, state3_enter_exec, ;, "FROM_SRC", "", "idle", "fr_src", "tr_15", "tdma3_ [idle -> fr_src : FROM_SRC / ]")
				FSM_CASE_TRANSIT (2, 4, state4_enter_exec, ;, "SLOT ", "", "idle", "tx", "tr_19", "tdma3_ [idle -> tx : SLOT  / ]")
				FSM_CASE_TRANSIT (3, 1, state1_enter_exec, ;, "default", "", "idle", "idle", "tr_10", "tdma3_ [idle -> idle : default / ]")
				}
				/*---------------------------------------------------------*/



			/** state (fr_rx) enter executives **/
			FSM_STATE_ENTER_FORCED (2, "fr_rx", state2_enter_exec, "tdma3_ [fr_rx enter execs]")
				FSM_PROFILE_SECTION_IN ("tdma3_ [fr_rx enter execs]", state2_enter_exec)
				{
				int next_hop;
				int net_id;
				int have_intact;
				int clock_level;
				
				pkptr =  op_pk_get (RX_IN_STRM);
				op_pk_nfd_get (pkptr, "TYPE", &type);
				
				if(type==0x00)//QWZ
					{
					QWZ_rcv(pkptr);
					printf("%d now has %d neighbor\n",my_address,nei_count);
					}
				
				else if(type==0x01)//向交互节点发送的上报帧
					{
					op_pk_nfd_get (pkptr, "Next_Hop", &next_hop);
					if(next_hop!=my_address)
						{
						op_pk_destroy(pkptr);
						}
					else
						{
						printf("zhgongzhuan shoudao\n");
						op_pk_send(pkptr,SINK_OUT_STRM);
						}
					}
				
				
				
				
				//pk_len = (double) op_pk_total_size_get (pkptr);
				
				/** Record Statistics **/
				/** The bits/sec or packets/sec statistics are recorded in		**/
				/** bits and packets, and then the OPNET statistic "capture		**/
				/** mode" is used to obtain a bucketized sum over time.			**/
				/** Record extra 0.0 data-points to enable proper computation	**/
				/** of the "sum/time" based statistics.							**/
				
				/*op_stat_write (num_pk_rcvd_stat, 1.0);
				op_stat_write (pk_sec_rcvd_stat, 1.0);
				op_stat_write (pk_sec_rcvd_stat, 0.0);
				
				op_stat_write (global_pk_rcvd_stat, 1.0);
				op_stat_write (global_pk_sec_rcvd_stat, 1.0);
				op_stat_write (global_pk_sec_rcvd_stat, 0.0);
				
				op_stat_write (num_bits_rcvd_stat, pk_len);
				op_stat_write (bits_sec_rcvd_stat, pk_len);
				op_stat_write (bits_sec_rcvd_stat, 0.0);
				
				op_stat_write (global_bits_rcvd_stat, pk_len);
				op_stat_write (global_bits_sec_rcvd_stat, pk_len);
				op_stat_write (global_bits_sec_rcvd_stat, 0.0);*/
				
				//op_pk_send (pkptr, SINK_OUT_STRM);
				
				
				
				
				}
				FSM_PROFILE_SECTION_OUT (state2_enter_exec)

			/** state (fr_rx) exit executives **/
			FSM_STATE_EXIT_FORCED (2, "fr_rx", "tdma3_ [fr_rx exit execs]")


			/** state (fr_rx) transition processing **/
			FSM_TRANSIT_FORCE (1, state1_enter_exec, ;, "default", "", "fr_rx", "idle", "tr_14", "tdma3_ [fr_rx -> idle : default / ]")
				/*---------------------------------------------------------*/



			/** state (fr_src) enter executives **/
			FSM_STATE_ENTER_FORCED (3, "fr_src", state3_enter_exec, "tdma3_ [fr_src enter execs]")
				FSM_PROFILE_SECTION_IN ("tdma3_ [fr_src enter execs]", state3_enter_exec)
				{
				pkptr =  op_pk_get (SRC_IN_STRM);
				op_pk_nfd_get (pkptr, "TYPE", &type);
				
				if(type==0x01)//向交互节点发送的上报帧
					{
					op_subq_pk_insert (1, pkptr, OPC_QPOS_TAIL);
					}
				
				}
				FSM_PROFILE_SECTION_OUT (state3_enter_exec)

			/** state (fr_src) exit executives **/
			FSM_STATE_EXIT_FORCED (3, "fr_src", "tdma3_ [fr_src exit execs]")


			/** state (fr_src) transition processing **/
			FSM_TRANSIT_FORCE (1, state1_enter_exec, ;, "default", "", "fr_src", "idle", "tr_80", "tdma3_ [fr_src -> idle : default / ]")
				/*---------------------------------------------------------*/



			/** state (tx) enter executives **/
			FSM_STATE_ENTER_FORCED (4, "tx", state4_enter_exec, "tdma3_ [tx enter execs]")
				FSM_PROFILE_SECTION_IN ("tdma3_ [tx enter execs]", state4_enter_exec)
				{
				int min;
				int intact_next;
				Ici* mac_ici;
				double x,y,z;
				current_time = op_sim_time();
				used_slots = (int) floor ((current_time / slot_length) + EPSILON);
				current_offset = used_slots % num_slots;
				//预约下一个时隙
				op_intrpt_schedule_self ((double) (used_slots +1) * slot_length, 0);
				//location get
				op_ima_obj_pos_get(my_node_id,&Latitude,&Longitude,&Height,&x,&y,&z);
				//printf("location get:%d-- %lf %lf %lf %lf %lf %lf\n",my_address,Latitude,Longitude,Height,x,y,z);
				//自身时隙，生成并发送QWZ
				if(time[current_offset]==1)
					{
					is_my_slot=1;
					if(my_node_state==2 && nei_count==0) my_node_state=1;
					//建包
					if(my_node_state!=3) QWZ_create();
					//重置部分消息
					for(i=0;i<8;i++)
						{
						for(j=0;j<5;j++)
							{
							a_frame_record_last[i][j]=a_frame_record[i][j];
							a_frame_record[i][j]=0xFF;
							}
						}
					nei_num_last=nei_count;
					nei_count=0;
					for(i=0;i<24;i++)
						if(my_two_nei[i]!=0) my_two_nei[i]--;
					}
				else
					{
					is_my_slot=0;
					}
				if(used_slots%200==0 && used_slots>0)//上报时间
					{
					pkptr=op_pk_create_fmt("link_report");
					op_pk_nfd_set(pkptr,"TYPE",0x01);
					op_pk_nfd_set(pkptr,"Source",my_address);
					op_pk_nfd_set (pkptr, "Nei_address_0", a_frame_record_last[0][0]);
					op_pk_nfd_set (pkptr, "Nei_address_1", a_frame_record_last[1][0]);
					op_pk_nfd_set (pkptr, "Nei_address_2", a_frame_record_last[2][0]);
					op_pk_nfd_set (pkptr, "Nei_address_3", a_frame_record_last[3][0]);
					op_pk_nfd_set (pkptr, "Nei_address_4", a_frame_record_last[4][0]);
					op_pk_nfd_set (pkptr, "Nei_address_5", a_frame_record_last[5][0]);
					op_pk_nfd_set (pkptr, "Nei_address_6", a_frame_record_last[6][0]);
					op_pk_nfd_set (pkptr, "Nei_address_7", a_frame_record_last[7][0]);
					op_pk_nfd_set(pkptr,"Longitude",Longitude);
					op_pk_nfd_set(pkptr,"Latitude",Latitude);
					op_pk_nfd_set(pkptr,"Height",Height);
					//选择下一跳
					min=0xFF;
					for(i=0;i<8;i++)
						{
						if(a_frame_record_last[i][0]!=0xFF && a_frame_record_last[i][2]<min)
							{
							min=a_frame_record_last[i][2];
							intact_next=a_frame_record_last[i][0];
							}
						}
					printf("abc:%d-%d--CL___%d\n",my_address,intact_next,min);
					mac_ici = (Ici*)op_ici_create("mac_to_net_ici");
					op_ici_attr_set_int32(mac_ici, "mac_to_net_ici", intact_next);
					
					//发送至NET
					for(i=0;i<8;i++)
						if(a_frame_record_last[i][0]==0xFF)
							break;
					op_pk_nfd_set (pkptr, "Nei_num",i);
					op_ici_install(mac_ici);
					op_pk_send(pkptr,SINK_OUT_STRM);
					}
				}
				FSM_PROFILE_SECTION_OUT (state4_enter_exec)

			/** state (tx) exit executives **/
			FSM_STATE_EXIT_FORCED (4, "tx", "tdma3_ [tx exit execs]")


			/** state (tx) transition processing **/
			FSM_PROFILE_SECTION_IN ("tdma3_ [tx trans conditions]", state4_trans_conds)
			FSM_INIT_COND ((!is_my_slot) || (NO_DATA))
			FSM_TEST_COND (is_my_slot && !NO_DATA )
			FSM_TEST_LOGIC ("tx")
			FSM_PROFILE_SECTION_OUT (state4_trans_conds)

			FSM_TRANSIT_SWITCH
				{
				FSM_CASE_TRANSIT (0, 1, state1_enter_exec, ;, "(!is_my_slot) || (NO_DATA)", "", "tx", "idle", "tr_18", "tdma3_ [tx -> idle : (!is_my_slot) || (NO_DATA) / ]")
				FSM_CASE_TRANSIT (1, 5, state5_enter_exec, ;, "is_my_slot && !NO_DATA ", "", "tx", "tx_queue", "tr_61", "tdma3_ [tx -> tx_queue : is_my_slot && !NO_DATA  / ]")
				}
				/*---------------------------------------------------------*/



			/** state (tx_queue) enter executives **/
			FSM_STATE_ENTER_FORCED (5, "tx_queue", state5_enter_exec, "tdma3_ [tx_queue enter execs]")
				FSM_PROFILE_SECTION_IN ("tdma3_ [tx_queue enter execs]", state5_enter_exec)
				{
				int FLAG=0;
				/* Determine if currently my slot. */
				current_time = op_sim_time();
				used_slots = (int) floor ((current_time / slot_length) + EPSILON);
				current_offset = used_slots % num_slots;
				time_left_in_slot = ((used_slots + 1)*slot_length) - current_time;
				
				/* dequeue the packet */
				if(op_subq_empty(0)==0)
					{
					pkptr=op_subq_pk_access (0, OPC_QPOS_HEAD);
					pk_len = (double) op_pk_total_size_get (pkptr); 
					pk_time = (double) pk_len / tx_data_rate;
					if(pk_time < time_left_in_slot)
						{
						pkptr=op_subq_pk_remove(0, OPC_QPOS_HEAD);
						FLAG=1;
						}
					}
				else if(op_subq_empty(1)==0 && FLAG==0)
					{
					pkptr=op_subq_pk_access (1, OPC_QPOS_HEAD);
					pk_len = (double) op_pk_total_size_get (pkptr); 
					pk_time = (double) pk_len / tx_data_rate;
					if(pk_time < time_left_in_slot)
						{
						pkptr=op_subq_pk_remove(1, OPC_QPOS_HEAD);
						FLAG=1;
						}
					}
				else if(op_subq_empty(2)==0 && FLAG==0)
					{
					pkptr=op_subq_pk_access (2, OPC_QPOS_HEAD);
					pk_len = (double) op_pk_total_size_get (pkptr); 
					pk_time = (double) pk_len / tx_data_rate;
					if(pk_time < time_left_in_slot)
						{
						pkptr=op_subq_pk_remove(2, OPC_QPOS_HEAD);
						FLAG=1;
						}
					}
				
				if (FLAG==1)
					{
					/* full pk */
					op_pk_nfd_set(pkptr,"SEND",my_address);
					op_pk_nfd_set(pkptr,"FL",pk_len);
				
					/* send it! */
					op_pk_print(pkptr);
					op_pk_send (pkptr, TX_OUT_STRM);
				
					} /* End if */
				
				else
					{
					is_my_slot=0;
					}
				}
				FSM_PROFILE_SECTION_OUT (state5_enter_exec)

			/** state (tx_queue) exit executives **/
			FSM_STATE_EXIT_FORCED (5, "tx_queue", "tdma3_ [tx_queue exit execs]")


			/** state (tx_queue) transition processing **/
			FSM_PROFILE_SECTION_IN ("tdma3_ [tx_queue trans conditions]", state5_trans_conds)
			FSM_INIT_COND (!is_my_slot || NO_DATA)
			FSM_TEST_COND (is_my_slot && !NO_DATA)
			FSM_TEST_LOGIC ("tx_queue")
			FSM_PROFILE_SECTION_OUT (state5_trans_conds)

			FSM_TRANSIT_SWITCH
				{
				FSM_CASE_TRANSIT (0, 1, state1_enter_exec, ;, "!is_my_slot || NO_DATA", "", "tx_queue", "idle", "tr_62", "tdma3_ [tx_queue -> idle : !is_my_slot || NO_DATA / ]")
				FSM_CASE_TRANSIT (1, 5, state5_enter_exec, ;, "is_my_slot && !NO_DATA", "", "tx_queue", "tx_queue", "tr_88", "tdma3_ [tx_queue -> tx_queue : is_my_slot && !NO_DATA / ]")
				}
				/*---------------------------------------------------------*/



			}


		FSM_EXIT (0,"tdma3_")
		}
	}




void
_op_tdma3__diag (OP_SIM_CONTEXT_ARG_OPT)
	{
#if defined (OPD_ALLOW_ODB)
#if !defined (VOSD_NO_FIN)
	int _op_block_origin = __LINE__+1;
#endif

	FIN_MT (_op_tdma3__diag ())

	if (1)
		{
		/* Temporary Variables */
		Packet*	pkptr;
		
		Objid 	tx_id, comp_id, tx_ch_id; 
		
		//double  floor();
		//double  fmod();
		
		int	used_slots;
		int	current_offset;
		int	next_offset;
		
		
		double	current_time;
		double	time_left_in_slot;
		double	pk_len;
		double	pk_time;
		
		double	my_next_slot_time;
		
		int		current_intrpt_type;
		
		int i,j;
		int nei_temp[8];
		/* End of Temporary Variables */

		/* Diagnostic Block */

		BINIT
		{
		printf ("Object ID = %d Current Sim Time = %g\n", my_id, op_sim_time ());
		printf ("My TDMA Offset = %d\n", my_offset);
		printf ("Number of TDMA Slots = %d\n", num_slots);
		printf ("Number of Packets Received = %d\n", num_pk_rcvd);
		printf ("Number of Bits Received = %d\n", num_bits_rcvd);
		printf ("Number of Packets Sent = %d\n", num_pk_sent);
		printf ("Number of Bits Sent = %d\n", num_bits_sent);
		
		}

		/* End of Diagnostic Block */

		}

	FOUT
#endif /* OPD_ALLOW_ODB */
	}




void
_op_tdma3__terminate (OP_SIM_CONTEXT_ARG_OPT)
	{

	FIN_MT (_op_tdma3__terminate ())


	/* No Termination Block */

	Vos_Poolmem_Dealloc (op_sv_ptr);

	FOUT
	}


/* Undefine shortcuts to state variables to avoid */
/* syntax error in direct access to fields of */
/* local variable prs_ptr in _op_tdma3__svar function. */
#undef my_offset
#undef slot_length
#undef tx_data_rate
#undef intrpt_flag
#undef num_pk_sent
#undef num_pk_rcvd
#undef num_bits_sent
#undef num_bits_rcvd
#undef num_pk_sent_stat
#undef global_pk_sent_stat
#undef num_pk_rcvd_stat
#undef global_pk_rcvd_stat
#undef my_node_id
#undef my_id
#undef num_bits_sent_stat
#undef global_bits_sent_stat
#undef num_bits_rcvd_stat
#undef global_bits_rcvd_stat
#undef bits_sec_rcvd_stat
#undef bits_sec_sent_stat
#undef pk_sec_rcvd_stat
#undef pk_sec_sent_stat
#undef global_bits_sec_rcvd_stat
#undef global_bits_sec_sent_stat
#undef global_pk_sec_rcvd_stat
#undef global_pk_sec_sent_stat
#undef my_address
#undef type
#undef my_neighbor
#undef nei_count
#undef my_two_nei_count_sum
#undef my_two_nei
#undef is_my_slot
#undef my_slot_end
#undef interactive_id
#undef evh1
#undef WAIT
#undef num_slots
#undef my_node_state
#undef my_clock_level
#undef a_frame_record
#undef INTACT_FLAG
#undef a_frame_record_last
#undef nei_num_last
#undef node_num
#undef Longitude
#undef Latitude
#undef Height
#undef time

#undef FIN_PREAMBLE_DEC
#undef FIN_PREAMBLE_CODE

#define FIN_PREAMBLE_DEC
#define FIN_PREAMBLE_CODE

VosT_Obtype
_op_tdma3__init (int * init_block_ptr)
	{
	VosT_Obtype obtype = OPC_NIL;
	FIN_MT (_op_tdma3__init (init_block_ptr))

	obtype = Vos_Define_Object_Prstate ("proc state vars (tdma3_)",
		sizeof (tdma3__state));
	*init_block_ptr = 0;

	FRET (obtype)
	}

VosT_Address
_op_tdma3__alloc (VosT_Obtype obtype, int init_block)
	{
#if !defined (VOSD_NO_FIN)
	int _op_block_origin = 0;
#endif
	tdma3__state * ptr;
	FIN_MT (_op_tdma3__alloc (obtype))

	ptr = (tdma3__state *)Vos_Alloc_Object (obtype);
	if (ptr != OPC_NIL)
		{
		ptr->_op_current_block = init_block;
#if defined (OPD_ALLOW_ODB)
		ptr->_op_current_state = "tdma3_ [init enter execs]";
#endif
		}
	FRET ((VosT_Address)ptr)
	}



void
_op_tdma3__svar (void * gen_ptr, const char * var_name, void ** var_p_ptr)
	{
	tdma3__state		*prs_ptr;

	FIN_MT (_op_tdma3__svar (gen_ptr, var_name, var_p_ptr))

	if (var_name == OPC_NIL)
		{
		*var_p_ptr = (void *)OPC_NIL;
		FOUT
		}
	prs_ptr = (tdma3__state *)gen_ptr;

	if (strcmp ("my_offset" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->my_offset);
		FOUT
		}
	if (strcmp ("slot_length" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->slot_length);
		FOUT
		}
	if (strcmp ("tx_data_rate" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->tx_data_rate);
		FOUT
		}
	if (strcmp ("intrpt_flag" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->intrpt_flag);
		FOUT
		}
	if (strcmp ("num_pk_sent" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->num_pk_sent);
		FOUT
		}
	if (strcmp ("num_pk_rcvd" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->num_pk_rcvd);
		FOUT
		}
	if (strcmp ("num_bits_sent" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->num_bits_sent);
		FOUT
		}
	if (strcmp ("num_bits_rcvd" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->num_bits_rcvd);
		FOUT
		}
	if (strcmp ("num_pk_sent_stat" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->num_pk_sent_stat);
		FOUT
		}
	if (strcmp ("global_pk_sent_stat" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->global_pk_sent_stat);
		FOUT
		}
	if (strcmp ("num_pk_rcvd_stat" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->num_pk_rcvd_stat);
		FOUT
		}
	if (strcmp ("global_pk_rcvd_stat" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->global_pk_rcvd_stat);
		FOUT
		}
	if (strcmp ("my_node_id" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->my_node_id);
		FOUT
		}
	if (strcmp ("my_id" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->my_id);
		FOUT
		}
	if (strcmp ("num_bits_sent_stat" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->num_bits_sent_stat);
		FOUT
		}
	if (strcmp ("global_bits_sent_stat" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->global_bits_sent_stat);
		FOUT
		}
	if (strcmp ("num_bits_rcvd_stat" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->num_bits_rcvd_stat);
		FOUT
		}
	if (strcmp ("global_bits_rcvd_stat" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->global_bits_rcvd_stat);
		FOUT
		}
	if (strcmp ("bits_sec_rcvd_stat" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->bits_sec_rcvd_stat);
		FOUT
		}
	if (strcmp ("bits_sec_sent_stat" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->bits_sec_sent_stat);
		FOUT
		}
	if (strcmp ("pk_sec_rcvd_stat" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->pk_sec_rcvd_stat);
		FOUT
		}
	if (strcmp ("pk_sec_sent_stat" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->pk_sec_sent_stat);
		FOUT
		}
	if (strcmp ("global_bits_sec_rcvd_stat" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->global_bits_sec_rcvd_stat);
		FOUT
		}
	if (strcmp ("global_bits_sec_sent_stat" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->global_bits_sec_sent_stat);
		FOUT
		}
	if (strcmp ("global_pk_sec_rcvd_stat" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->global_pk_sec_rcvd_stat);
		FOUT
		}
	if (strcmp ("global_pk_sec_sent_stat" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->global_pk_sec_sent_stat);
		FOUT
		}
	if (strcmp ("my_address" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->my_address);
		FOUT
		}
	if (strcmp ("type" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->type);
		FOUT
		}
	if (strcmp ("my_neighbor" , var_name) == 0)
		{
		*var_p_ptr = (void *) (prs_ptr->my_neighbor);
		FOUT
		}
	if (strcmp ("nei_count" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->nei_count);
		FOUT
		}
	if (strcmp ("my_two_nei_count_sum" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->my_two_nei_count_sum);
		FOUT
		}
	if (strcmp ("my_two_nei" , var_name) == 0)
		{
		*var_p_ptr = (void *) (prs_ptr->my_two_nei);
		FOUT
		}
	if (strcmp ("is_my_slot" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->is_my_slot);
		FOUT
		}
	if (strcmp ("my_slot_end" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->my_slot_end);
		FOUT
		}
	if (strcmp ("interactive_id" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->interactive_id);
		FOUT
		}
	if (strcmp ("evh1" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->evh1);
		FOUT
		}
	if (strcmp ("WAIT" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->WAIT);
		FOUT
		}
	if (strcmp ("num_slots" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->num_slots);
		FOUT
		}
	if (strcmp ("my_node_state" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->my_node_state);
		FOUT
		}
	if (strcmp ("my_clock_level" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->my_clock_level);
		FOUT
		}
	if (strcmp ("a_frame_record" , var_name) == 0)
		{
		*var_p_ptr = (void *) (prs_ptr->a_frame_record);
		FOUT
		}
	if (strcmp ("INTACT_FLAG" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->INTACT_FLAG);
		FOUT
		}
	if (strcmp ("a_frame_record_last" , var_name) == 0)
		{
		*var_p_ptr = (void *) (prs_ptr->a_frame_record_last);
		FOUT
		}
	if (strcmp ("nei_num_last" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->nei_num_last);
		FOUT
		}
	if (strcmp ("node_num" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->node_num);
		FOUT
		}
	if (strcmp ("Longitude" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->Longitude);
		FOUT
		}
	if (strcmp ("Latitude" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->Latitude);
		FOUT
		}
	if (strcmp ("Height" , var_name) == 0)
		{
		*var_p_ptr = (void *) (&prs_ptr->Height);
		FOUT
		}
	if (strcmp ("time" , var_name) == 0)
		{
		*var_p_ptr = (void *) (prs_ptr->time);
		FOUT
		}
	*var_p_ptr = (void *)OPC_NIL;

	FOUT
	}

