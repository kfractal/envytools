/*
 *  Copyright (c) 2013-2016, NVIDIA CORPORATION.  All rights reserved.
 */
#include "gen_registers.h"
#if 0
#include "dev_bus.h"
#include "dev_ce2_pri.h"
#include "dev_fifo.h"
#include "dev_ctxsw_prog.h"
#include "dev_fb.h"
#include "dev_fifo.h"
#include "dev_flush.h"
#include "dev_graphics_nobundle.h"
#include "dev_ltc.h"
#include "dev_master.h"
#include "dev_mmu.h"
#include "dev_pbdma.h"
#include "dev_pri_ringmaster.h"
#include "dev_pri_ringstation_sys.h"
#include "hwproject.h"
#include "dev_pwr_pri.h"
#include "dev_ram.h"
#include "dev_therm.h"
#include "dev_top.h"
#include "dev_trim.h"
#include "dev_timer.h"

/* some header elements added after final chip def'n */
#ifndef NV_CTXSW_MAIN_IMAGE_PRIV_ACCESS_MAP_CONFIG
#include "dev_ctxsw_prog_addendum.h"
#endif
#endif

static void emit_bus_group()
{
	begin_group( G_SPEC( bus ) );
	emit_register( R_SPEC( bar1_block, NV_PBUS_BAR1_BLOCK ) );

	emit_field   ( F_SPEC( ptr,   NV_PBUS_BAR1_BLOCK_PTR, "f") );

	emit_field   ( F_SPEC( target,  NV_PBUS_BAR1_BLOCK_TARGET, ".") );
	emit_constant( C_SPEC( vid_mem, NV_PBUS_BAR1_BLOCK_TARGET_VID_MEM, "f") );

	emit_field   ( F_SPEC( mode,    NV_PBUS_BAR1_BLOCK_MODE, ".") );
	emit_constant( C_SPEC( virtual, NV_PBUS_BAR1_BLOCK_MODE_VIRTUAL, "f") );

	emit_register( R_SPEC( bar2_block, NV_PBUS_BAR2_BLOCK ) );

	emit_field   ( F_SPEC( ptr,   NV_PBUS_BAR2_BLOCK_PTR, "f") );

	emit_field   ( F_SPEC( target,  NV_PBUS_BAR2_BLOCK_TARGET, ".") );
	emit_constant( C_SPEC( vid_mem, NV_PBUS_BAR2_BLOCK_TARGET_VID_MEM, "f") );

	emit_field   ( F_SPEC( mode,    NV_PBUS_BAR2_BLOCK_MODE, ".") );
	emit_constant( C_SPEC( virtual, NV_PBUS_BAR2_BLOCK_MODE_VIRTUAL, "f") );

	end_scope();

	emit_constant( C_SPEC( bar1_block_ptr_shift, NV_PBUS_BAR1_BLOCK_PTR_SHIFT, "v") );
	emit_constant( C_SPEC( bar2_block_ptr_shift, NV_PBUS_BAR2_BLOCK_PTR_SHIFT, "v") );

	emit_register( R_SPEC( intr_0, NV_PBUS_INTR_0 ) );
	emit_field   ( F_SPEC( pri_squash, NV_PBUS_INTR_0_PRI_SQUASH, "m") );
	emit_field   ( F_SPEC( pri_fecserr, NV_PBUS_INTR_0_PRI_FECSERR, "m") );
	emit_field   ( F_SPEC( pri_timeout, NV_PBUS_INTR_0_PRI_TIMEOUT, "m") );

	emit_register( R_SPEC( intr_en_0, NV_PBUS_INTR_EN_0 ) );
	emit_field   ( F_SPEC( pri_squash, NV_PBUS_INTR_EN_0_PRI_SQUASH, "m") );
	emit_field   ( F_SPEC( pri_fecserr, NV_PBUS_INTR_EN_0_PRI_FECSERR, "m") );
	emit_field   ( F_SPEC( pri_timeout, NV_PBUS_INTR_EN_0_PRI_TIMEOUT, "m") );
	end_group();
}


static void emit_ccsr_group()
{
	begin_group  ( G_SPEC( ccsr ) );

	emit_register( IR_SPEC( channel_inst, NV_PCCSR_CHANNEL_INST ) );
	emit_constant( C_SPEC( _size_1,       NV_PCCSR_CHANNEL_INST__SIZE_1, "v") );

	emit_field   ( F_SPEC( ptr, NV_PCCSR_CHANNEL_INST_PTR, "f") );

	emit_field   ( F_SPEC( target,  NV_PCCSR_CHANNEL_INST_TARGET, ".") );
	emit_constant( C_SPEC( vid_mem, NV_PCCSR_CHANNEL_INST_TARGET_VID_MEM, "f") );

	emit_field   ( F_SPEC( bind,  NV_PCCSR_CHANNEL_INST_BIND, ".") );
	emit_constant( C_SPEC( false, NV_PCCSR_CHANNEL_INST_BIND_FALSE, "f") );
	emit_constant( C_SPEC( true,  NV_PCCSR_CHANNEL_INST_BIND_TRUE,  "f") );

	emit_register(IR_SPEC( channel, NV_PCCSR_CHANNEL ) );
	emit_constant( C_SPEC( _size_1, NV_PCCSR_CHANNEL__SIZE_1, "v") );
	emit_field   ( F_SPEC( enable,  NV_PCCSR_CHANNEL_ENABLE, "v") );
	emit_field   ( F_SPEC( enable_set, NV_PCCSR_CHANNEL_ENABLE_SET, "f") );
	emit_constant( C_SPEC( true,       NV_PCCSR_CHANNEL_ENABLE_SET_TRUE, "f") );
	emit_field   ( F_SPEC( enable_clr, NV_PCCSR_CHANNEL_ENABLE_CLR, ".") );
	emit_constant( C_SPEC( true,       NV_PCCSR_CHANNEL_ENABLE_CLR_TRUE, "f") );
	end_field();
	emit_field   ( F_SPEC( runlist, NV_PCCSR_CHANNEL_RUNLIST, "f") );
	emit_field   ( F_SPEC( status,  NV_PCCSR_CHANNEL_STATUS, "v") );
	emit_field   ( F_SPEC( busy,    NV_PCCSR_CHANNEL_BUSY, "v") );

	end_group();
}

static void emit_ce2_group()
{
	begin_group  ( G_SPEC( ce2 ) );

	emit_register( R_SPEC( intr_status, NV_PCE2_COP2_INTR_STATUS ) );
	emit_field   ( F_SPEC( blockpipe, NV_PCE2_COP2_INTR_STATUS_BLOCKPIPE, ".") );
	emit_constant( C_SPEC( pending,NV_PCE2_COP2_INTR_STATUS_BLOCKPIPE_PENDING, "f") );
	emit_constant( C_SPEC( reset, NV_PCE2_COP2_INTR_STATUS_BLOCKPIPE_RESET, "f") );
	emit_field   ( F_SPEC( nonblockpipe, NV_PCE2_COP2_INTR_STATUS_NONBLOCKPIPE, ".") );
	emit_constant( C_SPEC( pending, NV_PCE2_COP2_INTR_STATUS_NONBLOCKPIPE_PENDING, "f") );
	emit_constant( C_SPEC( reset, NV_PCE2_COP2_INTR_STATUS_NONBLOCKPIPE_RESET, "f") );
	emit_field   ( F_SPEC( launcherr, NV_PCE2_COP2_INTR_STATUS_LAUNCHERR, ".") );
	emit_constant( C_SPEC( pending, NV_PCE2_COP2_INTR_STATUS_LAUNCHERR_PENDING, "f") );
	emit_constant( C_SPEC( reset, NV_PCE2_COP2_INTR_STATUS_LAUNCHERR_RESET, "f") );

	end_group();
}


static void emit_ctxsw_prog_group()
{
	begin_group( G_SPEC( ctxsw_prog ) );
	emit_constant( C_SPEC( fecs_header,       NV_CTXSW_FECS_HEADER, "v") ); /* this is goofy/wrong */

	emit_offset  ( O_SPEC( main_image_num_gpcs,        NV_CTXSW_MAIN_IMAGE_NUM_GPCS ) );
	emit_offset  ( O_SPEC( main_image_patch_count,     NV_CTXSW_MAIN_IMAGE_PRI_PATCH_COUNT ) );
	emit_offset  ( O_SPEC( main_image_patch_adr_lo,    NV_CTXSW_MAIN_IMAGE_PRI_PATCH_ADR_LO ) );
	emit_offset  ( O_SPEC( main_image_patch_adr_hi,    NV_CTXSW_MAIN_IMAGE_PRI_PATCH_ADR_HI ) );
	emit_offset  ( O_SPEC( main_image_zcull,           NV_CTXSW_MAIN_IMAGE_ZCULL ) );
	emit_field   ( F_SPEC( mode,                       NV_CTXSW_MAIN_IMAGE_ZCULL_MODE, ".") );
	emit_constant( C_SPEC( no_ctxsw,                   NV_CTXSW_MAIN_IMAGE_ZCULL_MODE_NO_CTXSW, "v") );
	emit_constant( C_SPEC( separate_buffer,            NV_CTXSW_MAIN_IMAGE_ZCULL_MODE_SEPARATE_BUFFER, "v") );
	emit_offset  ( O_SPEC( main_image_zcull_ptr,       NV_CTXSW_MAIN_IMAGE_ZCULL_PTR) );
	emit_offset  ( O_SPEC( main_image_pm,              NV_CTXSW_MAIN_IMAGE_PM ) );
	emit_field   ( F_SPEC( mode,                       NV_CTXSW_MAIN_IMAGE_PM_MODE, "m") );
	emit_constant( C_SPEC( no_ctxsw,                   NV_CTXSW_MAIN_IMAGE_PM_MODE_NO_CTXSW, "f") );
	emit_field   ( F_SPEC( smpc_mode,                  NV_CTXSW_MAIN_IMAGE_SMPC_MODE, "m") );
	emit_constant( C_SPEC( ctxsw,                      NV_CTXSW_MAIN_IMAGE_SMPC_MODE_CTXSW, "f") );
	emit_constant( C_SPEC( no_ctxsw,                   NV_CTXSW_MAIN_IMAGE_SMPC_MODE_NO_CTXSW, "f") );
	emit_offset  ( O_SPEC( main_image_pm_ptr,          NV_CTXSW_MAIN_IMAGE_PM_PTR) );
	emit_offset  ( O_SPEC( main_image_num_save_ops,    NV_CTXSW_MAIN_IMAGE_NUM_SAVE_OPERATIONS ) );
	emit_offset  ( O_SPEC( main_image_num_restore_ops, NV_CTXSW_MAIN_IMAGE_NUM_RESTORE_OPERATIONS ) );
	emit_offset  ( O_SPEC( main_image_magic_value,     NV_CTXSW_MAIN_IMAGE_MAGIC_VALUE) );
	emit_field   ( F_SPEC( v,                          NV_CTXSW_MAIN_IMAGE_MAGIC_VALUE_V, ".") );
	emit_constant( C_SPEC( value,                      NV_CTXSW_MAIN_IMAGE_MAGIC_VALUE_V_VALUE, "v") );

	emit_offset( O_SPEC( local_priv_register_ctl,      NV_CTXSW_LOCAL_PRIV_REGISTER_CTL ) );
	emit_field( F_SPEC( offset,                        NV_CTXSW_LOCAL_PRIV_REGISTER_CTL_OFFSET, "v") );
	end_scope();
	emit_offset( O_SPEC( local_image_ppc_info,         NV_CTXSW_LOCAL_IMAGE_PPC_INFO ) );
	emit_field ( F_SPEC( num_ppcs,                     NV_CTXSW_LOCAL_IMAGE_PPC_INFO_NUM_PPCS, "v") );
	emit_field ( F_SPEC( ppc_mask,                     NV_CTXSW_LOCAL_IMAGE_PPC_INFO_PPC_MASK, "v") );
	end_scope();
	emit_offset( O_SPEC( local_image_num_tpcs,         NV_CTXSW_LOCAL_IMAGE_NUM_TPCS ) );
	end_scope();
	emit_offset  ( O_SPEC( local_magic_value,          NV_CTXSW_LOCAL_MAGIC_VALUE) );
	emit_field   ( F_SPEC( v,                          NV_CTXSW_LOCAL_MAGIC_VALUE_V, ".") );
	emit_constant( C_SPEC( value,                      NV_CTXSW_LOCAL_MAGIC_VALUE_V_VALUE, "v") );
	end_scope();

	emit_offset( O_SPEC( main_extended_buffer_ctl,                 NV_CTXSW_MAIN_EXTENDED_BUFFER_CTL) );
	emit_field ( F_SPEC( offset,                                   NV_CTXSW_MAIN_EXTENDED_BUFFER_CTL_OFFSET, "v") );
	emit_field ( F_SPEC( size,                                     NV_CTXSW_MAIN_EXTENDED_BUFFER_CTL_SIZE, "v" ) );
	end_scope();

	emit_constant( C_SPEC( extended_buffer_segments_size_in_bytes,               NV_CTXSW_EXTENDED_BUFFER_SEGMENTS_SIZE_IN_BYTES, "v") );
	emit_constant( C_SPEC( extended_marker_size_in_bytes,                        NV_CTXSW_EXTENDED_MARKER_SIZE_IN_BYTES, "v") );
	emit_constant( C_SPEC( extended_sm_dsm_perf_counter_register_stride,         NV_CTXSW_EXTENDED_SM_DSM_PERF_COUNTER_REGISTER_STRIDE, "v") );
	emit_constant( C_SPEC( extended_sm_dsm_perf_counter_control_register_stride, NV_CTXSW_EXTENDED_SM_DSM_PERF_COUNTER_CONTROL_REGISTER_STRIDE, "v") );
	emit_constant( C_SPEC( extended_num_smpc_quadrants,                          NV_CTXSW_EXTENDED_NUM_SMPC_QUADRANTS, "v" ) );
	end_scope();

	emit_offset  ( O_SPEC( main_image_priv_access_map_config, NV_CTXSW_MAIN_IMAGE_PRIV_ACCESS_MAP_CONFIG) );
	emit_field   ( F_SPEC( mode, NV_CTXSW_MAIN_IMAGE_PRIV_ACCESS_MAP_CONFIG_MODE, "") );
	emit_constant( C_SPEC( allow_all, NV_CTXSW_MAIN_IMAGE_PRIV_ACCESS_MAP_CONFIG_MODE_ALLOW_ALL, "f") );
	emit_constant( C_SPEC( use_map, NV_CTXSW_MAIN_IMAGE_PRIV_ACCESS_MAP_CONFIG_MODE_USE_MAP, "f") );

	emit_offset  ( O_SPEC( main_image_priv_access_map_addr_lo, NV_CTXSW_MAIN_IMAGE_PRIV_ACCESS_MAP_ADDR_LO) );
	emit_offset  ( O_SPEC( main_image_priv_access_map_addr_hi, NV_CTXSW_MAIN_IMAGE_PRIV_ACCESS_MAP_ADDR_HI) );

	emit_offset  ( O_SPEC( main_image_misc_options, NV_CTXSW_MAIN_IMAGE_MISC_OPTIONS) );
	emit_field   ( F_SPEC( verif_features, NV_CTXSW_MAIN_IMAGE_MISC_OPTIONS_VERIF_FEATURES, "m") );
	emit_constant( C_SPEC( disabled, NV_CTXSW_MAIN_IMAGE_MISC_OPTIONS_VERIF_FEATURES_DISABLED, "f") );

	end_group();

}


static void emit_fb_group()
{
	begin_group( G_SPEC( fb ) );

	emit_register( R_SPEC( mmu_ctrl, NV_PFB_PRI_MMU_CTRL ) );
	emit_field   ( F_SPEC( vm_pg_size, NV_PFB_PRI_MMU_CTRL_VM_PG_SIZE, "f") );
	emit_constant( C_SPEC( 128kb, NV_PFB_PRI_MMU_CTRL_VM_PG_SIZE_128KB, "f") );
	emit_constant( C_SPEC( 64kb, NV_PFB_PRI_MMU_CTRL_VM_PG_SIZE_64KB, "f") );
	emit_field   ( F_SPEC( pri_fifo_empty, NV_PFB_PRI_MMU_CTRL_PRI_FIFO_EMPTY, "v") );
	emit_constant( C_SPEC( false, NV_PFB_PRI_MMU_CTRL_PRI_FIFO_EMPTY_FALSE, "f") );
	emit_field   ( F_SPEC( pri_fifo_space, NV_PFB_PRI_MMU_CTRL_PRI_FIFO_SPACE, "v") );

	emit_register( R_SPEC( mmu_invalidate_pdb, NV_PFB_PRI_MMU_INVALIDATE_PDB ) );
	emit_field   ( F_SPEC( aperture, NV_PFB_PRI_MMU_INVALIDATE_PDB_APERTURE, ".") );
	emit_constant( C_SPEC( vid_mem, NV_PFB_PRI_MMU_INVALIDATE_PDB_APERTURE_VID_MEM, "f") );
	emit_field   ( F_SPEC( addr, NV_PFB_PRI_MMU_INVALIDATE_PDB_ADDR, "f") );

	emit_register( R_SPEC( mmu_invalidate, NV_PFB_PRI_MMU_INVALIDATE ) );
	emit_field   ( F_SPEC( all_va, NV_PFB_PRI_MMU_INVALIDATE_ALL_VA, ".") );
	emit_constant( C_SPEC( true, NV_PFB_PRI_MMU_INVALIDATE_ALL_VA_TRUE, "f") );
	emit_field   ( F_SPEC( all_pdb, NV_PFB_PRI_MMU_INVALIDATE_ALL_PDB, ".") );
	emit_constant( C_SPEC( true, NV_PFB_PRI_MMU_INVALIDATE_ALL_PDB_TRUE, "f") );
	emit_field   ( F_SPEC( trigger, NV_PFB_PRI_MMU_INVALIDATE_TRIGGER, "") );
	emit_constant( C_SPEC( true, NV_PFB_PRI_MMU_INVALIDATE_TRIGGER_TRUE, "f") );

	emit_register( R_SPEC( mmu_debug_wr, NV_PFB_PRI_MMU_DEBUG_WR ) );
	emit_field   ( F_SPEC( aperture, NV_PFB_PRI_MMU_DEBUG_WR_APERTURE, "") );
	emit_constant( C_SPEC( vid_mem, NV_PFB_PRI_MMU_DEBUG_WR_APERTURE_VID_MEM, "f") );
	emit_field   ( F_SPEC( vol, NV_PFB_PRI_MMU_DEBUG_WR_VOL, ".") );
	emit_constant( C_SPEC( false, NV_PFB_PRI_MMU_DEBUG_WR_VOL_FALSE, "f") );
	emit_constant( C_SPEC( true, NV_PFB_PRI_MMU_DEBUG_WR_VOL_TRUE, "") );
	emit_field   ( F_SPEC( addr, NV_PFB_PRI_MMU_DEBUG_WR_ADDR, "f") );
	emit_constant( C_SPEC( alignment, NV_PFB_PRI_MMU_DEBUG_WR_ADDR_ALIGNMENT, "v") );

	emit_register( R_SPEC( mmu_debug_rd, NV_PFB_PRI_MMU_DEBUG_RD ) );
	emit_field   ( F_SPEC( aperture, NV_PFB_PRI_MMU_DEBUG_RD_APERTURE, ".") );
	emit_constant( C_SPEC( vid_mem, NV_PFB_PRI_MMU_DEBUG_RD_APERTURE_VID_MEM, "f") );
	emit_field   ( F_SPEC( vol, NV_PFB_PRI_MMU_DEBUG_RD_VOL, ".") );
	emit_constant( C_SPEC( false, NV_PFB_PRI_MMU_DEBUG_RD_VOL_FALSE, "f") );
	emit_field   ( F_SPEC( addr, NV_PFB_PRI_MMU_DEBUG_RD_ADDR, "f") );
	emit_constant( C_SPEC( alignment, NV_PFB_PRI_MMU_DEBUG_RD_ADDR_ALIGNMENT, "v") );

	emit_register( R_SPEC( mmu_debug_ctrl, NV_PFB_PRI_MMU_DEBUG_CTRL) );
	emit_field   ( F_SPEC( debug, NV_PFB_PRI_MMU_DEBUG_CTRL_DEBUG, "v") );
	emit_field   ( F_SPEC( debug, NV_PFB_PRI_MMU_DEBUG_CTRL_DEBUG, "m") );
	emit_constant( C_SPEC( enabled, NV_PFB_PRI_MMU_DEBUG_CTRL_DEBUG_ENABLED, "v") );
	emit_constant( C_SPEC( disabled, NV_PFB_PRI_MMU_DEBUG_CTRL_DEBUG_DISABLED, "v") );

	emit_register( R_SPEC( mmu_vpr_info, NV_PFB_PRI_MMU_VPR_INFO) );
	emit_field   ( F_SPEC( fetch, NV_PFB_PRI_MMU_VPR_INFO_FETCH, "v") );
	emit_constant( C_SPEC( false, NV_PFB_PRI_MMU_VPR_INFO_FETCH_FALSE, "v") );
	emit_constant( C_SPEC( true, NV_PFB_PRI_MMU_VPR_INFO_FETCH_TRUE, "v") );

	end_group();

}


static void emit_fifo_group()
{
	begin_group( G_SPEC( fifo ) );
	emit_register( R_SPEC( bar1_base, NV_PFIFO_BAR1_BASE ) );
	emit_field   ( F_SPEC( ptr, NV_PFIFO_BAR1_BASE_PTR, "f") );
	emit_constant( C_SPEC( align_shift, NV_PFIFO_BAR1_BASE_PTR_ALIGN_SHIFT, "v") );
	emit_field   ( F_SPEC( valid, NV_PFIFO_BAR1_BASE_VALID, ".") );
	emit_constant( C_SPEC( false, NV_PFIFO_BAR1_BASE_VALID_FALSE, "f") );
	emit_constant( C_SPEC( true, NV_PFIFO_BAR1_BASE_VALID_TRUE, "f") );

	emit_register( R_SPEC( runlist_base, NV_PFIFO_RUNLIST_BASE ) );
	emit_field   ( F_SPEC( ptr, NV_PFIFO_RUNLIST_BASE_PTR, "f") );
	emit_field   ( F_SPEC( target, NV_PFIFO_RUNLIST_BASE_TARGET, ".") );
	emit_constant( C_SPEC( vid_mem, NV_PFIFO_RUNLIST_BASE_TARGET_VID_MEM, "f") );


	emit_register( R_SPEC( runlist, NV_PFIFO_RUNLIST ) );
	emit_field   ( F_SPEC( engine, NV_PFIFO_RUNLIST_ID, "f") );

	emit_register( IR_SPEC( eng_runlist_base, NV_PFIFO_ENG_RUNLIST_BASE ) );
	emit_constant( C_SPEC( _size_1, NV_PFIFO_ENG_RUNLIST_BASE__SIZE_1, "v") );

	emit_register( IR_SPEC( eng_runlist, NV_PFIFO_ENG_RUNLIST ) );
	emit_constant( C_SPEC( _size_1, NV_PFIFO_ENG_RUNLIST__SIZE_1, "v") );
	emit_field   ( F_SPEC( length, NV_PFIFO_ENG_RUNLIST_LENGTH, "f") );
	emit_field   ( F_SPEC( pending, NV_PFIFO_ENG_RUNLIST_PENDING, ".") );
	emit_constant( C_SPEC( true, NV_PFIFO_ENG_RUNLIST_PENDING_TRUE, "f") );

	emit_register( IR_SPEC( runlist_timeslice, NV_PFIFO_RUNLIST_TIMESLICE ) );
	emit_field   ( F_SPEC( timeout, NV_PFIFO_RUNLIST_TIMESLICE_TIMEOUT, ".") );
	emit_constant( C_SPEC( 128, NV_PFIFO_RUNLIST_TIMESLICE_TIMEOUT_128, "f") );
	emit_field   ( F_SPEC( timescale, NV_PFIFO_RUNLIST_TIMESLICE_TIMESCALE, ".") );
	emit_constant( C_SPEC( 3, NV_PFIFO_RUNLIST_TIMESLICE_TIMESCALE_3, "f") );
	emit_field   ( F_SPEC( enable, NV_PFIFO_RUNLIST_TIMESLICE_ENABLE, ".") );
	emit_constant( C_SPEC( true, NV_PFIFO_RUNLIST_TIMESLICE_ENABLE_TRUE, "f") );

	emit_register( R_SPEC( eng_timeout, NV_PFIFO_ENG_TIMEOUT ) );
	emit_field   ( F_SPEC( period, NV_PFIFO_ENG_TIMEOUT_PERIOD, ".") );
	emit_constant( C_SPEC( max, NV_PFIFO_ENG_TIMEOUT_PERIOD_MAX, "f") );
	emit_field   ( F_SPEC( detection, NV_PFIFO_ENG_TIMEOUT_DETECTION, ".") );
	emit_constant( C_SPEC( enabled, NV_PFIFO_ENG_TIMEOUT_DETECTION_ENABLED, "f") );
	emit_constant( C_SPEC( disabled, NV_PFIFO_ENG_TIMEOUT_DETECTION_DISABLED, "f") );

	emit_register( IR_SPEC( pb_timeslice, NV_PFIFO_PB_TIMESLICE ) );
	emit_field   ( F_SPEC( timeout, NV_PFIFO_PB_TIMESLICE_TIMEOUT, ".") );
	emit_constant( C_SPEC( 16, NV_PFIFO_PB_TIMESLICE_TIMEOUT_16, "f") );
	emit_field   ( F_SPEC( timescale, NV_PFIFO_PB_TIMESLICE_TIMESCALE, ".") );
	emit_constant( C_SPEC( 0, NV_PFIFO_PB_TIMESLICE_TIMESCALE_0, "f") );
	emit_field   ( F_SPEC( enable, NV_PFIFO_PB_TIMESLICE_ENABLE, ".") );
	emit_constant( C_SPEC( true, NV_PFIFO_PB_TIMESLICE_ENABLE_TRUE, "f") );

	emit_register( IR_SPEC( pbdma_map, NV_PFIFO_PBDMA_MAP ) );

	emit_register( R_SPEC( intr_0, NV_PFIFO_INTR_0 ) );
	emit_field   ( F_SPEC( bind_error, NV_PFIFO_INTR_0_BIND_ERROR, ".") );
	emit_constant( C_SPEC( pending, NV_PFIFO_INTR_0_BIND_ERROR_PENDING, "f") );
	emit_constant( C_SPEC( reset, NV_PFIFO_INTR_0_BIND_ERROR_RESET, "f") );
	emit_field   ( F_SPEC( pio_error, NV_PFIFO_INTR_0_PIO_ERROR, ".") );
	emit_constant( C_SPEC( pending, NV_PFIFO_INTR_0_PIO_ERROR_PENDING, "f") );
	emit_constant( C_SPEC( reset, NV_PFIFO_INTR_0_PIO_ERROR_RESET, "f") );
	emit_field   ( F_SPEC( sched_error, NV_PFIFO_INTR_0_SCHED_ERROR, ".") );
	emit_constant( C_SPEC( pending, NV_PFIFO_INTR_0_SCHED_ERROR_PENDING, "f") );
	emit_constant( C_SPEC( reset, NV_PFIFO_INTR_0_SCHED_ERROR_RESET, "f") );
	emit_field   ( F_SPEC( chsw_error, NV_PFIFO_INTR_0_CHSW_ERROR, ".") );
	emit_constant( C_SPEC( pending, NV_PFIFO_INTR_0_CHSW_ERROR_PENDING, "f") );
	emit_constant( C_SPEC( reset, NV_PFIFO_INTR_0_CHSW_ERROR_RESET, "f") );
	emit_field   ( F_SPEC( fb_flush_timeout, NV_PFIFO_INTR_0_FB_FLUSH_TIMEOUT, ".") );
	emit_constant( C_SPEC( pending, NV_PFIFO_INTR_0_FB_FLUSH_TIMEOUT_PENDING, "f") );
	emit_constant( C_SPEC( reset, NV_PFIFO_INTR_0_FB_FLUSH_TIMEOUT_RESET, "f") );
	emit_field   ( F_SPEC( lb_error, NV_PFIFO_INTR_0_LB_ERROR, ".") );
	emit_constant( C_SPEC( pending, NV_PFIFO_INTR_0_LB_ERROR_PENDING, "f") );
	emit_constant( C_SPEC( reset, NV_PFIFO_INTR_0_LB_ERROR_RESET, "f") );
	emit_field   ( F_SPEC( dropped_mmu_fault, NV_PFIFO_INTR_0_DROPPED_MMU_FAULT, ".") );
	emit_constant( C_SPEC( pending, NV_PFIFO_INTR_0_DROPPED_MMU_FAULT_PENDING, "f") );
	emit_constant( C_SPEC( reset, NV_PFIFO_INTR_0_DROPPED_MMU_FAULT_RESET, "f") );
	emit_field   ( F_SPEC( mmu_fault, NV_PFIFO_INTR_0_MMU_FAULT, ".") );
	emit_constant( C_SPEC( pending, NV_PFIFO_INTR_0_MMU_FAULT_PENDING, "f") );
	emit_field   ( F_SPEC( pbdma_intr, NV_PFIFO_INTR_0_PBDMA_INTR, ".") );
	emit_constant( C_SPEC( pending, NV_PFIFO_INTR_0_PBDMA_INTR_PENDING, "f") );
	emit_field   ( F_SPEC( runlist_event, NV_PFIFO_INTR_0_RUNLIST_EVENT, ".") );
	emit_constant( C_SPEC( pending, NV_PFIFO_INTR_0_RUNLIST_EVENT_PENDING, "f") );
	emit_field   ( F_SPEC( channel_intr, NV_PFIFO_INTR_0_CHANNEL_INTR, ".") );
	emit_constant( C_SPEC( pending, NV_PFIFO_INTR_0_CHANNEL_INTR_PENDING, "f") );
	emit_constant( C_SPEC( reset, NV_PFIFO_INTR_0_CHANNEL_INTR_RESET, ".") );

	emit_register( R_SPEC( intr_en_0, NV_PFIFO_INTR_EN_0 ) );
	emit_field   ( F_SPEC( sched_error, NV_PFIFO_INTR_EN_0_SCHED_ERROR, "fm") );
	emit_field   ( F_SPEC( mmu_fault, NV_PFIFO_INTR_EN_0_MMU_FAULT, "fm") );

	emit_register( R_SPEC( intr_en_1, NV_PFIFO_INTR_EN_1 ) );

	emit_register( R_SPEC( intr_bind_error, NV_PFIFO_INTR_BIND_ERROR ) );

	emit_register( R_SPEC( intr_sched_error, NV_PFIFO_INTR_SCHED_ERROR ) );
	emit_field   ( F_SPEC( code, NV_PFIFO_INTR_SCHED_ERROR_CODE, "f") );
	emit_constant( C_SPEC( ctxsw_timeout, NV_PFIFO_INTR_SCHED_ERROR_CODE_CTXSW_TIMEOUT, "v") );

	emit_register( R_SPEC( intr_chsw_error, NV_PFIFO_INTR_CHSW_ERROR ) );

	emit_register( R_SPEC( intr_mmu_fault_id, NV_PFIFO_INTR_MMU_FAULT_ID ) );
	end_scope();

	emit_constant( C_SPEC( intr_mmu_fault_eng_id_graphics, NV_PFIFO_INTR_MMU_FAULT_ENG_ID_GRAPHICS, "") );

	emit_register( IR_SPEC( intr_mmu_fault_inst, NV_PFIFO_INTR_MMU_FAULT_INST ) );
	emit_field   ( F_SPEC( ptr, NV_PFIFO_INTR_MMU_FAULT_INST_PTR, "v") );
	end_scope();

	emit_constant( C_SPEC( intr_mmu_fault_inst_ptr_align_shift, NV_PFIFO_INTR_MMU_FAULT_INST_PTR_ALIGN_SHIFT, "v") );

	emit_register( IR_SPEC( intr_mmu_fault_lo, NV_PFIFO_INTR_MMU_FAULT_LO ) );

	emit_register( IR_SPEC( intr_mmu_fault_hi, NV_PFIFO_INTR_MMU_FAULT_HI ) );

	emit_register( IR_SPEC( intr_mmu_fault_info, NV_PFIFO_INTR_MMU_FAULT_INFO ) );
	emit_field   ( F_SPEC( type, NV_PFIFO_INTR_MMU_FAULT_INFO_TYPE, "v") );
	emit_field   ( F_SPEC( engine_subid, NV_PFIFO_INTR_MMU_FAULT_INFO_ENGINE_SUBID, "v") );
	emit_constant( C_SPEC( gpc, NV_PFIFO_INTR_MMU_FAULT_INFO_ENGINE_SUBID_GPC, "v") );
	emit_constant( C_SPEC( hub, NV_PFIFO_INTR_MMU_FAULT_INFO_ENGINE_SUBID_HUB, "v") );
	emit_field   ( F_SPEC( client, NV_PFIFO_INTR_MMU_FAULT_INFO_CLIENT, "v") );

	emit_register( R_SPEC( intr_pbdma_id, NV_PFIFO_INTR_PBDMA_ID ) );
	emit_field   (IF_SPEC( status, NV_PFIFO_INTR_PBDMA_ID_STATUS, "f") );
	emit_constant( C_SPEC( _size_1, NV_PFIFO_INTR_PBDMA_ID_STATUS__SIZE_1, "v") );

	emit_register( R_SPEC( intr_runlist, NV_PFIFO_INTR_RUNLIST ) );

	emit_register( R_SPEC( fb_timeout, NV_PFIFO_FB_TIMEOUT ) );
	emit_field   ( F_SPEC( period, NV_PFIFO_FB_TIMEOUT_PERIOD, "m") );
	emit_constant( C_SPEC( max, NV_PFIFO_FB_TIMEOUT_PERIOD_MAX, "f") );

	emit_register( R_SPEC( pb_timeout, NV_PFIFO_PB_TIMEOUT ) );
	emit_field   ( F_SPEC( detection, NV_PFIFO_PB_TIMEOUT_DETECTION, ".") );
	emit_constant( C_SPEC( enabled, NV_PFIFO_PB_TIMEOUT_DETECTION_ENABLED, "f") );


	emit_register( R_SPEC( error_sched_disable, NV_PFIFO_ERROR_SCHED_DISABLE ) );

	emit_register( R_SPEC( sched_disable, NV_PFIFO_SCHED_DISABLE ) );
	emit_field   (IF_SPEC( runlist, NV_PFIFO_SCHED_DISABLE_RUNLIST, "fm") );
	end_scope();
	emit_constant( C_SPEC( sched_disable_true, NV_PFIFO_SCHED_DISABLE_TRUE, "v") );


	emit_register( R_SPEC( preempt, NV_PFIFO_PREEMPT ) );
	emit_field   ( F_SPEC( pending, NV_PFIFO_PREEMPT_PENDING, ".") );
	emit_constant( C_SPEC( true, NV_PFIFO_PREEMPT_PENDING_TRUE, "f") );

	emit_field   ( F_SPEC( type, NV_PFIFO_PREEMPT_TYPE, ".") );
	emit_constant( C_SPEC( channel, NV_PFIFO_PREEMPT_TYPE_CHANNEL, "f") );
	emit_constant( C_SPEC( tsg, NV_PFIFO_PREEMPT_TYPE_TSG, "f") );
	emit_field   ( F_SPEC( chid, NV_PFIFO_PREEMPT_ID, "f") );
	emit_field   ( F_SPEC( id, NV_PFIFO_PREEMPT_ID, "f") );

	emit_register( IR_SPEC( trigger_mmu_fault, NV_PFIFO_TRIGGER_MMU_FAULT) );
	emit_field   (  F_SPEC( id, NV_PFIFO_TRIGGER_MMU_FAULT_ID, "f") );
	emit_field   (  F_SPEC( enable, NV_PFIFO_TRIGGER_MMU_FAULT_ENABLE, "f") );

	emit_register( IR_SPEC( engine_status, NV_PFIFO_ENGINE_STATUS ) );
	emit_constant( C_SPEC( _size_1, NV_PFIFO_ENGINE_STATUS__SIZE_1, "v") );
	emit_field   ( F_SPEC( id, NV_PFIFO_ENGINE_STATUS_ID, "v") );
	emit_field   ( F_SPEC( id_type, NV_PFIFO_ENGINE_STATUS_ID_TYPE, "v") );
	emit_constant( C_SPEC( chid, NV_PFIFO_ENGINE_STATUS_ID_TYPE_CHID, "v") );
	emit_constant( C_SPEC( tsgid, NV_PFIFO_ENGINE_STATUS_ID_TYPE_TSGID, "v") );
	emit_field   ( F_SPEC( ctx_status, NV_PFIFO_ENGINE_STATUS_CTX_STATUS, "v") );
	emit_constant( C_SPEC( valid, NV_PFIFO_ENGINE_STATUS_CTX_STATUS_VALID, "v") );
	emit_constant( C_SPEC( ctxsw_load, NV_PFIFO_ENGINE_STATUS_CTX_STATUS_CTXSW_LOAD, "v") );
	emit_constant( C_SPEC( ctxsw_save, NV_PFIFO_ENGINE_STATUS_CTX_STATUS_CTXSW_SAVE, "v") );
	emit_constant( C_SPEC( ctxsw_switch, NV_PFIFO_ENGINE_STATUS_CTX_STATUS_CTXSW_SWITCH, "v") );
	emit_field   ( F_SPEC( next_id, NV_PFIFO_ENGINE_STATUS_NEXT_ID, "v") );
	emit_field   ( F_SPEC( next_id_type, NV_PFIFO_ENGINE_STATUS_NEXT_ID_TYPE, "v") );
	emit_constant( C_SPEC( chid, NV_PFIFO_ENGINE_STATUS_NEXT_ID_TYPE_CHID, "v") );
	emit_field   ( F_SPEC( faulted, NV_PFIFO_ENGINE_STATUS_FAULTED, "v") );
	emit_constant( C_SPEC( true, NV_PFIFO_ENGINE_STATUS_FAULTED_TRUE, "v") );
	emit_field   ( F_SPEC( engine, NV_PFIFO_ENGINE_STATUS_ENGINE, "v") );
	emit_constant( C_SPEC( idle, NV_PFIFO_ENGINE_STATUS_ENGINE_IDLE, "v") );
	emit_constant( C_SPEC( busy, NV_PFIFO_ENGINE_STATUS_ENGINE_BUSY, "v") );
	emit_field   ( F_SPEC( ctxsw, NV_PFIFO_ENGINE_STATUS_CTXSW, "v") );
	emit_constant( C_SPEC( in_progress, NV_PFIFO_ENGINE_STATUS_CTXSW_IN_PROGRESS, "fv") );

	emit_register( IR_SPEC( pbdma_status, NV_PFIFO_PBDMA_STATUS ) );
	emit_constant( C_SPEC( _size_1, NV_PFIFO_PBDMA_STATUS__SIZE_1, "v") );
	emit_field   ( F_SPEC( id, NV_PFIFO_PBDMA_STATUS_ID, "v") );
	emit_field   ( F_SPEC( id_type, NV_PFIFO_PBDMA_STATUS_ID_TYPE, "v") );
	emit_constant( C_SPEC( chid, NV_PFIFO_PBDMA_STATUS_ID_TYPE_CHID, "v") );
	emit_constant( C_SPEC( tsgid, NV_PFIFO_PBDMA_STATUS_ID_TYPE_TSGID, "v") );
	emit_field   ( F_SPEC( chan_status, NV_PFIFO_PBDMA_STATUS_CHAN_STATUS, "v") );
	emit_constant( C_SPEC( valid, NV_PFIFO_PBDMA_STATUS_CHAN_STATUS_VALID, "v") );
	emit_constant( C_SPEC( chsw_load, NV_PFIFO_PBDMA_STATUS_CHAN_STATUS_CHSW_LOAD, "v") );
	emit_constant( C_SPEC( chsw_save, NV_PFIFO_PBDMA_STATUS_CHAN_STATUS_CHSW_SAVE, "v") );
	emit_constant( C_SPEC( chsw_switch, NV_PFIFO_PBDMA_STATUS_CHAN_STATUS_CHSW_SWITCH, "v") );
	emit_field   ( F_SPEC( next_id, NV_PFIFO_PBDMA_STATUS_NEXT_ID, "v") );
	emit_field   ( F_SPEC( next_id_type, NV_PFIFO_PBDMA_STATUS_NEXT_ID_TYPE, "v") );
	emit_constant( C_SPEC( chid, NV_PFIFO_PBDMA_STATUS_NEXT_ID_TYPE_CHID, "v") );
	emit_field   ( F_SPEC( chsw, NV_PFIFO_PBDMA_STATUS_CHSW, "v") );
	emit_constant( C_SPEC( in_progress, NV_PFIFO_PBDMA_STATUS_CHSW_IN_PROGRESS, "v") );

	end_group();

}


static void emit_flush_group()
{
	begin_group( G_SPEC( flush  ) );

	emit_register( R_SPEC( l2_system_invalidate, NV_UFLUSH_L2_SYSMEM_INVALIDATE ) );
	emit_field   ( F_SPEC( pending, NV_UFLUSH_L2_SYSMEM_INVALIDATE_PENDING, "v") );
	emit_constant( C_SPEC( busy, NV_UFLUSH_L2_SYSMEM_INVALIDATE_PENDING_BUSY, "fv") );
	emit_field   ( F_SPEC( outstanding, NV_UFLUSH_L2_SYSMEM_INVALIDATE_OUTSTANDING, "v") );
	emit_constant( C_SPEC( true, NV_UFLUSH_L2_SYSMEM_INVALIDATE_OUTSTANDING_TRUE, "v") );

	emit_register( R_SPEC( l2_flush_dirty, NV_UFLUSH_L2_FLUSH_DIRTY ) );
	emit_field   ( F_SPEC( pending, NV_UFLUSH_L2_FLUSH_DIRTY_PENDING, "v") );
	emit_constant( C_SPEC( empty, NV_UFLUSH_L2_FLUSH_DIRTY_PENDING_EMPTY, "") );
	emit_constant( C_SPEC( busy, NV_UFLUSH_L2_FLUSH_DIRTY_PENDING_BUSY, "fv") );
	emit_field   ( F_SPEC( outstanding, NV_UFLUSH_L2_FLUSH_DIRTY_OUTSTANDING, "v") );
	emit_constant( C_SPEC( false, NV_UFLUSH_L2_FLUSH_DIRTY_OUTSTANDING_FALSE, "") );
	emit_constant( C_SPEC( true, NV_UFLUSH_L2_FLUSH_DIRTY_OUTSTANDING_TRUE, "v") );

	emit_register( R_SPEC( fb_flush, NV_UFLUSH_FB_FLUSH ) );
	emit_field   ( F_SPEC( pending, NV_UFLUSH_FB_FLUSH_PENDING, "v") );
	emit_constant( C_SPEC( busy, NV_UFLUSH_FB_FLUSH_PENDING_BUSY, "fv") );
	emit_field   ( F_SPEC( outstanding, NV_UFLUSH_FB_FLUSH_OUTSTANDING, "v") );
	emit_constant( C_SPEC( true, NV_UFLUSH_FB_FLUSH_OUTSTANDING_TRUE, "v") );

	end_group();

}




static void emit_gmmu_group()
{
	begin_group( G_SPEC( gmmu ) );
	begin_scope( S_SPEC( pde ) );

	emit_field   (WF_SPEC( aperture_big, NV_MMU_PDE_APERTURE_BIG,    ".") );
	emit_constant( C_SPEC( invalid,      NV_MMU_PDE_APERTURE_BIG_INVALID,      "f") );
	emit_constant( C_SPEC( video_memory, NV_MMU_PDE_APERTURE_BIG_VIDEO_MEMORY, "f") );

	emit_field   (WF_SPEC( size, NV_MMU_PDE_SIZE, ".") );
	emit_constant( C_SPEC( full, NV_MMU_PDE_SIZE_FULL, "f") );

	emit_field   (WF_SPEC( address_big_sys, NV_MMU_PDE_ADDRESS_BIG_SYS, "f") );

	emit_field   (WF_SPEC( aperture_small, NV_MMU_PDE_APERTURE_SMALL, ".") );
	emit_constant( C_SPEC( invalid,        NV_MMU_PDE_APERTURE_SMALL_INVALID, "f") );
	emit_constant( C_SPEC( video_memory,   NV_MMU_PDE_APERTURE_SMALL_VIDEO_MEMORY, "f") );

	emit_field   (WF_SPEC( vol_small, NV_MMU_PDE_VOL_SMALL, ".") );
	emit_constant( C_SPEC( true,      NV_MMU_PDE_VOL_SMALL_TRUE,  "f") );
	emit_constant( C_SPEC( false,     NV_MMU_PDE_VOL_SMALL_FALSE, "f") );


	emit_field   (WF_SPEC( vol_big, NV_MMU_PDE_VOL_BIG, ".") );
	emit_constant( C_SPEC( true,    NV_MMU_PDE_VOL_BIG_TRUE, "f") );
	emit_constant( C_SPEC( false,   NV_MMU_PDE_VOL_BIG_FALSE, "f") );

	emit_field   (WF_SPEC( address_small_sys, NV_MMU_PDE_ADDRESS_SMALL_SYS, "f") );
	end_field();

	/* pde scope */
	emit_constant( C_SPEC( address_shift, NV_MMU_PDE_ADDRESS_SHIFT, "v") );
	emit_constant( C_SPEC( _size, NV_MMU_PDE__SIZE, "v") );
	end_scope(); /* pde */


	begin_scope( S_SPEC( pte ) );
	emit_constant( C_SPEC( _size, NV_MMU_PTE__SIZE, "v") );

	emit_field   (WF_SPEC( valid, NV_MMU_PTE_VALID, ".") );
	emit_constant( C_SPEC( true,  NV_MMU_PTE_VALID_TRUE, "f") );
	emit_constant( C_SPEC( false, NV_MMU_PTE_VALID_FALSE, "f") );

	emit_field   (WF_SPEC( address_sys, NV_MMU_PTE_ADDRESS_SYS, "f") );

	emit_field   (WF_SPEC( vol,   NV_MMU_PTE_VOL, ".") );
	emit_constant( C_SPEC( true,  NV_MMU_PTE_VOL_TRUE, "f") );
	emit_constant( C_SPEC( false, NV_MMU_PTE_VOL_FALSE, "f") );

	emit_field   (WF_SPEC( aperture,     NV_MMU_PTE_APERTURE, ".") );
	emit_constant( C_SPEC( video_memory, NV_MMU_PTE_APERTURE_VIDEO_MEMORY, "f") );

	emit_field   (WF_SPEC( read_only, NV_MMU_PTE_READ_ONLY, ".") );
	emit_constant( C_SPEC( true,      NV_MMU_PTE_READ_ONLY_TRUE, "f") );

	emit_field   (WF_SPEC( write_disable, NV_MMU_PTE_WRITE_DISABLE, ".") );
	emit_constant( C_SPEC( true,          NV_MMU_PTE_WRITE_DISABLE_TRUE, "f") );

	emit_field   (WF_SPEC( read_disable, NV_MMU_PTE_READ_DISABLE, ".") );
	emit_constant( C_SPEC( true,         NV_MMU_PTE_READ_DISABLE_TRUE, "f") );

	emit_field   (WF_SPEC( comptagline, NV_MMU_PTE_COMPTAGLINE, "f") );

	end_field();
	emit_constant( C_SPEC( address_shift, NV_MMU_PTE_ADDRESS_SHIFT, "v") );

	emit_field   (WF_SPEC( kind, NV_MMU_PTE_KIND, "f") );
	emit_constant( C_SPEC( invalid, NV_MMU_PTE_KIND_INVALID, "v") );
	emit_constant( C_SPEC( pitch, NV_MMU_PTE_KIND_PITCH, "v") );
	emit_constant( C_SPEC( z16, NV_MMU_PTE_KIND_Z16, "v") );
	emit_constant( C_SPEC( z16_2c, NV_MMU_PTE_KIND_Z16_2C, "v") );
	emit_constant( C_SPEC( z16_ms2_2c, NV_MMU_PTE_KIND_Z16_MS2_2C, "v") );
	emit_constant( C_SPEC( z16_ms4_2c, NV_MMU_PTE_KIND_Z16_MS4_2C, "v") );
	emit_constant( C_SPEC( z16_ms8_2c, NV_MMU_PTE_KIND_Z16_MS8_2C, "v") );
	emit_constant( C_SPEC( z16_ms16_2c, NV_MMU_PTE_KIND_Z16_MS16_2C, "v") );
	emit_constant( C_SPEC( z16_2z, NV_MMU_PTE_KIND_Z16_2Z, "v") );
	emit_constant( C_SPEC( z16_ms2_2z, NV_MMU_PTE_KIND_Z16_MS2_2Z, "v") );
	emit_constant( C_SPEC( z16_ms4_2z, NV_MMU_PTE_KIND_Z16_MS4_2Z, "v") );
	emit_constant( C_SPEC( z16_ms8_2z, NV_MMU_PTE_KIND_Z16_MS8_2Z, "v") );
	emit_constant( C_SPEC( z16_ms16_2z, NV_MMU_PTE_KIND_Z16_MS16_2Z, "v") );
	emit_constant( C_SPEC( z16_4cz, NV_MMU_PTE_KIND_Z16_4CZ, "v") );
	emit_constant( C_SPEC( z16_ms2_4cz, NV_MMU_PTE_KIND_Z16_MS2_4CZ, "v") );
	emit_constant( C_SPEC( z16_ms4_4cz, NV_MMU_PTE_KIND_Z16_MS4_4CZ, "v") );
	emit_constant( C_SPEC( z16_ms8_4cz, NV_MMU_PTE_KIND_Z16_MS8_4CZ, "v") );
	emit_constant( C_SPEC( z16_ms16_4cz, NV_MMU_PTE_KIND_Z16_MS16_4CZ, "v") );
	emit_constant( C_SPEC( s8z24, NV_MMU_PTE_KIND_S8Z24, "v") );
	emit_constant( C_SPEC( s8z24_1z, NV_MMU_PTE_KIND_S8Z24_1Z, "v") );
	emit_constant( C_SPEC( s8z24_ms2_1z, NV_MMU_PTE_KIND_S8Z24_MS2_1Z, "v") );
	emit_constant( C_SPEC( s8z24_ms4_1z, NV_MMU_PTE_KIND_S8Z24_MS4_1Z, "v") );
	emit_constant( C_SPEC( s8z24_ms8_1z, NV_MMU_PTE_KIND_S8Z24_MS8_1Z, "v") );
	emit_constant( C_SPEC( s8z24_ms16_1z, NV_MMU_PTE_KIND_S8Z24_MS16_1Z, "v") );
	emit_constant( C_SPEC( s8z24_2cz, NV_MMU_PTE_KIND_S8Z24_2CZ, "v") );
	emit_constant( C_SPEC( s8z24_ms2_2cz, NV_MMU_PTE_KIND_S8Z24_MS2_2CZ, "v") );
	emit_constant( C_SPEC( s8z24_ms4_2cz, NV_MMU_PTE_KIND_S8Z24_MS4_2CZ, "v") );
	emit_constant( C_SPEC( s8z24_ms8_2cz, NV_MMU_PTE_KIND_S8Z24_MS8_2CZ, "v") );
	emit_constant( C_SPEC( s8z24_ms16_2cz, NV_MMU_PTE_KIND_S8Z24_MS16_2CZ, "v") );
	emit_constant( C_SPEC( s8z24_2cs, NV_MMU_PTE_KIND_S8Z24_2CS, "v") );
	emit_constant( C_SPEC( s8z24_ms2_2cs, NV_MMU_PTE_KIND_S8Z24_MS2_2CS, "v") );
	emit_constant( C_SPEC( s8z24_ms4_2cs, NV_MMU_PTE_KIND_S8Z24_MS4_2CS, "v") );
	emit_constant( C_SPEC( s8z24_ms8_2cs, NV_MMU_PTE_KIND_S8Z24_MS8_2CS, "v") );
	emit_constant( C_SPEC( s8z24_ms16_2cs, NV_MMU_PTE_KIND_S8Z24_MS16_2CS, "v") );
	emit_constant( C_SPEC( s8z24_4cszv, NV_MMU_PTE_KIND_S8Z24_4CSZV, "v") );
	emit_constant( C_SPEC( s8z24_ms2_4cszv, NV_MMU_PTE_KIND_S8Z24_MS2_4CSZV, "v") );
	emit_constant( C_SPEC( s8z24_ms4_4cszv, NV_MMU_PTE_KIND_S8Z24_MS4_4CSZV, "v") );
	emit_constant( C_SPEC( s8z24_ms8_4cszv, NV_MMU_PTE_KIND_S8Z24_MS8_4CSZV, "v") );
	emit_constant( C_SPEC( s8z24_ms16_4cszv, NV_MMU_PTE_KIND_S8Z24_MS16_4CSZV, "v") );
	emit_constant( C_SPEC( v8z24_ms4_vc12, NV_MMU_PTE_KIND_V8Z24_MS4_VC12, "v") );
	emit_constant( C_SPEC( v8z24_ms4_vc4, NV_MMU_PTE_KIND_V8Z24_MS4_VC4, "v") );
	emit_constant( C_SPEC( v8z24_ms8_vc8, NV_MMU_PTE_KIND_V8Z24_MS8_VC8, "v") );
	emit_constant( C_SPEC( v8z24_ms8_vc24, NV_MMU_PTE_KIND_V8Z24_MS8_VC24, "v") );
	emit_constant( C_SPEC( v8z24_ms4_vc12_1zv, NV_MMU_PTE_KIND_V8Z24_MS4_VC12_1ZV, "v") );
	emit_constant( C_SPEC( v8z24_ms4_vc4_1zv, NV_MMU_PTE_KIND_V8Z24_MS4_VC4_1ZV, "v") );
	emit_constant( C_SPEC( v8z24_ms8_vc8_1zv, NV_MMU_PTE_KIND_V8Z24_MS8_VC8_1ZV, "v") );
	emit_constant( C_SPEC( v8z24_ms8_vc24_1zv, NV_MMU_PTE_KIND_V8Z24_MS8_VC24_1ZV, "v") );
	emit_constant( C_SPEC( v8z24_ms4_vc12_2cs, NV_MMU_PTE_KIND_V8Z24_MS4_VC12_2CS, "v") );
	emit_constant( C_SPEC( v8z24_ms4_vc4_2cs, NV_MMU_PTE_KIND_V8Z24_MS4_VC4_2CS, "v") );
	emit_constant( C_SPEC( v8z24_ms8_vc8_2cs, NV_MMU_PTE_KIND_V8Z24_MS8_VC8_2CS, "v") );
	emit_constant( C_SPEC( v8z24_ms8_vc24_2cs, NV_MMU_PTE_KIND_V8Z24_MS8_VC24_2CS, "v") );
	emit_constant( C_SPEC( v8z24_ms4_vc12_2czv, NV_MMU_PTE_KIND_V8Z24_MS4_VC12_2CZV, "v") );
	emit_constant( C_SPEC( v8z24_ms4_vc4_2czv, NV_MMU_PTE_KIND_V8Z24_MS4_VC4_2CZV, "v") );
	emit_constant( C_SPEC( v8z24_ms8_vc8_2czv, NV_MMU_PTE_KIND_V8Z24_MS8_VC8_2CZV, "v") );
	emit_constant( C_SPEC( v8z24_ms8_vc24_2czv, NV_MMU_PTE_KIND_V8Z24_MS8_VC24_2CZV, "v") );
	emit_constant( C_SPEC( v8z24_ms4_vc12_2zv, NV_MMU_PTE_KIND_V8Z24_MS4_VC12_2ZV, "v") );
	emit_constant( C_SPEC( v8z24_ms4_vc4_2zv, NV_MMU_PTE_KIND_V8Z24_MS4_VC4_2ZV, "v") );
	emit_constant( C_SPEC( v8z24_ms8_vc8_2zv, NV_MMU_PTE_KIND_V8Z24_MS8_VC8_2ZV, "v") );
	emit_constant( C_SPEC( v8z24_ms8_vc24_2zv, NV_MMU_PTE_KIND_V8Z24_MS8_VC24_2ZV, "v") );
	emit_constant( C_SPEC( v8z24_ms4_vc12_4cszv, NV_MMU_PTE_KIND_V8Z24_MS4_VC12_4CSZV, "v") );
	emit_constant( C_SPEC( v8z24_ms4_vc4_4cszv, NV_MMU_PTE_KIND_V8Z24_MS4_VC4_4CSZV, "v") );
	emit_constant( C_SPEC( v8z24_ms8_vc8_4cszv, NV_MMU_PTE_KIND_V8Z24_MS8_VC8_4CSZV, "v") );
	emit_constant( C_SPEC( v8z24_ms8_vc24_4cszv, NV_MMU_PTE_KIND_V8Z24_MS8_VC24_4CSZV, "v") );
	emit_constant( C_SPEC( z24s8, NV_MMU_PTE_KIND_Z24S8, "v") );
	emit_constant( C_SPEC( z24s8_1z, NV_MMU_PTE_KIND_Z24S8_1Z, "v") );
	emit_constant( C_SPEC( z24s8_ms2_1z, NV_MMU_PTE_KIND_Z24S8_MS2_1Z, "v") );
	emit_constant( C_SPEC( z24s8_ms4_1z, NV_MMU_PTE_KIND_Z24S8_MS4_1Z, "v") );
	emit_constant( C_SPEC( z24s8_ms8_1z, NV_MMU_PTE_KIND_Z24S8_MS8_1Z, "v") );
	emit_constant( C_SPEC( z24s8_ms16_1z, NV_MMU_PTE_KIND_Z24S8_MS16_1Z, "v") );
	emit_constant( C_SPEC( z24s8_2cs, NV_MMU_PTE_KIND_Z24S8_2CS, "v") );
	emit_constant( C_SPEC( z24s8_ms2_2cs, NV_MMU_PTE_KIND_Z24S8_MS2_2CS, "v") );
	emit_constant( C_SPEC( z24s8_ms4_2cs, NV_MMU_PTE_KIND_Z24S8_MS4_2CS, "v") );
	emit_constant( C_SPEC( z24s8_ms8_2cs, NV_MMU_PTE_KIND_Z24S8_MS8_2CS, "v") );
	emit_constant( C_SPEC( z24s8_ms16_2cs, NV_MMU_PTE_KIND_Z24S8_MS16_2CS, "v") );
	emit_constant( C_SPEC( z24s8_2cz, NV_MMU_PTE_KIND_Z24S8_2CZ, "v") );
	emit_constant( C_SPEC( z24s8_ms2_2cz, NV_MMU_PTE_KIND_Z24S8_MS2_2CZ, "v") );
	emit_constant( C_SPEC( z24s8_ms4_2cz, NV_MMU_PTE_KIND_Z24S8_MS4_2CZ, "v") );
	emit_constant( C_SPEC( z24s8_ms8_2cz, NV_MMU_PTE_KIND_Z24S8_MS8_2CZ, "v") );
	emit_constant( C_SPEC( z24s8_ms16_2cz, NV_MMU_PTE_KIND_Z24S8_MS16_2CZ, "v") );
	emit_constant( C_SPEC( z24s8_4cszv, NV_MMU_PTE_KIND_Z24S8_4CSZV, "v") );
	emit_constant( C_SPEC( z24s8_ms2_4cszv, NV_MMU_PTE_KIND_Z24S8_MS2_4CSZV, "v") );
	emit_constant( C_SPEC( z24s8_ms4_4cszv, NV_MMU_PTE_KIND_Z24S8_MS4_4CSZV, "v") );
	emit_constant( C_SPEC( z24s8_ms8_4cszv, NV_MMU_PTE_KIND_Z24S8_MS8_4CSZV, "v") );
	emit_constant( C_SPEC( z24s8_ms16_4cszv, NV_MMU_PTE_KIND_Z24S8_MS16_4CSZV, "v") );
	emit_constant( C_SPEC( z24v8_ms4_vc12, NV_MMU_PTE_KIND_Z24V8_MS4_VC12, "v") );
	emit_constant( C_SPEC( z24v8_ms4_vc4, NV_MMU_PTE_KIND_Z24V8_MS4_VC4, "v") );
	emit_constant( C_SPEC( z24v8_ms8_vc8, NV_MMU_PTE_KIND_Z24V8_MS8_VC8, "v") );
	emit_constant( C_SPEC( z24v8_ms8_vc24, NV_MMU_PTE_KIND_Z24V8_MS8_VC24, "v") );
	emit_constant( C_SPEC( z24v8_ms4_vc12_1zv, NV_MMU_PTE_KIND_Z24V8_MS4_VC12_1ZV, "v") );
	emit_constant( C_SPEC( z24v8_ms4_vc4_1zv, NV_MMU_PTE_KIND_Z24V8_MS4_VC4_1ZV, "v") );
	emit_constant( C_SPEC( z24v8_ms8_vc8_1zv, NV_MMU_PTE_KIND_Z24V8_MS8_VC8_1ZV, "v") );
	emit_constant( C_SPEC( z24v8_ms8_vc24_1zv, NV_MMU_PTE_KIND_Z24V8_MS8_VC24_1ZV, "v") );
	emit_constant( C_SPEC( z24v8_ms4_vc12_2cs, NV_MMU_PTE_KIND_Z24V8_MS4_VC12_2CS, "v") );
	emit_constant( C_SPEC( z24v8_ms4_vc4_2cs, NV_MMU_PTE_KIND_Z24V8_MS4_VC4_2CS, "v") );
	emit_constant( C_SPEC( z24v8_ms8_vc8_2cs, NV_MMU_PTE_KIND_Z24V8_MS8_VC8_2CS, "v") );
	emit_constant( C_SPEC( z24v8_ms8_vc24_2cs, NV_MMU_PTE_KIND_Z24V8_MS8_VC24_2CS, "v") );
	emit_constant( C_SPEC( z24v8_ms4_vc12_2czv, NV_MMU_PTE_KIND_Z24V8_MS4_VC12_2CZV, "v") );
	emit_constant( C_SPEC( z24v8_ms4_vc4_2czv, NV_MMU_PTE_KIND_Z24V8_MS4_VC4_2CZV, "v") );
	emit_constant( C_SPEC( z24v8_ms8_vc8_2czv, NV_MMU_PTE_KIND_Z24V8_MS8_VC8_2CZV, "v") );
	emit_constant( C_SPEC( z24v8_ms8_vc24_2czv, NV_MMU_PTE_KIND_Z24V8_MS8_VC24_2CZV, "v") );
	emit_constant( C_SPEC( z24v8_ms4_vc12_2zv, NV_MMU_PTE_KIND_Z24V8_MS4_VC12_2ZV, "v") );
	emit_constant( C_SPEC( z24v8_ms4_vc4_2zv, NV_MMU_PTE_KIND_Z24V8_MS4_VC4_2ZV, "v") );
	emit_constant( C_SPEC( z24v8_ms8_vc8_2zv, NV_MMU_PTE_KIND_Z24V8_MS8_VC8_2ZV, "v") );
	emit_constant( C_SPEC( z24v8_ms8_vc24_2zv, NV_MMU_PTE_KIND_Z24V8_MS8_VC24_2ZV, "v") );
	emit_constant( C_SPEC( z24v8_ms4_vc12_4cszv, NV_MMU_PTE_KIND_Z24V8_MS4_VC12_4CSZV, "v") );
	emit_constant( C_SPEC( z24v8_ms4_vc4_4cszv, NV_MMU_PTE_KIND_Z24V8_MS4_VC4_4CSZV, "v") );
	emit_constant( C_SPEC( z24v8_ms8_vc8_4cszv, NV_MMU_PTE_KIND_Z24V8_MS8_VC8_4CSZV, "v") );
	emit_constant( C_SPEC( z24v8_ms8_vc24_4cszv, NV_MMU_PTE_KIND_Z24V8_MS8_VC24_4CSZV, "v") );
	emit_constant( C_SPEC( zf32, NV_MMU_PTE_KIND_ZF32, "v") );
	emit_constant( C_SPEC( zf32_1z, NV_MMU_PTE_KIND_ZF32_1Z, "v") );
	emit_constant( C_SPEC( zf32_ms2_1z, NV_MMU_PTE_KIND_ZF32_MS2_1Z, "v") );
	emit_constant( C_SPEC( zf32_ms4_1z, NV_MMU_PTE_KIND_ZF32_MS4_1Z, "v") );
	emit_constant( C_SPEC( zf32_ms8_1z, NV_MMU_PTE_KIND_ZF32_MS8_1Z, "v") );
	emit_constant( C_SPEC( zf32_ms16_1z, NV_MMU_PTE_KIND_ZF32_MS16_1Z, "v") );
	emit_constant( C_SPEC( zf32_2cs, NV_MMU_PTE_KIND_ZF32_2CS, "v") );
	emit_constant( C_SPEC( zf32_ms2_2cs, NV_MMU_PTE_KIND_ZF32_MS2_2CS, "v") );
	emit_constant( C_SPEC( zf32_ms4_2cs, NV_MMU_PTE_KIND_ZF32_MS4_2CS, "v") );
	emit_constant( C_SPEC( zf32_ms8_2cs, NV_MMU_PTE_KIND_ZF32_MS8_2CS, "v") );
	emit_constant( C_SPEC( zf32_ms16_2cs, NV_MMU_PTE_KIND_ZF32_MS16_2CS, "v") );
	emit_constant( C_SPEC( zf32_2cz, NV_MMU_PTE_KIND_ZF32_2CZ, "v") );
	emit_constant( C_SPEC( zf32_ms2_2cz, NV_MMU_PTE_KIND_ZF32_MS2_2CZ, "v") );
	emit_constant( C_SPEC( zf32_ms4_2cz, NV_MMU_PTE_KIND_ZF32_MS4_2CZ, "v") );
	emit_constant( C_SPEC( zf32_ms8_2cz, NV_MMU_PTE_KIND_ZF32_MS8_2CZ, "v") );
	emit_constant( C_SPEC( zf32_ms16_2cz, NV_MMU_PTE_KIND_ZF32_MS16_2CZ, "v") );
	emit_constant( C_SPEC( x8z24_x16v8s8_ms4_vc12, NV_MMU_PTE_KIND_X8Z24_X16V8S8_MS4_VC12, "v") );
	emit_constant( C_SPEC( x8z24_x16v8s8_ms4_vc4, NV_MMU_PTE_KIND_X8Z24_X16V8S8_MS4_VC4, "v") );
	emit_constant( C_SPEC( x8z24_x16v8s8_ms8_vc8, NV_MMU_PTE_KIND_X8Z24_X16V8S8_MS8_VC8, "v") );
	emit_constant( C_SPEC( x8z24_x16v8s8_ms8_vc24, NV_MMU_PTE_KIND_X8Z24_X16V8S8_MS8_VC24, "v") );
	emit_constant( C_SPEC( x8z24_x16v8s8_ms4_vc12_1cs, NV_MMU_PTE_KIND_X8Z24_X16V8S8_MS4_VC12_1CS, "v") );
	emit_constant( C_SPEC( x8z24_x16v8s8_ms4_vc4_1cs, NV_MMU_PTE_KIND_X8Z24_X16V8S8_MS4_VC4_1CS, "v") );
	emit_constant( C_SPEC( x8z24_x16v8s8_ms8_vc8_1cs, NV_MMU_PTE_KIND_X8Z24_X16V8S8_MS8_VC8_1CS, "v") );
	emit_constant( C_SPEC( x8z24_x16v8s8_ms8_vc24_1cs, NV_MMU_PTE_KIND_X8Z24_X16V8S8_MS8_VC24_1CS, "v") );
	emit_constant( C_SPEC( x8z24_x16v8s8_ms4_vc12_1zv, NV_MMU_PTE_KIND_X8Z24_X16V8S8_MS4_VC12_1ZV, "v") );
	emit_constant( C_SPEC( x8z24_x16v8s8_ms4_vc4_1zv, NV_MMU_PTE_KIND_X8Z24_X16V8S8_MS4_VC4_1ZV, "v") );
	emit_constant( C_SPEC( x8z24_x16v8s8_ms8_vc8_1zv, NV_MMU_PTE_KIND_X8Z24_X16V8S8_MS8_VC8_1ZV, "v") );
	emit_constant( C_SPEC( x8z24_x16v8s8_ms8_vc24_1zv, NV_MMU_PTE_KIND_X8Z24_X16V8S8_MS8_VC24_1ZV, "v") );
	emit_constant( C_SPEC( x8z24_x16v8s8_ms4_vc12_1czv, NV_MMU_PTE_KIND_X8Z24_X16V8S8_MS4_VC12_1CZV, "v") );
	emit_constant( C_SPEC( x8z24_x16v8s8_ms4_vc4_1czv, NV_MMU_PTE_KIND_X8Z24_X16V8S8_MS4_VC4_1CZV, "v") );
	emit_constant( C_SPEC( x8z24_x16v8s8_ms8_vc8_1czv, NV_MMU_PTE_KIND_X8Z24_X16V8S8_MS8_VC8_1CZV, "v") );
	emit_constant( C_SPEC( x8z24_x16v8s8_ms8_vc24_1czv, NV_MMU_PTE_KIND_X8Z24_X16V8S8_MS8_VC24_1CZV, "v") );
	emit_constant( C_SPEC( x8z24_x16v8s8_ms4_vc12_2cs, NV_MMU_PTE_KIND_X8Z24_X16V8S8_MS4_VC12_2CS, "v") );
	emit_constant( C_SPEC( x8z24_x16v8s8_ms4_vc4_2cs, NV_MMU_PTE_KIND_X8Z24_X16V8S8_MS4_VC4_2CS, "v") );
	emit_constant( C_SPEC( x8z24_x16v8s8_ms8_vc8_2cs, NV_MMU_PTE_KIND_X8Z24_X16V8S8_MS8_VC8_2CS, "v") );
	emit_constant( C_SPEC( x8z24_x16v8s8_ms8_vc24_2cs, NV_MMU_PTE_KIND_X8Z24_X16V8S8_MS8_VC24_2CS, "v") );
	emit_constant( C_SPEC( x8z24_x16v8s8_ms4_vc12_2cszv, NV_MMU_PTE_KIND_X8Z24_X16V8S8_MS4_VC12_2CSZV, "v") );
	emit_constant( C_SPEC( x8z24_x16v8s8_ms4_vc4_2cszv, NV_MMU_PTE_KIND_X8Z24_X16V8S8_MS4_VC4_2CSZV, "v") );
	emit_constant( C_SPEC( x8z24_x16v8s8_ms8_vc8_2cszv, NV_MMU_PTE_KIND_X8Z24_X16V8S8_MS8_VC8_2CSZV, "v") );
	emit_constant( C_SPEC( x8z24_x16v8s8_ms8_vc24_2cszv, NV_MMU_PTE_KIND_X8Z24_X16V8S8_MS8_VC24_2CSZV, "v") );
	emit_constant( C_SPEC( zf32_x16v8s8_ms4_vc12, NV_MMU_PTE_KIND_ZF32_X16V8S8_MS4_VC12, "v") );
	emit_constant( C_SPEC( zf32_x16v8s8_ms4_vc4, NV_MMU_PTE_KIND_ZF32_X16V8S8_MS4_VC4, "v") );
	emit_constant( C_SPEC( zf32_x16v8s8_ms8_vc8, NV_MMU_PTE_KIND_ZF32_X16V8S8_MS8_VC8, "v") );
	emit_constant( C_SPEC( zf32_x16v8s8_ms8_vc24, NV_MMU_PTE_KIND_ZF32_X16V8S8_MS8_VC24, "v") );
	emit_constant( C_SPEC( zf32_x16v8s8_ms4_vc12_1cs, NV_MMU_PTE_KIND_ZF32_X16V8S8_MS4_VC12_1CS, "v") );
	emit_constant( C_SPEC( zf32_x16v8s8_ms4_vc4_1cs, NV_MMU_PTE_KIND_ZF32_X16V8S8_MS4_VC4_1CS, "v") );
	emit_constant( C_SPEC( zf32_x16v8s8_ms8_vc8_1cs, NV_MMU_PTE_KIND_ZF32_X16V8S8_MS8_VC8_1CS, "v") );
	emit_constant( C_SPEC( zf32_x16v8s8_ms8_vc24_1cs, NV_MMU_PTE_KIND_ZF32_X16V8S8_MS8_VC24_1CS, "v") );
	emit_constant( C_SPEC( zf32_x16v8s8_ms4_vc12_1zv, NV_MMU_PTE_KIND_ZF32_X16V8S8_MS4_VC12_1ZV, "v") );
	emit_constant( C_SPEC( zf32_x16v8s8_ms4_vc4_1zv, NV_MMU_PTE_KIND_ZF32_X16V8S8_MS4_VC4_1ZV, "v") );
	emit_constant( C_SPEC( zf32_x16v8s8_ms8_vc8_1zv, NV_MMU_PTE_KIND_ZF32_X16V8S8_MS8_VC8_1ZV, "v") );
	emit_constant( C_SPEC( zf32_x16v8s8_ms8_vc24_1zv, NV_MMU_PTE_KIND_ZF32_X16V8S8_MS8_VC24_1ZV, "v") );
	emit_constant( C_SPEC( zf32_x16v8s8_ms4_vc12_1czv, NV_MMU_PTE_KIND_ZF32_X16V8S8_MS4_VC12_1CZV, "v") );
	emit_constant( C_SPEC( zf32_x16v8s8_ms4_vc4_1czv, NV_MMU_PTE_KIND_ZF32_X16V8S8_MS4_VC4_1CZV, "v") );
	emit_constant( C_SPEC( zf32_x16v8s8_ms8_vc8_1czv, NV_MMU_PTE_KIND_ZF32_X16V8S8_MS8_VC8_1CZV, "v") );
	emit_constant( C_SPEC( zf32_x16v8s8_ms8_vc24_1czv, NV_MMU_PTE_KIND_ZF32_X16V8S8_MS8_VC24_1CZV, "v") );
	emit_constant( C_SPEC( zf32_x16v8s8_ms4_vc12_2cs, NV_MMU_PTE_KIND_ZF32_X16V8S8_MS4_VC12_2CS, "v") );
	emit_constant( C_SPEC( zf32_x16v8s8_ms4_vc4_2cs, NV_MMU_PTE_KIND_ZF32_X16V8S8_MS4_VC4_2CS, "v") );
	emit_constant( C_SPEC( zf32_x16v8s8_ms8_vc8_2cs, NV_MMU_PTE_KIND_ZF32_X16V8S8_MS8_VC8_2CS, "v") );
	emit_constant( C_SPEC( zf32_x16v8s8_ms8_vc24_2cs, NV_MMU_PTE_KIND_ZF32_X16V8S8_MS8_VC24_2CS, "v") );
	emit_constant( C_SPEC( zf32_x16v8s8_ms4_vc12_2cszv, NV_MMU_PTE_KIND_ZF32_X16V8S8_MS4_VC12_2CSZV, "v") );
	emit_constant( C_SPEC( zf32_x16v8s8_ms4_vc4_2cszv, NV_MMU_PTE_KIND_ZF32_X16V8S8_MS4_VC4_2CSZV, "v") );
	emit_constant( C_SPEC( zf32_x16v8s8_ms8_vc8_2cszv, NV_MMU_PTE_KIND_ZF32_X16V8S8_MS8_VC8_2CSZV, "v") );
	emit_constant( C_SPEC( zf32_x16v8s8_ms8_vc24_2cszv, NV_MMU_PTE_KIND_ZF32_X16V8S8_MS8_VC24_2CSZV, "v") );
	emit_constant( C_SPEC( zf32_x24s8, NV_MMU_PTE_KIND_ZF32_X24S8, "v") );
	emit_constant( C_SPEC( zf32_x24s8_1cs, NV_MMU_PTE_KIND_ZF32_X24S8_1CS, "v") );
	emit_constant( C_SPEC( zf32_x24s8_ms2_1cs, NV_MMU_PTE_KIND_ZF32_X24S8_MS2_1CS, "v") );
	emit_constant( C_SPEC( zf32_x24s8_ms4_1cs, NV_MMU_PTE_KIND_ZF32_X24S8_MS4_1CS, "v") );
	emit_constant( C_SPEC( zf32_x24s8_ms8_1cs, NV_MMU_PTE_KIND_ZF32_X24S8_MS8_1CS, "v") );
	emit_constant( C_SPEC( zf32_x24s8_ms16_1cs, NV_MMU_PTE_KIND_ZF32_X24S8_MS16_1CS, "v") );
	emit_constant( C_SPEC( zf32_x24s8_2cszv, NV_MMU_PTE_KIND_ZF32_X24S8_2CSZV, "v") );
	emit_constant( C_SPEC( zf32_x24s8_ms2_2cszv, NV_MMU_PTE_KIND_ZF32_X24S8_MS2_2CSZV, "v") );
	emit_constant( C_SPEC( zf32_x24s8_ms4_2cszv, NV_MMU_PTE_KIND_ZF32_X24S8_MS4_2CSZV, "v") );
	emit_constant( C_SPEC( zf32_x24s8_ms8_2cszv, NV_MMU_PTE_KIND_ZF32_X24S8_MS8_2CSZV, "v") );
	emit_constant( C_SPEC( zf32_x24s8_ms16_2cszv, NV_MMU_PTE_KIND_ZF32_X24S8_MS16_2CSZV, "v") );
	emit_constant( C_SPEC( zf32_x24s8_2cs, NV_MMU_PTE_KIND_ZF32_X24S8_2CS, "v") );
	emit_constant( C_SPEC( zf32_x24s8_ms2_2cs, NV_MMU_PTE_KIND_ZF32_X24S8_MS2_2CS, "v") );
	emit_constant( C_SPEC( zf32_x24s8_ms4_2cs, NV_MMU_PTE_KIND_ZF32_X24S8_MS4_2CS, "v") );
	emit_constant( C_SPEC( zf32_x24s8_ms8_2cs, NV_MMU_PTE_KIND_ZF32_X24S8_MS8_2CS, "v") );
	emit_constant( C_SPEC( zf32_x24s8_ms16_2cs, NV_MMU_PTE_KIND_ZF32_X24S8_MS16_2CS, "v") );
	emit_constant( C_SPEC( generic_16bx2, NV_MMU_PTE_KIND_GENERIC_16BX2, "v") );
	emit_constant( C_SPEC( c32_2c, NV_MMU_PTE_KIND_C32_2C, "v") );
	emit_constant( C_SPEC( c32_2cbr, NV_MMU_PTE_KIND_C32_2CBR, "v") );
	emit_constant( C_SPEC( c32_2cba, NV_MMU_PTE_KIND_C32_2CBA, "v") );
	emit_constant( C_SPEC( c32_2cra, NV_MMU_PTE_KIND_C32_2CRA, "v") );
	emit_constant( C_SPEC( c32_2bra, NV_MMU_PTE_KIND_C32_2BRA, "v") );
	emit_constant( C_SPEC( c32_ms2_2c, NV_MMU_PTE_KIND_C32_MS2_2C, "v") );
	emit_constant( C_SPEC( c32_ms2_2cbr, NV_MMU_PTE_KIND_C32_MS2_2CBR, "v") );
	emit_constant( C_SPEC( c32_ms2_2cra, NV_MMU_PTE_KIND_C32_MS2_2CRA, "v") );
	emit_constant( C_SPEC( c32_ms4_2c, NV_MMU_PTE_KIND_C32_MS4_2C, "v") );
	emit_constant( C_SPEC( c32_ms4_2cbr, NV_MMU_PTE_KIND_C32_MS4_2CBR, "v") );
	emit_constant( C_SPEC( c32_ms4_2cba, NV_MMU_PTE_KIND_C32_MS4_2CBA, "v") );
	emit_constant( C_SPEC( c32_ms4_2cra, NV_MMU_PTE_KIND_C32_MS4_2CRA, "v") );
	emit_constant( C_SPEC( c32_ms4_2bra, NV_MMU_PTE_KIND_C32_MS4_2BRA, "v") );
	emit_constant( C_SPEC( c32_ms8_ms16_2c, NV_MMU_PTE_KIND_C32_MS8_MS16_2C, "v") );
	emit_constant( C_SPEC( c32_ms8_ms16_2cra, NV_MMU_PTE_KIND_C32_MS8_MS16_2CRA, "v") );
	emit_constant( C_SPEC( c64_2c, NV_MMU_PTE_KIND_C64_2C, "v") );
	emit_constant( C_SPEC( c64_2cbr, NV_MMU_PTE_KIND_C64_2CBR, "v") );
	emit_constant( C_SPEC( c64_2cba, NV_MMU_PTE_KIND_C64_2CBA, "v") );
	emit_constant( C_SPEC( c64_2cra, NV_MMU_PTE_KIND_C64_2CRA, "v") );
	emit_constant( C_SPEC( c64_2bra, NV_MMU_PTE_KIND_C64_2BRA, "v") );
	emit_constant( C_SPEC( c64_ms2_2c, NV_MMU_PTE_KIND_C64_MS2_2C, "v") );
	emit_constant( C_SPEC( c64_ms2_2cbr, NV_MMU_PTE_KIND_C64_MS2_2CBR, "v") );
	emit_constant( C_SPEC( c64_ms2_2cra, NV_MMU_PTE_KIND_C64_MS2_2CRA, "v") );
	emit_constant( C_SPEC( c64_ms4_2c, NV_MMU_PTE_KIND_C64_MS4_2C, "v") );
	emit_constant( C_SPEC( c64_ms4_2cbr, NV_MMU_PTE_KIND_C64_MS4_2CBR, "v") );
	emit_constant( C_SPEC( c64_ms4_2cba, NV_MMU_PTE_KIND_C64_MS4_2CBA, "v") );
	emit_constant( C_SPEC( c64_ms4_2cra, NV_MMU_PTE_KIND_C64_MS4_2CRA, "v") );
	emit_constant( C_SPEC( c64_ms4_2bra, NV_MMU_PTE_KIND_C64_MS4_2BRA, "v") );
	emit_constant( C_SPEC( c64_ms8_ms16_2c, NV_MMU_PTE_KIND_C64_MS8_MS16_2C, "v") );
	emit_constant( C_SPEC( c64_ms8_ms16_2cra, NV_MMU_PTE_KIND_C64_MS8_MS16_2CRA, "v") );
	emit_constant( C_SPEC( c128_2c, NV_MMU_PTE_KIND_C128_2C, "v") );
	emit_constant( C_SPEC( c128_2cr, NV_MMU_PTE_KIND_C128_2CR, "v") );
	emit_constant( C_SPEC( c128_ms2_2c, NV_MMU_PTE_KIND_C128_MS2_2C, "v") );
	emit_constant( C_SPEC( c128_ms2_2cr, NV_MMU_PTE_KIND_C128_MS2_2CR, "v") );
	emit_constant( C_SPEC( c128_ms4_2c, NV_MMU_PTE_KIND_C128_MS4_2C, "v") );
	emit_constant( C_SPEC( c128_ms4_2cr, NV_MMU_PTE_KIND_C128_MS4_2CR, "v") );
	emit_constant( C_SPEC( c128_ms8_ms16_2c, NV_MMU_PTE_KIND_C128_MS8_MS16_2C, "v") );
	emit_constant( C_SPEC( c128_ms8_ms16_2cr, NV_MMU_PTE_KIND_C128_MS8_MS16_2CR, "v") );
	emit_constant( C_SPEC( x8c24, NV_MMU_PTE_KIND_X8C24, "v") );
	emit_constant( C_SPEC( pitch_no_swizzle, NV_MMU_PTE_KIND_PITCH_NO_SWIZZLE, "v") );
	emit_constant( C_SPEC( smsked_message, NV_MMU_PTE_KIND_SMSKED_MESSAGE, "v") );
	emit_constant( C_SPEC( smhost_message, NV_MMU_PTE_KIND_SMHOST_MESSAGE, "v") );

	end_group();

}


static void emit_gr_group()
{
	begin_group( G_SPEC( gr ) );
	emit_register( R_SPEC( intr, NV_PGRAPH_INTR ) );
	emit_field   ( F_SPEC( notify, NV_PGRAPH_INTR_NOTIFY, ".") );
	emit_constant( C_SPEC( pending, NV_PGRAPH_INTR_NOTIFY_PENDING, "f") );
	emit_constant( C_SPEC( reset, NV_PGRAPH_INTR_NOTIFY_RESET, "f") );
	emit_field   ( F_SPEC( semaphore, NV_PGRAPH_INTR_SEMAPHORE, ".") );
	emit_constant( C_SPEC( pending, NV_PGRAPH_INTR_SEMAPHORE_PENDING, "f") );
	emit_constant( C_SPEC( reset, NV_PGRAPH_INTR_SEMAPHORE_RESET, "f") );
	emit_field   ( F_SPEC( semaphore_timeout, NV_PGRAPH_INTR_SEMAPHORE_TIMEOUT, ".") );
	emit_constant( C_SPEC( not_pending, NV_PGRAPH_INTR_SEMAPHORE_TIMEOUT_NOT_PENDING, "f") );
	emit_constant( C_SPEC( pending, NV_PGRAPH_INTR_SEMAPHORE_TIMEOUT_PENDING, "f") );
	emit_constant( C_SPEC( reset, NV_PGRAPH_INTR_SEMAPHORE_TIMEOUT_RESET, "f") );
	emit_field   ( F_SPEC( illegal_method, NV_PGRAPH_INTR_ILLEGAL_METHOD, ".") );
	emit_constant( C_SPEC( pending, NV_PGRAPH_INTR_ILLEGAL_METHOD_PENDING, "f") );
	emit_constant( C_SPEC( reset, NV_PGRAPH_INTR_ILLEGAL_METHOD_RESET, "f") );
	emit_field   ( F_SPEC( illegal_notify, NV_PGRAPH_INTR_ILLEGAL_NOTIFY, ".") );
	emit_constant( C_SPEC( pending, NV_PGRAPH_INTR_ILLEGAL_NOTIFY_PENDING, "f") );
	emit_constant( C_SPEC( reset, NV_PGRAPH_INTR_ILLEGAL_NOTIFY_RESET, "f") );
	emit_field   ( F_SPEC( firmware_method, NV_PGRAPH_INTR_FIRMWARE_METHOD, "f") );
	emit_constant( C_SPEC( pending, NV_PGRAPH_INTR_FIRMWARE_METHOD_PENDING, "f") );
	emit_constant( C_SPEC( reset, NV_PGRAPH_INTR_FIRMWARE_METHOD_RESET, "f") );
	emit_field   ( F_SPEC( illegal_class, NV_PGRAPH_INTR_ILLEGAL_CLASS, ".") );
	emit_constant( C_SPEC( pending, NV_PGRAPH_INTR_ILLEGAL_CLASS_PENDING, "f") );
	emit_constant( C_SPEC( reset, NV_PGRAPH_INTR_ILLEGAL_CLASS_RESET, "f") );
	emit_field   ( F_SPEC( fecs_error, NV_PGRAPH_INTR_FECS_ERROR, ".") );
	emit_constant( C_SPEC( pending, NV_PGRAPH_INTR_FECS_ERROR_PENDING, "f") );
	emit_constant( C_SPEC( reset, NV_PGRAPH_INTR_FECS_ERROR_RESET, "f") );
	emit_field   ( F_SPEC( class_error, NV_PGRAPH_INTR_CLASS_ERROR, ".") );
	emit_constant( C_SPEC( pending, NV_PGRAPH_INTR_CLASS_ERROR_PENDING, "f") );
	emit_constant( C_SPEC( reset, NV_PGRAPH_INTR_CLASS_ERROR_RESET, "f") );
	emit_field   ( F_SPEC( exception, NV_PGRAPH_INTR_EXCEPTION, ".") );
	emit_constant( C_SPEC( pending, NV_PGRAPH_INTR_EXCEPTION_PENDING, "f") );
	emit_constant( C_SPEC( reset, NV_PGRAPH_INTR_EXCEPTION_RESET, "f") );

	emit_register( R_SPEC( fecs_intr, NV_PGRAPH_FECS_INTR ) );

	emit_register( R_SPEC( class_error, NV_PGRAPH_CLASS_ERROR ) );
	emit_field   ( F_SPEC( code, NV_PGRAPH_CLASS_ERROR_CODE, "v") );

	emit_register( R_SPEC( intr_nonstall, NV_PGRAPH_NONSTALL_INTR ) );
	emit_field   ( F_SPEC( trap, NV_PGRAPH_NONSTALL_INTR_TRAP, ".") );
	emit_constant( C_SPEC( pending, NV_PGRAPH_NONSTALL_INTR_TRAP_PENDING, "f") );

	emit_register( R_SPEC( intr_en, NV_PGRAPH_INTR_EN ) );

	emit_register( R_SPEC( exception, NV_PGRAPH_EXCEPTION ) );
	emit_field   ( F_SPEC( fe, NV_PGRAPH_EXCEPTION_FE, "m") );
	emit_field   ( F_SPEC( gpc, NV_PGRAPH_EXCEPTION_GPC, "m") );
	emit_field   ( F_SPEC( memfmt, NV_PGRAPH_EXCEPTION_MEMFMT, "m") );
	emit_field   ( F_SPEC( ds, NV_PGRAPH_EXCEPTION_DS, "m") );

	emit_register( R_SPEC( exception1, NV_PGRAPH_EXCEPTION1 ) );
	emit_field   ( F_SPEC( gpc, NV_PGRAPH_EXCEPTION1_GPC, ".") );
	emit_constant( C_SPEC( 0_pending, NV_PGRAPH_EXCEPTION1_GPC_0_PENDING, "f") );

	emit_register( R_SPEC( exception2, NV_PGRAPH_EXCEPTION2 ) );

	emit_register( R_SPEC( exception_en, NV_PGRAPH_EXCEPTION_EN ) );
	emit_field   ( F_SPEC( fe, NV_PGRAPH_EXCEPTION_EN_FE, "m") );

	emit_register( R_SPEC( exception1_en, NV_PGRAPH_EXCEPTION1_EN ) );

	emit_register( R_SPEC( exception2_en, NV_PGRAPH_EXCEPTION2_EN ) );

	emit_register( R_SPEC( gpfifo_ctl, NV_PGRAPH_GRFIFO_CONTROL ) );
	emit_field   ( F_SPEC( access, NV_PGRAPH_GRFIFO_CONTROL_ACCESS, "f") );
	emit_constant( C_SPEC( disabled, NV_PGRAPH_GRFIFO_CONTROL_ACCESS_DISABLED, "f") );
	emit_constant( C_SPEC( enabled, NV_PGRAPH_GRFIFO_CONTROL_ACCESS_ENABLED, "f") );
	emit_field   ( F_SPEC( semaphore_access, NV_PGRAPH_GRFIFO_CONTROL_SEMAPHORE_ACCESS, "f") );
	emit_constant( C_SPEC( enabled, NV_PGRAPH_GRFIFO_CONTROL_SEMAPHORE_ACCESS_ENABLED, "") );

	emit_register( R_SPEC( gpfifo_status, NV_PGRAPH_GRFIFO_STATUS) );

	emit_register( R_SPEC( trapped_addr, NV_PGRAPH_TRAPPED_ADDR ) );
	emit_field   ( F_SPEC( mthd, NV_PGRAPH_TRAPPED_ADDR_MTHD, "v") );
	emit_field   ( F_SPEC( subch, NV_PGRAPH_TRAPPED_ADDR_SUBCH, "v") );

	emit_register( R_SPEC( trapped_data_lo, NV_PGRAPH_TRAPPED_DATA_LOW ) );

	emit_register( R_SPEC( trapped_data_hi, NV_PGRAPH_TRAPPED_DATA_HIGH ) );

	emit_register( R_SPEC( status, NV_PGRAPH_STATUS ) );
	emit_field   ( F_SPEC( fe_method_upper, NV_PGRAPH_STATUS_FE_METHOD_UPPER, "v") );
	emit_field   ( F_SPEC( fe_method_lower, NV_PGRAPH_STATUS_FE_METHOD_LOWER, "v") );
	emit_constant( C_SPEC( idle, NV_PGRAPH_STATUS_FE_METHOD_LOWER_IDLE, "v") );
	emit_field   ( F_SPEC( fe_gi, NV_PGRAPH_STATUS_FE_GI, "v") );

	emit_register( R_SPEC( status_mask, NV_PGRAPH_STATUS_MASK ) );

	emit_register( R_SPEC( status_1, NV_PGRAPH_STATUS1 ) );

	emit_register( R_SPEC( status_2, NV_PGRAPH_STATUS2) );

	emit_register( R_SPEC( engine_status, NV_PGRAPH_ENGINE_STATUS ) );
	emit_field   ( F_SPEC( value, NV_PGRAPH_ENGINE_STATUS_VALUE, ".") );
	emit_constant( C_SPEC( busy, NV_PGRAPH_ENGINE_STATUS_VALUE_BUSY, "f") );

	emit_register( R_SPEC( pri_be0_becs_be_exception, NV_PGRAPH_PRI_BE0_BECS_BE_EXCEPTION) );
	emit_register( R_SPEC( pri_be0_becs_be_exception_en, NV_PGRAPH_PRI_BE0_BECS_BE_EXCEPTION_EN) );

	emit_register( R_SPEC( pri_gpc0_gpccs_gpc_exception, NV_PGRAPH_PRI_GPC0_GPCCS_GPC_EXCEPTION) );
	emit_register( R_SPEC( pri_gpc0_gpccs_gpc_exception_en, NV_PGRAPH_PRI_GPC0_GPCCS_GPC_EXCEPTION_EN) );

	emit_register( R_SPEC( pri_gpc0_tpc0_tpccs_tpc_exception, NV_PGRAPH_PRI_GPC0_TPC0_TPCCS_TPC_EXCEPTION) );
	emit_register( R_SPEC( pri_gpc0_tpc0_tpccs_tpc_exception_en, NV_PGRAPH_PRI_GPC0_TPC0_TPCCS_TPC_EXCEPTION_EN) );

	emit_register( R_SPEC( activity_0, NV_PGRAPH_ACTIVITY0 ) );

	emit_register( R_SPEC( activity_1, NV_PGRAPH_ACTIVITY1 ) );

	emit_register( R_SPEC( activity_2, NV_PGRAPH_ACTIVITY2 ) );

	emit_register( R_SPEC( activity_4, NV_PGRAPH_ACTIVITY4 ) );

	emit_register( R_SPEC( pri_gpc0_gcc_dbg, NV_PGRAPH_PRI_GPC0_GCC_DBG ) );

	emit_register( R_SPEC( pri_gpcs_gcc_dbg, NV_PGRAPH_PRI_GPCS_GCC_DBG ) );

	emit_field   ( F_SPEC( invalidate, NV_PGRAPH_PRI_GPCS_GCC_DBG_INVALIDATE, "m") );

	emit_register( R_SPEC( pri_gpc0_tpc0_sm_cache_control, NV_PGRAPH_PRI_GPC0_TPC0_SM_CACHE_CONTROL ) );

	emit_register( R_SPEC( pri_gpcs_tpcs_sm_cache_control, NV_PGRAPH_PRI_GPCS_TPCS_SM_CACHE_CONTROL ) );

	emit_field   ( F_SPEC( invalidate_cache, NV_PGRAPH_PRI_GPCS_TPCS_SM_CACHE_CONTROL_INVALIDATE_ICACHE, "m") );

	emit_register( R_SPEC( pri_sked_activity, NV_PGRAPH_PRI_SKED_ACTIVITY ) );

	emit_register( R_SPEC( pri_gpc0_gpccs_gpc_activity0, NV_PGRAPH_PRI_GPC0_GPCCS_GPC_ACTIVITY0 ) );

	emit_register( R_SPEC( pri_gpc0_gpccs_gpc_activity1, NV_PGRAPH_PRI_GPC0_GPCCS_GPC_ACTIVITY1 ) );

	emit_register( R_SPEC( pri_gpc0_gpccs_gpc_activity2, NV_PGRAPH_PRI_GPC0_GPCCS_GPC_ACTIVITY2 ) );

	emit_register( R_SPEC( pri_gpc0_gpccs_gpc_activity3, NV_PGRAPH_PRI_GPC0_GPCCS_GPC_ACTIVITY3 ) );

	emit_register( R_SPEC( pri_gpc0_tpc0_tpccs_tpc_activity_0, NV_PGRAPH_PRI_GPC0_TPC0_TPCCS_TPC_ACTIVITY0) );

	emit_register( R_SPEC( pri_gpc0_tpcs_tpccs_tpc_activity_0, NV_PGRAPH_PRI_GPC0_TPCS_TPCCS_TPC_ACTIVITY0) );

	emit_register( R_SPEC( pri_gpcs_gpccs_gpc_activity_0, NV_PGRAPH_PRI_GPCS_GPCCS_GPC_ACTIVITY0) );

	emit_register( R_SPEC( pri_gpcs_gpccs_gpc_activity_1, NV_PGRAPH_PRI_GPCS_GPCCS_GPC_ACTIVITY1) );

	emit_register( R_SPEC( pri_gpcs_gpccs_gpc_activity_2, NV_PGRAPH_PRI_GPCS_GPCCS_GPC_ACTIVITY2) );

	emit_register( R_SPEC( pri_gpcs_gpccs_gpc_activity_3, NV_PGRAPH_PRI_GPCS_GPCCS_GPC_ACTIVITY3) );

	emit_register( R_SPEC( pri_gpcs_tpc0_tpccs_tpc_activity_0, NV_PGRAPH_PRI_GPCS_TPC0_TPCCS_TPC_ACTIVITY0) );

	emit_register( R_SPEC( pri_gpcs_tpcs_tpccs_tpc_activity_0, NV_PGRAPH_PRI_GPCS_TPCS_TPCCS_TPC_ACTIVITY0) );

	emit_register( R_SPEC( pri_be0_becs_be_activity0, NV_PGRAPH_PRI_BE0_BECS_BE_ACTIVITY0) );

	emit_register( R_SPEC( pri_bes_becs_be_activity0, NV_PGRAPH_PRI_BES_BECS_BE_ACTIVITY0) );

	emit_register( R_SPEC( pri_ds_mpipe_status, NV_PGRAPH_PRI_DS_MPIPE_STATUS) );

	emit_register( R_SPEC( pri_fe_go_idle_on_status, NV_PGRAPH_PRI_FE_GO_IDLE_ON_STATUS) );

	emit_register( R_SPEC( pri_fe_go_idle_check, NV_PGRAPH_PRI_FE_GO_IDLE_CHECK) );

	emit_register( R_SPEC( pri_fe_go_idle_info, NV_PGRAPH_PRI_FE_GO_IDLE_INFO) );

	emit_register( R_SPEC( pri_gpc0_tpc0_tex_m_tex_subunits_status, NV_PGRAPH_PRI_GPC0_TPC0_TEX_M_TEX_SUBUNITS_STATUS) );

	emit_register( R_SPEC( pri_be0_crop_status1, NV_PGRAPH_PRI_BE0_CROP_STATUS1) );

	emit_register( R_SPEC( pri_bes_crop_status1, NV_PGRAPH_PRI_BES_CROP_STATUS1) );

	emit_register( R_SPEC( pri_be0_zrop_status, NV_PGRAPH_PRI_BE0_ZROP_STATUS) );

	emit_register( R_SPEC( pri_be0_zrop_status2, NV_PGRAPH_PRI_BE0_ZROP_STATUS2) );

	emit_register( R_SPEC( pri_bes_zrop_status, NV_PGRAPH_PRI_BES_ZROP_STATUS) );

	emit_register( R_SPEC( pri_bes_zrop_status2, NV_PGRAPH_PRI_BES_ZROP_STATUS2) );

	emit_register( R_SPEC( pipe_bundle_address, NV_PGRAPH_PIPE_BUNDLE_ADDRESS ) );
	emit_field   ( F_SPEC( value, NV_PGRAPH_PIPE_BUNDLE_ADDRESS_VALUE, "v") );

	emit_register( R_SPEC( pipe_bundle_data, NV_PGRAPH_PIPE_BUNDLE_DATA ) );

	emit_register( R_SPEC( pipe_bundle_config, NV_PGRAPH_PIPE_BUNDLE_CONFIG ) );
	emit_field   ( F_SPEC( override_pipe_mode, NV_PGRAPH_PIPE_BUNDLE_CONFIG_OVERRIDE_PIPE_MODE, ".") );
	emit_constant( C_SPEC( disabled, NV_PGRAPH_PIPE_BUNDLE_CONFIG_OVERRIDE_PIPE_MODE_DISABLED, "f") );
	emit_constant( C_SPEC( enabled, NV_PGRAPH_PIPE_BUNDLE_CONFIG_OVERRIDE_PIPE_MODE_ENABLED, "f") );

	emit_register( R_SPEC( fe_hww_esr, NV_PGRAPH_PRI_FE_HWW_ESR ) );
	emit_field   ( F_SPEC( reset, NV_PGRAPH_PRI_FE_HWW_ESR_RESET, ".") );
	emit_constant( C_SPEC( active, NV_PGRAPH_PRI_FE_HWW_ESR_RESET_ACTIVE, "f") );
	emit_field   ( F_SPEC( en, NV_PGRAPH_PRI_FE_HWW_ESR_EN, ".") );
	emit_constant( C_SPEC( enable, NV_PGRAPH_PRI_FE_HWW_ESR_EN_ENABLE, "f") );

	emit_register( R_SPEC( fe_go_idle_timeout, NV_PGRAPH_PRI_FE_GO_IDLE_TIMEOUT ) );
	emit_field   ( F_SPEC( count, NV_PGRAPH_PRI_FE_GO_IDLE_TIMEOUT_COUNT, "f") );
	emit_constant( C_SPEC( disabled, NV_PGRAPH_PRI_FE_GO_IDLE_TIMEOUT_COUNT_DISABLED, "f") );

	emit_register( IR_SPEC( fe_object_table, NV_PGRAPH_PRI_FE_OBJECT_TABLE ) );
	emit_field   ( F_SPEC( nvclass, NV_PGRAPH_PRI_FE_OBJECT_TABLE_NVCLASS, "v") );

	emit_register( R_SPEC( pri_mme_shadow_raw_index, NV_PGRAPH_PRI_MME_SHADOW_RAM_INDEX ) );
	emit_field   ( F_SPEC( write, NV_PGRAPH_PRI_MME_SHADOW_RAM_INDEX_WRITE, ".") );
	emit_constant( C_SPEC( trigger, NV_PGRAPH_PRI_MME_SHADOW_RAM_INDEX_WRITE_TRIGGER, "f") );

	emit_register( R_SPEC( pri_mme_shadow_raw_data, NV_PGRAPH_PRI_MME_SHADOW_RAM_DATA ) );

	emit_register( R_SPEC( mme_hww_esr, NV_PGRAPH_PRI_MME_HWW_ESR ) );
	emit_field   ( F_SPEC( reset, NV_PGRAPH_PRI_MME_HWW_ESR_RESET, ".") );
	emit_constant( C_SPEC( active, NV_PGRAPH_PRI_MME_HWW_ESR_RESET_ACTIVE, "f") );
	emit_field   ( F_SPEC( en, NV_PGRAPH_PRI_MME_HWW_ESR_EN, ".") );
	emit_constant( C_SPEC( enable, NV_PGRAPH_PRI_MME_HWW_ESR_EN_ENABLE, "f") );

	emit_register( R_SPEC( memfmt_hww_esr, NV_PGRAPH_PRI_MEMFMT_HWW_ESR ) );
	emit_field   ( F_SPEC( reset, NV_PGRAPH_PRI_MEMFMT_HWW_ESR_RESET, ".") );
	emit_constant( C_SPEC( active, NV_PGRAPH_PRI_MEMFMT_HWW_ESR_RESET_ACTIVE, "f") );
	emit_field   ( F_SPEC( en, NV_PGRAPH_PRI_MEMFMT_HWW_ESR_EN, ".") );
	emit_constant( C_SPEC( enable, NV_PGRAPH_PRI_MEMFMT_HWW_ESR_EN_ENABLE, "f") );
	end_scope();

	emit_register( R_SPEC( fecs_cpuctl, NV_PGRAPH_PRI_FECS_FALCON_CPUCTL ) );
	emit_field   ( F_SPEC( startcpu, NV_PGRAPH_PRI_FECS_FALCON_CPUCTL_STARTCPU, "f") );
	emit_register( R_SPEC( fecs_dmactl, NV_PGRAPH_PRI_FECS_FALCON_DMACTL ) );
	emit_field   ( F_SPEC( require_ctx, NV_PGRAPH_PRI_FECS_FALCON_DMACTL_REQUIRE_CTX, "f") );
	emit_field   ( F_SPEC( dmem_scrubbing, NV_PGRAPH_PRI_FECS_FALCON_DMACTL_DMEM_SCRUBBING, "m") );
	emit_field   ( F_SPEC( imem_scrubbing, NV_PGRAPH_PRI_FECS_FALCON_DMACTL_IMEM_SCRUBBING, "m") );

	emit_register( R_SPEC( fecs_os, NV_PGRAPH_PRI_FECS_FALCON_OS ) );

	emit_register( R_SPEC( fecs_idlestate, NV_PGRAPH_PRI_FECS_FALCON_IDLESTATE ) );

	emit_register( R_SPEC( fecs_mailbox0, NV_PGRAPH_PRI_FECS_FALCON_MAILBOX0 ) );

	emit_register( R_SPEC( fecs_mailbox1, NV_PGRAPH_PRI_FECS_FALCON_MAILBOX1 ) );

	emit_register( R_SPEC( fecs_irqstat, NV_PGRAPH_PRI_FECS_FALCON_IRQSTAT ) );

	emit_register( R_SPEC( fecs_irqmode, NV_PGRAPH_PRI_FECS_FALCON_IRQMODE ) );

	emit_register( R_SPEC( fecs_irqmask, NV_PGRAPH_PRI_FECS_FALCON_IRQMASK ) );

	emit_register( R_SPEC( fecs_irqdest, NV_PGRAPH_PRI_FECS_FALCON_IRQDEST ) );

	emit_register( R_SPEC( fecs_curctx, NV_PGRAPH_PRI_FECS_FALCON_CURCTX ) );

	emit_register( R_SPEC( fecs_nxtctx, NV_PGRAPH_PRI_FECS_FALCON_NXTCTX ) );

	emit_register( R_SPEC( fecs_engctl, NV_PGRAPH_PRI_FECS_FALCON_ENGCTL ) );

	emit_register( R_SPEC( fecs_debug1, NV_PGRAPH_PRI_FECS_FALCON_DEBUG1 ) );

	emit_register( R_SPEC( fecs_debuginfo, NV_PGRAPH_PRI_FECS_FALCON_DEBUGINFO ) );

	emit_register( R_SPEC( fecs_icd_cmd, NV_PGRAPH_PRI_FECS_FALCON_ICD_CMD ) );
	emit_field   ( F_SPEC( opc, NV_PGRAPH_PRI_FECS_FALCON_ICD_CMD_OPC, "") );
	emit_constant( C_SPEC( rreg, NV_PGRAPH_PRI_FECS_FALCON_ICD_CMD_OPC_RREG, "f") );
	emit_constant( C_SPEC( rstat, NV_PGRAPH_PRI_FECS_FALCON_ICD_CMD_OPC_RSTAT, "f") );
	 emit_field   ( F_SPEC( idx, NV_PGRAPH_PRI_FECS_FALCON_ICD_CMD_IDX, "f") );

	emit_register( R_SPEC( fecs_icd_rdata, NV_PGRAPH_PRI_FECS_FALCON_ICD_RDATA ) );

	emit_register( IR_SPEC( fecs_imemc, NV_PGRAPH_PRI_FECS_FALCON_IMEMC ) );
	emit_field   ( F_SPEC( offs, NV_PGRAPH_PRI_FECS_FALCON_IMEMC_OFFS, "f") );
	emit_field   ( F_SPEC( blk, NV_PGRAPH_PRI_FECS_FALCON_IMEMC_BLK, "f") );
	emit_field   ( F_SPEC( aincw, NV_PGRAPH_PRI_FECS_FALCON_IMEMC_AINCW, "f") );

	emit_register( IR_SPEC( fecs_imemd, NV_PGRAPH_PRI_FECS_FALCON_IMEMD ) );

	emit_register( IR_SPEC( fecs_imemt, NV_PGRAPH_PRI_FECS_FALCON_IMEMT ) );
	emit_field   ( F_SPEC( tag, NV_PGRAPH_PRI_FECS_FALCON_IMEMT_TAG, "f") );

	emit_register( IR_SPEC( fecs_dmemc, NV_PGRAPH_PRI_FECS_FALCON_DMEMC ) );
	emit_field   ( F_SPEC( offs, NV_PGRAPH_PRI_FECS_FALCON_DMEMC_OFFS, "") );
	emit_field   ( F_SPEC( blk, NV_PGRAPH_PRI_FECS_FALCON_DMEMC_BLK, "f") );
	emit_field   ( F_SPEC( aincw, NV_PGRAPH_PRI_FECS_FALCON_DMEMC_AINCW, "f") );

	emit_register( IR_SPEC( fecs_dmemd, NV_PGRAPH_PRI_FECS_FALCON_DMEMD ) );
	emit_register( R_SPEC( fecs_dmatrfbase, NV_PGRAPH_PRI_FECS_FALCON_DMATRFBASE ) );
	emit_register( R_SPEC( fecs_dmatrfmoffs, NV_PGRAPH_PRI_FECS_FALCON_DMATRFMOFFS ) );
	emit_register( R_SPEC( fecs_dmatrffboffs, NV_PGRAPH_PRI_FECS_FALCON_DMATRFFBOFFS ) );
	emit_register( R_SPEC( fecs_dmatrfcmd, NV_PGRAPH_PRI_FECS_FALCON_DMATRFCMD ) );
	emit_field   ( F_SPEC( imem, NV_PGRAPH_PRI_FECS_FALCON_DMATRFCMD_IMEM, "f") );
	emit_field   ( F_SPEC( write, NV_PGRAPH_PRI_FECS_FALCON_DMATRFCMD_WRITE, "f") );
	emit_field   ( F_SPEC( size, NV_PGRAPH_PRI_FECS_FALCON_DMATRFCMD_SIZE, "f") );
	emit_field   ( F_SPEC( ctxdma, NV_PGRAPH_PRI_FECS_FALCON_DMATRFCMD_CTXDMA, "f") );
	emit_register( R_SPEC( fecs_bootvec, NV_PGRAPH_PRI_FECS_FALCON_BOOTVEC ) );
	emit_field   ( F_SPEC( vec, NV_PGRAPH_PRI_FECS_FALCON_BOOTVEC_VEC, "f") );
	emit_register( R_SPEC( fecs_falcon_hwcfg, NV_PGRAPH_PRI_FECS_FALCON_HWCFG ) );
	emit_register( R_SPEC( gpcs_gpccs_falcon_hwcfg, NV_PGRAPH_PRI_GPCS_GPCCS_FALCON_HWCFG ) );

	emit_register( R_SPEC( fecs_falcon_rm, NV_PGRAPH_PRI_FECS_FALCON_RM ) );

	emit_register( R_SPEC( fecs_current_ctx, NV_PGRAPH_PRI_FECS_CURRENT_CTX ) );
	emit_field   ( F_SPEC( ptr, NV_PGRAPH_PRI_FECS_CURRENT_CTX_PTR, "fv") );
	emit_field   ( F_SPEC( target, NV_PGRAPH_PRI_FECS_CURRENT_CTX_TARGET, ".") );
	emit_field   ( F_SPEC( target, NV_PGRAPH_PRI_FECS_CURRENT_CTX_TARGET , "sfmv") );
	emit_constant( C_SPEC( vid_mem, NV_PGRAPH_PRI_FECS_CURRENT_CTX_TARGET_VID_MEM, "f") );
	emit_field   ( F_SPEC( valid, NV_PGRAPH_PRI_FECS_CURRENT_CTX_VALID, "sfmv") );
	emit_constant( C_SPEC( false, NV_PGRAPH_PRI_FECS_CURRENT_CTX_VALID_FALSE, "f") );

	emit_register( R_SPEC( fecs_method_data, NV_PGRAPH_PRI_FECS_METHOD_DATA ) );

	emit_register( R_SPEC( fecs_method_push, NV_PGRAPH_PRI_FECS_METHOD_PUSH ) );
	emit_field   ( F_SPEC( adr, NV_PGRAPH_PRI_FECS_METHOD_PUSH_ADR, "f") );
	emit_constant( C_SPEC( bind_pointer, NV_PGRAPH_PRI_FECS_METHOD_PUSH_ADR_BIND_POINTER, "fv") );
	emit_constant( C_SPEC( discover_image_size, NV_PGRAPH_PRI_FECS_METHOD_PUSH_ADR_DISCOVER_IMAGE_SIZE, "v") );
	emit_constant( C_SPEC( wfi_golden_save, NV_PGRAPH_PRI_FECS_METHOD_PUSH_ADR_WFI_GOLDEN_SAVE, "v") );
	emit_constant( C_SPEC( restore_golden, NV_PGRAPH_PRI_FECS_METHOD_PUSH_ADR_RESTORE_GOLDEN, "v") );
	emit_constant( C_SPEC( discover_zcull_image_size, NV_PGRAPH_PRI_FECS_METHOD_PUSH_ADR_DISCOVER_ZCULL_IMAGE_SIZE, "v") );
	emit_constant( C_SPEC( discover_pm_image_size, NV_PGRAPH_PRI_FECS_METHOD_PUSH_ADR_DISCOVER_PM_IMAGE_SIZE, "v") );
	emit_constant( C_SPEC( discover_reglist_image_size, NV_PGRAPH_PRI_FECS_METHOD_PUSH_ADR_DISCOVER_REGLIST_IMAGE_SIZE, "v") );
	emit_constant( C_SPEC( set_reglist_bind_instance, NV_PGRAPH_PRI_FECS_METHOD_PUSH_ADR_SET_REGLIST_BIND_INSTANCE, "v") );
	emit_constant( C_SPEC( set_reglist_virtual_address, NV_PGRAPH_PRI_FECS_METHOD_PUSH_ADR_SET_REGLIST_VIRTUAL_ADDRESS, "v") );
	emit_constant( C_SPEC( stop_ctxsw, NV_PGRAPH_PRI_FECS_METHOD_PUSH_ADR_STOP_CTXSW, "v") );
	emit_constant( C_SPEC( start_ctxsw, NV_PGRAPH_PRI_FECS_METHOD_PUSH_ADR_START_CTXSW, "v") );
	emit_constant( C_SPEC( set_watchdog_timeout, NV_PGRAPH_PRI_FECS_METHOD_PUSH_ADR_SET_WATCHDOG_TIMEOUT, "f") );
	emit_constant( C_SPEC( halt_pipeline, NV_PGRAPH_PRI_FECS_METHOD_PUSH_ADR_HALT_PIPELINE, "v") );

	emit_register( R_SPEC( fecs_host_int_status, NV_PGRAPH_PRI_FECS_HOST_INT_STATUS) );
	emit_field   ( F_SPEC( umimp_firmware_method, NV_PGRAPH_PRI_FECS_HOST_INT_STATUS_UMIMP_FIRMWARE_METHOD, "f") );
	emit_field   ( F_SPEC( umimp_illegal_method, NV_PGRAPH_PRI_FECS_HOST_INT_STATUS_UMIMP_ILLEGAL_METHOD, "f") );
	emit_register( R_SPEC( fecs_host_int_clear, NV_PGRAPH_PRI_FECS_HOST_INT_CLEAR) );
	emit_register( R_SPEC( fecs_host_int_enable, NV_PGRAPH_PRI_FECS_HOST_INT_ENABLE ) );
	emit_field   ( F_SPEC( fault_during_ctxsw, NV_PGRAPH_PRI_FECS_HOST_INT_ENABLE_FAULT_DURING_CTXSW, ".") );
	emit_constant( C_SPEC( enable, NV_PGRAPH_PRI_FECS_HOST_INT_ENABLE_FAULT_DURING_CTXSW_ENABLE, "f") );
	emit_field   ( F_SPEC( umimp_firmware_method, NV_PGRAPH_PRI_FECS_HOST_INT_ENABLE_UMIMP_FIRMWARE_METHOD, ".") );
	emit_constant( C_SPEC( enable, NV_PGRAPH_PRI_FECS_HOST_INT_ENABLE_UMIMP_FIRMWARE_METHOD_ENABLE, "f") );
	emit_field   ( F_SPEC( umimp_illegal_method, NV_PGRAPH_PRI_FECS_HOST_INT_ENABLE_UMIMP_ILLEGAL_METHOD, ".") );
	emit_constant( C_SPEC( enable, NV_PGRAPH_PRI_FECS_HOST_INT_ENABLE_UMIMP_ILLEGAL_METHOD_ENABLE, "f") );
	emit_field   ( F_SPEC( watchdog, NV_PGRAPH_PRI_FECS_HOST_INT_ENABLE_WATCHDOG, ".") );
	emit_constant( C_SPEC( enable, NV_PGRAPH_PRI_FECS_HOST_INT_ENABLE_WATCHDOG_ENABLE, "f") );

	emit_register( R_SPEC( fecs_ctxsw_reset_ctl, NV_PGRAPH_PRI_FECS_CTXSW_RESET_CTL ) );
	emit_field   ( F_SPEC( sys_halt, NV_PGRAPH_PRI_FECS_CTXSW_RESET_CTL_SYS_HALT, ".") );
	emit_constant( C_SPEC( disabled, NV_PGRAPH_PRI_FECS_CTXSW_RESET_CTL_SYS_HALT_DISABLED, "f") );
	emit_field   ( F_SPEC( gpc_halt, NV_PGRAPH_PRI_FECS_CTXSW_RESET_CTL_GPC_HALT, ".") );
	emit_constant( C_SPEC( disabled, NV_PGRAPH_PRI_FECS_CTXSW_RESET_CTL_GPC_HALT_DISABLED, "f") );
	emit_field   ( F_SPEC( be_halt, NV_PGRAPH_PRI_FECS_CTXSW_RESET_CTL_BE_HALT, ".") );
	emit_constant( C_SPEC( disabled, NV_PGRAPH_PRI_FECS_CTXSW_RESET_CTL_BE_HALT_DISABLED, "f") );
	emit_field   ( F_SPEC( sys_engine_reset, NV_PGRAPH_PRI_FECS_CTXSW_RESET_CTL_SYS_ENGINE_RESET, ".") );
	emit_constant( C_SPEC( disabled, NV_PGRAPH_PRI_FECS_CTXSW_RESET_CTL_SYS_ENGINE_RESET_DISABLED, "f") );
	emit_field   ( F_SPEC( gpc_engine_reset, NV_PGRAPH_PRI_FECS_CTXSW_RESET_CTL_GPC_ENGINE_RESET, ".") );
	emit_constant( C_SPEC( disabled, NV_PGRAPH_PRI_FECS_CTXSW_RESET_CTL_GPC_ENGINE_RESET_DISABLED, "f") );
	emit_field   ( F_SPEC( be_engine_reset, NV_PGRAPH_PRI_FECS_CTXSW_RESET_CTL_BE_ENGINE_RESET, ".") );
	emit_constant( C_SPEC( disabled, NV_PGRAPH_PRI_FECS_CTXSW_RESET_CTL_BE_ENGINE_RESET_DISABLED, "f") );
	emit_field   ( F_SPEC( sys_context_reset, NV_PGRAPH_PRI_FECS_CTXSW_RESET_CTL_SYS_CONTEXT_RESET, ".") );
	emit_constant( C_SPEC( enabled, NV_PGRAPH_PRI_FECS_CTXSW_RESET_CTL_SYS_CONTEXT_RESET_ENABLED, "f") );
	emit_constant( C_SPEC( disabled, NV_PGRAPH_PRI_FECS_CTXSW_RESET_CTL_SYS_CONTEXT_RESET_DISABLED, "f") );
	emit_field   ( F_SPEC( gpc_context_reset, NV_PGRAPH_PRI_FECS_CTXSW_RESET_CTL_GPC_CONTEXT_RESET, ".") );
	emit_constant( C_SPEC( enabled, NV_PGRAPH_PRI_FECS_CTXSW_RESET_CTL_GPC_CONTEXT_RESET_ENABLED, "f") );
	emit_constant( C_SPEC( disabled, NV_PGRAPH_PRI_FECS_CTXSW_RESET_CTL_GPC_CONTEXT_RESET_DISABLED, "f") );
	emit_field   ( F_SPEC( be_context_reset, NV_PGRAPH_PRI_FECS_CTXSW_RESET_CTL_BE_CONTEXT_RESET, "") );
	emit_constant( C_SPEC( enabled, NV_PGRAPH_PRI_FECS_CTXSW_RESET_CTL_BE_CONTEXT_RESET_ENABLED, "f") );
	emit_constant( C_SPEC( disabled, NV_PGRAPH_PRI_FECS_CTXSW_RESET_CTL_BE_CONTEXT_RESET_DISABLED, "f") );

	emit_register( R_SPEC( fecs_ctx_state_store_major_rev_id, NV_PGRAPH_PRI_FECS_CTX_STATE_STORE_MAJOR_REV_ID ) );

	emit_register( IR_SPEC( fecs_ctxsw_mailbox, NV_PGRAPH_PRI_FECS_CTXSW_MAILBOX ) );
	emit_constant( C_SPEC( _size_1, NV_PGRAPH_PRI_FECS_CTXSW_MAILBOX__SIZE_1, "v") );
	emit_field   ( F_SPEC( value, NV_PGRAPH_PRI_FECS_CTXSW_MAILBOX_VALUE, "f") );
	emit_constant( C_SPEC( pass, NV_PGRAPH_PRI_FECS_CTXSW_MAILBOX_VALUE_PASS, "v") );
	emit_constant( C_SPEC( fail, NV_PGRAPH_PRI_FECS_CTXSW_MAILBOX_VALUE_FAIL, "v") );

	emit_register( IR_SPEC( fecs_ctxsw_mailbox_set, NV_PGRAPH_PRI_FECS_CTXSW_MAILBOX_SET ) );
	emit_field   ( F_SPEC( value, NV_PGRAPH_PRI_FECS_CTXSW_MAILBOX_SET_VALUE, "f") );

	emit_register( IR_SPEC( fecs_ctxsw_mailbox_clear, NV_PGRAPH_PRI_FECS_CTXSW_MAILBOX_CLEAR ) );
	emit_field   ( F_SPEC( value, NV_PGRAPH_PRI_FECS_CTXSW_MAILBOX_CLEAR_VALUE, "f") );

	emit_register( R_SPEC( fecs_fs, NV_PGRAPH_PRI_FECS_FS ) );
	emit_field   ( F_SPEC( num_available_gpcs, NV_PGRAPH_PRI_FECS_FS_NUM_AVAILABLE_GPCS, "") );
	emit_field   ( F_SPEC( num_available_fbps, NV_PGRAPH_PRI_FECS_FS_NUM_AVAILABLE_FBPS, "") );

	emit_register( R_SPEC( fecs_cfg, NV_PGRAPH_PRI_FECS_CFG_FALCON ) );
	emit_field   ( F_SPEC( imem_sz, NV_PGRAPH_PRI_FECS_CFG_FALCON_IMEM_SZ, "v") );

	emit_register( R_SPEC( fecs_rc_lanes, NV_PGRAPH_PRI_FECS_RC_LANES ) );
	emit_field   ( F_SPEC( num_chains, NV_PGRAPH_PRI_FECS_RC_LANES_NUM_CHAINS, "") );

	emit_register( R_SPEC( fecs_ctxsw_status_1, NV_PGRAPH_PRI_FECS_CTXSW_STATUS_1 ) );
	emit_field   ( F_SPEC( arb_busy, NV_PGRAPH_PRI_FECS_CTXSW_STATUS_1_ARB_BUSY, "sfmv") );
	emit_register( R_SPEC( fecs_arb_ctx_adr, NV_PGRAPH_PRI_FECS_ARB_CTX_ADR ) );
	emit_register( R_SPEC( fecs_new_ctx, NV_PGRAPH_PRI_FECS_NEW_CTX ) );
	emit_field   ( F_SPEC( ptr, NV_PGRAPH_PRI_FECS_NEW_CTX_PTR, "sfmv") );
	emit_field   ( F_SPEC( target, NV_PGRAPH_PRI_FECS_NEW_CTX_TARGET, "sfmv") );
	emit_field   ( F_SPEC( valid, NV_PGRAPH_PRI_FECS_NEW_CTX_VALID, "sfmv") );
	emit_register( R_SPEC( fecs_arb_ctx_ptr, NV_PGRAPH_PRI_FECS_ARB_CTX_PTR ) );
	emit_field   ( F_SPEC( ptr, NV_PGRAPH_PRI_FECS_ARB_CTX_PTR_PTR , "sfmv") );
	emit_field   ( F_SPEC( target, NV_PGRAPH_PRI_FECS_ARB_CTX_PTR_TARGET , "sfmv") );
	emit_register( R_SPEC( fecs_arb_ctx_cmd , NV_PGRAPH_PRI_FECS_ARB_CTX_CMD ) );
	emit_field   ( F_SPEC( cmd, NV_PGRAPH_PRI_FECS_ARB_CTX_CMD_CMD , "sfmv") );

	emit_register( R_SPEC( fecs_ctxsw_status_fe_0, NV_PGRAPH_PRI_FECS_CTXSW_STATUS_FE_0) );

	emit_register( R_SPEC( gpc0_gpccs_ctxsw_status_gpc_0, NV_PGRAPH_PRI_GPC0_GPCCS_CTXSW_STATUS_GPC_0) );

	emit_register( R_SPEC( gpc0_gpccs_ctxsw_status_1, NV_PGRAPH_PRI_GPC0_GPCCS_CTXSW_STATUS_1) );

	emit_register( R_SPEC( fecs_ctxsw_idlestate, NV_PGRAPH_PRI_FECS_CTXSW_IDLESTATE) );

	emit_register( R_SPEC( gpc0_gpccs_ctxsw_idlestate, NV_PGRAPH_PRI_GPC0_GPCCS_CTXSW_IDLESTATE) );

	emit_register( R_SPEC( rstr2d_gpc_map0, NV_PGRAPH_PRI_RSTR2D_GPC_MAP0 ) );

	emit_register( R_SPEC( rstr2d_gpc_map1, NV_PGRAPH_PRI_RSTR2D_GPC_MAP1 ) );

	emit_register( R_SPEC( rstr2d_gpc_map2, NV_PGRAPH_PRI_RSTR2D_GPC_MAP2 ) );

	emit_register( R_SPEC( rstr2d_gpc_map3, NV_PGRAPH_PRI_RSTR2D_GPC_MAP3 ) );

	emit_register( R_SPEC( rstr2d_gpc_map4, NV_PGRAPH_PRI_RSTR2D_GPC_MAP4 ) );

	emit_register( R_SPEC( rstr2d_gpc_map5, NV_PGRAPH_PRI_RSTR2D_GPC_MAP5 ) );

	emit_register( R_SPEC( rstr2d_map_table_cfg, NV_PGRAPH_PRI_RSTR2D_MAP_TABLE_CONFIG ) );
	emit_field   ( F_SPEC( row_offset, NV_PGRAPH_PRI_RSTR2D_MAP_TABLE_CONFIG_ROW_OFFSET, "f") );
	emit_field   ( F_SPEC( num_entries, NV_PGRAPH_PRI_RSTR2D_MAP_TABLE_CONFIG_NUM_ENTRIES, "f") );

	emit_register( R_SPEC( pd_hww_esr, NV_PGRAPH_PRI_PD_HWW_ESR ) );
	emit_field   ( F_SPEC( reset, NV_PGRAPH_PRI_PD_HWW_ESR_RESET, ".") );
	emit_constant( C_SPEC( active, NV_PGRAPH_PRI_PD_HWW_ESR_RESET_ACTIVE, "f") );
	emit_field   ( F_SPEC( en, NV_PGRAPH_PRI_PD_HWW_ESR_EN, ".") );
	emit_constant( C_SPEC( enable, NV_PGRAPH_PRI_PD_HWW_ESR_EN_ENABLE, "f") );

	emit_register( IR_SPEC( pd_num_tpc_per_gpc, NV_PGRAPH_PRI_PD_NUM_TPC_PER_GPC ) );
	emit_constant( C_SPEC( _size_1, NV_PGRAPH_PRI_PD_NUM_TPC_PER_GPC__SIZE_1, "v") );
	emit_field   ( F_SPEC( count0, NV_PGRAPH_PRI_PD_NUM_TPC_PER_GPC_COUNT0, "f") );
	emit_field   ( F_SPEC( count1, NV_PGRAPH_PRI_PD_NUM_TPC_PER_GPC_COUNT1, "f") );
	emit_field   ( F_SPEC( count2, NV_PGRAPH_PRI_PD_NUM_TPC_PER_GPC_COUNT2, "f") );
	emit_field   ( F_SPEC( count3, NV_PGRAPH_PRI_PD_NUM_TPC_PER_GPC_COUNT3, "f") );
	emit_field   ( F_SPEC( count4, NV_PGRAPH_PRI_PD_NUM_TPC_PER_GPC_COUNT4, "f") );
	emit_field   ( F_SPEC( count5, NV_PGRAPH_PRI_PD_NUM_TPC_PER_GPC_COUNT5, "f") );
	emit_field   ( F_SPEC( count6, NV_PGRAPH_PRI_PD_NUM_TPC_PER_GPC_COUNT6, "f") );
	emit_field   ( F_SPEC( count7, NV_PGRAPH_PRI_PD_NUM_TPC_PER_GPC_COUNT7, "f") );

	emit_register( R_SPEC( pd_ab_dist_cfg0, NV_PGRAPH_PRI_PD_AB_DIST_CONFIG_0 ) );
	emit_field   ( F_SPEC( timeslice_enable, NV_PGRAPH_PRI_PD_AB_DIST_CONFIG_0_TIMESLICE_ENABLE, ".") );
	emit_constant( C_SPEC( en, NV_PGRAPH_PRI_PD_AB_DIST_CONFIG_0_TIMESLICE_ENABLE_EN, "f") );
	emit_constant( C_SPEC( dis, NV_PGRAPH_PRI_PD_AB_DIST_CONFIG_0_TIMESLICE_ENABLE_DIS, "f") );

	emit_register( R_SPEC( pd_ab_dist_cfg1, NV_PGRAPH_PRI_PD_AB_DIST_CONFIG_1 ) );
	emit_field   ( F_SPEC( max_batches, NV_PGRAPH_PRI_PD_AB_DIST_CONFIG_1_MAX_BATCHES, ".") );
	emit_constant( C_SPEC( init, NV_PGRAPH_PRI_PD_AB_DIST_CONFIG_1_MAX_BATCHES_INIT, "f") );
	emit_field   ( F_SPEC( max_output, NV_PGRAPH_PRI_PD_AB_DIST_CONFIG_1_MAX_OUTPUT, "f") );
	emit_constant( C_SPEC( granularity, NV_PGRAPH_PRI_PD_AB_DIST_CONFIG_1_MAX_OUTPUT_GRANULARITY, "v") );

	emit_register( R_SPEC( pd_ab_dist_cfg2, NV_PGRAPH_PRI_PD_AB_DIST_CONFIG_2 ) );
	emit_field   ( F_SPEC( token_limit, NV_PGRAPH_PRI_PD_AB_DIST_CONFIG_2_TOKEN_LIMIT, "f") );
	emit_constant( C_SPEC( init, NV_PGRAPH_PRI_PD_AB_DIST_CONFIG_2_TOKEN_LIMIT_INIT, "v") );
	emit_field   ( F_SPEC( state_limit, NV_PGRAPH_PRI_PD_AB_DIST_CONFIG_2_STATE_LIMIT, "f") );
	emit_constant( C_SPEC( scc_bundle_granularity, NV_PGRAPH_PRI_PD_AB_DIST_CONFIG_2_STATE_LIMIT_SCC_BUNDLE_GRANULARITY, "v") );
	emit_constant( C_SPEC( min_gpm_fifo_depths, NV_PGRAPH_PRI_PD_AB_DIST_CONFIG_2_STATE_LIMIT_MIN_GPM_FIFO_DEPTHS, "v") );

	emit_register( R_SPEC( pd_pagepool, NV_PGRAPH_PRI_PD_RM_PAGEPOOL ) );
	emit_field   ( F_SPEC( total_pages, NV_PGRAPH_PRI_PD_RM_PAGEPOOL_TOTAL_PAGES, "f") );
	emit_field   ( F_SPEC( valid, NV_PGRAPH_PRI_PD_RM_PAGEPOOL_VALID, ".") );
	emit_constant( C_SPEC( true, NV_PGRAPH_PRI_PD_RM_PAGEPOOL_VALID_TRUE, "f") );

	emit_register( IR_SPEC( pd_dist_skip_table, NV_PGRAPH_PRI_PD_DIST_SKIP_TABLE ) );
	emit_constant( C_SPEC( _size_1, NV_PGRAPH_PRI_PD_DIST_SKIP_TABLE__SIZE_1, "v") );
	emit_field   ( F_SPEC( gpc_4n0_mask, NV_PGRAPH_PRI_PD_DIST_SKIP_TABLE_GPC_4N0_MASK, "f") );
	emit_field   ( F_SPEC( gpc_4n1_mask, NV_PGRAPH_PRI_PD_DIST_SKIP_TABLE_GPC_4N1_MASK, "f") );
	emit_field   ( F_SPEC( gpc_4n2_mask, NV_PGRAPH_PRI_PD_DIST_SKIP_TABLE_GPC_4N2_MASK, "f") );
	emit_field   ( F_SPEC( gpc_4n3_mask, NV_PGRAPH_PRI_PD_DIST_SKIP_TABLE_GPC_4N3_MASK, "f") );

	emit_register( IR_SPEC( pd_alpha_ratio_table, NV_PGRAPH_PRI_PD_ALPHA_RATIO_TABLE ) );
	emit_constant( C_SPEC( _size_1, NV_PGRAPH_PRI_PD_ALPHA_RATIO_TABLE__SIZE_1, "v") );
	emit_field   ( F_SPEC( gpc_4n0_mask, NV_PGRAPH_PRI_PD_ALPHA_RATIO_TABLE_GPC_4N0_MASK, "f") );
	emit_field   ( F_SPEC( gpc_4n1_mask, NV_PGRAPH_PRI_PD_ALPHA_RATIO_TABLE_GPC_4N1_MASK, "f") );
	emit_field   ( F_SPEC( gpc_4n2_mask, NV_PGRAPH_PRI_PD_ALPHA_RATIO_TABLE_GPC_4N2_MASK, "f") );
	emit_field   ( F_SPEC( gpc_4n3_mask, NV_PGRAPH_PRI_PD_ALPHA_RATIO_TABLE_GPC_4N3_MASK, "f") );

	emit_register( IR_SPEC( pd_beta_ratio_table, NV_PGRAPH_PRI_PD_BETA_RATIO_TABLE ) );
	emit_constant( C_SPEC( _size_1, NV_PGRAPH_PRI_PD_BETA_RATIO_TABLE__SIZE_1, "v") );
	emit_field   ( F_SPEC( gpc_4n0_mask, NV_PGRAPH_PRI_PD_BETA_RATIO_TABLE_GPC_4N0_MASK, "f") );
	emit_field   ( F_SPEC( gpc_4n1_mask, NV_PGRAPH_PRI_PD_BETA_RATIO_TABLE_GPC_4N1_MASK, "f") );
	emit_field   ( F_SPEC( gpc_4n2_mask, NV_PGRAPH_PRI_PD_BETA_RATIO_TABLE_GPC_4N2_MASK, "f") );
	emit_field   ( F_SPEC( gpc_4n3_mask, NV_PGRAPH_PRI_PD_BETA_RATIO_TABLE_GPC_4N3_MASK, "f") );

	emit_register( R_SPEC( ds_debug, NV_PGRAPH_PRI_DS_DEBUG ) );

	emit_field   ( F_SPEC( timeslice_mode, NV_PGRAPH_PRI_DS_DEBUG_TIMESLICE_MODE, ".") );
	emit_constant( C_SPEC( disable, NV_PGRAPH_PRI_DS_DEBUG_TIMESLICE_MODE_DISABLE, "f") );
	emit_constant( C_SPEC( enable, NV_PGRAPH_PRI_DS_DEBUG_TIMESLICE_MODE_ENABLE, "f") );

	emit_register( R_SPEC( ds_zbc_color_r, NV_PGRAPH_PRI_DS_ZBC_COLOR_R ) );
	emit_field   ( F_SPEC( val, NV_PGRAPH_PRI_DS_ZBC_COLOR_R_VAL, "f") );

	emit_register( R_SPEC( ds_zbc_color_g, NV_PGRAPH_PRI_DS_ZBC_COLOR_G ) );
	emit_field   ( F_SPEC( val, NV_PGRAPH_PRI_DS_ZBC_COLOR_G_VAL, "f") );

	emit_register( R_SPEC( ds_zbc_color_b, NV_PGRAPH_PRI_DS_ZBC_COLOR_B ) );
	emit_field   ( F_SPEC( val, NV_PGRAPH_PRI_DS_ZBC_COLOR_B_VAL, "f") );

	emit_register( R_SPEC( ds_zbc_color_a, NV_PGRAPH_PRI_DS_ZBC_COLOR_A ) );
	emit_field   ( F_SPEC( val, NV_PGRAPH_PRI_DS_ZBC_COLOR_A_VAL, "f") );

	emit_register( R_SPEC( ds_zbc_color_fmt, NV_PGRAPH_PRI_DS_ZBC_COLOR_FMT ) );
	emit_field   ( F_SPEC( val, NV_PGRAPH_PRI_DS_ZBC_COLOR_FMT_VAL, "f") );
	emit_constant( C_SPEC( invalid, NV_PGRAPH_PRI_DS_ZBC_COLOR_FMT_VAL_INVALID, "f") );
	emit_constant( C_SPEC( zero, NV_PGRAPH_PRI_DS_ZBC_COLOR_FMT_VAL_ZERO, "v") );
	emit_constant( C_SPEC( unorm_one, NV_PGRAPH_PRI_DS_ZBC_COLOR_FMT_VAL_UNORM_ONE, "v") );
	emit_constant( C_SPEC( rf32_gf32_bf32_af32, NV_PGRAPH_PRI_DS_ZBC_COLOR_FMT_VAL_RF32_GF32_BF32_AF32, "v") );
	emit_constant( C_SPEC( a8_b8_g8_r8, NV_PGRAPH_PRI_DS_ZBC_COLOR_FMT_VAL_A8B8G8R8, "v") );

	emit_register( R_SPEC( ds_zbc_z, NV_PGRAPH_PRI_DS_ZBC_Z ) );
	emit_field   ( F_SPEC( val, NV_PGRAPH_PRI_DS_ZBC_Z_VAL, "") );
	emit_constant( C_SPEC( _init, NV_PGRAPH_PRI_DS_ZBC_Z_VAL__INIT, "") );

	emit_register( R_SPEC( ds_zbc_z_fmt, NV_PGRAPH_PRI_DS_ZBC_Z_FMT ) );
	emit_field   ( F_SPEC( val, NV_PGRAPH_PRI_DS_ZBC_Z_FMT_VAL, "f") );
	emit_constant( C_SPEC( invalid, NV_PGRAPH_PRI_DS_ZBC_Z_FMT_VAL_INVALID, "f") );
	emit_constant( C_SPEC( fp32, NV_PGRAPH_PRI_DS_ZBC_Z_FMT_VAL_FP32, "v") );

	emit_register( R_SPEC( ds_zbc_tbl_index, NV_PGRAPH_PRI_DS_ZBC_TBL_INDEX ) );
	emit_field   ( F_SPEC( val, NV_PGRAPH_PRI_DS_ZBC_TBL_INDEX_VAL, "f") );

	emit_register( R_SPEC( ds_zbc_tbl_ld, NV_PGRAPH_PRI_DS_ZBC_TBL_LD ) );
	emit_field   ( F_SPEC( select, NV_PGRAPH_PRI_DS_ZBC_TBL_LD_SELECT, ".") );
	emit_constant( C_SPEC( c, NV_PGRAPH_PRI_DS_ZBC_TBL_LD_SELECT_C, "f") );
	emit_constant( C_SPEC( z, NV_PGRAPH_PRI_DS_ZBC_TBL_LD_SELECT_Z, "f") );
	emit_field   ( F_SPEC( action, NV_PGRAPH_PRI_DS_ZBC_TBL_LD_ACTION, ".") );
	emit_constant( C_SPEC( write, NV_PGRAPH_PRI_DS_ZBC_TBL_LD_ACTION_WRITE, "f") );
	emit_field   ( F_SPEC( trigger, NV_PGRAPH_PRI_DS_ZBC_TBL_LD_TRIGGER, ".") );
	emit_constant( C_SPEC( active, NV_PGRAPH_PRI_DS_ZBC_TBL_LD_TRIGGER_ACTIVE, "f") );

	emit_register( R_SPEC( ds_tga_constraintlogic, NV_PGRAPH_PRI_DS_TGA_CONSTRAINTLOGIC ) );
	emit_field   ( F_SPEC( beta_cbsize, NV_PGRAPH_PRI_DS_TGA_CONSTRAINTLOGIC_BETA_CBSIZE, "f") );
	emit_field   ( F_SPEC( alpha_cbsize, NV_PGRAPH_PRI_DS_TGA_CONSTRAINTLOGIC_ALPHA_CBSIZE, "f") );

	emit_register( R_SPEC( ds_hww_esr, NV_PGRAPH_PRI_DS_HWW_ESR ) );
	emit_field   ( F_SPEC( reset, NV_PGRAPH_PRI_DS_HWW_ESR_RESET, "") );
	emit_constant( C_SPEC( task, NV_PGRAPH_PRI_DS_HWW_ESR_RESET_TASK, "") );

	emit_field   ( F_SPEC( en, NV_PGRAPH_PRI_DS_HWW_ESR_EN, ".") );
	emit_constant( C_SPEC( enabled, NV_PGRAPH_PRI_DS_HWW_ESR_EN_ENABLED, "f") );


	emit_register( R_SPEC( ds_hww_report_mask, NV_PGRAPH_PRI_DS_HWW_REPORT_MASK ) );
	emit_field   ( F_SPEC( sph0_err, NV_PGRAPH_PRI_DS_HWW_REPORT_MASK_SPH0_ERR, ".") );
	emit_constant( C_SPEC( report, NV_PGRAPH_PRI_DS_HWW_REPORT_MASK_SPH0_ERR_REPORT, "f") );
	emit_field   ( F_SPEC( sph1_err, NV_PGRAPH_PRI_DS_HWW_REPORT_MASK_SPH1_ERR, ".") );
	emit_constant( C_SPEC( report, NV_PGRAPH_PRI_DS_HWW_REPORT_MASK_SPH1_ERR_REPORT, "f") );
	emit_field   ( F_SPEC( sph2_err, NV_PGRAPH_PRI_DS_HWW_REPORT_MASK_SPH2_ERR, ".") );
	emit_constant( C_SPEC( report, NV_PGRAPH_PRI_DS_HWW_REPORT_MASK_SPH2_ERR_REPORT, "f") );
	emit_field   ( F_SPEC( sph3_err, NV_PGRAPH_PRI_DS_HWW_REPORT_MASK_SPH3_ERR, ".") );
	emit_constant( C_SPEC( report, NV_PGRAPH_PRI_DS_HWW_REPORT_MASK_SPH3_ERR_REPORT, "f") );
	emit_field   ( F_SPEC( sph4_err, NV_PGRAPH_PRI_DS_HWW_REPORT_MASK_SPH4_ERR, ".") );
	emit_constant( C_SPEC( report, NV_PGRAPH_PRI_DS_HWW_REPORT_MASK_SPH4_ERR_REPORT, "f") );
	emit_field   ( F_SPEC( sph5_err, NV_PGRAPH_PRI_DS_HWW_REPORT_MASK_SPH5_ERR, ".") );
	emit_constant( C_SPEC( report, NV_PGRAPH_PRI_DS_HWW_REPORT_MASK_SPH5_ERR_REPORT, "f") );
	emit_field   ( F_SPEC( sph6_err, NV_PGRAPH_PRI_DS_HWW_REPORT_MASK_SPH6_ERR, ".") );
	emit_constant( C_SPEC( report, NV_PGRAPH_PRI_DS_HWW_REPORT_MASK_SPH6_ERR_REPORT, "f") );
	emit_field   ( F_SPEC( sph7_err, NV_PGRAPH_PRI_DS_HWW_REPORT_MASK_SPH7_ERR, ".") );
	emit_constant( C_SPEC( report, NV_PGRAPH_PRI_DS_HWW_REPORT_MASK_SPH7_ERR_REPORT, "f") );
	emit_field   ( F_SPEC( sph8_err, NV_PGRAPH_PRI_DS_HWW_REPORT_MASK_SPH8_ERR, ".") );
	emit_constant( C_SPEC( report, NV_PGRAPH_PRI_DS_HWW_REPORT_MASK_SPH8_ERR_REPORT, "f") );
	emit_field   ( F_SPEC( sph9_err, NV_PGRAPH_PRI_DS_HWW_REPORT_MASK_SPH9_ERR, ".") );
	emit_constant( C_SPEC( report, NV_PGRAPH_PRI_DS_HWW_REPORT_MASK_SPH9_ERR_REPORT, "f") );
	emit_field   ( F_SPEC( sph10_err, NV_PGRAPH_PRI_DS_HWW_REPORT_MASK_SPH10_ERR, ".") );
	emit_constant( C_SPEC( report, NV_PGRAPH_PRI_DS_HWW_REPORT_MASK_SPH10_ERR_REPORT, "f") );
	emit_field   ( F_SPEC( sph11_err, NV_PGRAPH_PRI_DS_HWW_REPORT_MASK_SPH11_ERR, ".") );
	emit_constant( C_SPEC( report, NV_PGRAPH_PRI_DS_HWW_REPORT_MASK_SPH11_ERR_REPORT, "f") );
	emit_field   ( F_SPEC( sph12_err, NV_PGRAPH_PRI_DS_HWW_REPORT_MASK_SPH12_ERR, ".") );
	emit_constant( C_SPEC( report, NV_PGRAPH_PRI_DS_HWW_REPORT_MASK_SPH12_ERR_REPORT, "f") );
	emit_field   ( F_SPEC( sph13_err, NV_PGRAPH_PRI_DS_HWW_REPORT_MASK_SPH13_ERR, ".") );
	emit_constant( C_SPEC( report, NV_PGRAPH_PRI_DS_HWW_REPORT_MASK_SPH13_ERR_REPORT, "f") );
	emit_field   ( F_SPEC( sph14_err, NV_PGRAPH_PRI_DS_HWW_REPORT_MASK_SPH14_ERR, ".") );
	emit_constant( C_SPEC( report, NV_PGRAPH_PRI_DS_HWW_REPORT_MASK_SPH14_ERR_REPORT, "f") );
	emit_field   ( F_SPEC( sph15_err, NV_PGRAPH_PRI_DS_HWW_REPORT_MASK_SPH15_ERR, ".") );
	emit_constant( C_SPEC( report, NV_PGRAPH_PRI_DS_HWW_REPORT_MASK_SPH15_ERR_REPORT, "f") );
	emit_field   ( F_SPEC( sph16_err, NV_PGRAPH_PRI_DS_HWW_REPORT_MASK_SPH16_ERR, ".") );
	emit_constant( C_SPEC( report, NV_PGRAPH_PRI_DS_HWW_REPORT_MASK_SPH16_ERR_REPORT, "f") );
	emit_field   ( F_SPEC( sph17_err, NV_PGRAPH_PRI_DS_HWW_REPORT_MASK_SPH17_ERR, ".") );
	emit_constant( C_SPEC( report, NV_PGRAPH_PRI_DS_HWW_REPORT_MASK_SPH17_ERR_REPORT, "f") );
	emit_field   ( F_SPEC( sph18_err, NV_PGRAPH_PRI_DS_HWW_REPORT_MASK_SPH18_ERR, ".") );
	emit_constant( C_SPEC( report, NV_PGRAPH_PRI_DS_HWW_REPORT_MASK_SPH18_ERR_REPORT, "f") );
	emit_field   ( F_SPEC( sph19_err, NV_PGRAPH_PRI_DS_HWW_REPORT_MASK_SPH19_ERR, ".") );
	emit_constant( C_SPEC( report, NV_PGRAPH_PRI_DS_HWW_REPORT_MASK_SPH19_ERR_REPORT, "f") );
	emit_field   ( F_SPEC( sph20_err, NV_PGRAPH_PRI_DS_HWW_REPORT_MASK_SPH20_ERR, ".") );
	emit_constant( C_SPEC( report, NV_PGRAPH_PRI_DS_HWW_REPORT_MASK_SPH20_ERR_REPORT, "f") );
	emit_field   ( F_SPEC( sph21_err, NV_PGRAPH_PRI_DS_HWW_REPORT_MASK_SPH21_ERR, ".") );
	emit_constant( C_SPEC( report, NV_PGRAPH_PRI_DS_HWW_REPORT_MASK_SPH21_ERR_REPORT, "f") );
	emit_field   ( F_SPEC( sph22_err, NV_PGRAPH_PRI_DS_HWW_REPORT_MASK_SPH22_ERR, ".") );
	emit_constant( C_SPEC( report, NV_PGRAPH_PRI_DS_HWW_REPORT_MASK_SPH22_ERR_REPORT, "f") );
	emit_field   ( F_SPEC( sph23_err, NV_PGRAPH_PRI_DS_HWW_REPORT_MASK_SPH23_ERR, ".") );
	emit_constant( C_SPEC( report, NV_PGRAPH_PRI_DS_HWW_REPORT_MASK_SPH23_ERR_REPORT, "f") );

	emit_register( IR_SPEC( ds_num_tpc_per_gpc, NV_PGRAPH_PRI_DS_NUM_TPC_PER_GPC ) );

	emit_register( R_SPEC( scc_bundle_cb_base, NV_PGRAPH_PRI_SCC_RM_BUNDLE_CB_BASE ) );
	emit_field   ( F_SPEC( addr_39_8, NV_PGRAPH_PRI_SCC_RM_BUNDLE_CB_BASE_ADDR_39_8, "f") );
	emit_constant( C_SPEC( align_bits, NV_PGRAPH_PRI_SCC_RM_BUNDLE_CB_BASE_ADDR_39_8_ALIGN_BITS, "v") );

	emit_register( R_SPEC( scc_bundle_cb_size, NV_PGRAPH_PRI_SCC_RM_BUNDLE_CB_SIZE ) );
	emit_field   ( F_SPEC( div_256b, NV_PGRAPH_PRI_SCC_RM_BUNDLE_CB_SIZE_DIV_256B, "f") );
	emit_constant( C_SPEC( _prod, NV_PGRAPH_PRI_SCC_RM_BUNDLE_CB_SIZE_DIV_256B__PROD, "v") );
	emit_constant( C_SPEC( byte_granularity, NV_PGRAPH_PRI_SCC_RM_BUNDLE_CB_SIZE_DIV_256B_BYTE_GRANULARITY, "v") );
	emit_field   ( F_SPEC( valid, NV_PGRAPH_PRI_SCC_RM_BUNDLE_CB_SIZE_VALID, ".") );
	emit_constant( C_SPEC( false, NV_PGRAPH_PRI_SCC_RM_BUNDLE_CB_SIZE_VALID_FALSE, "") );
	emit_constant( C_SPEC( true, NV_PGRAPH_PRI_SCC_RM_BUNDLE_CB_SIZE_VALID_TRUE, "f") );

	emit_register( R_SPEC( scc_pagepool_base, NV_PGRAPH_PRI_SCC_RM_PAGEPOOL_BASE ) );
	emit_field   ( F_SPEC( addr_39_8, NV_PGRAPH_PRI_SCC_RM_PAGEPOOL_BASE_ADDR_39_8, "f") );
	emit_constant( C_SPEC( align_bits, NV_PGRAPH_PRI_SCC_RM_PAGEPOOL_BASE_ADDR_39_8_ALIGN_BITS, "v") );

	emit_register( R_SPEC( scc_pagepool, NV_PGRAPH_PRI_SCC_RM_PAGEPOOL ) );
	emit_field   ( F_SPEC( total_pages, NV_PGRAPH_PRI_SCC_RM_PAGEPOOL_TOTAL_PAGES, "f") );
	emit_constant( C_SPEC( hwmax, NV_PGRAPH_PRI_SCC_RM_PAGEPOOL_TOTAL_PAGES_HWMAX, "v") );
	emit_constant( C_SPEC( hwmax_value, NV_PGRAPH_PRI_SCC_RM_PAGEPOOL_TOTAL_PAGES_HWMAX_VALUE, "v") );
	emit_constant( C_SPEC( byte_granularity, NV_PGRAPH_PRI_SCC_RM_PAGEPOOL_TOTAL_PAGES_BYTE_GRANULARITY, "v") );
	emit_field   ( F_SPEC( max_valid_pages, NV_PGRAPH_PRI_SCC_RM_PAGEPOOL_MAX_VALID_PAGES, "") );
	emit_field   ( F_SPEC( valid, NV_PGRAPH_PRI_SCC_RM_PAGEPOOL_VALID, ".") );
	emit_constant( C_SPEC( true, NV_PGRAPH_PRI_SCC_RM_PAGEPOOL_VALID_TRUE, "f") );

	emit_register( R_SPEC( scc_init, NV_PGRAPH_PRI_SCC_INIT ) );
	emit_field   ( F_SPEC( ram, NV_PGRAPH_PRI_SCC_INIT_RAM, ".") );
	emit_constant( C_SPEC( trigger, NV_PGRAPH_PRI_SCC_INIT_RAM_TRIGGER, "f") );

	emit_register( R_SPEC( scc_hww_esr, NV_PGRAPH_PRI_SCC_HWW_ESR ) );
	emit_field   ( F_SPEC( reset, NV_PGRAPH_PRI_SCC_HWW_ESR_RESET, ".") );
	emit_constant( C_SPEC( active, NV_PGRAPH_PRI_SCC_HWW_ESR_RESET_ACTIVE, "f") );
	emit_field   ( F_SPEC( en, NV_PGRAPH_PRI_SCC_HWW_ESR_EN, ".") );
	emit_constant( C_SPEC( enable, NV_PGRAPH_PRI_SCC_HWW_ESR_EN_ENABLE, "f") );

	emit_register( R_SPEC( sked_hww_esr, NV_PGRAPH_PRI_SKED_HWW_ESR ) );
	emit_field   ( F_SPEC( reset, NV_PGRAPH_PRI_SKED_HWW_ESR_RESET, ".") );
	emit_constant( C_SPEC( active, NV_PGRAPH_PRI_SKED_HWW_ESR_RESET_ACTIVE, "f") );

	emit_register( R_SPEC( cwd_fs, NV_PGRAPH_PRI_CWD_FS ) );
	emit_field   ( F_SPEC( num_gpcs, NV_PGRAPH_PRI_CWD_FS_NUM_GPCS, "f") );
	emit_field   ( F_SPEC( num_tpcs, NV_PGRAPH_PRI_CWD_FS_NUM_TPCS, "f") );

	emit_register( R_SPEC( gpc0_fs_gpc, NV_PGRAPH_PRI_GPC0_GPCCS_FS_GPC ) );
	emit_field   ( F_SPEC( num_available_tpcs, NV_PGRAPH_PRI_GPC0_GPCCS_FS_GPC_NUM_AVAILABLE_TPCS, "v") );
	emit_field   ( F_SPEC( num_available_zculls, NV_PGRAPH_PRI_GPC0_GPCCS_FS_GPC_NUM_AVAILABLE_ZCULLS, "v") );

	emit_register( R_SPEC( gpc0_cfg, NV_PGRAPH_PRI_GPC0_GPCCS_CFG_FALCON ) );
	emit_field   ( F_SPEC( imem_sz, NV_PGRAPH_PRI_GPC0_GPCCS_CFG_FALCON_IMEM_SZ, "v") );

	emit_register( R_SPEC( gpccs_rc_lanes, NV_PGRAPH_PRI_GPC0_GPCCS_RC_LANES ) );
	emit_field   ( F_SPEC( num_chains, NV_PGRAPH_PRI_GPC0_GPCCS_RC_LANES_NUM_CHAINS, "") );

	emit_register( IR_SPEC( gpccs_rc_lane_size, NV_PGRAPH_PRI_GPC0_GPCCS_RC_LANE_SIZE ) );
	emit_constant( C_SPEC( _size_1, NV_PGRAPH_PRI_GPC0_GPCCS_RC_LANE_SIZE__SIZE_1, "v") ); /*dodgy?*/
	emit_field   ( F_SPEC( v, NV_PGRAPH_PRI_GPC0_GPCCS_RC_LANE_SIZE_V, "") );
	emit_constant( C_SPEC( 0, NV_PGRAPH_PRI_GPC0_GPCCS_RC_LANE_SIZE_V_0, "") );

	emit_register( R_SPEC( gpc0_zcull_fs, NV_PGRAPH_PRI_GPC0_ZCULL_FS ) );
	emit_field   ( F_SPEC( num_sms, NV_PGRAPH_PRI_GPC0_ZCULL_FS_NUM_SMS, "f") );
	emit_field   ( F_SPEC( num_active_banks, NV_PGRAPH_PRI_GPC0_ZCULL_FS_NUM_ACTIVE_BANKS, "f") );

	emit_register( R_SPEC( gpc0_zcull_ram_addr, NV_PGRAPH_PRI_GPC0_ZCULL_RAM_ADDR ) );
	emit_field   ( F_SPEC( tiles_per_hypertile_row_per_gpc, NV_PGRAPH_PRI_GPC0_ZCULL_RAM_ADDR_TILES_PER_HYPERTILE_ROW_PER_GPC, "f") );
	emit_field   ( F_SPEC( row_offset, NV_PGRAPH_PRI_GPC0_ZCULL_RAM_ADDR_ROW_OFFSET, "f") );

	emit_register( R_SPEC( gpc0_zcull_sm_num_rcp, NV_PGRAPH_PRI_GPC0_ZCULL_SM_NUM_RCP ) );
	emit_field   ( F_SPEC( conservative, NV_PGRAPH_PRI_GPC0_ZCULL_SM_NUM_RCP_CONSERVATIVE, "f") );
	emit_constant( C_SPEC( _max, NV_PGRAPH_PRI_GPC0_ZCULL_SM_NUM_RCP_CONSERVATIVE__MAX, "v") );

	emit_register( R_SPEC( gpc0_zcull_total_ram_size, NV_PGRAPH_PRI_GPC0_ZCULL_TOTAL_RAM_SIZE ) );
	emit_field   ( F_SPEC( num_aliquots, NV_PGRAPH_PRI_GPC0_ZCULL_TOTAL_RAM_SIZE_NUM_ALIQUOTS, "f") );

	emit_register( IR_SPEC( gpc0_zcull_zcsize, NV_PGRAPH_PRI_GPC0_ZCULL_ZCSIZE ) );
	emit_field   ( F_SPEC( height, NV_PGRAPH_PRI_GPC0_ZCULL_ZCSIZE_HEIGHT, ".") );
	emit_constant( C_SPEC( subregion__multiple, NV_PGRAPH_PRI_GPC0_ZCULL_ZCSIZE_HEIGHT_SUBREGION__MULTIPLE, "v") );
	emit_field   ( F_SPEC( width, NV_PGRAPH_PRI_GPC0_ZCULL_ZCSIZE_WIDTH, ".") );
	emit_constant( C_SPEC( subregion__multiple, NV_PGRAPH_PRI_GPC0_ZCULL_ZCSIZE_WIDTH_SUBREGION__MULTIPLE, "v") );

	emit_register( R_SPEC( gpc0_gpm_pd_active_tpcs, NV_PGRAPH_PRI_GPC0_GPM_PD_ACTIVE_TPCS ) );
	emit_field   ( F_SPEC( num, NV_PGRAPH_PRI_GPC0_GPM_PD_ACTIVE_TPCS_NUM, "f") );

	emit_register( IR_SPEC( gpc0_gpm_pd_sm_id, NV_PGRAPH_PRI_GPC0_GPM_PD_SM_ID ) );
	emit_field   ( F_SPEC( id, NV_PGRAPH_PRI_GPC0_GPM_PD_SM_ID_ID, "f") );

	emit_register( IR_SPEC( gpc0_gpm_pd_pes_tpc_id_mask, NV_PGRAPH_PRI_GPC0_GPM_PD_PES_TPC_ID_MASK ) );
	emit_field   ( F_SPEC( mask, NV_PGRAPH_PRI_GPC0_GPM_PD_PES_TPC_ID_MASK_MASK, "v") );

	emit_register( R_SPEC( gpc0_gpm_sd_active_tpcs, NV_PGRAPH_PRI_GPC0_GPM_SD_ACTIVE_TPCS ) );
	emit_field   ( F_SPEC( num, NV_PGRAPH_PRI_GPC0_GPM_SD_ACTIVE_TPCS_NUM, "f") );

	emit_register( R_SPEC( gpc0_tpc0_pe_cfg_smid, NV_PGRAPH_PRI_GPC0_TPC0_PE_CFG_SMID ) );
	emit_field   ( F_SPEC( value, NV_PGRAPH_PRI_GPC0_TPC0_PE_CFG_SMID_VALUE, "f") );

	emit_register( R_SPEC( gpc0_tpc0_l1c_cfg_smid, NV_PGRAPH_PRI_GPC0_TPC0_L1C_CFG_SMID ) );
	emit_field   ( F_SPEC( value, NV_PGRAPH_PRI_GPC0_TPC0_L1C_CFG_SMID_VALUE, "f") );

	emit_register( R_SPEC( gpc0_tpc0_sm_cfg, NV_PGRAPH_PRI_GPC0_TPC0_SM_CONFIG ) );
	emit_field   ( F_SPEC( sm_id, NV_PGRAPH_PRI_GPC0_TPC0_SM_CONFIG_SM_ID, "f") );

	emit_register( R_SPEC( gpc0_tpc0_sm_arch, NV_PGRAPH_PRI_GPC0_TPC0_SM_ARCH ) );
	emit_field   ( F_SPEC( warp_count, NV_PGRAPH_PRI_GPC0_TPC0_SM_ARCH_WARP_COUNT, "v") );
	emit_field   ( F_SPEC( spa_version, NV_PGRAPH_PRI_GPC0_TPC0_SM_ARCH_SPA_VERSION, "v") );
	emit_constant( C_SPEC( smkepler_lp, NV_PGRAPH_PRI_GPC0_TPC0_SM_ARCH_SPA_VERSION_SMKEPLER_LP, "v") );

	emit_register( R_SPEC( gpc0_ppc0_pes_vsc_strem, NV_PGRAPH_PRI_GPC0_PPC0_PES_VSC_STREAM ) );
	emit_field   ( F_SPEC( master_pe, NV_PGRAPH_PRI_GPC0_PPC0_PES_VSC_STREAM_MASTER_PE, "m") );
	emit_constant( C_SPEC( true, NV_PGRAPH_PRI_GPC0_PPC0_PES_VSC_STREAM_MASTER_PE_TRUE, "f") );

	emit_register( R_SPEC( gpc0_ppc0_cbm_cfg, NV_PGRAPH_PRI_GPC0_PPC0_CBM_CONFIG ) );
	emit_field   ( F_SPEC( start_offset, NV_PGRAPH_PRI_GPC0_PPC0_CBM_CONFIG_START_OFFSET, "fmv") );
	emit_field   ( F_SPEC( size, NV_PGRAPH_PRI_GPC0_PPC0_CBM_CONFIG_SIZE, "fmv") );
	emit_constant( C_SPEC( default, NV_PGRAPH_PRI_GPC0_PPC0_CBM_CONFIG_SIZE_DEFAULT, "v") );
	emit_constant( C_SPEC( granularity, NV_PGRAPH_PRI_GPC0_PPC0_CBM_CONFIG_SIZE_GRANULARITY, "v") );
	emit_field   ( F_SPEC( timeslice_mode, NV_PGRAPH_PRI_GPC0_PPC0_CBM_CONFIG_TIMESLICE_MODE, "f") );


	emit_register( R_SPEC( gpc0_ppc0_cbm_cfg2, NV_PGRAPH_PRI_GPC0_PPC0_CBM_CONFIG2 ) );
	emit_field   ( F_SPEC( start_offset, NV_PGRAPH_PRI_GPC0_PPC0_CBM_CONFIG2_START_OFFSET, "f") );
	emit_field   ( F_SPEC( size, NV_PGRAPH_PRI_GPC0_PPC0_CBM_CONFIG2_SIZE, "fmv") );
	emit_constant( C_SPEC( default, NV_PGRAPH_PRI_GPC0_PPC0_CBM_CONFIG2_SIZE_DEFAULT, "v") );
	emit_constant( C_SPEC( granularity, NV_PGRAPH_PRI_GPC0_PPC0_CBM_CONFIG2_SIZE_GRANULARITY, "v") );

	emit_register( R_SPEC( gpccs_falcon_addr, NV_PGRAPH_PRI_GPCS_GPCCS_FALCON_ADDR ) );
	emit_field   ( F_SPEC( lsb, NV_PGRAPH_PRI_GPCS_GPCCS_FALCON_ADDR_LSB, "") );
	emit_constant( C_SPEC( init, NV_PGRAPH_PRI_GPCS_GPCCS_FALCON_ADDR_LSB_INIT, "") );
	emit_field   ( F_SPEC( msb, NV_PGRAPH_PRI_GPCS_GPCCS_FALCON_ADDR_MSB, "") );
	emit_constant( C_SPEC( init, NV_PGRAPH_PRI_GPCS_GPCCS_FALCON_ADDR_MSB_INIT, "") );
	emit_field   ( F_SPEC( ext, NV_PGRAPH_PRI_GPCS_GPCCS_FALCON_ADDR_EXT, "") );

	emit_register( R_SPEC( gpccs_cpuctl, NV_PGRAPH_PRI_GPCS_GPCCS_FALCON_CPUCTL ) );
	emit_field   ( F_SPEC( startcpu, NV_PGRAPH_PRI_GPCS_GPCCS_FALCON_CPUCTL_STARTCPU, "f") );

	emit_register( R_SPEC( gpccs_dmactl, NV_PGRAPH_PRI_GPCS_GPCCS_FALCON_DMACTL ) );
	emit_field   ( F_SPEC( require_ctx, NV_PGRAPH_PRI_GPCS_GPCCS_FALCON_DMACTL_REQUIRE_CTX, "f") );
	emit_field   ( F_SPEC( dmem_scrubbing, NV_PGRAPH_PRI_GPCS_GPCCS_FALCON_DMACTL_DMEM_SCRUBBING, "m") );
	emit_field   ( F_SPEC( imem_scrubbing, NV_PGRAPH_PRI_GPCS_GPCCS_FALCON_DMACTL_IMEM_SCRUBBING, "m") );

	emit_register( IR_SPEC( gpccs_imemc, NV_PGRAPH_PRI_GPCS_GPCCS_FALCON_IMEMC ) );
	emit_field   ( F_SPEC( offs, NV_PGRAPH_PRI_GPCS_GPCCS_FALCON_IMEMC_OFFS, "f") );
	emit_field   ( F_SPEC( blk, NV_PGRAPH_PRI_GPCS_GPCCS_FALCON_IMEMC_BLK, "f") );
	emit_field   ( F_SPEC( aincw, NV_PGRAPH_PRI_GPCS_GPCCS_FALCON_IMEMC_AINCW, "f") );

	emit_register( IR_SPEC( gpccs_imemd, NV_PGRAPH_PRI_GPCS_GPCCS_FALCON_IMEMD ) );

	emit_register( IR_SPEC( gpccs_imemt, NV_PGRAPH_PRI_GPCS_GPCCS_FALCON_IMEMT ) );
	emit_constant( C_SPEC( _size_1, NV_PGRAPH_PRI_GPCS_GPCCS_FALCON_IMEMT__SIZE_1, "v") ); /*dodgy?*/
	emit_field   ( F_SPEC( tag, NV_PGRAPH_PRI_GPCS_GPCCS_FALCON_IMEMT_TAG, "f") );

	emit_register( IR_SPEC( gpccs_dmemc, NV_PGRAPH_PRI_GPCS_GPCCS_FALCON_DMEMC ) );
	emit_field   ( F_SPEC( offs, NV_PGRAPH_PRI_GPCS_GPCCS_FALCON_DMEMC_OFFS, "f") );
	emit_field   ( F_SPEC( blk, NV_PGRAPH_PRI_GPCS_GPCCS_FALCON_DMEMC_BLK, "f") );
	emit_field   ( F_SPEC( aincw, NV_PGRAPH_PRI_GPCS_GPCCS_FALCON_DMEMC_AINCW, "f") );

	emit_register( IR_SPEC( gpccs_dmemd, NV_PGRAPH_PRI_GPCS_GPCCS_FALCON_DMEMD ) );

	emit_register( IR_SPEC( gpccs_ctxsw_mailbox, NV_PGRAPH_PRI_GPCS_GPCCS_CTXSW_MAILBOX ) );
	emit_field   ( F_SPEC( value, NV_PGRAPH_PRI_GPCS_GPCCS_CTXSW_MAILBOX_VALUE, "f") );

	emit_register( R_SPEC( gpcs_setup_bundle_cb_base, NV_PGRAPH_PRI_GPCS_SETUP_RM_BUNDLE_CB_BASE ) );
	emit_field   ( F_SPEC( addr_39_8, NV_PGRAPH_PRI_GPCS_SETUP_RM_BUNDLE_CB_BASE_ADDR_39_8, "") );
	emit_constant( C_SPEC( init, NV_PGRAPH_PRI_GPCS_SETUP_RM_BUNDLE_CB_BASE_ADDR_39_8_INIT, "") );

	emit_register( R_SPEC( gpcs_setup_bundle_cb_size, NV_PGRAPH_PRI_GPCS_SETUP_RM_BUNDLE_CB_SIZE ) );
	emit_field   ( F_SPEC( div_256b, NV_PGRAPH_PRI_GPCS_SETUP_RM_BUNDLE_CB_SIZE_DIV_256B, "") );
	emit_constant( C_SPEC( init, NV_PGRAPH_PRI_GPCS_SETUP_RM_BUNDLE_CB_SIZE_DIV_256B_INIT, "") );
	emit_constant( C_SPEC( _prod, NV_PGRAPH_PRI_GPCS_SETUP_RM_BUNDLE_CB_SIZE_DIV_256B__PROD, "") );
	emit_field   ( F_SPEC( valid, NV_PGRAPH_PRI_GPCS_SETUP_RM_BUNDLE_CB_SIZE_VALID, "") );
	emit_constant( C_SPEC( false, NV_PGRAPH_PRI_GPCS_SETUP_RM_BUNDLE_CB_SIZE_VALID_FALSE, "") );
	emit_constant( C_SPEC( true, NV_PGRAPH_PRI_GPCS_SETUP_RM_BUNDLE_CB_SIZE_VALID_TRUE, "") );

	emit_register( R_SPEC( gpcs_setup_attrib_cb_base, NV_PGRAPH_PRI_GPCS_SETUP_RM_ATTRIB_CB_BASE ) );
	emit_field   ( F_SPEC( addr_39_12, NV_PGRAPH_PRI_GPCS_SETUP_RM_ATTRIB_CB_BASE_ADDR_39_12, "f") );
	emit_constant( C_SPEC( align_bits, NV_PGRAPH_PRI_GPCS_SETUP_RM_ATTRIB_CB_BASE_ADDR_39_12_ALIGN_BITS, "v") );
	emit_field   ( F_SPEC( valid, NV_PGRAPH_PRI_GPCS_SETUP_RM_ATTRIB_CB_BASE_VALID, ".") );
	emit_constant( C_SPEC( true, NV_PGRAPH_PRI_GPCS_SETUP_RM_ATTRIB_CB_BASE_VALID_TRUE, "f") );

	emit_register( R_SPEC( crstr_gpc_map0, NV_PGRAPH_PRI_GPCS_CRSTR_GPC_MAP0 ) );
	emit_field   ( F_SPEC( tile0, NV_PGRAPH_PRI_GPCS_CRSTR_GPC_MAP0_TILE0, "f") );
	emit_field   ( F_SPEC( tile1, NV_PGRAPH_PRI_GPCS_CRSTR_GPC_MAP0_TILE1, "f") );
	emit_field   ( F_SPEC( tile2, NV_PGRAPH_PRI_GPCS_CRSTR_GPC_MAP0_TILE2, "f") );
	emit_field   ( F_SPEC( tile3, NV_PGRAPH_PRI_GPCS_CRSTR_GPC_MAP0_TILE3, "f") );
	emit_field   ( F_SPEC( tile4, NV_PGRAPH_PRI_GPCS_CRSTR_GPC_MAP0_TILE4, "f") );
	emit_field   ( F_SPEC( tile5, NV_PGRAPH_PRI_GPCS_CRSTR_GPC_MAP0_TILE5, "f") );

	emit_register( R_SPEC( crstr_gpc_map1, NV_PGRAPH_PRI_GPCS_CRSTR_GPC_MAP1 ) );
	emit_field   ( F_SPEC( tile6, NV_PGRAPH_PRI_GPCS_CRSTR_GPC_MAP1_TILE6, "f") );
	emit_field   ( F_SPEC( tile7, NV_PGRAPH_PRI_GPCS_CRSTR_GPC_MAP1_TILE7, "f") );
	emit_field   ( F_SPEC( tile8, NV_PGRAPH_PRI_GPCS_CRSTR_GPC_MAP1_TILE8, "f") );
	emit_field   ( F_SPEC( tile9, NV_PGRAPH_PRI_GPCS_CRSTR_GPC_MAP1_TILE9, "f") );
	emit_field   ( F_SPEC( tile10, NV_PGRAPH_PRI_GPCS_CRSTR_GPC_MAP1_TILE10, "f") );
	emit_field   ( F_SPEC( tile11, NV_PGRAPH_PRI_GPCS_CRSTR_GPC_MAP1_TILE11, "f") );

	emit_register( R_SPEC( crstr_gpc_map2, NV_PGRAPH_PRI_GPCS_CRSTR_GPC_MAP2 ) );
	emit_field   ( F_SPEC( tile12, NV_PGRAPH_PRI_GPCS_CRSTR_GPC_MAP2_TILE12, "f") );
	emit_field   ( F_SPEC( tile13, NV_PGRAPH_PRI_GPCS_CRSTR_GPC_MAP2_TILE13, "f") );
	emit_field   ( F_SPEC( tile14, NV_PGRAPH_PRI_GPCS_CRSTR_GPC_MAP2_TILE14, "f") );
	emit_field   ( F_SPEC( tile15, NV_PGRAPH_PRI_GPCS_CRSTR_GPC_MAP2_TILE15, "f") );
	emit_field   ( F_SPEC( tile16, NV_PGRAPH_PRI_GPCS_CRSTR_GPC_MAP2_TILE16, "f") );
	emit_field   ( F_SPEC( tile17, NV_PGRAPH_PRI_GPCS_CRSTR_GPC_MAP2_TILE17, "f") );

	emit_register( R_SPEC( crstr_gpc_map3, NV_PGRAPH_PRI_GPCS_CRSTR_GPC_MAP3 ) );
	emit_field   ( F_SPEC( tile18, NV_PGRAPH_PRI_GPCS_CRSTR_GPC_MAP3_TILE18, "f") );
	emit_field   ( F_SPEC( tile19, NV_PGRAPH_PRI_GPCS_CRSTR_GPC_MAP3_TILE19, "f") );
	emit_field   ( F_SPEC( tile20, NV_PGRAPH_PRI_GPCS_CRSTR_GPC_MAP3_TILE20, "f") );
	emit_field   ( F_SPEC( tile21, NV_PGRAPH_PRI_GPCS_CRSTR_GPC_MAP3_TILE21, "f") );
	emit_field   ( F_SPEC( tile22, NV_PGRAPH_PRI_GPCS_CRSTR_GPC_MAP3_TILE22, "f") );
	emit_field   ( F_SPEC( tile23, NV_PGRAPH_PRI_GPCS_CRSTR_GPC_MAP3_TILE23, "f") );

	emit_register( R_SPEC( crstr_gpc_map4, NV_PGRAPH_PRI_GPCS_CRSTR_GPC_MAP4 ) );
	emit_field   ( F_SPEC( tile24, NV_PGRAPH_PRI_GPCS_CRSTR_GPC_MAP4_TILE24, "f") );
	emit_field   ( F_SPEC( tile25, NV_PGRAPH_PRI_GPCS_CRSTR_GPC_MAP4_TILE25, "f") );
	emit_field   ( F_SPEC( tile26, NV_PGRAPH_PRI_GPCS_CRSTR_GPC_MAP4_TILE26, "f") );
	emit_field   ( F_SPEC( tile27, NV_PGRAPH_PRI_GPCS_CRSTR_GPC_MAP4_TILE27, "f") );
	emit_field   ( F_SPEC( tile28, NV_PGRAPH_PRI_GPCS_CRSTR_GPC_MAP4_TILE28, "f") );
	emit_field   ( F_SPEC( tile29, NV_PGRAPH_PRI_GPCS_CRSTR_GPC_MAP4_TILE29, "f") );

	emit_register( R_SPEC( crstr_gpc_map5, NV_PGRAPH_PRI_GPCS_CRSTR_GPC_MAP5 ) );
	emit_field   ( F_SPEC( tile30, NV_PGRAPH_PRI_GPCS_CRSTR_GPC_MAP5_TILE30, "f") );
	emit_field   ( F_SPEC( tile31, NV_PGRAPH_PRI_GPCS_CRSTR_GPC_MAP5_TILE31, "f") );
	emit_field   ( F_SPEC( tile32, NV_PGRAPH_PRI_GPCS_CRSTR_GPC_MAP5_TILE32, "f") );
	emit_field   ( F_SPEC( tile33, NV_PGRAPH_PRI_GPCS_CRSTR_GPC_MAP5_TILE33, "f") );
	emit_field   ( F_SPEC( tile34, NV_PGRAPH_PRI_GPCS_CRSTR_GPC_MAP5_TILE34, "f") );
	emit_field   ( F_SPEC( tile35, NV_PGRAPH_PRI_GPCS_CRSTR_GPC_MAP5_TILE35, "f") );

	emit_register( R_SPEC( crstr_map_table_cfg, NV_PGRAPH_PRI_GPCS_CRSTR_MAP_TABLE_CONFIG ) );
	emit_field   ( F_SPEC( row_offset, NV_PGRAPH_PRI_GPCS_CRSTR_MAP_TABLE_CONFIG_ROW_OFFSET, "f") );
	emit_field   ( F_SPEC( num_entries, NV_PGRAPH_PRI_GPCS_CRSTR_MAP_TABLE_CONFIG_NUM_ENTRIES, "f") );

	emit_register( R_SPEC( gpcs_zcull_sm_in_gpc_number_map0, NV_PGRAPH_PRI_GPCS_ZCULL_SM_IN_GPC_NUMBER_MAP0 ) );
	emit_field   ( F_SPEC( tile_0, NV_PGRAPH_PRI_GPCS_ZCULL_SM_IN_GPC_NUMBER_MAP0_TILE_0, "f") );
	emit_field   ( F_SPEC( tile_1, NV_PGRAPH_PRI_GPCS_ZCULL_SM_IN_GPC_NUMBER_MAP0_TILE_1, "f") );
	emit_field   ( F_SPEC( tile_2, NV_PGRAPH_PRI_GPCS_ZCULL_SM_IN_GPC_NUMBER_MAP0_TILE_2, "f") );
	emit_field   ( F_SPEC( tile_3, NV_PGRAPH_PRI_GPCS_ZCULL_SM_IN_GPC_NUMBER_MAP0_TILE_3, "f") );
	emit_field   ( F_SPEC( tile_4, NV_PGRAPH_PRI_GPCS_ZCULL_SM_IN_GPC_NUMBER_MAP0_TILE_4, "f") );
	emit_field   ( F_SPEC( tile_5, NV_PGRAPH_PRI_GPCS_ZCULL_SM_IN_GPC_NUMBER_MAP0_TILE_5, "f") );
	emit_field   ( F_SPEC( tile_6, NV_PGRAPH_PRI_GPCS_ZCULL_SM_IN_GPC_NUMBER_MAP0_TILE_6, "f") );
	emit_field   ( F_SPEC( tile_7, NV_PGRAPH_PRI_GPCS_ZCULL_SM_IN_GPC_NUMBER_MAP0_TILE_7, "f") );

	emit_register( R_SPEC( gpcs_zcull_sm_in_gpc_number_map1, NV_PGRAPH_PRI_GPCS_ZCULL_SM_IN_GPC_NUMBER_MAP1 ) );
	emit_field   ( F_SPEC( tile_8, NV_PGRAPH_PRI_GPCS_ZCULL_SM_IN_GPC_NUMBER_MAP1_TILE_8, "f") );
	emit_field   ( F_SPEC( tile_9, NV_PGRAPH_PRI_GPCS_ZCULL_SM_IN_GPC_NUMBER_MAP1_TILE_9, "f") );
	emit_field   ( F_SPEC( tile_10, NV_PGRAPH_PRI_GPCS_ZCULL_SM_IN_GPC_NUMBER_MAP1_TILE_10, "f") );
	emit_field   ( F_SPEC( tile_11, NV_PGRAPH_PRI_GPCS_ZCULL_SM_IN_GPC_NUMBER_MAP1_TILE_11, "f") );
	emit_field   ( F_SPEC( tile_12, NV_PGRAPH_PRI_GPCS_ZCULL_SM_IN_GPC_NUMBER_MAP1_TILE_12, "f") );
	emit_field   ( F_SPEC( tile_13, NV_PGRAPH_PRI_GPCS_ZCULL_SM_IN_GPC_NUMBER_MAP1_TILE_13, "f") );
	emit_field   ( F_SPEC( tile_14, NV_PGRAPH_PRI_GPCS_ZCULL_SM_IN_GPC_NUMBER_MAP1_TILE_14, "f") );
	emit_field   ( F_SPEC( tile_15, NV_PGRAPH_PRI_GPCS_ZCULL_SM_IN_GPC_NUMBER_MAP1_TILE_15, "f") );

	emit_register( R_SPEC( gpcs_zcull_sm_in_gpc_number_map2, NV_PGRAPH_PRI_GPCS_ZCULL_SM_IN_GPC_NUMBER_MAP2 ) );
	emit_field   ( F_SPEC( tile_16, NV_PGRAPH_PRI_GPCS_ZCULL_SM_IN_GPC_NUMBER_MAP2_TILE_16, "f") );
	emit_field   ( F_SPEC( tile_17, NV_PGRAPH_PRI_GPCS_ZCULL_SM_IN_GPC_NUMBER_MAP2_TILE_17, "f") );
	emit_field   ( F_SPEC( tile_18, NV_PGRAPH_PRI_GPCS_ZCULL_SM_IN_GPC_NUMBER_MAP2_TILE_18, "f") );
	emit_field   ( F_SPEC( tile_19, NV_PGRAPH_PRI_GPCS_ZCULL_SM_IN_GPC_NUMBER_MAP2_TILE_19, "f") );
	emit_field   ( F_SPEC( tile_20, NV_PGRAPH_PRI_GPCS_ZCULL_SM_IN_GPC_NUMBER_MAP2_TILE_20, "f") );
	emit_field   ( F_SPEC( tile_21, NV_PGRAPH_PRI_GPCS_ZCULL_SM_IN_GPC_NUMBER_MAP2_TILE_21, "f") );
	emit_field   ( F_SPEC( tile_22, NV_PGRAPH_PRI_GPCS_ZCULL_SM_IN_GPC_NUMBER_MAP2_TILE_22, "f") );
	emit_field   ( F_SPEC( tile_23, NV_PGRAPH_PRI_GPCS_ZCULL_SM_IN_GPC_NUMBER_MAP2_TILE_23, "") );

	emit_register( R_SPEC( gpcs_zcull_sm_in_gpc_number_map3, NV_PGRAPH_PRI_GPCS_ZCULL_SM_IN_GPC_NUMBER_MAP3 ) );
	emit_field   ( F_SPEC( tile_24, NV_PGRAPH_PRI_GPCS_ZCULL_SM_IN_GPC_NUMBER_MAP3_TILE_24, "f") );
	emit_field   ( F_SPEC( tile_25, NV_PGRAPH_PRI_GPCS_ZCULL_SM_IN_GPC_NUMBER_MAP3_TILE_25, "f") );
	emit_field   ( F_SPEC( tile_26, NV_PGRAPH_PRI_GPCS_ZCULL_SM_IN_GPC_NUMBER_MAP3_TILE_26, "f") );
	emit_field   ( F_SPEC( tile_27, NV_PGRAPH_PRI_GPCS_ZCULL_SM_IN_GPC_NUMBER_MAP3_TILE_27, "f") );
	emit_field   ( F_SPEC( tile_28, NV_PGRAPH_PRI_GPCS_ZCULL_SM_IN_GPC_NUMBER_MAP3_TILE_28, "f") );
	emit_field   ( F_SPEC( tile_29, NV_PGRAPH_PRI_GPCS_ZCULL_SM_IN_GPC_NUMBER_MAP3_TILE_29, "f") );
	emit_field   ( F_SPEC( tile_30, NV_PGRAPH_PRI_GPCS_ZCULL_SM_IN_GPC_NUMBER_MAP3_TILE_30, "f") );
	emit_field   ( F_SPEC( tile_31, NV_PGRAPH_PRI_GPCS_ZCULL_SM_IN_GPC_NUMBER_MAP3_TILE_31, "f") );

	emit_register( R_SPEC( gpcs_gpm_pd_cfg, NV_PGRAPH_PRI_GPCS_GPM_PD_CONFIG ) );
	emit_field   ( F_SPEC( timeslice_mode, NV_PGRAPH_PRI_GPCS_GPM_PD_CONFIG_TIMESLICE_MODE, ".") );
	emit_constant( C_SPEC( disable, NV_PGRAPH_PRI_GPCS_GPM_PD_CONFIG_TIMESLICE_MODE_DISABLE, "f") );
	emit_constant( C_SPEC( enable, NV_PGRAPH_PRI_GPCS_GPM_PD_CONFIG_TIMESLICE_MODE_ENABLE, "f") );

	emit_register( R_SPEC( gpcs_gcc_pagepool_base, NV_PGRAPH_PRI_GPCS_GCC_RM_PAGEPOOL_BASE ) );
	emit_field   ( F_SPEC( addr_39_8, NV_PGRAPH_PRI_GPCS_GCC_RM_PAGEPOOL_BASE_ADDR_39_8, "f") );

	emit_register( R_SPEC( gpcs_gcc_pagepool, NV_PGRAPH_PRI_GPCS_GCC_RM_PAGEPOOL ) );
	emit_field   ( F_SPEC( total_pages, NV_PGRAPH_PRI_GPCS_GCC_RM_PAGEPOOL_TOTAL_PAGES, "f") );

	emit_register( R_SPEC( gpcs_tpcs_pe_vaf, NV_PGRAPH_PRI_GPCS_TPCS_PE_VAF ) );
	emit_field   ( F_SPEC( fast_mode_switch, NV_PGRAPH_PRI_GPCS_TPCS_PE_VAF_FAST_MODE_SWITCH, ".") );
	emit_constant( C_SPEC( true, NV_PGRAPH_PRI_GPCS_TPCS_PE_VAF_FAST_MODE_SWITCH_TRUE, "f") );

	emit_register( R_SPEC( gpcs_tpcs_pe_pin_cb_global_base_addr, NV_PGRAPH_PRI_GPCS_TPCS_PE_PIN_CB_GLOBAL_BASE_ADDR ) );
	emit_field   ( F_SPEC( v, NV_PGRAPH_PRI_GPCS_TPCS_PE_PIN_CB_GLOBAL_BASE_ADDR_V, "f") );
	emit_field   ( F_SPEC( valid, NV_PGRAPH_PRI_GPCS_TPCS_PE_PIN_CB_GLOBAL_BASE_ADDR_VALID, "f") );
	emit_constant( C_SPEC( true, NV_PGRAPH_PRI_GPCS_TPCS_PE_PIN_CB_GLOBAL_BASE_ADDR_VALID_TRUE, "f") );

	emit_register( R_SPEC( gpcs_tpcs_mpc_vtg_debug, NV_PGRAPH_PRI_GPCS_TPCS_MPC_VTG_DEBUG ) );
	emit_field   ( F_SPEC( timeslice_mode, NV_PGRAPH_PRI_GPCS_TPCS_MPC_VTG_DEBUG_TIMESLICE_MODE, ".") );
	emit_constant( C_SPEC( disabled, NV_PGRAPH_PRI_GPCS_TPCS_MPC_VTG_DEBUG_TIMESLICE_MODE_DISABLED, "f") );
	emit_constant( C_SPEC( enabled, NV_PGRAPH_PRI_GPCS_TPCS_MPC_VTG_DEBUG_TIMESLICE_MODE_ENABLED, "f") );

	emit_register( R_SPEC( gpcs_tpcs_sm_hww_warp_esr_report_mask, NV_PGRAPH_PRI_GPCS_TPCS_SM_HWW_WARP_ESR_REPORT_MASK ) );
	emit_field   ( F_SPEC( stack_error, NV_PGRAPH_PRI_GPCS_TPCS_SM_HWW_WARP_ESR_REPORT_MASK_STACK_ERROR, ".") );
	emit_constant( C_SPEC( report, NV_PGRAPH_PRI_GPCS_TPCS_SM_HWW_WARP_ESR_REPORT_MASK_STACK_ERROR_REPORT, "f") );
	emit_field   ( F_SPEC( api_stack_error, NV_PGRAPH_PRI_GPCS_TPCS_SM_HWW_WARP_ESR_REPORT_MASK_API_STACK_ERROR, ".") );
	emit_constant( C_SPEC( report, NV_PGRAPH_PRI_GPCS_TPCS_SM_HWW_WARP_ESR_REPORT_MASK_API_STACK_ERROR_REPORT, "f") );
	emit_field   ( F_SPEC( ret_empty_stack_error, NV_PGRAPH_PRI_GPCS_TPCS_SM_HWW_WARP_ESR_REPORT_MASK_RET_EMPTY_STACK_ERROR, ".") );
	emit_constant( C_SPEC( report, NV_PGRAPH_PRI_GPCS_TPCS_SM_HWW_WARP_ESR_REPORT_MASK_RET_EMPTY_STACK_ERROR_REPORT, "f") );
	emit_field   ( F_SPEC( pc_wrap, NV_PGRAPH_PRI_GPCS_TPCS_SM_HWW_WARP_ESR_REPORT_MASK_PC_WRAP, ".") );
	emit_constant( C_SPEC( report, NV_PGRAPH_PRI_GPCS_TPCS_SM_HWW_WARP_ESR_REPORT_MASK_PC_WRAP_REPORT, "f") );
	emit_field   ( F_SPEC( misaligned_pc, NV_PGRAPH_PRI_GPCS_TPCS_SM_HWW_WARP_ESR_REPORT_MASK_MISALIGNED_PC, ".") );
	emit_constant( C_SPEC( report, NV_PGRAPH_PRI_GPCS_TPCS_SM_HWW_WARP_ESR_REPORT_MASK_MISALIGNED_PC_REPORT, "f") );
	emit_field   ( F_SPEC( pc_overflow, NV_PGRAPH_PRI_GPCS_TPCS_SM_HWW_WARP_ESR_REPORT_MASK_PC_OVERFLOW, ".") );
	emit_constant( C_SPEC( report, NV_PGRAPH_PRI_GPCS_TPCS_SM_HWW_WARP_ESR_REPORT_MASK_PC_OVERFLOW_REPORT, "f") );
	emit_field   ( F_SPEC( misaligned_immc_addr, NV_PGRAPH_PRI_GPCS_TPCS_SM_HWW_WARP_ESR_REPORT_MASK_MISALIGNED_IMMC_ADDR, ".") );
	emit_constant( C_SPEC( report, NV_PGRAPH_PRI_GPCS_TPCS_SM_HWW_WARP_ESR_REPORT_MASK_MISALIGNED_IMMC_ADDR_REPORT, "f") );
	emit_field   ( F_SPEC( misaligned_reg, NV_PGRAPH_PRI_GPCS_TPCS_SM_HWW_WARP_ESR_REPORT_MASK_MISALIGNED_REG, ".") );
	emit_constant( C_SPEC( report, NV_PGRAPH_PRI_GPCS_TPCS_SM_HWW_WARP_ESR_REPORT_MASK_MISALIGNED_REG_REPORT, "f") );
	emit_field   ( F_SPEC( illegal_instr_encoding, NV_PGRAPH_PRI_GPCS_TPCS_SM_HWW_WARP_ESR_REPORT_MASK_ILLEGAL_INSTR_ENCODING, ".") );
	emit_constant( C_SPEC( report, NV_PGRAPH_PRI_GPCS_TPCS_SM_HWW_WARP_ESR_REPORT_MASK_ILLEGAL_INSTR_ENCODING_REPORT, "f") );
	emit_field   ( F_SPEC( illegal_sph_instr_combo, NV_PGRAPH_PRI_GPCS_TPCS_SM_HWW_WARP_ESR_REPORT_MASK_ILLEGAL_SPH_INSTR_COMBO, ".") );
	emit_constant( C_SPEC( report, NV_PGRAPH_PRI_GPCS_TPCS_SM_HWW_WARP_ESR_REPORT_MASK_ILLEGAL_SPH_INSTR_COMBO_REPORT, "f") );
	emit_field   ( F_SPEC( illegal_instr_param, NV_PGRAPH_PRI_GPCS_TPCS_SM_HWW_WARP_ESR_REPORT_MASK_ILLEGAL_INSTR_PARAM, ".") );
	emit_constant( C_SPEC( report, NV_PGRAPH_PRI_GPCS_TPCS_SM_HWW_WARP_ESR_REPORT_MASK_ILLEGAL_INSTR_PARAM_REPORT, "f") );
	emit_field   ( F_SPEC( invalid_const_addr, NV_PGRAPH_PRI_GPCS_TPCS_SM_HWW_WARP_ESR_REPORT_MASK_INVALID_CONST_ADDR, ".") );
	emit_constant( C_SPEC( report, NV_PGRAPH_PRI_GPCS_TPCS_SM_HWW_WARP_ESR_REPORT_MASK_INVALID_CONST_ADDR_REPORT, "f") );
	emit_field   ( F_SPEC( oor_reg, NV_PGRAPH_PRI_GPCS_TPCS_SM_HWW_WARP_ESR_REPORT_MASK_OOR_REG, ".") );
	emit_constant( C_SPEC( report, NV_PGRAPH_PRI_GPCS_TPCS_SM_HWW_WARP_ESR_REPORT_MASK_OOR_REG_REPORT, "f") );
	emit_field   ( F_SPEC( oor_addr, NV_PGRAPH_PRI_GPCS_TPCS_SM_HWW_WARP_ESR_REPORT_MASK_OOR_ADDR, ".") );
	emit_constant( C_SPEC( report, NV_PGRAPH_PRI_GPCS_TPCS_SM_HWW_WARP_ESR_REPORT_MASK_OOR_ADDR_REPORT, "f") );
	emit_field   ( F_SPEC( misaligned_addr, NV_PGRAPH_PRI_GPCS_TPCS_SM_HWW_WARP_ESR_REPORT_MASK_MISALIGNED_ADDR, ".") );
	emit_constant( C_SPEC( report, NV_PGRAPH_PRI_GPCS_TPCS_SM_HWW_WARP_ESR_REPORT_MASK_MISALIGNED_ADDR_REPORT, "f") );
	emit_field   ( F_SPEC( invalid_addr_space, NV_PGRAPH_PRI_GPCS_TPCS_SM_HWW_WARP_ESR_REPORT_MASK_INVALID_ADDR_SPACE, ".") );
	emit_constant( C_SPEC( report, NV_PGRAPH_PRI_GPCS_TPCS_SM_HWW_WARP_ESR_REPORT_MASK_INVALID_ADDR_SPACE_REPORT, "f") );
	emit_field   ( F_SPEC( illegal_instr_param2, NV_PGRAPH_PRI_GPCS_TPCS_SM_HWW_WARP_ESR_REPORT_MASK_ILLEGAL_INSTR_PARAM2, ".") );
	emit_constant( C_SPEC( report, NV_PGRAPH_PRI_GPCS_TPCS_SM_HWW_WARP_ESR_REPORT_MASK_ILLEGAL_INSTR_PARAM2_REPORT, "f") );
	emit_field   ( F_SPEC( invalid_const_addr_ldc, NV_PGRAPH_PRI_GPCS_TPCS_SM_HWW_WARP_ESR_REPORT_MASK_INVALID_CONST_ADDR_LDC, ".") );
	emit_constant( C_SPEC( report, NV_PGRAPH_PRI_GPCS_TPCS_SM_HWW_WARP_ESR_REPORT_MASK_INVALID_CONST_ADDR_LDC_REPORT, "f") );
	emit_field   ( F_SPEC( geometry_sm_error, NV_PGRAPH_PRI_GPCS_TPCS_SM_HWW_WARP_ESR_REPORT_MASK_GEOMETRY_SM_ERROR, ".") );
	emit_constant( C_SPEC( report, NV_PGRAPH_PRI_GPCS_TPCS_SM_HWW_WARP_ESR_REPORT_MASK_GEOMETRY_SM_ERROR_REPORT, "f") );
	emit_field   ( F_SPEC( divergent, NV_PGRAPH_PRI_GPCS_TPCS_SM_HWW_WARP_ESR_REPORT_MASK_DIVERGENT, ".") );
	emit_constant( C_SPEC( report, NV_PGRAPH_PRI_GPCS_TPCS_SM_HWW_WARP_ESR_REPORT_MASK_DIVERGENT_REPORT, "f") );

	emit_register( R_SPEC( gpcs_tpcs_sm_hww_global_esr_report_mask, NV_PGRAPH_PRI_GPCS_TPCS_SM_HWW_GLOBAL_ESR_REPORT_MASK ) );
	emit_field   ( F_SPEC( sm_to_sm_fault, NV_PGRAPH_PRI_GPCS_TPCS_SM_HWW_GLOBAL_ESR_REPORT_MASK_SM_TO_SM_FAULT, ".") );
	emit_constant( C_SPEC( report, NV_PGRAPH_PRI_GPCS_TPCS_SM_HWW_GLOBAL_ESR_REPORT_MASK_SM_TO_SM_FAULT_REPORT, "f") );
	emit_field   ( F_SPEC( l1_error, NV_PGRAPH_PRI_GPCS_TPCS_SM_HWW_GLOBAL_ESR_REPORT_MASK_L1_ERROR, ".") );
	emit_constant( C_SPEC( report, NV_PGRAPH_PRI_GPCS_TPCS_SM_HWW_GLOBAL_ESR_REPORT_MASK_L1_ERROR_REPORT, "f") );
	emit_field   ( F_SPEC( multiple_warp_errors, NV_PGRAPH_PRI_GPCS_TPCS_SM_HWW_GLOBAL_ESR_REPORT_MASK_MULTIPLE_WARP_ERRORS, ".") );
	emit_constant( C_SPEC( report, NV_PGRAPH_PRI_GPCS_TPCS_SM_HWW_GLOBAL_ESR_REPORT_MASK_MULTIPLE_WARP_ERRORS_REPORT, "f") );
	emit_field   ( F_SPEC( physical_stack_overflow_error, NV_PGRAPH_PRI_GPCS_TPCS_SM_HWW_GLOBAL_ESR_REPORT_MASK_PHYSICAL_STACK_OVERFLOW_ERROR, ".") );
	emit_constant( C_SPEC( report, NV_PGRAPH_PRI_GPCS_TPCS_SM_HWW_GLOBAL_ESR_REPORT_MASK_PHYSICAL_STACK_OVERFLOW_ERROR_REPORT, "f") );
	emit_field   ( F_SPEC( bpt_int, NV_PGRAPH_PRI_GPCS_TPCS_SM_HWW_GLOBAL_ESR_REPORT_MASK_BPT_INT, ".") );
	emit_constant( C_SPEC( report, NV_PGRAPH_PRI_GPCS_TPCS_SM_HWW_GLOBAL_ESR_REPORT_MASK_BPT_INT_REPORT, "f") );
	emit_field   ( F_SPEC( bpt_pause, NV_PGRAPH_PRI_GPCS_TPCS_SM_HWW_GLOBAL_ESR_REPORT_MASK_BPT_PAUSE, ".") );
	emit_constant( C_SPEC( report, NV_PGRAPH_PRI_GPCS_TPCS_SM_HWW_GLOBAL_ESR_REPORT_MASK_BPT_PAUSE_REPORT, "f") );
	emit_field   ( F_SPEC( single_step_complete, NV_PGRAPH_PRI_GPCS_TPCS_SM_HWW_GLOBAL_ESR_REPORT_MASK_SINGLE_STEP_COMPLETE, ".") );
	emit_constant( C_SPEC( report, NV_PGRAPH_PRI_GPCS_TPCS_SM_HWW_GLOBAL_ESR_REPORT_MASK_SINGLE_STEP_COMPLETE_REPORT, "f") );

	emit_register( R_SPEC( gpcs_tpcs_tpccs_tpc_exception_en, NV_PGRAPH_PRI_GPCS_TPCS_TPCCS_TPC_EXCEPTION_EN ) );
	emit_field   ( F_SPEC( sm, NV_PGRAPH_PRI_GPC0_TPC0_TPCCS_TPC_EXCEPTION_EN_SM, "." ) );
	emit_constant( C_SPEC( enabled, NV_PGRAPH_PRI_GPC0_TPC0_TPCCS_TPC_EXCEPTION_EN_SM_ENABLED, "f" ) );

	emit_register( R_SPEC( gpc0_tpc0_tpccs_tpc_exception_en, NV_PGRAPH_PRI_GPC0_TPC0_TPCCS_TPC_EXCEPTION_EN ) );

	emit_field   ( F_SPEC( sm, NV_PGRAPH_PRI_GPC0_TPC0_TPCCS_TPC_EXCEPTION_EN_SM, "v" ) );
	emit_constant( C_SPEC( enabled, NV_PGRAPH_PRI_GPC0_TPC0_TPCCS_TPC_EXCEPTION_EN_SM_ENABLED, "f" ) );

	emit_register( R_SPEC( gpcs_gpccs_gpc_exception_en, NV_PGRAPH_PRI_GPCS_GPCCS_GPC_EXCEPTION_EN ) );
	emit_field   ( F_SPEC( tpc, NV_PGRAPH_PRI_GPCS_GPCCS_GPC_EXCEPTION_EN_TPC, "f" ) );

	emit_register( R_SPEC( gpc0_gpccs_gpc_exception, NV_PGRAPH_PRI_GPC0_GPCCS_GPC_EXCEPTION ) );
	emit_field   ( F_SPEC( tpc, NV_PGRAPH_PRI_GPC0_GPCCS_GPC_EXCEPTION_TPC, "v" ) );
	emit_constant( C_SPEC( 0_pending, NV_PGRAPH_PRI_GPC0_GPCCS_GPC_EXCEPTION_TPC_0_PENDING, "v" ) );

	emit_register( R_SPEC( gpc0_tpc0_tpccs_tpc_exception, NV_PGRAPH_PRI_GPC0_TPC0_TPCCS_TPC_EXCEPTION ) );
	emit_field   ( F_SPEC( sm, NV_PGRAPH_PRI_GPC0_TPC0_TPCCS_TPC_EXCEPTION_SM, "v" ) );
	emit_constant( C_SPEC( pending, NV_PGRAPH_PRI_GPC0_TPC0_TPCCS_TPC_EXCEPTION_SM_PENDING, "v" ) );

	emit_register( R_SPEC( gpc0_tpc0_sm_dbgr_control0, NV_PGRAPH_PRI_GPC0_TPC0_SM_DBGR_CONTROL0 ) );
	emit_field   ( F_SPEC( debugger_mode, NV_PGRAPH_PRI_GPC0_TPC0_SM_DBGR_CONTROL0_DEBUGGER_MODE, "v" ) );
	emit_constant( C_SPEC( on, NV_PGRAPH_PRI_GPC0_TPC0_SM_DBGR_CONTROL0_DEBUGGER_MODE_ON, "v" ) );
	emit_constant( C_SPEC( off, NV_PGRAPH_PRI_GPC0_TPC0_SM_DBGR_CONTROL0_DEBUGGER_MODE_OFF, "v" ) );
	emit_field   ( F_SPEC( stop_trigger, NV_PGRAPH_PRI_GPC0_TPC0_SM_DBGR_CONTROL0_STOP_TRIGGER, "." ) );
	emit_constant( C_SPEC( enable, NV_PGRAPH_PRI_GPC0_TPC0_SM_DBGR_CONTROL0_STOP_TRIGGER_ENABLE, "f" ) );
	emit_constant( C_SPEC( disable, NV_PGRAPH_PRI_GPC0_TPC0_SM_DBGR_CONTROL0_STOP_TRIGGER_DISABLE, "f" ) );
	emit_field   ( F_SPEC( run_trigger, NV_PGRAPH_PRI_GPC0_TPC0_SM_DBGR_CONTROL0_RUN_TRIGGER, "." ) );
	emit_constant( C_SPEC( task, NV_PGRAPH_PRI_GPC0_TPC0_SM_DBGR_CONTROL0_RUN_TRIGGER_TASK, "f" ) );

	emit_field   ( F_SPEC( stop_on_any_warp, NV_PGRAPH_PRI_GPC0_TPC0_SM_DBGR_CONTROL0_STOP_ON_ANY_WARP, "v" ) );
	emit_constant( C_SPEC( disable, NV_PGRAPH_PRI_GPC0_TPC0_SM_DBGR_CONTROL0_STOP_ON_ANY_WARP_DISABLE, "f" ) );

	emit_field   ( F_SPEC( stop_on_any_sm, NV_PGRAPH_PRI_GPC0_TPC0_SM_DBGR_CONTROL0_STOP_ON_ANY_SM, "v" ) );
	emit_constant( C_SPEC( disable, NV_PGRAPH_PRI_GPC0_TPC0_SM_DBGR_CONTROL0_STOP_ON_ANY_SM_DISABLE, "f" ) );

	emit_register( R_SPEC( gpc0_tpc0_sm_warp_valid_mask, NV_PGRAPH_PRI_GPC0_TPC0_SM_DBGR_WARP_VALID_MASK_0 ) );
	emit_register( R_SPEC( gpc0_tpc0_sm_dbgr_bpt_pause_mask, NV_PGRAPH_PRI_GPC0_TPC0_SM_DBGR_BPT_PAUSE_MASK_0 ) );
	emit_register( R_SPEC( gpc0_tpc0_sm_dbgr_bpt_trap_mask, NV_PGRAPH_PRI_GPC0_TPC0_SM_DBGR_BPT_TRAP_MASK_0 ) );

	emit_constant( C_SPEC( stop_on_any_warp_disable, NV_PGRAPH_PRI_GPC0_TPC0_SM_DBGR_CONTROL0_STOP_ON_ANY_WARP_DISABLE, "v" ) );
	emit_constant( C_SPEC( stop_on_any_sm_disable, NV_PGRAPH_PRI_GPC0_TPC0_SM_DBGR_CONTROL0_STOP_ON_ANY_SM_DISABLE, "v" ) );


	emit_register( R_SPEC( gpc0_tpc0_sm_dbgr_status0, NV_PGRAPH_PRI_GPC0_TPC0_SM_DBGR_STATUS0 ) );
	emit_field   ( F_SPEC( sm_in_trap_mode, NV_PGRAPH_PRI_GPC0_TPC0_SM_DBGR_STATUS0_SM_IN_TRAP_MODE, "v" ) );
	emit_field   ( F_SPEC( locked_down, NV_PGRAPH_PRI_GPC0_TPC0_SM_DBGR_STATUS0_LOCKED_DOWN, "v" ) );
	emit_constant( C_SPEC( true, NV_PGRAPH_PRI_GPC0_TPC0_SM_DBGR_STATUS0_LOCKED_DOWN_TRUE, "v" ) );

	emit_register( R_SPEC( gpcs_tpcs_sm_hww_global_esr, NV_PGRAPH_PRI_GPCS_TPCS_SM_HWW_GLOBAL_ESR ) );
	emit_field   ( F_SPEC( bpt_int, NV_PGRAPH_PRI_GPCS_TPCS_SM_HWW_GLOBAL_ESR_BPT_INT, ".") );
	emit_constant( C_SPEC( pending, NV_PGRAPH_PRI_GPCS_TPCS_SM_HWW_GLOBAL_ESR_BPT_INT_PENDING, "f") );
	emit_field   ( F_SPEC( bpt_pause, NV_PGRAPH_PRI_GPCS_TPCS_SM_HWW_GLOBAL_ESR_BPT_PAUSE, ".") );
	emit_constant( C_SPEC( pending, NV_PGRAPH_PRI_GPCS_TPCS_SM_HWW_GLOBAL_ESR_BPT_PAUSE_PENDING, "f") );
	emit_field   ( F_SPEC( single_step_complete, NV_PGRAPH_PRI_GPCS_TPCS_SM_HWW_GLOBAL_ESR_SINGLE_STEP_COMPLETE, ".") );
	emit_constant( C_SPEC( pending, NV_PGRAPH_PRI_GPCS_TPCS_SM_HWW_GLOBAL_ESR_SINGLE_STEP_COMPLETE_PENDING, "f") );

	emit_register( R_SPEC( gpc0_tpc0_sm_hww_global_esr, NV_PGRAPH_PRI_GPC0_TPC0_SM_HWW_GLOBAL_ESR ) );
	emit_field   ( F_SPEC( bpt_int, NV_PGRAPH_PRI_GPC0_TPC0_SM_HWW_GLOBAL_ESR_BPT_INT, ".") );
	emit_constant( C_SPEC( pending, NV_PGRAPH_PRI_GPC0_TPC0_SM_HWW_GLOBAL_ESR_BPT_INT_PENDING, "f") );
	emit_field   ( F_SPEC( bpt_pause, NV_PGRAPH_PRI_GPC0_TPC0_SM_HWW_GLOBAL_ESR_BPT_PAUSE, ".") );
	emit_constant( C_SPEC( pending, NV_PGRAPH_PRI_GPC0_TPC0_SM_HWW_GLOBAL_ESR_BPT_PAUSE_PENDING, "f") );
	emit_field   ( F_SPEC( single_step_complete, NV_PGRAPH_PRI_GPC0_TPC0_SM_HWW_GLOBAL_ESR_SINGLE_STEP_COMPLETE, ".") );
	emit_constant( C_SPEC( pending, NV_PGRAPH_PRI_GPC0_TPC0_SM_HWW_GLOBAL_ESR_SINGLE_STEP_COMPLETE_PENDING, "f") );

	emit_register( R_SPEC( gpc0_tpc0_sm_hww_warp_esr, NV_PGRAPH_PRI_GPC0_TPC0_SM_HWW_WARP_ESR ) );
	emit_field   ( F_SPEC( error, NV_PGRAPH_PRI_GPC0_TPC0_SM_HWW_WARP_ESR_ERROR, "v") );
	emit_constant( C_SPEC( none, NV_PGRAPH_PRI_GPC0_TPC0_SM_HWW_WARP_ESR_ERROR_NONE, "v") );
	emit_constant( C_SPEC( none, NV_PGRAPH_PRI_GPC0_TPC0_SM_HWW_WARP_ESR_ERROR_NONE, "f") );

	emit_register( R_SPEC( gpc0_tpc0_sm_halfctl_ctrl, NV_PGRAPH_PRI_GPC0_TPC0_SM_HALFCTL_CTRL ) );
	emit_register( R_SPEC( gpcs_tpcs_sm_halfctl_ctrl, NV_PGRAPH_PRI_GPCS_TPCS_SM_HALFCTL_CTRL ) );
	emit_field   ( F_SPEC( sctl_read_quad_ctl, NV_PGRAPH_PRI_GPCS_TPCS_SM_HALFCTL_CTRL_SCTL_READ_QUAD_CTL, "m") );
	emit_field   ( F_SPEC( sctl_read_quad_ctl, NV_PGRAPH_PRI_GPCS_TPCS_SM_HALFCTL_CTRL_SCTL_READ_QUAD_CTL, "f") );

	emit_register( R_SPEC( gpc0_tpc0_sm_debug_sfe_control, NV_PGRAPH_PRI_GPC0_TPC0_SM_DEBUG_SFE_CONTROL ) );
	emit_register( R_SPEC( gpcs_tpcs_sm_debug_sfe_control, NV_PGRAPH_PRI_GPCS_TPCS_SM_DEBUG_SFE_CONTROL ) );
	emit_field   ( F_SPEC( read_half_ctl, NV_PGRAPH_PRI_GPCS_TPCS_SM_DEBUG_SFE_CONTROL_READ_HALF_CTL, "m") );
	emit_field   ( F_SPEC( read_half_ctl, NV_PGRAPH_PRI_GPCS_TPCS_SM_DEBUG_SFE_CONTROL_READ_HALF_CTL, "f") );

	emit_register( R_SPEC( gpcs_tpcs_pes_vsc_vpc, NV_PGRAPH_PRI_GPCS_PPCS_PES_VSC_VPC ) );
	emit_field   ( F_SPEC( fast_mode_switch, NV_PGRAPH_PRI_GPCS_PPCS_PES_VSC_VPC_FAST_MODE_SWITCH, ".") );
	emit_constant( C_SPEC( true, NV_PGRAPH_PRI_GPCS_PPCS_PES_VSC_VPC_FAST_MODE_SWITCH_TRUE, "f") );

	emit_register( R_SPEC( ppcs_wwdx_map_gpc_map0, NV_PGRAPH_PRI_GPCS_PPCS_WWDX_GPC_MAP0 ) );

	emit_register( R_SPEC( ppcs_wwdx_map_gpc_map1, NV_PGRAPH_PRI_GPCS_PPCS_WWDX_GPC_MAP1 ) );

	emit_register( R_SPEC( ppcs_wwdx_map_gpc_map2, NV_PGRAPH_PRI_GPCS_PPCS_WWDX_GPC_MAP2 ) );

	emit_register( R_SPEC( ppcs_wwdx_map_gpc_map3, NV_PGRAPH_PRI_GPCS_PPCS_WWDX_GPC_MAP3 ) );

	emit_register( R_SPEC( ppcs_wwdx_map_gpc_map4, NV_PGRAPH_PRI_GPCS_PPCS_WWDX_GPC_MAP4 ) );

	emit_register( R_SPEC( ppcs_wwdx_map_gpc_map5, NV_PGRAPH_PRI_GPCS_PPCS_WWDX_GPC_MAP5 ) );

	emit_register( R_SPEC( ppcs_wwdx_map_table_cfg, NV_PGRAPH_PRI_GPCS_PPCS_WWDX_MAP_TABLE_CONFIG ) );
	emit_field   ( F_SPEC( row_offset, NV_PGRAPH_PRI_GPCS_PPCS_WWDX_MAP_TABLE_CONFIG_ROW_OFFSET, "f") );
	emit_field   ( F_SPEC( num_entries, NV_PGRAPH_PRI_GPCS_PPCS_WWDX_MAP_TABLE_CONFIG_NUM_ENTRIES, "f") );
	emit_field   ( F_SPEC( normalized_num_entries, NV_PGRAPH_PRI_GPCS_PPCS_WWDX_MAP_TABLE_CONFIG_NORMALIZED_NUM_ENTRIES, "f") );
	emit_field   ( F_SPEC( normalized_shift_value, NV_PGRAPH_PRI_GPCS_PPCS_WWDX_MAP_TABLE_CONFIG_NORMALIZED_SHIFT_VALUE, "f") );
	emit_field   ( F_SPEC( coeff5_mod_value, NV_PGRAPH_PRI_GPCS_PPCS_WWDX_MAP_TABLE_CONFIG_COEFF5_MOD_VALUE, "f") );

	emit_register( R_SPEC( gpcs_ppcs_wwdx_sm_num_rcp, NV_PGRAPH_PRI_GPCS_PPCS_WWDX_SM_NUM_RCP ) );
	emit_field   ( F_SPEC( conservative, NV_PGRAPH_PRI_GPCS_PPCS_WWDX_SM_NUM_RCP_CONSERVATIVE, "f") );

	emit_register( R_SPEC( ppcs_wwdx_map_table_cfg2, NV_PGRAPH_PRI_GPCS_PPCS_WWDX_MAP_TABLE_CONFIG2 ) );
	emit_field   ( F_SPEC( coeff6_mod_value, NV_PGRAPH_PRI_GPCS_PPCS_WWDX_MAP_TABLE_CONFIG2_COEFF6_MOD_VALUE, "f") );
	emit_field   ( F_SPEC( coeff7_mod_value, NV_PGRAPH_PRI_GPCS_PPCS_WWDX_MAP_TABLE_CONFIG2_COEFF7_MOD_VALUE, "f") );
	emit_field   ( F_SPEC( coeff8_mod_value, NV_PGRAPH_PRI_GPCS_PPCS_WWDX_MAP_TABLE_CONFIG2_COEFF8_MOD_VALUE, "f") );
	emit_field   ( F_SPEC( coeff9_mod_value, NV_PGRAPH_PRI_GPCS_PPCS_WWDX_MAP_TABLE_CONFIG2_COEFF9_MOD_VALUE, "f") );
	emit_field   ( F_SPEC( coeff10_mod_value, NV_PGRAPH_PRI_GPCS_PPCS_WWDX_MAP_TABLE_CONFIG2_COEFF10_MOD_VALUE, "f") );
	emit_field   ( F_SPEC( coeff11_mod_value, NV_PGRAPH_PRI_GPCS_PPCS_WWDX_MAP_TABLE_CONFIG2_COEFF11_MOD_VALUE, "f") );

	emit_register( R_SPEC( gpcs_ppcs_cbm_cfg, NV_PGRAPH_PRI_GPCS_PPCS_CBM_CONFIG ) );
	emit_field   ( F_SPEC( timeslice_mode, NV_PGRAPH_PRI_GPCS_PPCS_CBM_CONFIG_TIMESLICE_MODE, ".") );
	emit_constant( C_SPEC( enable, NV_PGRAPH_PRI_GPCS_PPCS_CBM_CONFIG_TIMESLICE_MODE_ENABLE, "v") );

	emit_register( R_SPEC( bes_zrop_settings, NV_PGRAPH_PRI_BES_ZROP_SETTINGS ) );
	emit_field   ( F_SPEC( num_active_fbps, NV_PGRAPH_PRI_BES_ZROP_SETTINGS_NUM_ACTIVE_FBPS, "f") );

	emit_register( R_SPEC( bes_crop_settings, NV_PGRAPH_PRI_BES_CROP_SETTINGS ) );
	emit_field   ( F_SPEC( num_active_fbps, NV_PGRAPH_PRI_BES_CROP_SETTINGS_NUM_ACTIVE_FBPS, "f") );
	end_scope();

	emit_constant( C_SPEC( zcull_bytes_per_aliquot_per_gpu, NV_PGRAPH_ZCULL_BYTES_PER_ALIQUOT_PER_GPC, "v") );

	emit_constant( C_SPEC( zcull_save_restore_header_bytes_per_gpc, NV_PGRAPH_ZCULL_SAVE_RESTORE_HEADER_BYTES_PER_GPC, "v") );

	emit_constant( C_SPEC( zcull_save_restore_subregion_header_bytes_per_gpc, NV_PGRAPH_ZCULL_SAVE_RESTORE_SUBREGION_HEADER_BYTES_PER_GPC, "v") );

	emit_constant( C_SPEC( zcull_subregion_qty, NV_PGRAPH_ZCULL_SUBREGION_QTY, "v") );


	emit_register( R_SPEC( pri_gpc0_tpc0_sm_dsm_perf_counter_control_sel0, NV_PGRAPH_PRI_GPC0_TPC0_SM_DSM_PERF_COUNTER_CONTROL_SEL0) );
	emit_register( R_SPEC( pri_gpc0_tpc0_sm_dsm_perf_counter_control_sel1, NV_PGRAPH_PRI_GPC0_TPC0_SM_DSM_PERF_COUNTER_CONTROL_SEL1) );

	emit_register( R_SPEC( pri_gpc0_tpc0_sm_dsm_perf_counter_control0, NV_PGRAPH_PRI_GPC0_TPC0_SM_DSM_PERF_COUNTER_CONTROL0) );
	emit_register( R_SPEC( pri_gpc0_tpc0_sm_dsm_perf_counter_control1, NV_PGRAPH_PRI_GPC0_TPC0_SM_DSM_PERF_COUNTER_CONTROL1) );
	emit_register( R_SPEC( pri_gpc0_tpc0_sm_dsm_perf_counter_control2, NV_PGRAPH_PRI_GPC0_TPC0_SM_DSM_PERF_COUNTER_CONTROL2) );
	emit_register( R_SPEC( pri_gpc0_tpc0_sm_dsm_perf_counter_control3, NV_PGRAPH_PRI_GPC0_TPC0_SM_DSM_PERF_COUNTER_CONTROL3) );
	emit_register( R_SPEC( pri_gpc0_tpc0_sm_dsm_perf_counter_control4, NV_PGRAPH_PRI_GPC0_TPC0_SM_DSM_PERF_COUNTER_CONTROL4) );
	emit_register( R_SPEC( pri_gpc0_tpc0_sm_dsm_perf_counter_control5, NV_PGRAPH_PRI_GPC0_TPC0_SM_DSM_PERF_COUNTER_CONTROL5) );

	emit_register( R_SPEC( pri_gpc0_tpc0_sm_dsm_perf_counter_status,   NV_PGRAPH_PRI_GPC0_TPC0_SM_DSM_PERF_COUNTER_STATUS) );
	emit_register( R_SPEC( pri_gpc0_tpc0_sm_dsm_perf_counter_status1,  NV_PGRAPH_PRI_GPC0_TPC0_SM_DSM_PERF_COUNTER_STATUS1) );

	emit_register( R_SPEC( pri_gpc0_tpc0_sm_dsm_perf_counter0_control, NV_PGRAPH_PRI_GPC0_TPC0_SM_DSM_PERF_COUNTER0_CONTROL) );
	emit_register( R_SPEC( pri_gpc0_tpc0_sm_dsm_perf_counter1_control, NV_PGRAPH_PRI_GPC0_TPC0_SM_DSM_PERF_COUNTER1_CONTROL) );
	emit_register( R_SPEC( pri_gpc0_tpc0_sm_dsm_perf_counter2_control, NV_PGRAPH_PRI_GPC0_TPC0_SM_DSM_PERF_COUNTER2_CONTROL) );
	emit_register( R_SPEC( pri_gpc0_tpc0_sm_dsm_perf_counter3_control, NV_PGRAPH_PRI_GPC0_TPC0_SM_DSM_PERF_COUNTER3_CONTROL) );
	emit_register( R_SPEC( pri_gpc0_tpc0_sm_dsm_perf_counter4_control, NV_PGRAPH_PRI_GPC0_TPC0_SM_DSM_PERF_COUNTER4_CONTROL) );
	emit_register( R_SPEC( pri_gpc0_tpc0_sm_dsm_perf_counter5_control, NV_PGRAPH_PRI_GPC0_TPC0_SM_DSM_PERF_COUNTER5_CONTROL) );
	emit_register( R_SPEC( pri_gpc0_tpc0_sm_dsm_perf_counter6_control, NV_PGRAPH_PRI_GPC0_TPC0_SM_DSM_PERF_COUNTER6_CONTROL) );
	emit_register( R_SPEC( pri_gpc0_tpc0_sm_dsm_perf_counter7_control, NV_PGRAPH_PRI_GPC0_TPC0_SM_DSM_PERF_COUNTER7_CONTROL) );

	emit_register( R_SPEC( pri_gpc0_tpc0_sm_dsm_perf_counter0, NV_PGRAPH_PRI_GPC0_TPC0_SM_DSM_PERF_COUNTER0) );
	emit_register( R_SPEC( pri_gpc0_tpc0_sm_dsm_perf_counter1, NV_PGRAPH_PRI_GPC0_TPC0_SM_DSM_PERF_COUNTER1) );
	emit_register( R_SPEC( pri_gpc0_tpc0_sm_dsm_perf_counter2, NV_PGRAPH_PRI_GPC0_TPC0_SM_DSM_PERF_COUNTER2) );
	emit_register( R_SPEC( pri_gpc0_tpc0_sm_dsm_perf_counter3, NV_PGRAPH_PRI_GPC0_TPC0_SM_DSM_PERF_COUNTER3) );
	emit_register( R_SPEC( pri_gpc0_tpc0_sm_dsm_perf_counter4, NV_PGRAPH_PRI_GPC0_TPC0_SM_DSM_PERF_COUNTER4) );
	emit_register( R_SPEC( pri_gpc0_tpc0_sm_dsm_perf_counter5, NV_PGRAPH_PRI_GPC0_TPC0_SM_DSM_PERF_COUNTER5) );
	emit_register( R_SPEC( pri_gpc0_tpc0_sm_dsm_perf_counter6, NV_PGRAPH_PRI_GPC0_TPC0_SM_DSM_PERF_COUNTER6) );
	emit_register( R_SPEC( pri_gpc0_tpc0_sm_dsm_perf_counter7, NV_PGRAPH_PRI_GPC0_TPC0_SM_DSM_PERF_COUNTER7) );

	emit_register( R_SPEC( fe_pwr_mode, NV_PGRAPH_PRI_FE_PWR_MODE ) );
	emit_field   ( F_SPEC( mode,     NV_PGRAPH_PRI_FE_PWR_MODE_MODE, "." ) );
	emit_constant( C_SPEC( auto,     NV_PGRAPH_PRI_FE_PWR_MODE_MODE_AUTO, "f" ) );
	emit_constant( C_SPEC( force_on, NV_PGRAPH_PRI_FE_PWR_MODE_MODE_FORCE_ON, "f" ) );
	emit_field   ( F_SPEC( req,  NV_PGRAPH_PRI_FE_PWR_MODE_REQ, "v" ) );
	emit_constant( C_SPEC( send, NV_PGRAPH_PRI_FE_PWR_MODE_REQ_SEND, "f" ) );
	emit_constant( C_SPEC( done, NV_PGRAPH_PRI_FE_PWR_MODE_REQ_DONE, "v" ) );


	emit_register( R_SPEC( gpc0_tpc0_l1c_dbg, NV_PGRAPH_PRI_GPC0_TPC0_L1C_DBG) );
	emit_field   ( F_SPEC( cya15, NV_PGRAPH_PRI_GPC0_TPC0_L1C_DBG_CYA15, "." ) );
	emit_constant( C_SPEC( en,   NV_PGRAPH_PRI_GPC0_TPC0_L1C_DBG_CYA15_EN, "f" ) );

	emit_register( R_SPEC( gpcs_tpcs_sm_sch_texlock, NV_PGRAPH_PRI_GPCS_TPCS_SM_SCH_TEXLOCK));
	emit_field   ( F_SPEC( tex_hash, NV_PGRAPH_PRI_GPCS_TPCS_SM_SCH_TEXLOCK_TEX_HASH, "m") );
	emit_constant( C_SPEC( disable, NV_PGRAPH_PRI_GPCS_TPCS_SM_SCH_TEXLOCK_TEX_HASH_DISABLE, "f") );
	emit_field   ( F_SPEC( tex_hash_tile, NV_PGRAPH_PRI_GPCS_TPCS_SM_SCH_TEXLOCK_TEX_HASH_TILE, "m") );
	emit_constant( C_SPEC( disable, NV_PGRAPH_PRI_GPCS_TPCS_SM_SCH_TEXLOCK_TEX_HASH_TILE_DISABLE, "f") );
	emit_field   ( F_SPEC( tex_hash_phase, NV_PGRAPH_PRI_GPCS_TPCS_SM_SCH_TEXLOCK_TEX_HASH_PHASE, "m") );
	emit_constant( C_SPEC( disable, NV_PGRAPH_PRI_GPCS_TPCS_SM_SCH_TEXLOCK_TEX_HASH_PHASE_DISABLE, "f") );
	emit_field   ( F_SPEC( tex_hash_tex, NV_PGRAPH_PRI_GPCS_TPCS_SM_SCH_TEXLOCK_TEX_HASH_TEX, "m") );
	emit_constant( C_SPEC( disable, NV_PGRAPH_PRI_GPCS_TPCS_SM_SCH_TEXLOCK_TEX_HASH_TEX_DISABLE, "f") );
	emit_field   ( F_SPEC( tex_hash_timeout, NV_PGRAPH_PRI_GPCS_TPCS_SM_SCH_TEXLOCK_TEX_HASH_TIMEOUT, "m") );
	emit_constant( C_SPEC( disable, NV_PGRAPH_PRI_GPCS_TPCS_SM_SCH_TEXLOCK_TEX_HASH_TIMEOUT_DISABLE, "f") );
	emit_field   ( F_SPEC( dot_t_unlock, NV_PGRAPH_PRI_GPCS_TPCS_SM_SCH_TEXLOCK_DOT_T_UNLOCK, "m") );
	emit_constant( C_SPEC( disable, NV_PGRAPH_PRI_GPCS_TPCS_SM_SCH_TEXLOCK_DOT_T_UNLOCK_DISABLE, "f") );

	emit_register( R_SPEC( gpcs_tpcs_sm_sch_macro_sched, NV_PGRAPH_PRI_GPCS_TPCS_SM_SCH_MACRO_SCHED) );
	emit_field   ( F_SPEC( lockboost_size, NV_PGRAPH_PRI_GPCS_TPCS_SM_SCH_MACRO_SCHED_LOCKBOOST_SIZE, "fm" ) );

	emit_register( R_SPEC( gpcs_tpcs_sm_dbgr_control0, NV_PGRAPH_PRI_GPCS_TPCS_SM_DBGR_CONTROL0 ) );
	emit_field   ( F_SPEC( debugger_mode, NV_PGRAPH_PRI_GPCS_TPCS_SM_DBGR_CONTROL0_DEBUGGER_MODE, "f" ) );
	emit_constant( C_SPEC( on, NV_PGRAPH_PRI_GPCS_TPCS_SM_DBGR_CONTROL0_DEBUGGER_MODE_ON, "v" ) );
	emit_field   ( F_SPEC( stop_trigger, NV_PGRAPH_PRI_GPCS_TPCS_SM_DBGR_CONTROL0_STOP_TRIGGER, "mv" ) );
	emit_constant( C_SPEC( enable, NV_PGRAPH_PRI_GPCS_TPCS_SM_DBGR_CONTROL0_STOP_TRIGGER_ENABLE, "f" ) );
	emit_constant( C_SPEC( disable, NV_PGRAPH_PRI_GPCS_TPCS_SM_DBGR_CONTROL0_STOP_TRIGGER_DISABLE, "f" ) );
	emit_field   ( F_SPEC( run_trigger, NV_PGRAPH_PRI_GPCS_TPCS_SM_DBGR_CONTROL0_RUN_TRIGGER, "mv" ) );
	emit_constant( C_SPEC( task, NV_PGRAPH_PRI_GPCS_TPCS_SM_DBGR_CONTROL0_RUN_TRIGGER_TASK, "f" ) );

	end_group();

}


static void emit_ltc_group()
{
	begin_group( G_SPEC( ltc ) );

	emit_register( R_SPEC( ltcs_lts0_cbc_ctrl1, NV_PLTCG_LTC0_LTS0_CBC_CTRL_1 ) );
	emit_register( R_SPEC( ltc0_lts0_dstg_cfg0, NV_PLTCG_LTC0_LTS0_DSTG_CFG0 ) );
	emit_register( R_SPEC( ltcs_ltss_dstg_cfg0, NV_PLTCG_LTCS_LTSS_DSTG_CFG0 ) );

	emit_register( R_SPEC( ltc0_lts0_tstg_cfg1, NV_PLTCG_LTC0_LTS0_TSTG_CFG_1 ) );
	emit_field   ( F_SPEC( active_ways, NV_PLTCG_LTC0_LTS0_TSTG_CFG_1_ACTIVE_WAYS, "v") );
	emit_field   ( F_SPEC( active_sets, NV_PLTCG_LTC0_LTS0_TSTG_CFG_1_ACTIVE_SETS, "v") );
	emit_constant( C_SPEC( all, NV_PLTCG_LTC0_LTS0_TSTG_CFG_1_ACTIVE_SETS_ALL, "v") );
	emit_constant( C_SPEC( half, NV_PLTCG_LTC0_LTS0_TSTG_CFG_1_ACTIVE_SETS_HALF, "v") );
	emit_constant( C_SPEC( quarter, NV_PLTCG_LTC0_LTS0_TSTG_CFG_1_ACTIVE_SETS_QUARTER, "v") );

	emit_register( R_SPEC( ltcs_ltss_cbc_ctrl1, NV_PLTCG_LTCS_LTSS_CBC_CTRL_1 ) );
	emit_field   ( F_SPEC( clean, NV_PLTCG_LTCS_LTSS_CBC_CTRL_1_CLEAN, ".") );
	emit_constant( C_SPEC( active, NV_PLTCG_LTCS_LTSS_CBC_CTRL_1_CLEAN_ACTIVE, "f") );
	emit_field   ( F_SPEC( invalidate, NV_PLTCG_LTCS_LTSS_CBC_CTRL_1_INVALIDATE, ".") );
	emit_constant( C_SPEC( active, NV_PLTCG_LTCS_LTSS_CBC_CTRL_1_INVALIDATE_ACTIVE, "f") );
	emit_field   ( F_SPEC( clear, NV_PLTCG_LTCS_LTSS_CBC_CTRL_1_CLEAR, "v") );
	emit_constant( C_SPEC( active, NV_PLTCG_LTCS_LTSS_CBC_CTRL_1_CLEAR_ACTIVE, "fv") );

	emit_register( R_SPEC( ltc0_lts0_cbc_ctrl1, NV_PLTCG_LTC0_LTS0_CBC_CTRL_1 ) );

	emit_register( R_SPEC( ltcs_ltss_cbc_ctrl2, NV_PLTCG_LTCS_LTSS_CBC_CTRL_2 ) );
	emit_field   ( F_SPEC( clear_lower_bound, NV_PLTCG_LTCS_LTSS_CBC_CTRL_2_CLEAR_LOWER_BOUND, "f") );

	emit_register( R_SPEC( ltcs_ltss_cbc_ctrl3, NV_PLTCG_LTCS_LTSS_CBC_CTRL_3 ) );
	emit_field   ( F_SPEC( clear_upper_bound, NV_PLTCG_LTCS_LTSS_CBC_CTRL_3_CLEAR_UPPER_BOUND, "f") );
	emit_constant( C_SPEC( init, NV_PLTCG_LTCS_LTSS_CBC_CTRL_3_CLEAR_UPPER_BOUND_INIT, "v") );

	emit_register( R_SPEC( ltcs_ltss_cbc_base, NV_PLTCG_LTCS_LTSS_CBC_BASE ) );
	emit_constant( C_SPEC( alignment_shift, NV_PLTCG_LTC0_LTS0_CBC_BASE_ALIGNMENT_SHIFT, "v") );
	emit_field   ( F_SPEC( address, NV_PLTCG_LTCS_LTSS_CBC_BASE_ADDRESS, "v") );

	emit_register( R_SPEC( ltcs_ltss_cbc_param, NV_PLTCG_LTCS_LTSS_CBC_PARAM ) );
	emit_field   ( F_SPEC( comptags_per_cache_line, NV_PLTCG_LTCS_LTSS_CBC_PARAM_COMPTAGS_PER_CACHE_LINE, "v") );
	emit_field   ( F_SPEC( cache_line_size, NV_PLTCG_LTCS_LTSS_CBC_PARAM_CACHE_LINE_SIZE, "v") );
	emit_field   ( F_SPEC( slices_per_fbp, NV_PLTCG_LTCS_LTSS_CBC_PARAM_SLICES_PER_FBP, "v") );

	emit_register( R_SPEC( ltcs_ltss_tstg_set_mgmt, NV_PLTCG_LTCS_LTSS_TSTG_SET_MGMT_0 ) );
	emit_field   ( F_SPEC( max_ways_evict_last, NV_PLTCG_LTCS_LTSS_TSTG_SET_MGMT_0_MAX_WAYS_EVICT_LAST, "f") );

	emit_register( R_SPEC( ltcs_ltss_dstg_zbc_index, NV_PLTCG_LTCS_LTSS_DSTG_ZBC_INDEX ) );
	emit_field   ( F_SPEC( address, NV_PLTCG_LTCS_LTSS_DSTG_ZBC_INDEX_ADDRESS, "f") );

	emit_register( IR_SPEC( ltcs_ltss_dstg_zbc_color_clear_value, NV_PLTCG_LTCS_LTSS_DSTG_ZBC_COLOR_CLEAR_VALUE ) );
	emit_constant( C_SPEC( _size_1, NV_PLTCG_LTCS_LTSS_DSTG_ZBC_COLOR_CLEAR_VALUE__SIZE_1, "v") );

	emit_register( R_SPEC( ltcs_ltss_dstg_zbc_depth_clear_value, NV_PLTCG_LTCS_LTSS_DSTG_ZBC_DEPTH_CLEAR_VALUE ) );
	emit_field   ( F_SPEC( field, NV_PLTCG_LTCS_LTSS_DSTG_ZBC_DEPTH_CLEAR_VALUE_FIELD, "") );
	end_scope();

	emit_register( R_SPEC( ltcs_ltss_tstg_set_mgmt_2, NV_PLTCG_LTCS_LTSS_TSTG_SET_MGMT_2 ) );
	emit_field   ( F_SPEC( l2_bypass_mode, NV_PLTCG_LTCS_LTSS_TSTG_SET_MGMT_2_L2_BYPASS_MODE, ".") );
	emit_constant( C_SPEC( enabled, NV_PLTCG_LTCS_LTSS_TSTG_SET_MGMT_2_L2_BYPASS_MODE_ENABLED, "f") );

	emit_register( R_SPEC( ltcs_ltss_g_elpg, NV_PLTCG_LTCS_LTSS_G_ELPG ) );
	emit_field   ( F_SPEC( flush,            NV_PLTCG_LTCS_LTSS_G_ELPG_FLUSH, "v") );
	emit_constant( C_SPEC( pending,          NV_PLTCG_LTCS_LTSS_G_ELPG_FLUSH_PENDING, "fv" ) );

	emit_register( R_SPEC( ltc0_ltss_g_elpg, NV_PLTCG_LTC0_LTSS_G_ELPG ) );
	emit_field   ( F_SPEC( flush,            NV_PLTCG_LTC0_LTSS_G_ELPG_FLUSH, "v") );
	emit_constant( C_SPEC( pending,          NV_PLTCG_LTC0_LTSS_G_ELPG_FLUSH_PENDING, "fv" ) );

	emit_register( R_SPEC( ltc0_ltss_intr, NV_PLTCG_LTC0_LTSS_INTR) );

	emit_register( R_SPEC( ltcs_ltss_intr, NV_PLTCG_LTCS_LTSS_INTR) );
	emit_field   ( F_SPEC( en_evicted_cb, NV_PLTCG_LTCS_LTSS_INTR_EN_EVICTED_CB, "m") );
	emit_register( R_SPEC( ltc0_lts0_intr, NV_PLTCG_LTC0_LTS0_INTR) );

	emit_register( R_SPEC( ltcs_ltss_tstg_cmgmt0,          NV_PLTCG_LTCS_LTSS_TSTG_CMGMT_0) );
	emit_field   ( F_SPEC( invalidate,                     NV_PLTCG_LTCS_LTSS_TSTG_CMGMT_0_INVALIDATE, "v") );
	emit_constant( C_SPEC( pending,                        NV_PLTCG_LTCS_LTSS_TSTG_CMGMT_0_INVALIDATE_PENDING, "fv" ) );
	emit_field   ( F_SPEC( max_cycles_between_invalidates, NV_PLTCG_LTCS_LTSS_TSTG_CMGMT_0_MAX_CYCLES_BETWEEN_INVALIDATES, "v") );
	emit_constant( C_SPEC( 3,                              NV_PLTCG_LTCS_LTSS_TSTG_CMGMT_0_MAX_CYCLES_BETWEEN_INVALIDATES_3, "fv" ) );
	emit_field   ( F_SPEC( invalidate_evict_last_class,    NV_PLTCG_LTCS_LTSS_TSTG_CMGMT_0_INVALIDATE_EVICT_LAST_CLASS, "v") );
	emit_constant( C_SPEC( true,                           NV_PLTCG_LTCS_LTSS_TSTG_CMGMT_0_INVALIDATE_EVICT_LAST_CLASS_TRUE, "fv" ) );
	emit_field   ( F_SPEC( invalidate_evict_normal_class,  NV_PLTCG_LTCS_LTSS_TSTG_CMGMT_0_INVALIDATE_EVICT_NORMAL_CLASS, "v") );
	emit_constant( C_SPEC( true,                           NV_PLTCG_LTCS_LTSS_TSTG_CMGMT_0_INVALIDATE_EVICT_NORMAL_CLASS_TRUE, "fv" ) );
	emit_field   ( F_SPEC( invalidate_evict_first_class,   NV_PLTCG_LTCS_LTSS_TSTG_CMGMT_0_INVALIDATE_EVICT_FIRST_CLASS, "v") );
	emit_constant( C_SPEC( true,                           NV_PLTCG_LTCS_LTSS_TSTG_CMGMT_0_INVALIDATE_EVICT_FIRST_CLASS_TRUE, "fv" ) );

	emit_register( R_SPEC( ltcs_ltss_tstg_cmgmt1,     NV_PLTCG_LTCS_LTSS_TSTG_CMGMT_1) );
	emit_field   ( F_SPEC( clean,                     NV_PLTCG_LTCS_LTSS_TSTG_CMGMT_1_CLEAN, "v") );
	emit_constant( C_SPEC( pending,                   NV_PLTCG_LTCS_LTSS_TSTG_CMGMT_1_CLEAN_PENDING, "fv" ) );
	emit_field   ( F_SPEC( max_cycles_between_cleans, NV_PLTCG_LTCS_LTSS_TSTG_CMGMT_1_MAX_CYCLES_BETWEEN_CLEANS, "v") );
	emit_constant( C_SPEC( 3,                         NV_PLTCG_LTCS_LTSS_TSTG_CMGMT_1_MAX_CYCLES_BETWEEN_CLEANS_3, "fv" ) );
	emit_field   ( F_SPEC( clean_wait_for_fb_to_pull, NV_PLTCG_LTCS_LTSS_TSTG_CMGMT_1_CLEAN_WAIT_FOR_FB_TO_PULL, "v") );
	emit_constant( C_SPEC( true,                      NV_PLTCG_LTCS_LTSS_TSTG_CMGMT_1_CLEAN_WAIT_FOR_FB_TO_PULL_TRUE, "fv" ) );
	emit_field   ( F_SPEC( clean_evict_last_class,    NV_PLTCG_LTCS_LTSS_TSTG_CMGMT_1_CLEAN_EVICT_LAST_CLASS, "v") );
	emit_constant( C_SPEC( true,                      NV_PLTCG_LTCS_LTSS_TSTG_CMGMT_1_CLEAN_EVICT_LAST_CLASS_TRUE, "fv" ) );
	emit_field   ( F_SPEC( clean_evict_normal_class,  NV_PLTCG_LTCS_LTSS_TSTG_CMGMT_1_CLEAN_EVICT_NORMAL_CLASS, "v") );
	emit_constant( C_SPEC( true,                      NV_PLTCG_LTCS_LTSS_TSTG_CMGMT_1_CLEAN_EVICT_NORMAL_CLASS_TRUE, "fv" ) );
	emit_field   ( F_SPEC( clean_evict_first_class,   NV_PLTCG_LTCS_LTSS_TSTG_CMGMT_1_CLEAN_EVICT_FIRST_CLASS, "v") );
	emit_constant( C_SPEC( true,                      NV_PLTCG_LTCS_LTSS_TSTG_CMGMT_1_CLEAN_EVICT_FIRST_CLASS_TRUE, "fv" ) );

	emit_register( R_SPEC( ltc0_ltss_tstg_cmgmt0,  NV_PLTCG_LTC0_LTSS_TSTG_CMGMT_0) );
	emit_field   ( F_SPEC( invalidate,             NV_PLTCG_LTC0_LTSS_TSTG_CMGMT_0_INVALIDATE, "v") );
	emit_constant( C_SPEC( pending,                NV_PLTCG_LTC0_LTSS_TSTG_CMGMT_0_INVALIDATE_PENDING, "fv" ) );

	emit_register( R_SPEC( ltc0_ltss_tstg_cmgmt1,  NV_PLTCG_LTC0_LTSS_TSTG_CMGMT_1) );
	emit_field   ( F_SPEC( clean,                  NV_PLTCG_LTC0_LTSS_TSTG_CMGMT_1_CLEAN, "v") );
	emit_constant( C_SPEC( pending,                NV_PLTCG_LTC0_LTSS_TSTG_CMGMT_1_CLEAN_PENDING, "fv" ) );

	end_group();

}

static void emit_mc_group()
{
	begin_group( G_SPEC( mc ) );
	emit_register( R_SPEC( boot_0, NV_PMC_BOOT_0 ) );
	emit_field   ( F_SPEC( architecture, NV_PMC_BOOT_0_ARCHITECTURE, "v") );
	emit_field   ( F_SPEC( implementation, NV_PMC_BOOT_0_IMPLEMENTATION, "v") );
	emit_field   ( F_SPEC( major_revision, NV_PMC_BOOT_0_MAJOR_REVISION, "v") );
	emit_field   ( F_SPEC( minor_revision, NV_PMC_BOOT_0_MINOR_REVISION, "v") );
	emit_register( R_SPEC( intr_0, NV_PMC_INTR_0 ) );
	emit_field   ( F_SPEC( pfifo, NV_PMC_INTR_0_PFIFO, ".") );
	emit_constant( C_SPEC( pending, NV_PMC_INTR_0_PFIFO_PENDING, "f") );
	emit_field   ( F_SPEC( pgraph, NV_PMC_INTR_0_PGRAPH, ".") );
	emit_constant( C_SPEC( pending, NV_PMC_INTR_0_PGRAPH_PENDING, "f") );
	emit_field   ( F_SPEC( pmu, NV_PMC_INTR_0_PMU, ".") );
	emit_constant( C_SPEC( pending, NV_PMC_INTR_0_PMU_PENDING, "f") );
	emit_field   ( F_SPEC( ltc, NV_PMC_INTR_0_LTC, ".") );
	emit_constant( C_SPEC( pending, NV_PMC_INTR_0_LTC_PENDING, "f") );
	emit_field   ( F_SPEC( priv_ring, NV_PMC_INTR_0_PRIV_RING, ".") );
	emit_constant( C_SPEC( pending, NV_PMC_INTR_0_PRIV_RING_PENDING, "f") );
	emit_field   ( F_SPEC( pbus, NV_PMC_INTR_0_PBUS, ".") );
	emit_constant( C_SPEC( pending, NV_PMC_INTR_0_PBUS_PENDING, "f") );

	emit_register( R_SPEC( intr_1, NV_PMC_INTR_1 ) );

	emit_register( R_SPEC( intr_mask_0, NV_PMC_INTR_MSK_0 ) );
	emit_field   ( F_SPEC( pmu, NV_PMC_INTR_MSK_0_PMU, ".") );
	emit_constant( C_SPEC( enabled, NV_PMC_INTR_MSK_0_PMU_ENABLED, "f") );
	emit_register( R_SPEC( intr_en_0, NV_PMC_INTR_EN_0 ) );
	emit_field   ( F_SPEC( inta, NV_PMC_INTR_EN_0_INTA, ".") );
	emit_constant( C_SPEC( disabled, NV_PMC_INTR_EN_0_INTA_DISABLED, "f") );
	emit_constant( C_SPEC( hardware, NV_PMC_INTR_EN_0_INTA_HARDWARE, "f") );


	emit_register( R_SPEC( intr_mask_1, NV_PMC_INTR_MSK_1 ) );
	emit_field   ( F_SPEC( pmu, NV_PMC_INTR_MSK_1_PMU, "") );
	emit_constant( C_SPEC( enabled, NV_PMC_INTR_MSK_1_PMU_ENABLED, "f") );

	emit_register( R_SPEC( intr_en_1, NV_PMC_INTR_EN_1 ) );
	emit_field   ( F_SPEC( inta, NV_PMC_INTR_EN_1_INTA, ".") );
	emit_constant( C_SPEC( disabled, NV_PMC_INTR_EN_1_INTA_DISABLED, "f") );
	emit_constant( C_SPEC( hardware, NV_PMC_INTR_EN_1_INTA_HARDWARE, "f") );

	emit_register( R_SPEC( enable, NV_PMC_ENABLE ) );
	emit_field   ( F_SPEC( xbar, NV_PMC_ENABLE_XBAR, ".") );
	emit_constant( C_SPEC( enabled, NV_PMC_ENABLE_XBAR_ENABLED, "f") );
	emit_field   ( F_SPEC( l2, NV_PMC_ENABLE_L2, ".") );
	emit_constant( C_SPEC( enabled, NV_PMC_ENABLE_L2_ENABLED, "f") );
	emit_field   ( F_SPEC( pmedia, NV_PMC_ENABLE_PMEDIA, "") );
	emit_field   ( F_SPEC( priv_ring, NV_PMC_ENABLE_PRIV_RING, ".") );
	emit_constant( C_SPEC( enabled, NV_PMC_ENABLE_PRIV_RING_ENABLED, "f") );
	emit_field   ( F_SPEC( ce0, NV_PMC_ENABLE_CE0, "m") );
	emit_field   ( F_SPEC( pfifo, NV_PMC_ENABLE_PFIFO, ".") );
	emit_constant( C_SPEC( enabled, NV_PMC_ENABLE_PFIFO_ENABLED, "f") );
	emit_field   ( F_SPEC( pgraph, NV_PMC_ENABLE_PGRAPH, ".") );
	emit_constant( C_SPEC( enabled, NV_PMC_ENABLE_PGRAPH_ENABLED, "f") );
	emit_field   ( F_SPEC( pwr, NV_PMC_ENABLE_PWR, "v") );
	emit_constant( C_SPEC( disabled, NV_PMC_ENABLE_PWR_DISABLED, "v") );
	emit_constant( C_SPEC( enabled, NV_PMC_ENABLE_PWR_ENABLED, "f") );
	emit_field   ( F_SPEC( pfb, NV_PMC_ENABLE_PFB, ".") );
	emit_constant( C_SPEC( enabled, NV_PMC_ENABLE_PFB_ENABLED, "f") );
	emit_field   ( F_SPEC( ce2, NV_PMC_ENABLE_CE2, "m") );
	emit_constant( C_SPEC( enabled, NV_PMC_ENABLE_CE2_ENABLED, "f") );
	emit_field   ( F_SPEC( blg, NV_PMC_ENABLE_BLG, ".") );
	emit_constant( C_SPEC( enabled, NV_PMC_ENABLE_BLG_ENABLED, "f") );
	emit_field   ( F_SPEC( perfmon, NV_PMC_ENABLE_PERFMON, ".") );
	emit_constant( C_SPEC( enabled, NV_PMC_ENABLE_PERFMON_ENABLED, "f") );
	emit_field   ( F_SPEC( hub, NV_PMC_ENABLE_HUB, ".") );
	emit_constant( C_SPEC( enabled, NV_PMC_ENABLE_HUB_ENABLED, "f") );

	emit_register( R_SPEC( enable_pb, NV_PMC_ENABLE_PB ) );
	emit_field   ( F_SPEC( 0, NV_PMC_ENABLE_PB_0, "") );
	emit_constant( C_SPEC( enabled, NV_PMC_ENABLE_PB_0_ENABLED, "v") );
	emit_field   (IF_SPEC( sel, NV_PMC_ENABLE_PB_SEL, "f") );

	emit_register( R_SPEC( elpg_enable, NV_PMC_ELPG_ENABLE ) );
	emit_field   ( F_SPEC( xbar, NV_PMC_ELPG_ENABLE_XBAR, ".") );
	emit_constant( C_SPEC( enabled, NV_PMC_ENABLE_XBAR_ENABLED, "f") );
	emit_field   ( F_SPEC( pfb, NV_PMC_ELPG_ENABLE_PFB, ".") );
	emit_constant( C_SPEC( enabled, NV_PMC_ENABLE_PFB_ENABLED, "f") );
	emit_field   ( F_SPEC( hub, NV_PMC_ELPG_ENABLE_HUB, ".") );
	emit_constant( C_SPEC( enabled, NV_PMC_ENABLE_HUB_ENABLED, "f") );

	end_group();

}


static void emit_pbdma_group()
{
	begin_group( G_SPEC( pbdma ) );
	emit_register( R_SPEC( gp_entry1, NV_PPBDMA_GP_ENTRY1 ) );
	emit_field   ( F_SPEC( get_hi, NV_PPBDMA_GP_ENTRY1_GET_HI, "v") );
	emit_field   ( F_SPEC( length, NV_PPBDMA_GP_ENTRY1_LENGTH , "fv") );

	emit_register( IR_SPEC( gp_base, NV_PPBDMA_GP_BASE ) );
	emit_constant( C_SPEC( _size_1, NV_PPBDMA_GP_BASE__SIZE_1, "v") );
	emit_field   ( F_SPEC( offset, NV_PPBDMA_GP_BASE_OFFSET, "f") );
	emit_field   ( F_SPEC( rsvd, NV_PPBDMA_GP_BASE_RSVD, "s") );

	emit_register( IR_SPEC( gp_base_hi, NV_PPBDMA_GP_BASE_HI ) );
	emit_field   ( F_SPEC( offset, NV_PPBDMA_GP_BASE_HI_OFFSET, "f") );
	emit_field   ( F_SPEC( limit2, NV_PPBDMA_GP_BASE_HI_LIMIT2, "f") );

	emit_register( IR_SPEC( gp_fetch, NV_PPBDMA_GP_FETCH ) );

	emit_register( IR_SPEC( gp_get, NV_PPBDMA_GP_GET ) );

	emit_register( IR_SPEC( gp_put, NV_PPBDMA_GP_PUT ) );

	emit_register( IR_SPEC( timeout, NV_PPBDMA_TIMEOUT ) );
	emit_constant( C_SPEC( _size_1, NV_PPBDMA_TIMEOUT__SIZE_1, "v") );
	emit_field   ( F_SPEC( period, NV_PPBDMA_TIMEOUT_PERIOD, "m" ) );
	emit_constant( C_SPEC( max, NV_PPBDMA_TIMEOUT_PERIOD_MAX, "f" ) );

	emit_register( IR_SPEC( pb_fetch, NV_PPBDMA_PB_FETCH ) );

	emit_register( IR_SPEC( pb_fetch_hi, NV_PPBDMA_PB_FETCH_HI ) );

	emit_register( IR_SPEC( get, NV_PPBDMA_GET ) );

	emit_register( IR_SPEC( get_hi, NV_PPBDMA_GET_HI ) );

	emit_register( IR_SPEC( put, NV_PPBDMA_PUT ) );

	emit_register( IR_SPEC( put_hi, NV_PPBDMA_PUT_HI ) );

	emit_register( IR_SPEC( formats, NV_PPBDMA_FORMATS ) );
	emit_field   ( F_SPEC( gp, NV_PPBDMA_FORMATS_GP, ".") );
	emit_constant( C_SPEC( fermi0, NV_PPBDMA_FORMATS_GP_FERMI0, "f") );
	emit_field   ( F_SPEC( pb, NV_PPBDMA_FORMATS_PB, ".") );
	emit_constant( C_SPEC( fermi1, NV_PPBDMA_FORMATS_PB_FERMI1, "f") );
	emit_field   ( F_SPEC( mp, NV_PPBDMA_FORMATS_MP, ".") );
	emit_constant( C_SPEC( fermi0, NV_PPBDMA_FORMATS_MP_FERMI0, "f") );

	emit_register( IR_SPEC( pb_header, NV_PPBDMA_PB_HEADER ) );
	emit_field   ( F_SPEC( priv, NV_PPBDMA_PB_HEADER_PRIV, ".") );
	emit_constant( C_SPEC( user, NV_PPBDMA_PB_HEADER_PRIV_USER, "f") );
	emit_field   ( F_SPEC( method, NV_PPBDMA_PB_HEADER_METHOD, ".") );
	emit_constant( C_SPEC( zero, NV_PPBDMA_PB_HEADER_METHOD_ZERO, "f") );
	emit_field   ( F_SPEC( subchannel, NV_PPBDMA_PB_HEADER_SUBCHANNEL, ".") );
	emit_constant( C_SPEC( zero, NV_PPBDMA_PB_HEADER_SUBCHANNEL_ZERO, "f") );
	emit_field   ( F_SPEC( level, NV_PPBDMA_PB_HEADER_LEVEL, ".") );
	emit_constant( C_SPEC( main, NV_PPBDMA_PB_HEADER_LEVEL_MAIN, "f") );
	emit_field   ( F_SPEC( first, NV_PPBDMA_PB_HEADER_FIRST, ".") );
	emit_constant( C_SPEC( true, NV_PPBDMA_PB_HEADER_FIRST_TRUE, "f") );
	emit_field   ( F_SPEC( type, NV_PPBDMA_PB_HEADER_TYPE, ".") );
	emit_constant( C_SPEC( inc, NV_PPBDMA_PB_HEADER_TYPE_INC, "f") );

	emit_register( IR_SPEC( hdr_shadow, NV_PPBDMA_HDR_SHADOW) );

	emit_register( IR_SPEC( subdevice, NV_PPBDMA_SUBDEVICE ) );
	emit_field   ( F_SPEC( id, NV_PPBDMA_SUBDEVICE_ID, "f") );
	emit_field   ( F_SPEC( status, NV_PPBDMA_SUBDEVICE_STATUS, ".") );
	emit_constant( C_SPEC( active, NV_PPBDMA_SUBDEVICE_STATUS_ACTIVE, "f") );
	emit_field   ( F_SPEC( channel_dma, NV_PPBDMA_SUBDEVICE_CHANNEL_DMA, ".") );
	emit_constant( C_SPEC( enable, NV_PPBDMA_SUBDEVICE_CHANNEL_DMA_ENABLE, "f") );

	emit_register( IR_SPEC( method0, NV_PPBDMA_METHOD0 ) );

	emit_register( IR_SPEC( data0, NV_PPBDMA_DATA0 ) );

	emit_register( IR_SPEC( target, NV_PPBDMA_TARGET ) );
	emit_field   ( F_SPEC( engine, NV_PPBDMA_TARGET_ENGINE, ".") );
	emit_constant( C_SPEC( sw, NV_PPBDMA_TARGET_ENGINE_SW, "f") );

	emit_register( IR_SPEC( acquire, NV_PPBDMA_ACQUIRE ) );
	emit_field   ( F_SPEC( retry_man, NV_PPBDMA_ACQUIRE_RETRY_MAN, ".") );
	emit_constant( C_SPEC( 2, NV_PPBDMA_ACQUIRE_RETRY_MAN_2, "f") );
	emit_field   ( F_SPEC( retry_exp, NV_PPBDMA_ACQUIRE_RETRY_EXP, ".") );
	emit_constant( C_SPEC( 2, NV_PPBDMA_ACQUIRE_RETRY_EXP_2, "f") );
	emit_field   ( F_SPEC( timeout_exp, NV_PPBDMA_ACQUIRE_TIMEOUT_EXP, ".") );
	emit_constant( C_SPEC( max, NV_PPBDMA_ACQUIRE_TIMEOUT_EXP_MAX, "f") );
	emit_field   ( F_SPEC( timeout_man, NV_PPBDMA_ACQUIRE_TIMEOUT_MAN, ".") );
	emit_constant( C_SPEC( max, NV_PPBDMA_ACQUIRE_TIMEOUT_MAN_MAX, "f") );
	emit_field   ( F_SPEC( timeout_en, NV_PPBDMA_ACQUIRE_TIMEOUT_EN, ".") );
	emit_constant( C_SPEC( disable, NV_PPBDMA_ACQUIRE_TIMEOUT_EN_DISABLE, "f") );

	emit_register( IR_SPEC( status, NV_PPBDMA_STATUS ) );

	emit_register( IR_SPEC( channel, NV_PPBDMA_CHANNEL ) );

	emit_register( IR_SPEC( signature, NV_PPBDMA_SIGNATURE ) );
	emit_field   ( F_SPEC( hw, NV_PPBDMA_SIGNATURE_HW, ".") );
	emit_constant( C_SPEC( valid, NV_PPBDMA_SIGNATURE_HW_VALID, "f") );
	emit_field   ( F_SPEC( sw, NV_PPBDMA_SIGNATURE_SW, ".") );
	emit_constant( C_SPEC( zero, NV_PPBDMA_SIGNATURE_SW_ZERO, "f") );

	emit_register( IR_SPEC( userd, NV_PPBDMA_USERD ) );
	emit_field   ( F_SPEC( target, NV_PPBDMA_USERD_TARGET, ".") );
	emit_constant( C_SPEC( vid_mem, NV_PPBDMA_USERD_TARGET_VID_MEM, "f") );
	emit_field   ( F_SPEC( addr, NV_PPBDMA_USERD_ADDR, "f") );

	emit_register( IR_SPEC( userd_hi, NV_PPBDMA_USERD_HI ) );
	emit_field   ( F_SPEC( addr, NV_PPBDMA_USERD_HI_ADDR, "f") );

	emit_register( IR_SPEC( hce_ctrl, NV_PPBDMA_HCE_CTRL ) );
	emit_field   ( F_SPEC( hce_priv_mode, NV_PPBDMA_HCE_CTRL_HCE_PRIV_MODE, ".") );
	emit_constant( C_SPEC( yes, NV_PPBDMA_HCE_CTRL_HCE_PRIV_MODE_YES, "f") );

emit_register( IR_SPEC( intr_0, NV_PPBDMA_INTR_0 ) );
	emit_field   ( F_SPEC( memreq, NV_PPBDMA_INTR_0_MEMREQ, "v") );
	emit_constant( C_SPEC( pending, NV_PPBDMA_INTR_0_MEMREQ_PENDING, "f") );
	emit_field   ( F_SPEC( memack_timeout, NV_PPBDMA_INTR_0_MEMACK_TIMEOUT, ".") );
	emit_constant( C_SPEC( pending, NV_PPBDMA_INTR_0_MEMACK_TIMEOUT_PENDING, "f") );
	emit_field   ( F_SPEC( memack_extra, NV_PPBDMA_INTR_0_MEMACK_EXTRA, ".") );
	emit_constant( C_SPEC( pending, NV_PPBDMA_INTR_0_MEMACK_EXTRA_PENDING, "f") );
	emit_field   ( F_SPEC( memdat_timeout, NV_PPBDMA_INTR_0_MEMDAT_TIMEOUT, ".") );
	emit_constant( C_SPEC( pending, NV_PPBDMA_INTR_0_MEMDAT_TIMEOUT_PENDING, "f") );
	emit_field   ( F_SPEC( memdat_extra, NV_PPBDMA_INTR_0_MEMDAT_EXTRA, ".") );
	emit_constant( C_SPEC( pending, NV_PPBDMA_INTR_0_MEMDAT_EXTRA_PENDING, "f") );
	emit_field   ( F_SPEC( memflush, NV_PPBDMA_INTR_0_MEMFLUSH, ".") );
	emit_constant( C_SPEC( pending, NV_PPBDMA_INTR_0_MEMFLUSH_PENDING, "f") );
	emit_field   ( F_SPEC( memop, NV_PPBDMA_INTR_0_MEMOP, ".") );
	emit_constant( C_SPEC( pending, NV_PPBDMA_INTR_0_MEMOP_PENDING, "f") );
	emit_field   ( F_SPEC( lbconnect, NV_PPBDMA_INTR_0_LBCONNECT, ".") );
	emit_constant( C_SPEC( pending, NV_PPBDMA_INTR_0_LBCONNECT_PENDING, "f") );
	emit_field   ( F_SPEC( lbreq, NV_PPBDMA_INTR_0_LBREQ, ".") );
	emit_constant( C_SPEC( pending, NV_PPBDMA_INTR_0_LBREQ_PENDING, "f") );
	emit_field   ( F_SPEC( lback_timeout, NV_PPBDMA_INTR_0_LBACK_TIMEOUT, ".") );
	emit_constant( C_SPEC( pending, NV_PPBDMA_INTR_0_LBACK_TIMEOUT_PENDING, "f") );
	emit_field   ( F_SPEC( lback_extra, NV_PPBDMA_INTR_0_LBACK_EXTRA, ".") );
	emit_constant( C_SPEC( pending, NV_PPBDMA_INTR_0_LBACK_EXTRA_PENDING, "f") );
	emit_field   ( F_SPEC( lbdat_timeout, NV_PPBDMA_INTR_0_LBDAT_TIMEOUT, ".") );
	emit_constant( C_SPEC( pending, NV_PPBDMA_INTR_0_LBDAT_TIMEOUT_PENDING, "f") );
	emit_field   ( F_SPEC( lbdat_extra, NV_PPBDMA_INTR_0_LBDAT_EXTRA, ".") );
	emit_constant( C_SPEC( pending, NV_PPBDMA_INTR_0_LBDAT_EXTRA_PENDING, "f") );
	emit_field   ( F_SPEC( gpfifo, NV_PPBDMA_INTR_0_GPFIFO, ".") );
	emit_constant( C_SPEC( pending, NV_PPBDMA_INTR_0_GPFIFO_PENDING, "f") );
	emit_field   ( F_SPEC( gpptr, NV_PPBDMA_INTR_0_GPPTR, ".") );
	emit_constant( C_SPEC( pending, NV_PPBDMA_INTR_0_GPPTR_PENDING, "f") );
	emit_field   ( F_SPEC( gpentry, NV_PPBDMA_INTR_0_GPENTRY, ".") );
	emit_constant( C_SPEC( pending, NV_PPBDMA_INTR_0_GPENTRY_PENDING, "f") );
	emit_field   ( F_SPEC( gpcrc, NV_PPBDMA_INTR_0_GPCRC, ".") );
	emit_constant( C_SPEC( pending, NV_PPBDMA_INTR_0_GPCRC_PENDING, "f") );
	emit_field   ( F_SPEC( pbptr, NV_PPBDMA_INTR_0_PBPTR, ".") );
	emit_constant( C_SPEC( pending, NV_PPBDMA_INTR_0_PBPTR_PENDING, "f") );
	emit_field   ( F_SPEC( pbentry, NV_PPBDMA_INTR_0_PBENTRY, ".") );
	emit_constant( C_SPEC( pending, NV_PPBDMA_INTR_0_PBENTRY_PENDING, "f") );
	emit_field   ( F_SPEC( pbcrc, NV_PPBDMA_INTR_0_PBCRC, ".") );
	emit_constant( C_SPEC( pending, NV_PPBDMA_INTR_0_PBCRC_PENDING, "f") );
	emit_field   ( F_SPEC( xbarconnect, NV_PPBDMA_INTR_0_XBARCONNECT, ".") );
	emit_constant( C_SPEC( pending, NV_PPBDMA_INTR_0_XBARCONNECT_PENDING, "f") );
	emit_field   ( F_SPEC( method, NV_PPBDMA_INTR_0_METHOD, ".") );
	emit_constant( C_SPEC( pending, NV_PPBDMA_INTR_0_METHOD_PENDING, "f") );
	emit_field   ( F_SPEC( methodcrc, NV_PPBDMA_INTR_0_METHODCRC, ".") );
	emit_constant( C_SPEC( pending, NV_PPBDMA_INTR_0_METHODCRC_PENDING, "f") );
	emit_field   ( F_SPEC( device, NV_PPBDMA_INTR_0_DEVICE, ".") );
	emit_constant( C_SPEC( pending, NV_PPBDMA_INTR_0_DEVICE_PENDING, "f") );
	emit_field   ( F_SPEC( semaphore, NV_PPBDMA_INTR_0_SEMAPHORE, ".") );
	emit_constant( C_SPEC( pending, NV_PPBDMA_INTR_0_SEMAPHORE_PENDING, "f") );
	emit_field   ( F_SPEC( acquire, NV_PPBDMA_INTR_0_ACQUIRE, ".") );
	emit_constant( C_SPEC( pending, NV_PPBDMA_INTR_0_ACQUIRE_PENDING, "f") );
	emit_field   ( F_SPEC( pri, NV_PPBDMA_INTR_0_PRI, ".") );
	emit_constant( C_SPEC( pending, NV_PPBDMA_INTR_0_PRI_PENDING, "f") );
	emit_field   ( F_SPEC( no_ctxsw_seg, NV_PPBDMA_INTR_0_NO_CTXSW_SEG, ".") );
	emit_constant( C_SPEC( pending, NV_PPBDMA_INTR_0_NO_CTXSW_SEG_PENDING, "f") );
	emit_field   ( F_SPEC( pbseg, NV_PPBDMA_INTR_0_PBSEG, ".") );
	emit_constant( C_SPEC( pending, NV_PPBDMA_INTR_0_PBSEG_PENDING, "f") );
	emit_field   ( F_SPEC( signature, NV_PPBDMA_INTR_0_SIGNATURE, ".") );
	emit_constant( C_SPEC( pending, NV_PPBDMA_INTR_0_SIGNATURE_PENDING, "f") );

	emit_register( IR_SPEC( intr_1, NV_PPBDMA_INTR_1 ) );

	emit_register( IR_SPEC( intr_en_0, NV_PPBDMA_INTR_EN_0 ) );
	emit_field   ( F_SPEC( lbreq, NV_PPBDMA_INTR_EN_0_LBREQ, ".") );
	emit_constant( C_SPEC( enabled, NV_PPBDMA_INTR_EN_0_LBREQ_ENABLED, "f") );

	emit_register( IR_SPEC( intr_en_1, NV_PPBDMA_INTR_EN_1 ) );

	emit_register( IR_SPEC( intr_stall, NV_PPBDMA_INTR_STALL ) );
	emit_field   ( F_SPEC( lbreq, NV_PPBDMA_INTR_STALL_LBREQ, ".") );
	emit_constant( C_SPEC( enabled, NV_PPBDMA_INTR_STALL_LBREQ_ENABLED, "f") );

	emit_register( R_SPEC( udma_nop, NV_UDMA_NOP ) );

	emit_register( IR_SPEC( syncpointa, NV_PPBDMA_SYNCPOINTA ) );
	emit_field   ( F_SPEC( payload, NV_PPBDMA_SYNCPOINTA_PAYLOAD, "v") );

	emit_register( IR_SPEC( syncpointb, NV_PPBDMA_SYNCPOINTB ) );
	emit_field   ( F_SPEC( op, NV_PPBDMA_SYNCPOINTB_OPERATION, "v") );
	emit_constant( C_SPEC( wait, NV_PPBDMA_SYNCPOINTB_OPERATION_WAIT, "v") );
	emit_field   ( F_SPEC( wait_switch, NV_PPBDMA_SYNCPOINTB_WAIT_SWITCH, "v") );
	emit_constant( C_SPEC( en, NV_PPBDMA_SYNCPOINTB_WAIT_SWITCH_EN, "v") );
	emit_field   ( F_SPEC( syncpt_index, NV_PPBDMA_SYNCPOINTB_SYNCPT_INDEX, "v") );

	end_group();

}


static void emit_pri_ringmaster_group()
{
	begin_group( G_SPEC( pri_ringmaster ) );
	emit_register( R_SPEC( command, NV_PPRIV_MASTER_RING_COMMAND ) );
	emit_field   ( F_SPEC( cmd, NV_PPRIV_MASTER_RING_COMMAND_CMD, "mv") );
	emit_constant( C_SPEC( no_cmd, NV_PPRIV_MASTER_RING_COMMAND_CMD_NO_CMD, "v") );
	emit_constant( C_SPEC( start_ring, NV_PPRIV_MASTER_RING_COMMAND_CMD_START_RING, "f") );
	emit_constant( C_SPEC( ack_interrupt, NV_PPRIV_MASTER_RING_COMMAND_CMD_ACK_INTERRUPT, "f") );
	emit_constant( C_SPEC( enumerate_stations, NV_PPRIV_MASTER_RING_COMMAND_CMD_ENUMERATE_STATIONS, "f") );
	emit_field   ( F_SPEC( cmd_enumerate_stations_bc_grp, NV_PPRIV_MASTER_RING_COMMAND_CMD_ENUMERATE_STATIONS_BC_GRP, ".") );
	emit_constant( C_SPEC( all, NV_PPRIV_MASTER_RING_COMMAND_CMD_ENUMERATE_STATIONS_BC_GRP_ALL, "f") );

	emit_register( R_SPEC( command_data, NV_PPRIV_MASTER_RING_COMMAND_DATA ) );

	emit_register( R_SPEC( start_results, NV_PPRIV_MASTER_RING_START_RESULTS ) );
	emit_field   ( F_SPEC( connectivity, NV_PPRIV_MASTER_RING_START_RESULTS_CONNECTIVITY, "v") );
	emit_constant( C_SPEC( pass, NV_PPRIV_MASTER_RING_START_RESULTS_CONNECTIVITY_PASS, "v") );

	emit_register( R_SPEC( intr_status0, NV_PPRIV_MASTER_RING_INTERRUPT_STATUS0 ) );

	emit_register( R_SPEC( intr_status1, NV_PPRIV_MASTER_RING_INTERRUPT_STATUS1 ) );

	emit_register( R_SPEC( global_ctl, NV_PPRIV_MASTER_RING_GLOBAL_CTL ) );
	emit_field   ( F_SPEC( ring_reset, NV_PPRIV_MASTER_RING_GLOBAL_CTL_RING_RESET, ".") );
	emit_constant( C_SPEC( asserted, NV_PPRIV_MASTER_RING_GLOBAL_CTL_RING_RESET_ASSERTED, "f") );
	emit_constant( C_SPEC( deasserted, NV_PPRIV_MASTER_RING_GLOBAL_CTL_RING_RESET_DEASSERTED, "f") );

	emit_register( R_SPEC( enum_fbp, NV_PPRIV_MASTER_RING_ENUMERATE_RESULTS_FBP ) );
	emit_field   ( F_SPEC( count, NV_PPRIV_MASTER_RING_ENUMERATE_RESULTS_FBP_COUNT, "v") );

	emit_register( R_SPEC( enum_gpc, NV_PPRIV_MASTER_RING_ENUMERATE_RESULTS_GPC ) );
	emit_field   ( F_SPEC( count, NV_PPRIV_MASTER_RING_ENUMERATE_RESULTS_GPC_COUNT, "v") );

	end_group();

}


static void emit_pri_ringstation_sys_group()
{
	begin_group( G_SPEC( pri_ringstation_sys ) );

	emit_register(IR_SPEC( master_config, NV_PPRIV_SYS_MASTER_SM_CONFIG) );

	emit_register( R_SPEC( decode_config, NV_PPRIV_SYS_PRIV_DECODE_CONFIG ) );
	emit_field   ( F_SPEC( ring, NV_PPRIV_SYS_PRIV_DECODE_CONFIG_RING, "m") );
	emit_constant( C_SPEC( drop_on_ring_not_started, NV_PPRIV_SYS_PRIV_DECODE_CONFIG_RING_DROP_ON_RING_NOT_STARTED, "f") );

	end_group();
}


static void emit_proj_group()
{
	begin_group( G_SPEC( proj ) );

	emit_constant( C_SPEC( gpc_base,        NV_GPC_PRI_BASE,        "v") );
	emit_constant( C_SPEC( gpc_shared_base, NV_GPC_PRI_SHARED_BASE, "v") );
	emit_constant( C_SPEC( gpc_stride,      NV_GPC_PRI_STRIDE,      "v") );

	emit_constant( C_SPEC( ltc_stride, NV_LTC_PRI_STRIDE, "v") );
	emit_constant( C_SPEC( lts_stride, NV_LTS_PRI_STRIDE, "v") );

	emit_constant( C_SPEC( ppc_in_gpc_base,   NV_PPC_IN_GPC_BASE,   "v") );
	emit_constant( C_SPEC( ppc_in_gpc_stride, NV_PPC_IN_GPC_STRIDE, "v") );

	emit_constant( C_SPEC( rop_base,        NV_ROP_PRI_BASE,        "v") );
	emit_constant( C_SPEC( rop_shared_base, NV_ROP_PRI_SHARED_BASE, "v") );
	emit_constant( C_SPEC( rop_stride,      NV_ROP_PRI_STRIDE,      "v") );


	emit_constant( C_SPEC( tpc_in_gpc_base,        NV_TPC_IN_GPC_BASE,        "v") );
	emit_constant( C_SPEC( tpc_in_gpc_stride,      NV_TPC_IN_GPC_STRIDE,      "v") );
	emit_constant( C_SPEC( tpc_in_gpc_shared_base, NV_TPC_IN_GPC_SHARED_BASE, "v") );

	emit_constant( C_SPEC( host_num_pbdma, NV_HOST_NUM_PBDMA, "v") );

	emit_constant( C_SPEC( scal_litter_num_tpc_per_gpc,  NV_SCAL_LITTER_NUM_TPC_PER_GPC,  "v") );
	emit_constant( C_SPEC( scal_litter_num_fbps,         NV_SCAL_LITTER_NUM_FBPS,         "v") );
	emit_constant( C_SPEC( scal_litter_num_gpcs,         NV_SCAL_LITTER_NUM_GPCS,         "v") );
	emit_constant( C_SPEC( scal_litter_num_pes_per_gpc,  NV_SCAL_LITTER_NUM_PES_PER_GPC,  "v") );
	emit_constant( C_SPEC( scal_litter_num_tpcs_per_pes, NV_SCAL_LITTER_NUM_TPCS_PER_PES, "v") );
	emit_constant( C_SPEC( scal_litter_num_zcull_banks,  NV_SCAL_LITTER_NUM_ZCULL_BANKS,  "v") );

	emit_constant( C_SPEC( scal_max_gpcs,        NV_SCAL_FAMILY_MAX_GPCS,        "v") );
	emit_constant( C_SPEC( scal_max_tpc_per_gpc, NV_SCAL_FAMILY_MAX_TPC_PER_GPC, "v") );

	end_group();

}


static void emit_pwr_group()
{
	begin_group( G_SPEC( pwr ) );
	emit_register( R_SPEC( falcon_irqsset, NV_PPWR_FALCON_IRQSSET ) );
	emit_field   ( F_SPEC( swgen0, NV_PPWR_FALCON_IRQSSET_SWGEN0, ".") );
	emit_constant( C_SPEC( set, NV_PPWR_FALCON_IRQSSET_SWGEN0_SET, "f") );

	emit_register( R_SPEC( falcon_irqsclr, NV_PPWR_FALCON_IRQSCLR ) );

	emit_register( R_SPEC( falcon_irqstat, NV_PPWR_FALCON_IRQSTAT ) );
	emit_field   ( F_SPEC( halt, NV_PPWR_FALCON_IRQSTAT_HALT, ".") );
	emit_constant( C_SPEC( true, NV_PPWR_FALCON_IRQSTAT_HALT_TRUE, "f") );
	emit_field   ( F_SPEC( exterr, NV_PPWR_FALCON_IRQSTAT_EXTERR, ".") );
	emit_constant( C_SPEC( true, NV_PPWR_FALCON_IRQSTAT_EXTERR_TRUE, "f") );
	emit_field   ( F_SPEC( swgen0, NV_PPWR_FALCON_IRQSTAT_SWGEN0, ".") );
	emit_constant( C_SPEC( true, NV_PPWR_FALCON_IRQSTAT_SWGEN0_TRUE, "f") );

	emit_register( R_SPEC( falcon_irqmode, NV_PPWR_FALCON_IRQMODE ) );

	emit_register( R_SPEC( falcon_irqmset, NV_PPWR_FALCON_IRQMSET ) );
	emit_field   ( F_SPEC( gptmr, NV_PPWR_FALCON_IRQMSET_GPTMR, "f") );
	emit_field   ( F_SPEC( wdtmr, NV_PPWR_FALCON_IRQMSET_WDTMR, "f") );
	emit_field   ( F_SPEC( mthd, NV_PPWR_FALCON_IRQMSET_MTHD, "f") );
	emit_field   ( F_SPEC( ctxsw, NV_PPWR_FALCON_IRQMSET_CTXSW, "f") );
	emit_field   ( F_SPEC( halt, NV_PPWR_FALCON_IRQMSET_HALT, "f") );
	emit_field   ( F_SPEC( exterr, NV_PPWR_FALCON_IRQMSET_EXTERR, "f") );
	emit_field   ( F_SPEC( swgen0, NV_PPWR_FALCON_IRQMSET_SWGEN0, "f") );
	emit_field   ( F_SPEC( swgen1, NV_PPWR_FALCON_IRQMSET_SWGEN1, "f") );

	emit_register( R_SPEC( falcon_irqmclr, NV_PPWR_FALCON_IRQMCLR ) );
	emit_field   ( F_SPEC( gptmr, NV_PPWR_FALCON_IRQMCLR_GPTMR, "f") );
	emit_field   ( F_SPEC( wdtmr, NV_PPWR_FALCON_IRQMCLR_WDTMR, "f") );
	emit_field   ( F_SPEC( mthd, NV_PPWR_FALCON_IRQMCLR_MTHD, "f") );
	emit_field   ( F_SPEC( ctxsw, NV_PPWR_FALCON_IRQMCLR_CTXSW, "f") );
	emit_field   ( F_SPEC( halt, NV_PPWR_FALCON_IRQMCLR_HALT, "f") );
	emit_field   ( F_SPEC( exterr, NV_PPWR_FALCON_IRQMCLR_EXTERR, "f") );
	emit_field   ( F_SPEC( swgen0, NV_PPWR_FALCON_IRQMCLR_SWGEN0, "f") );
	emit_field   ( F_SPEC( swgen1, NV_PPWR_FALCON_IRQMCLR_SWGEN1, "f") );
	emit_field   ( F_SPEC( ext, NV_PPWR_FALCON_IRQMCLR_EXT, "f") );

	emit_register( R_SPEC( falcon_irqmask, NV_PPWR_FALCON_IRQMASK ) );

	emit_register( R_SPEC( falcon_irqdest, NV_PPWR_FALCON_IRQDEST ) );
	emit_field   ( F_SPEC( host_gptmr, NV_PPWR_FALCON_IRQDEST_HOST_GPTMR, "f") );
	emit_field   ( F_SPEC( host_wdtmr, NV_PPWR_FALCON_IRQDEST_HOST_WDTMR, "f") );
	emit_field   ( F_SPEC( host_mthd, NV_PPWR_FALCON_IRQDEST_HOST_MTHD, "f") );
	emit_field   ( F_SPEC( host_ctxsw, NV_PPWR_FALCON_IRQDEST_HOST_CTXSW, "f") );
	emit_field   ( F_SPEC( host_halt, NV_PPWR_FALCON_IRQDEST_HOST_HALT, "f") );
	emit_field   ( F_SPEC( host_exterr, NV_PPWR_FALCON_IRQDEST_HOST_EXTERR, "f") );
	emit_field   ( F_SPEC( host_swgen0, NV_PPWR_FALCON_IRQDEST_HOST_SWGEN0, "f") );
	emit_field   ( F_SPEC( host_swgen1, NV_PPWR_FALCON_IRQDEST_HOST_SWGEN1, "f") );
	emit_field   ( F_SPEC( host_ext, NV_PPWR_FALCON_IRQDEST_HOST_EXT, "f") );
	emit_field   ( F_SPEC( target_gptmr, NV_PPWR_FALCON_IRQDEST_TARGET_GPTMR, "f") );
	emit_field   ( F_SPEC( target_wdtmr, NV_PPWR_FALCON_IRQDEST_TARGET_WDTMR, "f") );
	emit_field   ( F_SPEC( target_mthd, NV_PPWR_FALCON_IRQDEST_TARGET_MTHD, "f") );
	emit_field   ( F_SPEC( target_ctxsw, NV_PPWR_FALCON_IRQDEST_TARGET_CTXSW, "f") );
	emit_field   ( F_SPEC( target_halt, NV_PPWR_FALCON_IRQDEST_TARGET_HALT, "f") );
	emit_field   ( F_SPEC( target_exterr, NV_PPWR_FALCON_IRQDEST_TARGET_EXTERR, "f") );
	emit_field   ( F_SPEC( target_swgen0, NV_PPWR_FALCON_IRQDEST_TARGET_SWGEN0, "f") );
	emit_field   ( F_SPEC( target_swgen1, NV_PPWR_FALCON_IRQDEST_TARGET_SWGEN1, "f") );
	emit_field   ( F_SPEC( target_ext, NV_PPWR_FALCON_IRQDEST_TARGET_EXT, "f") );

	emit_register( R_SPEC( falcon_curctx, NV_PPWR_FALCON_CURCTX ) );

	emit_register( R_SPEC( falcon_nxtctx, NV_PPWR_FALCON_NXTCTX ) );

	emit_register( R_SPEC( falcon_mailbox0, NV_PPWR_FALCON_MAILBOX0 ) );

	emit_register( R_SPEC( falcon_mailbox1, NV_PPWR_FALCON_MAILBOX1 ) );

	emit_register( R_SPEC( falcon_itfen, NV_PPWR_FALCON_ITFEN ) );
	emit_field   ( F_SPEC( ctxen, NV_PPWR_FALCON_ITFEN_CTXEN, ".") );
	emit_constant( C_SPEC( enable, NV_PPWR_FALCON_ITFEN_CTXEN_ENABLE, "f") );

	emit_register( R_SPEC( falcon_idlestate, NV_PPWR_FALCON_IDLESTATE ) );
	emit_field   ( F_SPEC( falcon_busy, NV_PPWR_FALCON_IDLESTATE_FALCON_BUSY, "v") );
	emit_field   ( F_SPEC( ext_busy, NV_PPWR_FALCON_IDLESTATE_EXT_BUSY, "v") );

	emit_register( R_SPEC( falcon_os, NV_PPWR_FALCON_OS ) );

	emit_register( R_SPEC( falcon_engctl, NV_PPWR_FALCON_ENGCTL ) );

	emit_register( R_SPEC( falcon_cpuctl, NV_PPWR_FALCON_CPUCTL ) );
	emit_field   ( F_SPEC( startcpu, NV_PPWR_FALCON_CPUCTL_STARTCPU, "f") );
	emit_field   ( F_SPEC( halt_intr, NV_PPWR_FALCON_CPUCTL_HALTED, "fvm") );

	emit_register( IR_SPEC( falcon_imemc, NV_PPWR_FALCON_IMEMC ) );
	emit_field   ( F_SPEC( offs, NV_PPWR_FALCON_IMEMC_OFFS, "f") );
	emit_field   ( F_SPEC( blk, NV_PPWR_FALCON_IMEMC_BLK, "f") );
	emit_field   ( F_SPEC( aincw, NV_PPWR_FALCON_IMEMC_AINCW, "f") );

	emit_register( IR_SPEC( falcon_imemd, NV_PPWR_FALCON_IMEMD ) );

	emit_register( IR_SPEC( falcon_imemt, NV_PPWR_FALCON_IMEMT ) );


	emit_register( R_SPEC( falcon_bootvec, NV_PPWR_FALCON_BOOTVEC ) );
	emit_field   ( F_SPEC( vec, NV_PPWR_FALCON_BOOTVEC_VEC, "f") );

	emit_register( R_SPEC( falcon_dmactl, NV_PPWR_FALCON_DMACTL ) );
	emit_field   ( F_SPEC( dmem_scrubbing, NV_PPWR_FALCON_DMACTL_DMEM_SCRUBBING, "m") );
	emit_field   ( F_SPEC( imem_scrubbing, NV_PPWR_FALCON_DMACTL_IMEM_SCRUBBING, "m") );

	emit_register( R_SPEC( falcon_hwcfg, NV_PPWR_FALCON_HWCFG ) );
	emit_field   ( F_SPEC( imem_size, NV_PPWR_FALCON_HWCFG_IMEM_SIZE, "v") );
	emit_field   ( F_SPEC( dmem_size, NV_PPWR_FALCON_HWCFG_DMEM_SIZE, "v") );

	emit_register( R_SPEC( falcon_dmatrfbase, NV_PPWR_FALCON_DMATRFBASE ) );

	emit_register( R_SPEC( falcon_dmatrfmoffs, NV_PPWR_FALCON_DMATRFMOFFS ) );

	emit_register( R_SPEC( falcon_dmatrfcmd, NV_PPWR_FALCON_DMATRFCMD ) );
	emit_field   ( F_SPEC( imem, NV_PPWR_FALCON_DMATRFCMD_IMEM, "f") );
	emit_field   ( F_SPEC( write, NV_PPWR_FALCON_DMATRFCMD_WRITE, "f") );
	emit_field   ( F_SPEC( size, NV_PPWR_FALCON_DMATRFCMD_SIZE, "f") );
	emit_field   ( F_SPEC( ctxdma, NV_PPWR_FALCON_DMATRFCMD_CTXDMA, "f") );

	emit_register( R_SPEC( falcon_dmatrffboffs, NV_PPWR_FALCON_DMATRFFBOFFS ) );

	emit_register( R_SPEC( falcon_exterraddr, NV_PPWR_FALCON_EXTERRADDR ) );

	emit_register( R_SPEC( falcon_exterrstat, NV_PPWR_FALCON_EXTERRSTAT ) );
	emit_field   ( F_SPEC( valid, NV_PPWR_FALCON_EXTERRSTAT_VALID, "mv") );
	emit_constant( C_SPEC( true, NV_PPWR_FALCON_EXTERRSTAT_VALID_TRUE, "v") );

	emit_register( R_SPEC( pmu_falcon_icd_cmd, NV_PPWR_FALCON_ICD_CMD ) );
	emit_field   ( F_SPEC( opc, NV_PPWR_FALCON_ICD_CMD_OPC, "") );
	emit_constant( C_SPEC( rreg, NV_PPWR_FALCON_ICD_CMD_OPC_RREG, "f") );
	emit_constant( C_SPEC( rstat, NV_PPWR_FALCON_ICD_CMD_OPC_RSTAT, "f") );
	 emit_field   ( F_SPEC( idx, NV_PPWR_FALCON_ICD_CMD_IDX, "f") );

	emit_register( R_SPEC( pmu_falcon_icd_rdata, NV_PPWR_FALCON_ICD_RDATA ) );

	emit_register( IR_SPEC( falcon_dmemc, NV_PPWR_FALCON_DMEMC ) );
	emit_field   ( F_SPEC( offs, NV_PPWR_FALCON_DMEMC_OFFS, "fm") );
	emit_field   ( F_SPEC( blk, NV_PPWR_FALCON_DMEMC_BLK, "fm") );
	emit_field   ( F_SPEC( aincw, NV_PPWR_FALCON_DMEMC_AINCW, "f") );
	emit_field   ( F_SPEC( aincr, NV_PPWR_FALCON_DMEMC_AINCR, "f") );

	emit_register( IR_SPEC( falcon_dmemd, NV_PPWR_FALCON_DMEMD ) );

	emit_register( R_SPEC( pmu_new_instblk, NV_PPWR_PMU_NEW_INSTBLK ) );
	emit_field   ( F_SPEC( ptr, NV_PPWR_PMU_NEW_INSTBLK_PTR, "f") );
	emit_field   ( F_SPEC( target, NV_PPWR_PMU_NEW_INSTBLK_TARGET, ".") );
	emit_constant( C_SPEC( fb, NV_PPWR_PMU_NEW_INSTBLK_TARGET_FB, "f") );
	emit_constant( C_SPEC( sys_coh, NV_PPWR_PMU_NEW_INSTBLK_TARGET_SYS_COH, "f") );
	emit_field   ( F_SPEC( valid, NV_PPWR_PMU_NEW_INSTBLK_VALID, "f") );

	emit_register( R_SPEC( pmu_mutex_id, NV_PPWR_PMU_MUTEX_ID ) );
	emit_field   ( F_SPEC( value, NV_PPWR_PMU_MUTEX_ID_VALUE, "v") );
	emit_constant( C_SPEC( init, NV_PPWR_PMU_MUTEX_ID_VALUE_INIT, "v") );
	emit_constant( C_SPEC( not_avail, NV_PPWR_PMU_MUTEX_ID_VALUE_NOT_AVAIL, "v") );

	emit_register( R_SPEC( pmu_mutex_id_release, NV_PPWR_PMU_MUTEX_ID_RELEASE ) );
	emit_field   ( F_SPEC( value, NV_PPWR_PMU_MUTEX_ID_RELEASE_VALUE, "fm") );
	emit_constant( C_SPEC( init, NV_PPWR_PMU_MUTEX_ID_RELEASE_VALUE_INIT, "") );

	emit_register( IR_SPEC( pmu_mutex, NV_PPWR_PMU_MUTEX ) );
	emit_constant( C_SPEC( _size_1, NV_PPWR_PMU_MUTEX__SIZE_1, "v") );
	emit_field   ( F_SPEC( value, NV_PPWR_PMU_MUTEX_VALUE, "fv") );
	emit_constant( C_SPEC( initial_lock, NV_PPWR_PMU_MUTEX_VALUE_INITIAL_LOCK, "f") );

	emit_register( IR_SPEC( pmu_queue_head, NV_PPWR_PMU_QUEUE_HEAD ) );
	emit_constant( C_SPEC( _size_1, NV_PPWR_PMU_QUEUE_HEAD__SIZE_1, "v") );
	emit_field   ( F_SPEC( address, NV_PPWR_PMU_QUEUE_HEAD_ADDRESS, "fv") );

	emit_register( IR_SPEC( pmu_queue_tail, NV_PPWR_PMU_QUEUE_TAIL ) );
	emit_constant( C_SPEC( _size_1, NV_PPWR_PMU_QUEUE_TAIL__SIZE_1, "v") );
	emit_field   ( F_SPEC( address, NV_PPWR_PMU_QUEUE_TAIL_ADDRESS, "fv") );

	emit_register( R_SPEC( pmu_msgq_head, NV_PPWR_PMU_MSGQ_HEAD ) );
	emit_field   ( F_SPEC( val, NV_PPWR_PMU_MSGQ_HEAD_VAL, "fv") );

	emit_register( R_SPEC( pmu_msgq_tail, NV_PPWR_PMU_MSGQ_TAIL ) );
	emit_field   ( F_SPEC( val, NV_PPWR_PMU_MSGQ_TAIL_VAL, "fv") );

	emit_register( IR_SPEC( pmu_idle_mask, NV_PPWR_PMU_IDLE_MASK ) );
	emit_field   ( F_SPEC( gr, NV_PPWR_PMU_IDLE_MASK_GR, ".") );
	emit_constant( C_SPEC( enabled, NV_PPWR_PMU_IDLE_MASK_GR_ENABLED, "f") );
	emit_field   ( F_SPEC( ce_2, NV_PPWR_PMU_IDLE_MASK_CE_2, ".") );
	emit_constant( C_SPEC( enabled, NV_PPWR_PMU_IDLE_MASK_CE_2_ENABLED, "f") );

	emit_register( IR_SPEC( pmu_idle_count, NV_PPWR_PMU_IDLE_COUNT ) );
	emit_field   ( F_SPEC( value, NV_PPWR_PMU_IDLE_COUNT_VALUE, "fv") );
	emit_field   ( F_SPEC( reset, NV_PPWR_PMU_IDLE_COUNT_RESET, "f") );

	emit_register( IR_SPEC( pmu_idle_ctrl, NV_PPWR_PMU_IDLE_CTRL ) );
	emit_field   ( F_SPEC( value, NV_PPWR_PMU_IDLE_CTRL_VALUE, "m") );
	emit_constant( C_SPEC( busy, NV_PPWR_PMU_IDLE_CTRL_VALUE_BUSY, "f") );
	emit_constant( C_SPEC( always, NV_PPWR_PMU_IDLE_CTRL_VALUE_ALWAYS, "f") );
	emit_field   ( F_SPEC( filter, NV_PPWR_PMU_IDLE_CTRL_FILTER, "m") );
	emit_constant( C_SPEC( disabled, NV_PPWR_PMU_IDLE_CTRL_FILTER_DISABLED, "f") );
	emit_register( IR_SPEC( pmu_idle_mask_supp, NV_PPWR_PMU_IDLE_MASK_SUPP ) );

	emit_register( IR_SPEC( pmu_idle_mask_1_supp, NV_PPWR_PMU_IDLE_MASK_1_SUPP ) );

	emit_register( IR_SPEC( pmu_idle_ctrl_supp, NV_PPWR_PMU_IDLE_CTRL_SUPP ) );

	emit_register( IR_SPEC( pmu_debug, NV_PPWR_PMU_DEBUG ) );
	emit_constant( C_SPEC( _size_1, NV_PPWR_PMU_DEBUG__SIZE_1, "v") );

	emit_register( IR_SPEC( pmu_mailbox, NV_PPWR_PMU_MAILBOX ) );
	emit_constant( C_SPEC( _size_1, NV_PPWR_PMU_MAILBOX__SIZE_1, "v") );

	emit_register( R_SPEC( pmu_bar0_addr, NV_PPWR_PMU_BAR0_ADDR ) );

	emit_register( R_SPEC( pmu_bar0_data, NV_PPWR_PMU_BAR0_DATA ) );

	emit_register( R_SPEC( pmu_bar0_ctl, NV_PPWR_PMU_BAR0_CTL ) );

	emit_register( R_SPEC( pmu_bar0_timeout, NV_PPWR_PMU_BAR0_TIMEOUT ) );

	emit_register( R_SPEC( pmu_bar0_fecs_error, NV_PPWR_PMU_BAR0_FECS_ERROR ) );

	emit_register( R_SPEC( pmu_bar0_error_status, NV_PPWR_PMU_BAR0_ERROR_STATUS ) );

	emit_register( IR_SPEC( pmu_pg_idlefilth, NV_PPWR_PMU_PG_IDLEFILTH ) );

	emit_register( IR_SPEC( pmu_pg_ppuidlefilth, NV_PPWR_PMU_PG_PPUIDLEFILTH ) );

	emit_register( IR_SPEC( pmu_pg_idle_cnt, NV_PPWR_PMU_PG_IDLE_CNT ) );

	emit_register( IR_SPEC( pmu_pg_intren, NV_PPWR_PMU_PG_INTREN ) );

	emit_register( IR_SPEC( fbif_transcfg, NV_PPWR_FBIF_TRANSCFG ) );
	emit_field   ( F_SPEC( target, NV_PPWR_FBIF_TRANSCFG_TARGET, ".") );
	emit_constant( C_SPEC( local_fb, NV_PPWR_FBIF_TRANSCFG_TARGET_LOCAL_FB, "f") );
	emit_constant( C_SPEC( coherent_sysmem, NV_PPWR_FBIF_TRANSCFG_TARGET_COHERENT_SYSMEM, "f") );
	emit_constant( C_SPEC( noncoherent_sysmem, NV_PPWR_FBIF_TRANSCFG_TARGET_NONCOHERENT_SYSMEM, "f") );
	emit_field   ( F_SPEC( mem_type, NV_PPWR_FBIF_TRANSCFG_MEM_TYPE, "") );
	emit_constant( C_SPEC( virtual, NV_PPWR_FBIF_TRANSCFG_MEM_TYPE_VIRTUAL, "f") );
	emit_constant( C_SPEC( physical, NV_PPWR_FBIF_TRANSCFG_MEM_TYPE_PHYSICAL, "f") );
	end_group();

}


static void emit_ram_group()
{
	begin_group( G_SPEC( ram ) );

	begin_scope( S_SPEC( in ) ); /* ram_in */
	emit_field   (WF_SPEC( ramfc, NV_RAMIN_RAMFC, "w") );
	emit_field   (WF_SPEC( page_dir_base_target, NV_RAMIN_PAGE_DIR_BASE_TARGET, "fw") );
	emit_constant( C_SPEC( vid_mem, NV_RAMIN_PAGE_DIR_BASE_TARGET_VID_MEM, "f") );
	emit_field   (WF_SPEC( page_dir_base_vol, NV_RAMIN_PAGE_DIR_BASE_VOL, "." ) );
	emit_constant( C_SPEC( true, NV_RAMIN_PAGE_DIR_BASE_VOL_TRUE, "f") );
	emit_field   (WF_SPEC( page_dir_base_lo, NV_RAMIN_PAGE_DIR_BASE_LO, "fw" ) );
	emit_field   (WF_SPEC( page_dir_base_hi, NV_RAMIN_PAGE_DIR_BASE_HI, "fw") );
	emit_field   (WF_SPEC( adr_limit_lo, NV_RAMIN_ADR_LIMIT_LO, "fw") );
	emit_field   ( F_SPEC( adr_limit_hi, NV_RAMIN_ADR_LIMIT_HI, "fw") );
	emit_field   (WF_SPEC( engine_cs, NV_RAMIN_ENGINE_CS, "") );
	emit_constant( C_SPEC( wfi, NV_RAMIN_ENGINE_CS_WFI, "") );
	emit_constant( C_SPEC( fg, NV_RAMIN_ENGINE_CS_FG, "") );
	emit_field   (WF_SPEC( gr_cs, NV_RAMIN_GR_CS, "") );
	emit_constant( C_SPEC( wfi, NV_RAMIN_GR_CS_WFI, "f") );
	emit_field   (WF_SPEC( gr_wfi_target, NV_RAMIN_GR_WFI_TARGET, "") );
	emit_field   (WF_SPEC( gr_wfi_mode, NV_RAMIN_GR_WFI_MODE, "") );
	emit_constant( C_SPEC( physical, NV_RAMIN_GR_WFI_MODE_PHYSICAL, "") );
	emit_constant( C_SPEC( virtual, NV_RAMIN_GR_WFI_MODE_VIRTUAL, "") );
	emit_field   (WF_SPEC( gr_wfi_ptr_lo, NV_RAMIN_GR_WFI_PTR_LO, "f") );
	emit_field   (WF_SPEC( gr_wfi_ptr_hi, NV_RAMIN_GR_WFI_PTR_HI, "f") );
	end_field();
	emit_constant( C_SPEC( base_shift, NV_RAMIN_BASE_SHIFT, "v") );
	emit_constant( C_SPEC( alloc_size, NV_RAMIN_ALLOC_SIZE, "v") );
	end_scope(); /* ram_in */

	begin_scope( S_SPEC( fc ) );
	emit_constant( C_SPEC( size_val, NV_RAMFC_SIZE_VAL, "v") );
	emit_field   (WF_SPEC( gp_put, NV_RAMFC_GP_PUT, "") );
	emit_field   (WF_SPEC( userd, NV_RAMFC_USERD, "") );
	emit_field   (WF_SPEC( userd_hi, NV_RAMFC_USERD_HI, "") );
	emit_field   (WF_SPEC( signature, NV_RAMFC_SIGNATURE, "") );
	emit_field   (WF_SPEC( gp_get, NV_RAMFC_GP_GET, "") );
	emit_field   (WF_SPEC( pb_get, NV_RAMFC_PB_GET, "") );
	emit_field   (WF_SPEC( pb_get_hi, NV_RAMFC_PB_GET_HI, "") );
	emit_field   (WF_SPEC( pb_top_level_get, NV_RAMFC_PB_TOP_LEVEL_GET, "") );
	emit_field   (WF_SPEC( pb_top_level_get_hi, NV_RAMFC_PB_TOP_LEVEL_GET_HI, "") );
	emit_field   (WF_SPEC( acquire, NV_RAMFC_ACQUIRE, "") );
	emit_field   (WF_SPEC( semaphorea, NV_RAMFC_SEMAPHOREA, "") );
	emit_field   (WF_SPEC( semaphoreb, NV_RAMFC_SEMAPHOREB, "") );
	emit_field   (WF_SPEC( semaphorec, NV_RAMFC_SEMAPHOREC, "") );
	emit_field   (WF_SPEC( semaphored, NV_RAMFC_SEMAPHORED, "") );
	emit_field   (WF_SPEC( gp_base, NV_RAMFC_GP_BASE, "") );
	emit_field   (WF_SPEC( gp_base_hi, NV_RAMFC_GP_BASE_HI, "") );
	emit_field   (WF_SPEC( gp_fetch, NV_RAMFC_GP_FETCH, "") );
	emit_field   (WF_SPEC( pb_fetch, NV_RAMFC_PB_FETCH, "") );
	emit_field   (WF_SPEC( pb_fetch_hi, NV_RAMFC_PB_FETCH_HI, "") );
	emit_field   (WF_SPEC( pb_put, NV_RAMFC_PB_PUT, "") );
	emit_field   (WF_SPEC( pb_put_hi, NV_RAMFC_PB_PUT_HI, "") );
	emit_field   (WF_SPEC( pb_header, NV_RAMFC_PB_HEADER, "") );
	emit_field   (WF_SPEC( pb_count, NV_RAMFC_PB_COUNT, "") );
	emit_field   (WF_SPEC( subdevice, NV_RAMFC_SUBDEVICE, "") );
	emit_field   (WF_SPEC( formats, NV_RAMFC_FORMATS, "") );

	emit_field   (WF_SPEC( syncpointa, NV_RAMFC_SYNCPOINTA, "") );
	emit_field   (WF_SPEC( syncpointb, NV_RAMFC_SYNCPOINTB, "") );
	emit_field   (WF_SPEC( target, NV_RAMFC_TARGET, "") );
	emit_field   (WF_SPEC( hce_ctrl, NV_RAMFC_HCE_CTRL, "") );
	emit_field   (WF_SPEC( chid, NV_RAMFC_CHID, "") );
	emit_field   (WF_SPEC( chid_id, NV_RAMFC_CHID_ID, "f") );
	emit_field   (WF_SPEC( runlist_timeslice, NV_RAMFC_RUNLIST_TIMESLICE, "") );
	emit_field   (WF_SPEC( pb_timeslice, NV_RAMFC_PB_TIMESLICE, "") );
	end_scope(); /* ram_fc */

	begin_scope( S_SPEC( userd ) );
	emit_constant( C_SPEC( base_shift, NV_RAMUSERD_BASE_SHIFT, "v") );
	emit_constant( C_SPEC( chan_size, NV_RAMUSERD_CHAN_SIZE, "v") );
	emit_field   (WF_SPEC( put, NV_RAMUSERD_PUT, "") );
	emit_field   (WF_SPEC( get, NV_RAMUSERD_GET, "") );
	emit_field   (WF_SPEC( ref, NV_RAMUSERD_REF, "") );
	emit_field   (WF_SPEC( put_hi, NV_RAMUSERD_PUT_HI, "") );
	emit_field   (WF_SPEC( ref_threshold, NV_RAMUSERD_REF_THRESHOLD, "") );
	emit_field   (WF_SPEC( top_level_get, NV_RAMUSERD_TOP_LEVEL_GET, "") );
	emit_field   (WF_SPEC( top_level_get_hi, NV_RAMUSERD_TOP_LEVEL_GET_HI, "") );
	emit_field   (WF_SPEC( get_hi, NV_RAMUSERD_GET_HI, "") );
	emit_field   (WF_SPEC( gp_get, NV_RAMUSERD_GP_GET, "") );
	emit_field   (WF_SPEC( gp_put, NV_RAMUSERD_GP_PUT, "") );
	emit_field   (WF_SPEC( gp_top_level_get, NV_RAMUSERD_GP_TOP_LEVEL_GET, "") );
	emit_field   (WF_SPEC( gp_top_level_get_hi, NV_RAMUSERD_GP_TOP_LEVEL_GET_HI, "") );
	end_scope(); /* ram_userd */

	begin_scope( S_SPEC( rl ) );
	emit_constant( C_SPEC( entry_size, NV_RAMRL_ENTRY_SIZE, "v") );
	emit_field   (F_SPEC( entry_chid, NV_RAMRL_ENTRY_CHID, "f") );
	emit_field   (F_SPEC( entry_id, NV_RAMRL_ENTRY_ID, "f") );
	emit_field   (F_SPEC( entry_type, NV_RAMRL_ENTRY_TYPE, "f") );
	emit_constant( C_SPEC( chid, NV_RAMRL_ENTRY_TYPE_CHID, "f") );
	emit_constant( C_SPEC( tsg, NV_RAMRL_ENTRY_TYPE_TSG, "f") );
	emit_field   (F_SPEC( entry_timeslice_scale, NV_RAMRL_ENTRY_TIMESLICE_SCALE, "f") );
	emit_constant( C_SPEC( 3, NV_RAMRL_ENTRY_TIMESLICE_SCALE_3, "f") );
	emit_field   (F_SPEC( entry_timeslice_timeout, NV_RAMRL_ENTRY_TIMESLICE_TIMEOUT, "f") );
	emit_constant( C_SPEC( 128, NV_RAMRL_ENTRY_TIMESLICE_TIMEOUT_128, "f") );
	emit_field   (F_SPEC( entry_tsg_length, NV_RAMRL_ENTRY_TSG_LENGTH, "f") );
	end_scope(); /* ram_rl */

	end_group();
}


static void emit_therm_group()
{
	begin_group( G_SPEC( therm ) );
	emit_register( R_SPEC( use_a, NV_THERM_USE_A ) );

	emit_register( R_SPEC( evt_ext_therm_0, NV_THERM_EVT_EXT_THERM_0 ) );

	emit_register( R_SPEC( evt_ext_therm_1, NV_THERM_EVT_EXT_THERM_1 ) );

	emit_register( R_SPEC( evt_ext_therm_2, NV_THERM_EVT_EXT_THERM_2 ) );

	emit_register( R_SPEC( weight_1, NV_THERM_WEIGHT_1 ) );

	emit_register( R_SPEC( config1, NV_THERM_CONFIG1 ) );

	emit_register( IR_SPEC( gate_ctrl, NV_THERM_GATE_CTRL ) );

	emit_field   ( F_SPEC( eng_clk, NV_THERM_GATE_CTRL_ENG_CLK, "m") );
	emit_constant( C_SPEC( run, NV_THERM_GATE_CTRL_ENG_CLK_RUN, "f") );
	emit_constant( C_SPEC( auto, NV_THERM_GATE_CTRL_ENG_CLK_AUTO, "f") );
	emit_constant( C_SPEC( stop, NV_THERM_GATE_CTRL_ENG_CLK_STOP, "f") );

	emit_field   ( F_SPEC( blk_clk, NV_THERM_GATE_CTRL_BLK_CLK, "m") );
	emit_constant( C_SPEC( run, NV_THERM_GATE_CTRL_BLK_CLK_RUN, "f") );
	emit_constant( C_SPEC( auto, NV_THERM_GATE_CTRL_BLK_CLK_AUTO, "f") );

	emit_field   ( F_SPEC( eng_pwr, NV_THERM_GATE_CTRL_ENG_PWR, "m") );
	emit_constant( C_SPEC( auto, NV_THERM_GATE_CTRL_ENG_PWR_AUTO, "fm") );
	emit_constant( C_SPEC( off, NV_THERM_GATE_CTRL_ENG_PWR_OFF, "") );

	emit_field   ( F_SPEC( eng_idle_filt_exp, NV_THERM_GATE_CTRL_ENG_IDLE_FILT_EXP, "fm") );
	emit_field   ( F_SPEC( eng_idle_filt_mant, NV_THERM_GATE_CTRL_ENG_IDLE_FILT_MANT, "fm") );
	emit_field   ( F_SPEC( eng_delay_after, NV_THERM_GATE_CTRL_ENG_DELAY_AFTER, "fm") );

	emit_register( R_SPEC( fecs_idle_filter, NV_THERM_FECS_IDLE_FILTER ) );
	emit_field   ( F_SPEC( value, NV_THERM_FECS_IDLE_FILTER_VALUE, "m") );

	emit_register( R_SPEC( hubmmu_idle_filter, NV_THERM_HUBMMU_IDLE_FILTER ) );
	emit_field   ( F_SPEC( value, NV_THERM_HUBMMU_IDLE_FILTER_VALUE, "m") );

	emit_register( IR_SPEC( clk_slowdown, NV_THERM_CLK_SLOWDOWN_0 ) );
	emit_field   (  F_SPEC( idle_factor, NV_THERM_CLK_SLOWDOWN_0_IDLE_FACTOR, "fmv" ) );
	emit_constant( C_SPEC( disabled, NV_THERM_CLK_SLOWDOWN_0_IDLE_FACTOR_DISABLED, "f") );

	end_group();

}


static void emit_top_group()
{
	begin_group( G_SPEC( top ) );
	emit_register( R_SPEC( num_gpcs, NV_PTOP_SCAL_NUM_GPCS ) );
	emit_field   ( F_SPEC( value, NV_PTOP_SCAL_NUM_GPCS_VALUE, "v") );

	emit_register( R_SPEC( tpc_per_gpc, NV_PTOP_SCAL_NUM_TPC_PER_GPC ) );
	emit_field   ( F_SPEC( value, NV_PTOP_SCAL_NUM_TPC_PER_GPC_VALUE, "v") );

	emit_register( R_SPEC( num_fbps, NV_PTOP_SCAL_NUM_FBPS ) );
	emit_field   ( F_SPEC( value, NV_PTOP_SCAL_NUM_FBPS_VALUE, "v") );

	emit_register( IR_SPEC( device_info, NV_PTOP_DEVICE_INFO ) );
	emit_constant( C_SPEC( _size_1, NV_PTOP_DEVICE_INFO__SIZE_1, "v") );
	emit_field   ( F_SPEC( chain, NV_PTOP_DEVICE_INFO_CHAIN, "v") );
	emit_constant( C_SPEC( enable, NV_PTOP_DEVICE_INFO_CHAIN_ENABLE, "v") );
	emit_field   ( F_SPEC( engine_enum, NV_PTOP_DEVICE_INFO_ENGINE_ENUM, "v") );
	emit_field   ( F_SPEC( runlist_enum, NV_PTOP_DEVICE_INFO_RUNLIST_ENUM, "v") );
	emit_field   ( F_SPEC( intr_enum, NV_PTOP_DEVICE_INFO_INTR_ENUM, "v") );
	emit_field   ( F_SPEC( reset_enum, NV_PTOP_DEVICE_INFO_RESET_ENUM, "v") );
	emit_field   ( F_SPEC( type_enum, NV_PTOP_DEVICE_INFO_TYPE_ENUM, "v") );
	emit_constant( C_SPEC( graphics, NV_PTOP_DEVICE_INFO_TYPE_ENUM_GRAPHICS, "") );
	emit_constant( C_SPEC( copy0, NV_PTOP_DEVICE_INFO_TYPE_ENUM_COPY0, "") );
	emit_field   ( F_SPEC( entry, NV_PTOP_DEVICE_INFO_ENTRY, "v") );
	emit_constant( C_SPEC( not_valid, NV_PTOP_DEVICE_INFO_ENTRY_NOT_VALID, "v") );
	emit_constant( C_SPEC( enum, NV_PTOP_DEVICE_INFO_ENTRY_ENUM, "v") );

	emit_register( R_SPEC( fs_status_fbp, NV_PTOP_FS_STATUS_FBP ) );
	emit_field   ( F_SPEC( cluster, NV_PTOP_FS_STATUS_FBP_CLUSTER, "v") );
	emit_constant( C_SPEC( enable, NV_PTOP_FS_STATUS_FBP_CLUSTER_ENABLE, "vf") );
	emit_constant( C_SPEC( disable, NV_PTOP_FS_STATUS_FBP_CLUSTER_DISABLE, "vf") );

	end_group();

}


static void emit_trim_group()
{
	begin_group( G_SPEC( trim ) );
	emit_register( R_SPEC( sys_gpcpll_cfg, NV_PTRIM_SYS_GPCPLL_CFG ) );
	emit_field   ( F_SPEC( enable, NV_PTRIM_SYS_GPCPLL_CFG_ENABLE, "mv") );
	emit_constant( C_SPEC( no, NV_PTRIM_SYS_GPCPLL_CFG_ENABLE_NO, "f") );
	emit_constant( C_SPEC( yes, NV_PTRIM_SYS_GPCPLL_CFG_ENABLE_YES, "f") );
	emit_field   ( F_SPEC( iddq, NV_PTRIM_SYS_GPCPLL_CFG_IDDQ, "mv") );
	emit_constant( C_SPEC( power_on, NV_PTRIM_SYS_GPCPLL_CFG_IDDQ_POWER_ON, "v") );
	emit_field   ( F_SPEC( enb_lckdet, NV_PTRIM_SYS_GPCPLL_CFG_ENB_LCKDET, "m") );
	emit_constant( C_SPEC( power_on, NV_PTRIM_SYS_GPCPLL_CFG_ENB_LCKDET_POWER_ON, "f") );
	emit_constant( C_SPEC( power_off, NV_PTRIM_SYS_GPCPLL_CFG_ENB_LCKDET_POWER_OFF, "f") );
	emit_field   ( F_SPEC( pll_lock, NV_PTRIM_SYS_GPCPLL_CFG_PLL_LOCK, "v") );
	emit_constant( C_SPEC( true, NV_PTRIM_SYS_GPCPLL_CFG_PLL_LOCK_TRUE, "f") );

	emit_register( R_SPEC( sys_gpcpll_coeff, NV_PTRIM_SYS_GPCPLL_COEFF ) );
	emit_field   ( F_SPEC( mdiv, NV_PTRIM_SYS_GPCPLL_COEFF_MDIV, "fmv") );
	emit_field   ( F_SPEC( ndiv, NV_PTRIM_SYS_GPCPLL_COEFF_NDIV, "fmv") );
	emit_field   ( F_SPEC( pldiv, NV_PTRIM_SYS_GPCPLL_COEFF_PLDIV, "fmv") );

	emit_register( R_SPEC( sys_sel_vco, NV_PTRIM_SYS_SEL_VCO ) );
	emit_field   ( F_SPEC( gpc2clk_out, NV_PTRIM_SYS_SEL_VCO_GPC2CLK_OUT, "m") );
	emit_constant( C_SPEC( init, NV_PTRIM_SYS_SEL_VCO_GPC2CLK_OUT_INIT, "") );
	emit_constant( C_SPEC( bypass, NV_PTRIM_SYS_SEL_VCO_GPC2CLK_OUT_BYPASS, "f") );
	emit_constant( C_SPEC( vco, NV_PTRIM_SYS_SEL_VCO_GPC2CLK_OUT_VCO, "f") );

	emit_register( R_SPEC( sys_gpc2clk_out, NV_PTRIM_SYS_GPC2CLK_OUT ) );
	emit_field   ( F_SPEC( bypdiv, NV_PTRIM_SYS_GPC2CLK_OUT_BYPDIV, "") );
	emit_constant( C_SPEC( by31, NV_PTRIM_SYS_GPC2CLK_OUT_BYPDIV_BY31, "f") );
	emit_field   ( F_SPEC( vcodiv, NV_PTRIM_SYS_GPC2CLK_OUT_VCODIV, "") );
	emit_constant( C_SPEC( by1, NV_PTRIM_SYS_GPC2CLK_OUT_VCODIV_BY1, "f") );
	emit_field   ( F_SPEC( sdiv14, NV_PTRIM_SYS_GPC2CLK_OUT_SDIV14, "m") );
	emit_constant( C_SPEC( indiv4_mode, NV_PTRIM_SYS_GPC2CLK_OUT_SDIV14_INDIV4_MODE, "f") );

	emit_register( IR_SPEC( gpc_clk_cntr_ncgpcclk_cfg, NV_PTRIM_GPC_CLK_CNTR_NCGPCCLK_CFG ) );
	emit_field   (  F_SPEC( noofipclks, NV_PTRIM_GPC_CLK_CNTR_NCGPCCLK_CFG_NOOFIPCLKS, "f" ) );
	emit_field   (  F_SPEC( write_en, NV_PTRIM_GPC_CLK_CNTR_NCGPCCLK_CFG_WRITE_EN, "." ) );
	emit_constant(  C_SPEC( asserted, NV_PTRIM_GPC_CLK_CNTR_NCGPCCLK_CFG_WRITE_EN_ASSERTED, "f" ) );
	emit_field   (  F_SPEC( enable,   NV_PTRIM_GPC_CLK_CNTR_NCGPCCLK_CFG_ENABLE, "." ) );
	emit_constant(  C_SPEC( asserted, NV_PTRIM_GPC_CLK_CNTR_NCGPCCLK_CFG_ENABLE_ASSERTED, "f" ) );
	emit_field   (  F_SPEC( reset,    NV_PTRIM_GPC_CLK_CNTR_NCGPCCLK_CFG_RESET, "." ) );
	emit_constant(  C_SPEC( asserted, NV_PTRIM_GPC_CLK_CNTR_NCGPCCLK_CFG_RESET_ASSERTED, "f" ) );

	emit_register(IR_SPEC( gpc_clk_cntr_ncgpcclk_cnt, NV_PTRIM_GPC_CLK_CNTR_NCGPCCLK_CNT ) );
	emit_field   ( F_SPEC( value,                     NV_PTRIM_GPC_CLK_CNTR_NCGPCCLK_CNT_VALUE, "v" ) );

	emit_register( R_SPEC( sys_gpcpll_cfg2, NV_PTRIM_SYS_GPCPLL_CFG2 ) );
	emit_field   ( F_SPEC( pll_stepa,       NV_PTRIM_SYS_GPCPLL_CFG2_PLL_STEPA, "fm" ) );

	emit_register( R_SPEC( sys_gpcpll_cfg3, NV_PTRIM_SYS_GPCPLL_CFG3 ) );
	emit_field   ( F_SPEC( pll_stepb,       NV_PTRIM_SYS_GPCPLL_CFG3_PLL_STEPB, "fm" ) );

	emit_register( R_SPEC( sys_gpcpll_ndiv_slowdown, NV_PTRIM_SYS_GPCPLL_NDIV_SLOWDOWN ) );
	emit_field   ( F_SPEC( slowdown_using_pll,       NV_PTRIM_SYS_GPCPLL_NDIV_SLOWDOWN_SLOWDOWN_USING_PLL, "m" ) );
	emit_constant( C_SPEC( yes,                      NV_PTRIM_SYS_GPCPLL_NDIV_SLOWDOWN_SLOWDOWN_USING_PLL_YES, "f" ) );
	emit_constant( C_SPEC( no,                       NV_PTRIM_SYS_GPCPLL_NDIV_SLOWDOWN_SLOWDOWN_USING_PLL_NO, "f" ) );
	emit_field   ( F_SPEC( en_dynramp,               NV_PTRIM_GPC_GPCPLL_NDIV_SLOWDOWN_EN_DYNRAMP, "m" ) );
	emit_constant( C_SPEC( yes,                      NV_PTRIM_GPC_GPCPLL_NDIV_SLOWDOWN_EN_DYNRAMP_YES, "f" ) );
	emit_constant( C_SPEC( no,                       NV_PTRIM_GPC_GPCPLL_NDIV_SLOWDOWN_EN_DYNRAMP_NO, "f" ) );

	/* note name diff here */
	emit_register( R_SPEC( gpc_bcast_gpcpll_ndiv_slowdown_debug, NV_PTRIM_GPC_BCAST_NDIV_SLOWDOWN_DEBUG ) );
	emit_field   ( F_SPEC( pll_dynramp_done_synced,              NV_PTRIM_GPC_BCAST_NDIV_SLOWDOWN_DEBUG_PLL_DYNRAMP_DONE_SYNCED, "v" ) );

	end_group();

}

static void emit_timer_group()
{
	begin_group( G_SPEC( timer ) );
	emit_register( R_SPEC( pri_timeout, NV_PTIMER_PRI_TIMEOUT ) );
	emit_field   ( F_SPEC( period, NV_PTIMER_PRI_TIMEOUT_PERIOD, "fmv") );
	emit_field   ( F_SPEC( en, NV_PTIMER_PRI_TIMEOUT_EN, "fmv") );
	emit_constant( C_SPEC( en_enabled, NV_PTIMER_PRI_TIMEOUT_EN_ENABLED, "f" ) );
	emit_constant( C_SPEC( en_disabled, NV_PTIMER_PRI_TIMEOUT_EN_DISABLED, "f" ) );

	emit_register( R_SPEC( pri_timeout_save_0, NV_PTIMER_PRI_TIMEOUT_SAVE_0 ) );
	emit_register( R_SPEC( pri_timeout_save_1, NV_PTIMER_PRI_TIMEOUT_SAVE_1 ) );
	emit_register( R_SPEC( pri_timeout_fecs_errcode, NV_PTIMER_PRI_TIMEOUT_FECS_ERRCODE ) );
	end_group();

}

void emit_groups_gk20a()
{
	emit_bus_group();
	emit_ccsr_group();
	emit_ce2_group();
	emit_ctxsw_prog_group();
	emit_fb_group();
	emit_fifo_group();
	emit_flush_group();
	emit_gmmu_group();
	emit_gr_group();
	emit_ltc_group();
	emit_mc_group();
	emit_pbdma_group();
	emit_pri_ringmaster_group();
	emit_pri_ringstation_sys_group();
	emit_proj_group();
	emit_pwr_group();
	emit_ram_group();
	emit_therm_group();
	emit_top_group();
	emit_trim_group();
	emit_timer_group();
}
