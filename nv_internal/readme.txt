
This is a tool/work in progress.  The idea is to take register/ip definitions
from rnndb, beef them up and then provide the following things:

    . a framework for systematically integrating new gpu ip (chips) definitions
in to the rnndb system.

    . a mechanism to generate headers directly useable by nouveau or other
(envy)tools.  or, better: to integrate with those alreadry in use if applicable.

    . the beginnings of a mechanism to scan code for uses of the myriad ip
versions for code which is making use of the wrong registers at the wrong time.


Below is one example of the sort of output which can be generated at the moment.
Note though: that the source of *truth* for any given ip version (gk20a, gm20b,
etc.) lies within the headers for those alone.  So, be careful at this point...

 ./gen_rnndb --verbose 1 --root ../rnndb/root.xml  | sort  | grep NV_THERM
#define NV_THERM                                     0x000207ff:0x00020000 /*d*/
#define NV_THERM_CLK_SLOWDOWN_0                                    0x20160 /*r*/
#define NV_THERM_CLK_SLOWDOWN_0_IDLE_FACTOR                          21:16 /*f*/
#define NV_THERM_CLK_SLOWDOWN_0_IDLE_FACTOR_DISABLED            0x00000000 /*c*/
#define NV_THERM_CONFIG1                                           0x20050 /*r*/
#define NV_THERM_EVT_EXT_THERM_0                                   0x20700 /*r*/
#define NV_THERM_EVT_EXT_THERM_1                                   0x20704 /*r*/
#define NV_THERM_EVT_EXT_THERM_2                                   0x20708 /*r*/
#define NV_THERM_FECS_IDLE_FILTER                                  0x20288 /*r*/
#define NV_THERM_FECS_IDLE_FILTER_VALUE                               31:0 /*f*/
#define NV_THERM_GATE_CTRL                                         0x20200 /*r*/
#define NV_THERM_GATE_CTRL_BLK_CLK                                     3:2 /*f*/
#define NV_THERM_GATE_CTRL_BLK_CLK_AUTO                         0x00000001 /*c*/
#define NV_THERM_GATE_CTRL_BLK_CLK_RUN                          0x00000000 /*c*/
#define NV_THERM_GATE_CTRL_ENG_CLK                                     1:0 /*f*/
#define NV_THERM_GATE_CTRL_ENG_CLK_AUTO                         0x00000001 /*c*/
#define NV_THERM_GATE_CTRL_ENG_CLK_RUN                          0x00000000 /*c*/
#define NV_THERM_GATE_CTRL_ENG_CLK_STOP                         0x00000002 /*c*/
#define NV_THERM_GATE_CTRL_ENG_DELAY_AFTER                           23:20 /*f*/
#define NV_THERM_GATE_CTRL_ENG_IDLE_FILT_EXP                          12:8 /*f*/
#define NV_THERM_GATE_CTRL_ENG_IDLE_FILT_MANT                        15:13 /*f*/
#define NV_THERM_GATE_CTRL_ENG_PWR                                     5:4 /*f*/
#define NV_THERM_GATE_CTRL_ENG_PWR_AUTO                         0x00000001 /*c*/
#define NV_THERM_GATE_CTRL_ENG_PWR_OFF                          0x00000002 /*c*/
#define NV_THERM_HUBMMU_IDLE_FILTER                                0x2028c /*r*/
#define NV_THERM_HUBMMU_IDLE_FILTER_VALUE                             31:0 /*f*/
#define NV_THERM_USE_A                                             0x20798 /*r*/
#define NV_THERM_WEIGHT_1                                          0x20024 /*r*/
