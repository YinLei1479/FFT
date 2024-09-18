/* Process model C form file: net_in_tdma_g.pr.c */
/* Portions of this file copyright 1986-2008 by OPNET Technologies, Inc. */



/* This variable carries the header into the object file */
const char net_in_tdma_g_pr_c [] = "MIL_3_Tfile_Hdr_ 145A 30A modeler 7 66EA98DA 66EA98DA 1 ray-laptop 28918 0 0 none none 0 0 none 0 0 0 0 0 0 0 0 1bcc 1                                                                                                                                                                                                                                                                                                                                                                                                         ";
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
//#define WAIT			(2222)
#define TS				(3333)

/* Transition Condition Macros */ 
#define FROM_RX			(current_intrpt_type == OPC_INTRPT_STRM) && (op_intrpt_strm () == RX_IN_STRM)
#define FROM_SRC 		(current_intrpt_type == OPC_INTRPT_STRM) && (op_intrpt_strm () == SRC_IN_STRM) 
#define TRANSMITTING	(op_stat_local_read (0) == 1.0) 
#define SLOT 			(current_intrpt_type == OPC_INTRPT_SELF) && (op_intrpt_code () == 0)
#define MY_SLOT 		(current_intrpt_type == OPC_INTRPT_SELF) && (op_intrpt_code () == 3000)
#define END  			(current_intrpt_type == OPC_INTRPT_STAT)

#define NET_IN			(current_intrpt_type == OPC_INTRPT_SELF) && (op_intrpt_code () == 4444)
#define SEND_NET_IN		(current_intrpt_type == OPC_INTRPT_SELF) && (op_intrpt_code () == 5555)
//#define WAIT_BEGIN 		(current_intrpt_type == OPC_INTRPT_SELF) && (op_intrpt_code () == WAIT)
#define DATA_ENQ 		(!(op_subq_empty (0)))
#define TIME_END		(current_intrpt_type == OPC_INTRPT_SELF) && (op_intrpt_code () == TS)


#define	SELF_INTRPT_SCHLD	(intrpt_flag == 1)

/* Global Variables */
int		tdma_pk_sent;
int		tdma_pk_rcvd;
int		tdma_bits_sent;
int		tdma_bits_rcvd;
int		tdma_setup;
int		tdma_id;
int		num_slots;


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
	Objid	                  		my_node_id                                      ;
	Objid	                  		my_id                                           ;
	int	                    		my_address                                      ;
	int	                    		type                                            ;
	} net_in_tdma_g_state;

#define my_node_id              		op_sv_ptr->my_node_id
#define my_id                   		op_sv_ptr->my_id
#define my_address              		op_sv_ptr->my_address
#define type                    		op_sv_ptr->type

/* These macro definitions will define a local variable called	*/
/* "op_sv_ptr" in each function containing a FIN statement.	*/
/* This variable points to the state variable data structure,	*/
/* and can be used from a C debugger to display their values.	*/
#undef FIN_PREAMBLE_DEC
#undef FIN_PREAMBLE_CODE
#define FIN_PREAMBLE_DEC	net_in_tdma_g_state *op_sv_ptr;
#define FIN_PREAMBLE_CODE	\
		op_sv_ptr = ((net_in_tdma_g_state *)(OP_SIM_CONTEXT_PTR->_op_mod_state_ptr));


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
	void net_in_tdma_g (OP_SIM_CONTEXT_ARG_OPT);
	VosT_Obtype _op_net_in_tdma_g_init (int * init_block_ptr);
	void _op_net_in_tdma_g_diag (OP_SIM_CONTEXT_ARG_OPT);
	void _op_net_in_tdma_g_terminate (OP_SIM_CONTEXT_ARG_OPT);
	VosT_Address _op_net_in_tdma_g_alloc (VosT_Obtype, int);
	void _op_net_in_tdma_g_svar (void *, const char *, void **);


#if defined (__cplusplus)
} /* end of 'extern "C"' */
#endif




/* Process model interrupt handling procedure */


