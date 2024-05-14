`ifndef RENODE_AXI_ASSIGN_SVH_
`define RENODE_AXI_ASSIGN_SVH_


`define __AXI_TO_AW(__opt_as, __lhs, __lhs_sep, __rhs, __rhs_sep)   \
  __opt_as __lhs``__lhs_sep``id     = __rhs``__rhs_sep``id;         \
  __opt_as __lhs``__lhs_sep``addr   = __rhs``__rhs_sep``addr;       \
  __opt_as __lhs``__lhs_sep``len    = __rhs``__rhs_sep``len;        \
  __opt_as __lhs``__lhs_sep``size   = __rhs``__rhs_sep``size;       \
  __opt_as __lhs``__lhs_sep``burst  = __rhs``__rhs_sep``burst;      \
  __opt_as __lhs``__lhs_sep``lock   = __rhs``__rhs_sep``lock;       \
  __opt_as __lhs``__lhs_sep``cache  = __rhs``__rhs_sep``cache;      \
  __opt_as __lhs``__lhs_sep``prot   = __rhs``__rhs_sep``prot;       \
  //__opt_as __lhs``__lhs_sep``qos    = __rhs``__rhs_sep``qos;        \
 // __opt_as __lhs``__lhs_sep``region = __rhs``__rhs_sep``region;     \
 // __opt_as __lhs``__lhs_sep``atop   = __rhs``__rhs_sep``atop;       \
 // __opt_as __lhs``__lhs_sep``user   = __rhs``__rhs_sep``user;
`define __AXI_TO_W(__opt_as, __lhs, __lhs_sep, __rhs, __rhs_sep)    \
  __opt_as __lhs``__lhs_sep``data   = __rhs``__rhs_sep``data;       \
  __opt_as __lhs``__lhs_sep``strb   = __rhs``__rhs_sep``strb;       \
  __opt_as __lhs``__lhs_sep``last   = __rhs``__rhs_sep``last;       \
 // __opt_as __lhs``__lhs_sep``user   = __rhs``__rhs_sep``user;
`define __AXI_TO_B(__opt_as, __lhs, __lhs_sep, __rhs, __rhs_sep)    \
  __opt_as __lhs``__lhs_sep``id     = __rhs``__rhs_sep``id;         \
  __opt_as __lhs``__lhs_sep``resp   = __rhs``__rhs_sep``resp;       \
 // __opt_as __lhs``__lhs_sep``user   = __rhs``__rhs_sep``user;
`define __AXI_TO_AR(__opt_as, __lhs, __lhs_sep, __rhs, __rhs_sep)   \
  __opt_as __lhs``__lhs_sep``id     = __rhs``__rhs_sep``id;         \
  __opt_as __lhs``__lhs_sep``addr   = __rhs``__rhs_sep``addr;       \
  __opt_as __lhs``__lhs_sep``len    = __rhs``__rhs_sep``len;        \
  __opt_as __lhs``__lhs_sep``size   = __rhs``__rhs_sep``size;       \
  __opt_as __lhs``__lhs_sep``burst  = __rhs``__rhs_sep``burst;      \
  __opt_as __lhs``__lhs_sep``lock   = __rhs``__rhs_sep``lock;       \
  __opt_as __lhs``__lhs_sep``cache  = __rhs``__rhs_sep``cache;      \
 // __opt_as __lhs``__lhs_sep``prot   = __rhs``__rhs_sep``prot;       \
 // __opt_as __lhs``__lhs_sep``qos    = __rhs``__rhs_sep``qos;        \
 // __opt_as __lhs``__lhs_sep``region = __rhs``__rhs_sep``region;     \
 // __opt_as __lhs``__lhs_sep``user   = __rhs``__rhs_sep``user;
`define __AXI_TO_R(__opt_as, __lhs, __lhs_sep, __rhs, __rhs_sep)    \
  __opt_as __lhs``__lhs_sep``id     = __rhs``__rhs_sep``id;         \
  __opt_as __lhs``__lhs_sep``data   = __rhs``__rhs_sep``data;       \
  __opt_as __lhs``__lhs_sep``resp   = __rhs``__rhs_sep``resp;       \
  __opt_as __lhs``__lhs_sep``last   = __rhs``__rhs_sep``last;       \
//  __opt_as __lhs``__lhs_sep``user   = __rhs``__rhs_sep``user;



`define __RESP_TO_RENODE(__lhs, __rhs)                               \
  assign __lhs``.awready = __rhs``.aw_ready;                         \
  assign __lhs``.arready = __rhs``.ar_ready;                         \
  assign __lhs``.wready = __rhs``.w_ready;                           \
  assign __lhs``.bvalid = __rhs``.b_valid;                           \
  `__AXI_TO_B(assign, __lhs``.b, ,__rhs``.b,. )         \
  assign __lhs``.rvalid = __rhs``.r_valid;                            \
  `__AXI_TO_R(assign, __lhs``.r, , __rhs``.r, .)   


`define __REQ_TO_RENODE(__lhs, __rhs)    \
  `__AXI_TO_AW(assign,__lhs``.aw,,__rhs``.aw,. ) \
   assign __lhs``.awvalid = __rhs``.aw_valid;                         \
  `__AXI_TO_W (assign,__lhs``.w,,__rhs``.w,.  )    \
  assign __lhs``.wvalid = __rhs``.w_valid;                           \
  assign __lhs``.bready = __rhs``.b_ready;                           \
  `__AXI_TO_AR(assign,__lhs``.ar,,__rhs``.ar,. )    \
  assign __lhs``.arvalid = __rhs``.ar_valid;                         \
  assign __lhs``.rready = __rhs``.r_ready;   


///

`define __RENODE_TO_RESP(__lhs, __rhs)                               \
  assign __lhs``.aw_ready = __rhs``.awready;                         \
  assign __lhs``.ar_ready = __rhs``.arready;                         \
  assign __lhs``.w_ready = __rhs``.wready;                           \
  assign __lhs``.b_valid = __rhs``.bvalid;                           \
  `__AXI_TO_B(assign, __lhs``.b,.,__rhs``.b, )         \
  assign __lhs``.r_valid = __rhs``.rvalid;                            \
  `__AXI_TO_R(assign, __lhs``.r,., __rhs``.r, )   
  

`define __RENODE_TO_REQ(__lhs, __rhs)    \
  `__AXI_TO_AW(assign,__lhs``.aw,.,__rhs``.aw, ) \
   assign __lhs``.aw_valid = __rhs``.awvalid;                         \
  `__AXI_TO_W (assign,__lhs``.w,.,__rhs``.w,)    \
  assign __lhs``.w_valid = __rhs``.wvalid;                           \
  assign __lhs``.b_ready = __rhs``.bready;                           \
  `__AXI_TO_AR(assign,__lhs``.ar,.,__rhs``.ar,)    \
  assign __lhs``.ar_valid = __rhs``.arvalid;                         \
  assign __lhs``.r_ready = __rhs``.rready;   

`endif