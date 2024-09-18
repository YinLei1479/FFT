/* Process model C form file: ground_net.pr.c */
/* Portions of this file copyright 1986-2008 by OPNET Technologies, Inc. */



/* This variable carries the header into the object file */
const char ground_net_pr_c [] = "MIL_3_Tfile_Hdr_ 145A 30A modeler 7 66EA993A 66EA993A 1 ray-laptop 28918 0 0 none none 0 0 none 0 0 0 0 0 0 0 0 1bcc 1                                                                                                                                                                                                                                                                                                                                                                                                         ";
#include <string.h>



/* OPNET system definitions */
#include <opnet.h>



/* Header Block */

#include <math.h>

/* Constant Definitions */

#define SRC_IN		(1)
#define SINK_OUT	(1)
#define TX_OUT		(2)
#define RX_IN		(2)



/* Transition Condition Macros */ 
#define FROM_RX_PK			(op_intrpt_type() == OPC_INTRPT_STRM) && (op_intrpt_strm () == RX_IN)
#define FROM_SRC_PK 		(op_intrpt_type() == OPC_INTRPT_STRM) && (op_intrpt_strm () == SRC_IN)



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
	int	                    		topo[24][24]                                    ;	/* 0 or 1 for the net topo */
	int	                    		node_num                                        ;
	int	                    		topo_address[24][3]                             ;
	} ground_net_state;

#define my_address              		op_sv_ptr->my_address
#define my_id                   		op_sv_ptr->my_id
#define my_node_id              		op_sv_ptr->my_node_id
#define type                    		op_sv_ptr->type
#define topo                    		op_sv_ptr->topo
#define node_num                		op_sv_ptr->node_num
#define topo_address            		op_sv_ptr->topo_address

/* These macro definitions will define a local variable called	*/
/* "op_sv_ptr" in each function containing a FIN statement.	*/
/* This variable points to the state variable data structure,	*/
/* and can be used from a C debugger to display their values.	*/
#undef FIN_PREAMBLE_DEC
#undef FIN_PREAMBLE_CODE
#define FIN_PREAMBLE_DEC	ground_net_state *op_sv_ptr;
#define FIN_PREAMBLE_CODE	\
		op_sv_ptr = ((ground_net_state *)(OP_SIM_CONTEXT_PTR->_op_mod_state_ptr));


/* No Function Block */

#if !defined (VOSD_NO_FIN)
enum { _op_block_origin = __LINE__ };
#endif

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
	void ground_net (OP_SIM_CONTEXT_ARG_OPT);
	VosT_Obtype _op_ground_net_init (int * init_block_ptr);
	void _op_ground_net_diag (OP_SIM_CONTEXT_ARG_OPT);
	void _op_ground_net_terminate (OP_SIM_CONTEXT_ARG_OPT);
	VosT_Address _op_ground_net_alloc (VosT_Obtype, int);
	void _op_ground_net_svar (void *, const char *, void **);


#if defined (__cplusplus)
} /* end of 'extern "C"' */
#endif




/* Process model interrupt handling procedure */