void
net_in_tdma_g (OP_SIM_CONTEXT_ARG_OPT)
	{
#if !defined (VOSD_NO_FIN)
	int _op_block_origin = 0;
#endif
	FIN_MT (net_in_tdma_g ());

		{
		/* Temporary Variables */
		Packet*	pkptr;
		
		
		int current_intrpt_type;
		/* End of Temporary Variables */


		FSM_ENTER ("net_in_tdma_g")

		FSM_BLOCK_SWITCH
			{
			/*---------------------------------------------------------*/
			/** state (init) enter executives **/
			FSM_STATE_ENTER_FORCED_NOLABEL (0, "init", "net_in_tdma_g [init enter execs]")
				FSM_PROFILE_SECTION_IN ("net_in_tdma_g [init enter execs]", state0_enter_exec)
				{
				my_id = op_id_self();
				my_node_id = op_topo_parent (my_id);
				op_ima_obj_attr_get(my_node_id,"Address",&my_address);
				}
				FSM_PROFILE_SECTION_OUT (state0_enter_exec)

			/** state (init) exit executives **/
			FSM_STATE_EXIT_FORCED (0, "init", "net_in_tdma_g [init exit execs]")


			/** state (init) transition processing **/
			FSM_TRANSIT_FORCE (1, state1_enter_exec, ;, "default", "", "init", "idle", "tr_12", "net_in_tdma_g [init -> idle : default / ]")
				/*---------------------------------------------------------*/



			/** state (idle) enter executives **/
			FSM_STATE_ENTER_UNFORCED (1, "idle", state1_enter_exec, "net_in_tdma_g [idle enter execs]")

			/** blocking after enter executives of unforced state. **/
			FSM_EXIT (3,"net_in_tdma_g")


			/** state (idle) exit executives **/
			FSM_STATE_EXIT_UNFORCED (1, "idle", "net_in_tdma_g [idle exit execs]")
				FSM_PROFILE_SECTION_IN ("net_in_tdma_g [idle exit execs]", state1_exit_exec)
				{
				current_intrpt_type = op_intrpt_type ();
				
				}
				FSM_PROFILE_SECTION_OUT (state1_exit_exec)


			/** state (idle) transition processing **/
			FSM_PROFILE_SECTION_IN ("net_in_tdma_g [idle trans conditions]", state1_trans_conds)
			FSM_INIT_COND (FROM_RX)
			FSM_TEST_COND (FROM_SRC)
			FSM_DFLT_COND
			FSM_TEST_LOGIC ("idle")
			FSM_PROFILE_SECTION_OUT (state1_trans_conds)

			FSM_TRANSIT_SWITCH
				{
				FSM_CASE_TRANSIT (0, 2, state2_enter_exec, ;, "FROM_RX", "", "idle", "fr_rx", "tr_13", "net_in_tdma_g [idle -> fr_rx : FROM_RX / ]")
				FSM_CASE_TRANSIT (1, 3, state3_enter_exec, ;, "FROM_SRC", "", "idle", "fr_src", "tr_15", "net_in_tdma_g [idle -> fr_src : FROM_SRC / ]")
				FSM_CASE_TRANSIT (2, 1, state1_enter_exec, ;, "default", "", "idle", "idle", "tr_10", "net_in_tdma_g [idle -> idle : default / ]")
				}
				/*---------------------------------------------------------*/



			/** state (fr_rx) enter executives **/
			FSM_STATE_ENTER_FORCED (2, "fr_rx", state2_enter_exec, "net_in_tdma_g [fr_rx enter execs]")
				FSM_PROFILE_SECTION_IN ("net_in_tdma_g [fr_rx enter execs]", state2_enter_exec)
				{
				int pk_id;
				
				pkptr =  op_pk_get (RX_IN_STRM);
				op_pk_nfd_get(pkptr,"TYPE",&type);
				op_pk_nfd_get(pkptr,"SEND",&pk_id);
				
				if(type==0x10)
					{
					if(my_address!=0xFE) op_pk_destroy(pkptr);
					else
						{
						op_pk_send(pkptr,SINK_OUT_STRM);
						}
					}
				
				
				
				
				
				}
				FSM_PROFILE_SECTION_OUT (state2_enter_exec)

			/** state (fr_rx) exit executives **/
			FSM_STATE_EXIT_FORCED (2, "fr_rx", "net_in_tdma_g [fr_rx exit execs]")


			/** state (fr_rx) transition processing **/
			FSM_TRANSIT_FORCE (1, state1_enter_exec, ;, "default", "", "fr_rx", "idle", "tr_14", "net_in_tdma_g [fr_rx -> idle : default / ]")
				/*---------------------------------------------------------*/



			/** state (fr_src) enter executives **/
			FSM_STATE_ENTER_FORCED (3, "fr_src", state3_enter_exec, "net_in_tdma_g [fr_src enter execs]")
				FSM_PROFILE_SECTION_IN ("net_in_tdma_g [fr_src enter execs]", state3_enter_exec)
				{
				pkptr =  op_pk_get (SRC_IN_STRM);
				op_pk_nfd_get (pkptr, "TYPE", &type);
				
				if(type==0x10)//交互节点的上报帧
					{
					op_pk_nfd_set(pkptr,"SEND",my_address);
					op_pk_nfd_set(pkptr,"FL",(double) op_pk_total_size_get (pkptr));
					///CRC
					op_pk_send(pkptr,TX_OUT_STRM);
					}
				}
				FSM_PROFILE_SECTION_OUT (state3_enter_exec)

			/** state (fr_src) exit executives **/
			FSM_STATE_EXIT_FORCED (3, "fr_src", "net_in_tdma_g [fr_src exit execs]")


			/** state (fr_src) transition processing **/
			FSM_TRANSIT_FORCE (1, state1_enter_exec, ;, "default", "", "fr_src", "idle", "tr_16", "net_in_tdma_g [fr_src -> idle : default / ]")
				/*---------------------------------------------------------*/



			}


		FSM_EXIT (0,"net_in_tdma_g")
		}
	}




