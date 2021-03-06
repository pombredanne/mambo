/*
  This file is part of MAMBO, a low-overhead dynamic binary modification tool:
      https://github.com/beehive-lab/mambo

  Copyright 2013-2016 Cosmin Gorgovan <cosmin at linux-geek dot org>

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
*/

#ifndef __PLUGIN_SUPPORT_H__
#define __PLUGIN_SUPPORT_H__

#include "../dbm.h"
#include "../scanner_public.h"

typedef struct {
  dbm_thread *thread_data;
  int plugin_id;
  inst_set inst_type;
  cc_type fragment_type;
  int fragment_id;
  int inst;
  mambo_cond cond;
  void *read_address;
  void *write_p;
  unsigned long *regs;
  bool replace;
  uint32_t pushed_regs;
  uint32_t available_regs;
  int plugin_pushed_reg_count;
} mambo_context;

typedef int (*mambo_callback)(mambo_context *ctx);

typedef enum {
  PRE_INST_C,
  POST_INST_C,
  PRE_BB_C,
  POST_BB_C,
  PRE_FRAGMENT_C,
  POST_FRAGMENT_C,
  PRE_SYSCALL_C,
  POST_SYSCALL_C,
  PRE_THREAD_C,
  POST_THREAD_C,
  EXIT_C,
  CALLBACK_MAX_IDX,
} mambo_cb_idx;

typedef enum {
  BRANCH_NONE = (1 << 0),
  BRANCH_DIRECT = (1 << 1),
  BRANCH_INDIRECT = (1 << 2),
  BRANCH_RETURN = (1 << 3),
  BRANCH_COND = (1 << 4),
  BRANCH_COND_PSR = (1 << 5),
  BRANCH_COND_CBZ = (1 << 6),
  BRANCH_COND_TBZ = (1 << 7), // A64-only
  BRANCH_COND_IT = (1 << 8),  // T32-only
  BRANCH_CALL = (1 << 9),
  BRANCH_INTERWORKING = (1 << 10), // A32 and T32
  BRANCH_TABLE = (1 << 11),        // T32-only
} mambo_branch_type;

typedef struct {
  mambo_callback cbs[CALLBACK_MAX_IDX];
  void *data;
} mambo_plugin;

enum mambo_plugin_error {
  MAMBO_SUCCESS = 0,
  MAMBO_INVALID_PLUGIN_ID = -1,
  MAMBO_CB_ALREADY_SET = -2,
  MAMBO_INVALID_CB = -3,
  MAMBO_INVALID_THREAD = -4,
};

/* Public functions */
mambo_context *mambo_register_plugin(void);

int mambo_register_pre_inst_cb(mambo_context *ctx, mambo_callback cb);
int mambo_register_post_inst_cb(mambo_context *ctx, mambo_callback cb);
int mambo_register_pre_basic_block_cb(mambo_context *ctx, mambo_callback cb);
int mambo_register_post_basic_block_cb(mambo_context *ctx, mambo_callback cb);
int mambo_register_pre_fragment_cb(mambo_context *ctx, mambo_callback cb);
int mambo_register_post_fragment_cb(mambo_context *ctx, mambo_callback cb);
int mambo_register_pre_syscall_cb(mambo_context *ctx, mambo_callback cb);
int mambo_register_post_syscall_cb(mambo_context *ctx, mambo_callback cb);
int mambo_register_pre_thread_cb(mambo_context *ctx, mambo_callback cb);
int mambo_register_post_thread_cb(mambo_context *ctx, mambo_callback cb);
int mambo_register_exit_cb(mambo_context *ctx, mambo_callback cb);

/* Memory management */
void *mambo_alloc(mambo_context *ctx, size_t size);
void mambo_free(mambo_context *ctx, void *ptr);

/* Access plugin data */
int mambo_set_plugin_data(mambo_context *ctx, void *data);
void *mambo_get_plugin_data(mambo_context *ctx);
int mambo_set_thread_plugin_data(mambo_context *ctx, void *data);
void *mambo_get_thread_plugin_data(mambo_context *ctx);

/* Scratch register management */
int mambo_get_scratch_regs(mambo_context *ctx, int count, ...);
int mambo_get_scratch_reg(mambo_context *ctx, int *regp);
int mambo_free_scratch_regs(mambo_context *ctx, uint32_t regs);
int mambo_free_scratch_reg(mambo_context *ctx, int reg);

/* Other */
int mambo_get_inst(mambo_context *ctx);
inst_set mambo_get_inst_type(mambo_context *ctx);
int mambo_get_fragment_id(mambo_context *ctx);
cc_type mambo_get_fragment_type(mambo_context *ctx);
int mambo_get_inst_len(mambo_context *ctx);
void *mambo_get_source_addr(mambo_context *ctx);
void *mambo_get_cc_addr(mambo_context *ctx);
void mambo_set_cc_addr(mambo_context *ctx, void *addr);
int mambo_get_thread_id(mambo_context *ctx);
bool mambo_is_cond(mambo_context *ctx);
mambo_cond mambo_get_cond(mambo_context *ctx);
mambo_cond mambo_get_inverted_cond(mambo_context *ctx, mambo_cond cond);
void mambo_replace_inst(mambo_context *ctx);
bool mambo_is_load(mambo_context *ctx);
bool mambo_is_store(mambo_context *ctx);
bool mambo_is_load_or_store(mambo_context *ctx);
int mambo_get_ld_st_size(mambo_context *ctx);

mambo_branch_type mambo_get_branch_type(mambo_context *ctx);

#endif