void
ground_net (OP_SIM_CONTEXT_ARG_OPT)
	{
#if !defined (VOSD_NO_FIN)
	int _op_block_origin = 0;
#endif
	FIN_MT (ground_net ());

		{
		/* Temporary Variables */
		Packet* pkptr;
		int i,j,z;
		/* End of Temporary Variables */


		FSM_ENTER ("ground_net")

		FSM_BLOCK_SWITCH
			{
			/*---------------------------------------------------------*/
			/** state (init) enter executives **/
			FSM_STATE_ENTER_FORCED_NOLABEL (0, "init", "ground_net [init enter execs]")
				FSM_PROFILE_SECTION_IN ("ground_net [init enter execs]", state0_enter_exec)
				{
				//initial begin
				my_id = op_id_self();
				my_node_id = op_topo_parent (my_id);
				op_ima_obj_attr_get(my_node_id,"Address",&my_address);
				for(i=0;i<24;i++)
					for(j=0;j<24;j++) topo[i][j]=0;
				for(i=0;i<24;i++)
					for(j=0;j<3;j++) topo_address[i][j]=0;
				node_num=13;
				printf("$$$$$$$$$$$$ground net over$$$$$$$$$$$$$\n");
				}
				FSM_PROFILE_SECTION_OUT (state0_enter_exec)

			/** state (init) exit executives **/
			FSM_STATE_EXIT_FORCED (0, "init", "ground_net [init exit execs]")


			/** state (init) transition processing **/
			FSM_TRANSIT_FORCE (1, state1_enter_exec, ;, "default", "", "init", "idle", "tr_0", "ground_net [init -> idle : default / ]")
				/*---------------------------------------------------------*/



			/** state (idle) enter executives **/
			FSM_STATE_ENTER_UNFORCED (1, "idle", state1_enter_exec, "ground_net [idle enter execs]")

			/** blocking after enter executives of unforced state. **/
			FSM_EXIT (3,"ground_net")


			/** state (idle) exit executives **/
			FSM_STATE_EXIT_UNFORCED (1, "idle", "ground_net [idle exit execs]")


			/** state (idle) transition processing **/
			FSM_PROFILE_SECTION_IN ("ground_net [idle trans conditions]", state1_trans_conds)
			FSM_INIT_COND (FROM_RX_PK)
			FSM_TEST_COND (FROM_SRC_PK)
			FSM_DFLT_COND
			FSM_TEST_LOGIC ("idle")
			FSM_PROFILE_SECTION_OUT (state1_trans_conds)

			FSM_TRANSIT_SWITCH
				{
				FSM_CASE_TRANSIT (0, 2, state2_enter_exec, ;, "FROM_RX_PK", "", "idle", "rx_rcv", "tr_3", "ground_net [idle -> rx_rcv : FROM_RX_PK / ]")
				FSM_CASE_TRANSIT (1, 3, state3_enter_exec, ;, "FROM_SRC_PK", "", "idle", "src_rcv", "tr_6", "ground_net [idle -> src_rcv : FROM_SRC_PK / ]")
				FSM_CASE_TRANSIT (2, 1, state1_enter_exec, ;, "default", "", "idle", "idle", "tr_1", "ground_net [idle -> idle : default / ]")
				}
				/*---------------------------------------------------------*/



			/** state (rx_rcv) enter executives **/
			FSM_STATE_ENTER_FORCED (2, "rx_rcv", state2_enter_exec, "ground_net [rx_rcv enter execs]")
				FSM_PROFILE_SECTION_IN ("ground_net [rx_rcv enter execs]", state2_enter_exec)
				{
				int a_topo[276];
				int num_elements=9;
				int int_array[9];
				unsigned char bit_sequence[35];
				
				//收到不同类型包的行为
				pkptr=op_pk_get(op_intrpt_strm());
				op_pk_nfd_get (pkptr, "TYPE", &type);
				
				
				
				if(type==0x10)//link_maintain
					{
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
					printf("\n ground get the topo:\n");
					for(i=0;i<node_num;i++)
					{
					for(j=0;j<node_num;j++)
						printf("%d   ",topo[i][j]);
					printf("\n");
					}
					op_pk_nfd_get(pkptr,"Nei_longtitude0",&topo_address[0][0]);
					op_pk_nfd_get(pkptr,"Nei_longtitude1",&topo_address[1][0]);
					op_pk_nfd_get(pkptr,"Nei_longtitude2",&topo_address[2][0]);
					op_pk_nfd_get(pkptr,"Nei_longtitude3",&topo_address[3][0]);
					op_pk_nfd_get(pkptr,"Nei_longtitude4",&topo_address[4][0]);
					op_pk_nfd_get(pkptr,"Nei_longtitude5",&topo_address[5][0]);
					op_pk_nfd_get(pkptr,"Nei_longtitude6",&topo_address[6][0]);
					op_pk_nfd_get(pkptr,"Nei_longtitude7",&topo_address[7][0]);
					op_pk_nfd_get(pkptr,"Nei_longtitude8",&topo_address[8][0]);
					op_pk_nfd_get(pkptr,"Nei_longtitude9",&topo_address[9][0]);
					op_pk_nfd_get(pkptr,"Nei_longtitude10",&topo_address[10][0]);
					op_pk_nfd_get(pkptr,"Nei_longtitude11",&topo_address[11][0]);
					op_pk_nfd_get(pkptr,"Nei_longtitude12",&topo_address[12][0]);
					op_pk_nfd_get(pkptr,"Nei_longtitude13",&topo_address[13][0]);
					op_pk_nfd_get(pkptr,"Nei_longtitude14",&topo_address[14][0]);
					op_pk_nfd_get(pkptr,"Nei_longtitude15",&topo_address[15][0]);
					op_pk_nfd_get(pkptr,"Nei_longtitude16",&topo_address[16][0]);
					op_pk_nfd_get(pkptr,"Nei_longtitude17",&topo_address[17][0]);
					op_pk_nfd_get(pkptr,"Nei_longtitude18",&topo_address[18][0]);
					op_pk_nfd_get(pkptr,"Nei_longtitude19",&topo_address[19][0]);
					op_pk_nfd_get(pkptr,"Nei_longtitude20",&topo_address[20][0]);
					op_pk_nfd_get(pkptr,"Nei_longtitude21",&topo_address[21][0]);
					op_pk_nfd_get(pkptr,"Nei_longtitude22",&topo_address[22][0]);
					op_pk_nfd_get(pkptr,"Nei_longtitude23",&topo_address[23][0]);
				
					op_pk_nfd_get(pkptr,"Nei_latitude0",&topo_address[0][1]);
					op_pk_nfd_get(pkptr,"Nei_latitude1",&topo_address[1][1]);
					op_pk_nfd_get(pkptr,"Nei_latitude2",&topo_address[2][1]);
					op_pk_nfd_get(pkptr,"Nei_latitude3",&topo_address[3][1]);
					op_pk_nfd_get(pkptr,"Nei_latitude4",&topo_address[4][1]);
					op_pk_nfd_get(pkptr,"Nei_latitude5",&topo_address[5][1]);
					op_pk_nfd_get(pkptr,"Nei_latitude6",&topo_address[6][1]);
					op_pk_nfd_get(pkptr,"Nei_latitude7",&topo_address[7][1]);
					op_pk_nfd_get(pkptr,"Nei_latitude8",&topo_address[8][1]);
					op_pk_nfd_get(pkptr,"Nei_latitude9",&topo_address[9][1]);
					op_pk_nfd_get(pkptr,"Nei_latitude10",&topo_address[10][1]);
					op_pk_nfd_get(pkptr,"Nei_latitude11",&topo_address[11][1]);
					op_pk_nfd_get(pkptr,"Nei_latitude12",&topo_address[12][1]);
					op_pk_nfd_get(pkptr,"Nei_latitude13",&topo_address[13][1]);
					op_pk_nfd_get(pkptr,"Nei_latitude14",&topo_address[14][1]);
					op_pk_nfd_get(pkptr,"Nei_latitude15",&topo_address[15][1]);
					op_pk_nfd_get(pkptr,"Nei_latitude16",&topo_address[16][1]);
					op_pk_nfd_get(pkptr,"Nei_latitude17",&topo_address[17][1]);
					op_pk_nfd_get(pkptr,"Nei_latitude18",&topo_address[18][1]);
					op_pk_nfd_get(pkptr,"Nei_latitude19",&topo_address[19][1]);
					op_pk_nfd_get(pkptr,"Nei_latitude20",&topo_address[20][1]);
					op_pk_nfd_get(pkptr,"Nei_latitude21",&topo_address[21][1]);
					op_pk_nfd_get(pkptr,"Nei_latitude22",&topo_address[22][1]);
					op_pk_nfd_get(pkptr,"Nei_latitude23",&topo_address[23][1]);
				
					op_pk_nfd_get(pkptr,"Nei_height0",&topo_address[0][2]);
					op_pk_nfd_get(pkptr,"Nei_height1",&topo_address[1][2]);
					op_pk_nfd_get(pkptr,"Nei_height2",&topo_address[2][2]);
					op_pk_nfd_get(pkptr,"Nei_height3",&topo_address[3][2]);
					op_pk_nfd_get(pkptr,"Nei_height4",&topo_address[4][2]);
					op_pk_nfd_get(pkptr,"Nei_height5",&topo_address[5][2]);
					op_pk_nfd_get(pkptr,"Nei_height6",&topo_address[6][2]);
					op_pk_nfd_get(pkptr,"Nei_height7",&topo_address[7][2]);
					op_pk_nfd_get(pkptr,"Nei_height8",&topo_address[8][2]);
					op_pk_nfd_get(pkptr,"Nei_height9",&topo_address[9][2]);
					op_pk_nfd_get(pkptr,"Nei_height10",&topo_address[10][2]);
					op_pk_nfd_get(pkptr,"Nei_height11",&topo_address[11][2]);
					op_pk_nfd_get(pkptr,"Nei_height12",&topo_address[12][2]);
					op_pk_nfd_get(pkptr,"Nei_height13",&topo_address[13][2]);
					op_pk_nfd_get(pkptr,"Nei_height14",&topo_address[14][2]);
					op_pk_nfd_get(pkptr,"Nei_height15",&topo_address[15][2]);
					op_pk_nfd_get(pkptr,"Nei_height16",&topo_address[16][2]);
					op_pk_nfd_get(pkptr,"Nei_height17",&topo_address[17][2]);
					op_pk_nfd_get(pkptr,"Nei_height18",&topo_address[18][2]);
					op_pk_nfd_get(pkptr,"Nei_height19",&topo_address[19][2]);
					op_pk_nfd_get(pkptr,"Nei_height20",&topo_address[20][2]);
					op_pk_nfd_get(pkptr,"Nei_height21",&topo_address[21][2]);
					op_pk_nfd_get(pkptr,"Nei_height22",&topo_address[22][2]);
					op_pk_nfd_get(pkptr,"Nei_height23",&topo_address[23][2]);
					
					op_pk_destroy(pkptr);
					}
				
				
				
				
				}
				FSM_PROFILE_SECTION_OUT (state2_enter_exec)

			/** state (rx_rcv) exit executives **/
			FSM_STATE_EXIT_FORCED (2, "rx_rcv", "ground_net [rx_rcv exit execs]")


			/** state (rx_rcv) transition processing **/
			FSM_TRANSIT_FORCE (1, state1_enter_exec, ;, "default", "", "rx_rcv", "idle", "tr_5", "ground_net [rx_rcv -> idle : default / ]")
				/*---------------------------------------------------------*/



			/** state (src_rcv) enter executives **/
			FSM_STATE_ENTER_FORCED (3, "src_rcv", state3_enter_exec, "ground_net [src_rcv enter execs]")
				FSM_PROFILE_SECTION_IN ("ground_net [src_rcv enter execs]", state3_enter_exec)
				{
				pkptr=op_pk_get(op_intrpt_strm());
				op_pk_nfd_get (pkptr, "TYPE", &type);
				
				if(type==0x15)//appointment_req_G
					op_pk_send(pkptr,TX_OUT);
				}
				FSM_PROFILE_SECTION_OUT (state3_enter_exec)

			/** state (src_rcv) exit executives **/
			FSM_STATE_EXIT_FORCED (3, "src_rcv", "ground_net [src_rcv exit execs]")


			/** state (src_rcv) transition processing **/
			FSM_TRANSIT_FORCE (1, state1_enter_exec, ;, "default", "", "src_rcv", "idle", "tr_7", "ground_net [src_rcv -> idle : default / ]")
				/*---------------------------------------------------------*/



			}


		FSM_EXIT (0,"ground_net")
		}
	}




