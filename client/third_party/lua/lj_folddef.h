/* This is a generated file. DO NOT EDIT! */

static const FoldFunc fold_func[] = {
  fold_kfold_numarith,
  fold_kfold_ldexp,
  fold_kfold_fpmath,
  fold_kfold_numpow,
  fold_kfold_numcomp,
  fold_kfold_intarith,
  fold_kfold_intovarith,
  fold_kfold_bnot,
  fold_kfold_bswap,
  fold_kfold_intcomp,
  fold_kfold_intcomp0,
  fold_kfold_int64arith,
  fold_kfold_int64arith2,
  fold_kfold_int64shift,
  fold_kfold_bnot64,
  fold_kfold_bswap64,
  fold_kfold_int64comp,
  fold_kfold_int64comp0,
  fold_kfold_snew_kptr,
  fold_kfold_snew_empty,
  fold_kfold_strref,
  fold_kfold_strref_snew,
  fold_kfold_strcmp,
  fold_kfold_add_kgc,
  fold_kfold_add_kptr,
  fold_kfold_add_kright,
  fold_kfold_tobit,
  fold_kfold_conv_kint_num,
  fold_kfold_conv_kintu32_num,
  fold_kfold_conv_kint_ext,
  fold_kfold_conv_kint_i64,
  fold_kfold_conv_kint64_num_i64,
  fold_kfold_conv_kint64_num_u64,
  fold_kfold_conv_kint64_int_i64,
  fold_kfold_conv_knum_int_num,
  fold_kfold_conv_knum_u32_num,
  fold_kfold_conv_knum_i64_num,
  fold_kfold_conv_knum_u64_num,
  fold_kfold_tostr_knum,
  fold_kfold_tostr_kint,
  fold_kfold_strto,
  lj_opt_cse,
  fold_kfold_kref,
  fold_shortcut_round,
  fold_shortcut_left,
  fold_shortcut_dropleft,
  fold_shortcut_leftleft,
  fold_simplify_numadd_negx,
  fold_simplify_numadd_xneg,
  fold_simplify_numsub_k,
  fold_simplify_numsub_negk,
  fold_simplify_numsub_xneg,
  fold_simplify_nummuldiv_k,
  fold_simplify_nummuldiv_negk,
  fold_simplify_nummuldiv_negneg,
  fold_simplify_numpow_xk,
  fold_simplify_numpow_kx,
  fold_shortcut_conv_num_int,
  fold_simplify_conv_int_num,
  fold_simplify_conv_i64_num,
  fold_simplify_conv_int_i64,
  fold_simplify_conv_flt_num,
  fold_simplify_tobit_conv,
  fold_simplify_floor_conv,
  fold_simplify_conv_sext,
  fold_simplify_conv_narrow,
  fold_cse_conv,
  fold_narrow_convert,
  fold_simplify_intadd_k,
  fold_simplify_intmul_k,
  fold_simplify_intsub_k,
  fold_simplify_intsub_kleft,
  fold_simplify_intadd_k64,
  fold_simplify_intsub_k64,
  fold_simplify_intmul_k32,
  fold_simplify_intmul_k64,
  fold_simplify_intmod_k,
  fold_simplify_intmod_kleft,
  fold_simplify_intsub,
  fold_simplify_intsubadd_leftcancel,
  fold_simplify_intsubsub_leftcancel,
  fold_simplify_intsubsub_rightcancel,
  fold_simplify_intsubadd_rightcancel,
  fold_simplify_intsubaddadd_cancel,
  fold_simplify_band_k,
  fold_simplify_bor_k,
  fold_simplify_bxor_k,
  fold_simplify_shift_ik,
  fold_simplify_shift_andk,
  fold_simplify_shift1_ki,
  fold_simplify_shift2_ki,
  fold_simplify_shiftk_andk,
  fold_simplify_andk_shiftk,
  fold_reassoc_intarith_k,
  fold_reassoc_intarith_k64,
  fold_reassoc_dup,
  fold_reassoc_bxor,
  fold_reassoc_shift,
  fold_reassoc_minmax_k,
  fold_reassoc_minmax_left,
  fold_reassoc_minmax_right,
  fold_abc_fwd,
  fold_abc_k,
  fold_abc_invar,
  fold_comm_swap,
  fold_comm_equal,
  fold_comm_comp,
  fold_comm_dup,
  fold_comm_bxor,
  fold_merge_eqne_snew_kgc,
  lj_opt_fwd_aload,
  fold_kfold_hload_kkptr,
  lj_opt_fwd_hload,
  lj_opt_fwd_uload,
  lj_opt_fwd_tab_len,
  fold_cse_uref,
  lj_opt_fwd_hrefk,
  fold_fwd_href_tnew,
  fold_fwd_href_tdup,
  fold_fload_tab_tnew_asize,
  fold_fload_tab_tnew_hmask,
  fold_fload_tab_tdup_asize,
  fold_fload_tab_tdup_hmask,
  fold_fload_tab_ah,
  fold_fload_str_len_kgc,
  fold_fload_str_len_snew,
  fold_fload_cdata_typeid_kgc,
  fold_fload_cdata_int64_kgc,
  fold_fload_cdata_typeid_cnew,
  fold_fload_cdata_ptr_int64_cnew,
  lj_opt_cse,
  lj_opt_fwd_fload,
  fold_fwd_sload,
  fold_xload_kptr,
  lj_opt_fwd_xload,
  fold_barrier_tab,
  fold_barrier_tnew_tdup,
  lj_opt_dse_ahstore,
  lj_opt_dse_ustore,
  lj_opt_dse_fstore,
  lj_opt_dse_xstore,
  lj_ir_emit
};