void
_op_net_in_tdma_g_diag (OP_SIM_CONTEXT_ARG_OPT)
	{
	/* No Diagnostic Block */
	}




void
_op_net_in_tdma_g_terminate (OP_SIM_CONTEXT_ARG_OPT)
	{

	FIN_MT (_op_net_in_tdma_g_terminate ())


	/* No Termination Block */

	Vos_Poolmem_Dealloc (op_sv_ptr);

	FOUT
	}


/* Undefine shortcuts to state variables to avoid */
/* syntax error in direct access to fields of */
/* local variable prs_ptr in _op_net_in_tdma_g_svar function. */
#undef my_node_id
#undef my_id
#undef my_address
#undef type

#undef FIN_PREAMBLE_DEC
#undef FIN_PREAMBLE_CODE

#define FIN_PREAMBLE_DEC
#define FIN_PREAMBLE_CODE

VosT_Obtype
_op_net_in_tdma_g_init (int * init_block_ptr)
	{
	VosT_Obtype obtype = OPC_NIL;
	FIN_MT (_op_net_in_tdma_g_init (init_block_ptr))

	obtype = Vos_Define_Object_Prstate ("proc state vars (net_in_tdma_g)",
		sizeof (net_in_tdma_g_state));
	*init_block_ptr = 0;

	FRET (obtype)
	}

VosT_Address
_op_net_in_tdma_g_alloc (VosT_Obtype obtype, int init_block)
	{
#if !defined (VOSD_NO_FIN)
	int _op_block_origin = 0;
#endif
	net_in_tdma_g_state * ptr;
	FIN_MT (_op_net_in_tdma_g_alloc (obtype))

	ptr = (net_in_tdma_g_state *)Vos_Alloc_Object (obtype);
	if (ptr != OPC_NIL)
		{
		ptr->_op_current_block = init_block;
#if defined (OPD_ALLOW_ODB)
		ptr->_op_current_state = "net_in_tdma_g [init enter execs]";
#endif
		}
	FRET ((VosT_Address)ptr)
	}



void
_op_net_in_tdma_g_svar (void * gen_ptr, const char * var_name, void ** var_p_ptr)
	{
	net_in_tdma_g_state		*prs_ptr;

	FIN_MT (_op_net_in_tdma_g_svar (gen_ptr, var_name, var_p_ptr))

	if (var_name == OPC_NIL)
		{
		*var_p_ptr = (void *)OPC_NIL;
		FOUT
		}
	prs_ptr = (net_in_tdma_g_state *)gen_ptr;

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
	*var_p_ptr = (void *)OPC_NIL;

	FOUT
	}