void
_op_ground_net_diag (OP_SIM_CONTEXT_ARG_OPT)
	{
	/* No Diagnostic Block */
	}




void
_op_ground_net_terminate (OP_SIM_CONTEXT_ARG_OPT)
	{

	FIN_MT (_op_ground_net_terminate ())


	/* No Termination Block */

	Vos_Poolmem_Dealloc (op_sv_ptr);

	FOUT
	}


/* Undefine shortcuts to state variables to avoid */
/* syntax error in direct access to fields of */
/* local variable prs_ptr in _op_ground_net_svar function. */
#undef my_address
#undef my_id
#undef my_node_id
#undef type
#undef topo
#undef node_num
#undef topo_address

#undef FIN_PREAMBLE_DEC
#undef FIN_PREAMBLE_CODE

#define FIN_PREAMBLE_DEC
#define FIN_PREAMBLE_CODE

VosT_Obtype
_op_ground_net_init (int * init_block_ptr)
	{
	VosT_Obtype obtype = OPC_NIL;
	FIN_MT (_op_ground_net_init (init_block_ptr))

	obtype = Vos_Define_Object_Prstate ("proc state vars (ground_net)",
		sizeof (ground_net_state));
	*init_block_ptr = 0;

	FRET (obtype)
	}

VosT_Address
_op_ground_net_alloc (VosT_Obtype obtype, int init_block)
	{
#if !defined (VOSD_NO_FIN)
	int _op_block_origin = 0;
#endif
	ground_net_state * ptr;
	FIN_MT (_op_ground_net_alloc (obtype))

	ptr = (ground_net_state *)Vos_Alloc_Object (obtype);
	if (ptr != OPC_NIL)
		{
		ptr->_op_current_block = init_block;
#if defined (OPD_ALLOW_ODB)
		ptr->_op_current_state = "ground_net [init enter execs]";
#endif
		}
	FRET ((VosT_Address)ptr)
	}



void
_op_ground_net_svar (void * gen_ptr, const char * var_name, void ** var_p_ptr)
	{
	ground_net_state		*prs_ptr;

	FIN_MT (_op_ground_net_svar (gen_ptr, var_name, var_p_ptr))

	if (var_name == OPC_NIL)
		{
		*var_p_ptr = (void *)OPC_NIL;
		FOUT
		}
	prs_ptr = (ground_net_state *)gen_ptr;

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
	if (strcmp ("topo" , var_name) == 0)
		{
		*var_p_ptr = (void *) (prs_ptr->topo);
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