static const uint32_t fold_hash[916] = {
0xffffffff,
0xffffffff,
0x5b4c8016,
0x0d4e7016,
0xffffffff,
0x1000701c,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0x29110c1a,
0xffffffff,
0xffffffff,
0x5b488016,
0x0d4a7016,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0x7b87fc07,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0x0d467016,
0xffffffff,
0x5a4c73ff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0x5153fc29,
0xffffffff,
0xffffffff,
0xffffffff,
0x5d408016,
0xffffffff,
0x594873ff,
0x8187440f,
0xffffffff,
0xffffffff,
0xffffffff,
0x8287fc0f,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0x6715ffff,
0xffffffff,
0xffffffff,
0xffffffff,
0x2a11fc1a,
0xffffffff,
0x1daa5a70,
0xffffffff,
0xffffffff,
0x0a0bfc16,
0x5c408c16,
0x6911ffff,
0x8db7ffff,
0xffffffff,
0xffffffff,
0x1caa59d4,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0x6a0dffff,
0x2b68d002,
0xffffffff,
0x3cab5695,
0xffffffff,
0x41aaa675,
0xffffffff,
0xffffffff,
0xffffffff,
0x27ae5800,
0xffffffff,
0x6a09ffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0x7f865c0f,
0xffffffff,
0xffffffff,
0xffffffff,
0x6a05ffff,
0x42abffff,
0x5e44881c,
0x5d50a016,
0x066c5816,
0x00646c1b,
0x75753bff,
0x1951fc18,
0x6264c81b,
0x1850641c,
0xffffffff,
0x6a01ffff,
0x87a7ffff,
0x4953fc1c,
0x8da80000,
0x4f52a3ff,
0x00606c1b,
0xffffffff,
0xffffffff,
0xffffffff,
0x5d428416,
0x88a53800,
0xffffffff,
0xffffffff,
0xffffffff,
0x05645816,
0xffffffff,
0x005c6c1b,
0x20aa71d6,
0xffffffff,
0xffffffff,
0xffffffff,
0x1399fc16,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0x157f33ff,
0xffffffff,
0xffffffff,
0x584dfc20,
0xffffffff,
0xffffffff,
0xffffffff,
0x8d9bffff,
0xffffffff,
0x055c5816,
0xffffffff,
0x00546c1b,
0xffffffff,
0xffffffff,
0x5849fc20,
0xffffffff,
0xffffffff,
0xffffffff,
0x8c97ffff,
0x5543fc1c,
0x05585816,
0xffffffff,
0x00506c1b,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0x8a93ffff,
0x26ae6c00,
0x05545816,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0x76753c17,
0x41aaa695,
0xffffffff,
0x898fffff,
0xffffffff,
0x05505816,
0xffffffff,
0xffffffff,
0xffffffff,
0x858867ff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0x848bffff,
0xffffffff,
0x054c5816,
0x79873c06,
0x47525bff,
0xffffffff,
0x3f695401,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0x8387ffff,
0xffffffff,
0x05485816,
0xffffffff,
0x5a4e5bff,
0xffffffff,
0xffffffff,
0x6264c816,
0x43aaa26e,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0x05445816,
0xffffffff,
0x5a4a5bff,
0xffffffff,
0xffffffff,
0xffffffff,
0x3455fc1b,
0x0c5a701c,
0x6366cbff,
0x0e3c7000,
0xffffffff,
0x05405816,
0xffffffff,
0x59465bff,
0xffffffff,
0xffffffff,
0xffffffff,
0x41aaa276,
0x0c56701c,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0x21aa7275,
0x0b52701c,
0x61489016,
0x6465fc33,
0x8d77ffff,
0xffffffff,
0x7b87fc05,
0xffffffff,
0xffffffff,
0x2a126bff,
0x385a6fff,
0xffffffff,
0x446dfc16,
0xffffffff,
0x7473ffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0x7d873000,
0xffffffff,
0x5c409016,
0x686fffff,
0x8187440d,
0xffffffff,
0xffffffff,
0x3554b81b,
0x8287fc0d,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0x686bffff,
0xffffffff,
0xffffffff,
0xffffffff,
0x8d9ffc00,
0x737a5fff,
0x41aaaa75,
0xffffffff,
0xffffffff,
0x5e40801c,
0x0b42701c,
0x6b67ffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0x2b68d000,
0xffffffff,
0xffffffff,
0x6d133017,
0xffffffff,
0xffffffff,
0x4c59fc16,
0xffffffff,
0xffffffff,
0x110bfc1c,
0x3aab566e,
0xffffffff,
0x5052a7ff,
0xffffffff,
0xffffffff,
0xffffffff,
0x6515fc28,
0x4a55fc16,
0x7f865c0d,
0x88a53c00,
0x41aaa296,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0x4451fc16,
0xffffffff,
0x60448bff,
0x21aa7295,
0xffffffff,
0x3cab5676,
0x04106c1b,
0xffffffff,
0x78873807,
0xffffffff,
0xffffffff,
0x574dfc16,
0xffffffff,
0x4e53ffff,
0xffffffff,
0x09145816,
0xffffffff,
0x040c6c1b,
0x8287fc00,
0x5e50a01c,
0x6467fc32,
0xffffffff,
0x5749fc16,
0xffffffff,
0xffffffff,
0xffffffff,
0x2a105816,
0x2e3e7c00,
0x04086c1b,
0x7083fc00,
0xffffffff,
0xffffffff,
0xffffffff,
0x5645fc16,
0xffffffff,
0x22aa6e6e,
0x5e42841c,
0x614e9c16,
0x090c5816,
0x04046c1b,
0x1eaa5ab3,
0xffffffff,
0xffffffff,
0xffffffff,
0x5441fc16,
0x41aaaa95,
0xffffffff,
0x5352a028,
0x09085816,
0x17505c16,
0x04006c1b,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0x6b43ffff,
0xffffffff,
0x09045816,
0xffffffff,
0x43aaa2ae,
0xffffffff,
0xffffffff,
0xffffffff,
0x083e5800,
0x7c865c00,
0xffffffff,
0x76753c15,
0x3051fc2e,
0x09005816,
0xffffffff,
0xffffffff,
0xffffffff,
0x3aab568e,
0xffffffff,
0x43aaa66e,
0xffffffff,
0x1daa5a71,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0x2a12701c,
0x5f66cfff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0x3cab5696,
0xffffffff,
0x100e701c,
0x41aaa676,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0x2a125c17,
0x3654b82e,
0x100a701c,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0x1006701c,
0xffffffff,
0x1951fc19,
0xffffffff,
0xffffffff,
0xffffffff,
0x23aa6e8e,
0xffffffff,
0x5b4e8016,
0xffffffff,
0x1eaa5ad3,
0x1002701c,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0x29130c1a,
0xffffffff,
0xffffffff,
0x0d4c7016,
0xffffffff,
0x475273ff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0x5b468016,
0x0d487016,
0x5a4e73ff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0x5d54a816,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0x5a4a73ff,
0x6615fc16,
0x3bab56ae,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0x594673ff,
0xffffffff,
0x61468c16,
0x8d17ffff,
0xffffffff,
0xffffffff,
0xffffffff,
0x2a13fc1a,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0x6913ffff,
0x40abfeb3,
0x8db9ffff,
0x41aaa696,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0x6a0fffff,
0x8db5ffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0x7a873c07,
0xffffffff,
0xffffffff,
0xffffffff,
0x6a0bffff,
0x3f695402,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0x24aa6eae,
0xffffffff,
0xffffffff,
0x6a07ffff,
0xffffffff,
0xffffffff,
0x066e5816,
0xffffffff,
0x00666c1b,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0x6a03ffff,
0xffffffff,
0x4b55fc1c,
0x066a5816,
0xffffffff,
0x00626c1b,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0x4851fc1c,
0x05665816,
0x18506016,
0x005e6c1b,
0x12986416,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0x8da1ffff,
0xffffffff,
0x3bab56ce,
0xffffffff,
0x43aaa6ae,
0xffffffff,
0xffffffff,
0x584ffc20,
0x7b87fc06,
0xffffffff,
0x5f4287ff,
0x8d9dffff,
0xffffffff,
0xffffffff,
0xffffffff,
0x00566c1b,
0xffffffff,
0xffffffff,
0x584bfc20,
0x5253fc28,
0xffffffff,
0xffffffff,
0xffffffff,
0x5645fc1c,
0xffffffff,
0x40abfed3,
0x00526c1b,
0x8187440e,
0xffffffff,
0x5847fc20,
0x8287fc0e,
0xffffffff,
0xffffffff,
0x8b95ffff,
0x2e3c7800,
0x5441fc1c,
0xffffffff,
0xffffffff,
0xffffffff,
0x17505c1c,
0xffffffff,
0xffffffff,
0x41aaaa76,
0xffffffff,
0x614c9816,
0x8991ffff,
0x1daa5a6f,
0x05525816,
0x4d585bff,
0xffffffff,
0x8087400c,
0xffffffff,
0xffffffff,
0xffffffff,
0x1baa59d3,
0x828dffff,
0x25aa6ece,
0x054e5816,
0x76753c1b,
0xffffffff,
0xffffffff,
0xffffffff,
0x2b68d001,
0xffffffff,
0xffffffff,
0xffffffff,
0x8689ffff,
0xffffffff,
0x054a5816,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0x43aca01b,
0x05465816,
0x7f865c0e,
0x5a4c5bff,
0x39ab55d3,
0x01626c16,
0x02686fff,
0x3457fc1b,
0xffffffff,
0xffffffff,
0x0f3e7000,
0x3dab55ae,
0x05425816,
0x1951fc17,
0x59485bff,
0xffffffff,
0xffffffff,
0xffffffff,
0x3153fc1b,
0x0c58701c,
0x5f64cbff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0x035a6c16,
0xffffffff,
0xffffffff,
0xffffffff,
0x0b54701c,
0xffffffff,
0x8779ffff,
0x1faa71d5,
0xffffffff,
0xffffffff,
0x2d5eb81b,
0x72b5fc08,
0xffffffff,
0xffffffff,
0xffffffff,
0x0b50701c,
0x456ffc16,
0x7b75ffff,
0xffffffff,
0xffffffff,
0x147e5c16,
0xffffffff,
0xffffffff,
0x2a106bff,
0xffffffff,
0x1eaa5ab4,
0x446bfc16,
0xffffffff,
0xffffffff,
0xffffffff,
0x41aaaa96,
0xffffffff,
0x3556b81b,
0x87a5fc00,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0x4e6dffff,
0xffffffff,
0xffffffff,
0xffffffff,
0x3252b81b,
0xffffffff,
0x5e54a81c,
0xffffffff,
0xffffffff,
0x0b44701c,
0x28b05c00,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0x73785fff,
0xffffffff,
0xffffffff,
0xffffffff,
0x0b40701c,
0xffffffff,
0x6b65ffff,
0xffffffff,
0xffffffff,
0x1daa5a72,
0xffffffff,
0xffffffff,
0xffffffff,
0x6266cc1b,
0xffffffff,
0x375bfc16,
0xffffffff,
0xffffffff,
0xffffffff,
0x3f695400,
0xffffffff,
0xffffffff,
0xffffffff,
0x6d113017,
0x3ead541b,
0xffffffff,
0x5d448816,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0x18506416,
0xffffffff,
0xffffffff,
0x16b37400,
0xffffffff,
0x4653fc16,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0x04126c1b,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0x574ffc16,
0xffffffff,
0x6855ffff,
0xffffffff,
0xffffffff,
0xffffffff,
0x040e6c1b,
0x41aaa275,
0xffffffff,
0xffffffff,
0xffffffff,
0x574bfc16,
0x6f826400,
0x6851ffff,
0x1eaa5ad4,
0x2a125816,
0xffffffff,
0x040a6c1b,
0x7185fc00,
0xffffffff,
0xffffffff,
0xffffffff,
0x5747fc16,
0x7b87fc04,
0xffffffff,
0xffffffff,
0x090e5816,
0xffffffff,
0x04066c1b,
0x6e81fc00,
0x1aac6c1b,
0x1850601c,
0x2e5cbbff,
0x5543fc16,
0xffffffff,
0xffffffff,
0xffffffff,
0x090a5816,
0xffffffff,
0x04026c1b,
0xffffffff,
0xffffffff,
0x8087440c,
0xffffffff,
0xffffffff,
0xffffffff,
0x6c45ffff,
0x8287fc0c,
0x09065816,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0x6b41ffff,
0x3353fc2e,
0x09025816,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0x2f50bbff,
0x073c5800,
0x6266cc16,
0x5f4083ff,
0xffffffff,
0xffffffff,
0x43aca41b,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0x2a10701c,
0x6364cfff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0x7e865c0c,
0xffffffff,
0xffffffff,
0x3656b82e,
0x41aaa295,
0x100c701c,
0x614a9416,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0x2c5ebc1b,
0xffffffff,
0x2a105c17,
0xffffffff,
0x1008701c,
0x3cab5675,
0xffffffff,
0xffffffff,
0x77873806,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0x1004701c,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff
};

#define fold_hashkey(k)    (lj_rol(lj_rol((k),17)-(k),16)%915)

