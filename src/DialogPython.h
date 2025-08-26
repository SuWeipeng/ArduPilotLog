#ifndef DIALOGPYTHON_H
#define DIALOGPYTHON_H

#include <QDialog>
#include <QLineEdit>
#include <QDir>
#include "APLLoggingCategory.h"

Q_DECLARE_LOGGING_CATEGORY(DIALOGPYTHON_LOG)

class APLReadConf;
class QFileDialog;

class DialogPython : public QDialog
{
    Q_OBJECT

public:
    DialogPython(QWidget *parent = 0);
    ~DialogPython();

    APLReadConf* getAPLReadConf() const { return _aplReadConf; }
    bool isDirExist(QString fullPath);

public slots:
    void showFile();

signals:
    void saveSuccess();

private:
    APLReadConf *_aplReadConf;
    QFileDialog *_qfileDialogLoad;

public:
    static inline QString example_01=R"(#!/usr/bin/python3
# -*- coding:utf-8 -*-
import numpy as np
from matplotlib import pyplot as plt
import sys
import os
from utilities.LogDBParser import LogDBParser
import platform
import matplotlib

# Check if running on Linux
if platform.system() == 'Linux':
    try:
        matplotlib.use('TkAgg')
    except ImportError:
        print("sudo apt-get install python3-tk")
        # Fallback to default backend
        pass

# Handle command line arguments
if len(sys.argv) > 1:
    db_name = sys.argv[1]
else:
    db_name = "00000023.db"

# Cross-platform path handling
db_path = os.path.join('..', db_name)

# Check if file exists
if not os.path.exists(db_path):
    print(f"Error: Database file not found: {db_path}")
    sys.exit(1)

log = LogDBParser(db_path)

try:
    data          = log.getData("TECS","TimeUS","th","hin","sp","spdem","h","hdem","dh","dhdem","f")
    tecs_timeus   = data[0]
    tecs_th       = data[1]
    tecs_hin      = data[2]
    tecs_sp       = data[3]
    tecs_spdem    = data[4]
    tecs_h        = data[5]
    tecs_hdem     = data[6]
    tecs_dh       = data[7]
    tecs_dhdem    = data[8]
    tecs_f        = data[9]

    data           = log.getData("ARSP0","TimeUS","Airspeed","Filt","Temp")
    arsp0_timeus   = data[0]
    arsp0_airspeed = data[1]
    arsp0_filt     = data[2]
    arsp0_temp     = data[3]

    data          = log.getData("GPS0","TimeUS","Lat","Lng","Alt","Spd")
    gps0_timeus   = data[0]
    gps0_lat      = data[1]
    gps0_lng      = data[2]
    gps0_alt      = data[3]
    gps0_spd      = data[4]

    data          = log.getData("BARO0","TimeUS","Alt","Press","Temp","Crt","SMS","Offset","GndTemp","Health")
    baro0_timeus  = data[0]
    baro0_alt     = data[1]
    baro0_press   = data[2]
    baro0_temp    = data[3]
    baro0_crt     = data[4]
    baro0_sms     = data[5]
    baro0_offset  = data[6]
    baro0_gndtemp = data[7]
    baro0_health  = data[8]
except Exception as e:
    print(f"Error: Problem reading data - {e}")
    sys.exit(1)

# matplotlib plotting
# Function plot_frequency() for plotting log recording frequency on the right side
def plot_frequency(label_dict, ax):
    calc_f = []
    for i in range(len(label_dict)):
        calc_f.append(eval(label_dict[i].lower()+"_timeus"))
    for idx in range(len(calc_f)):
        f = []
        for i in range(len(calc_f[idx])-1):
            delta_t = calc_f[idx][i+1]-calc_f[idx][i]
            if delta_t > 0:
                f.append(1e6/delta_t)
        cnt = list(range(len(f)))

        ax.plot(cnt,f,
                 label=label_dict[idx],
                 marker = '.',
                 markersize = 2,
                 linestyle=":",
                 linewidth=0.6)
        ax.legend()

# Plot layout
fig  = plt.figure(figsize=(16, 8), dpi=120)  # Modified dpi to a more reasonable value
axc = plt.subplot2grid((1,6),(0,0),colspan=5)
axc.ticklabel_format(style='sci', scilimits=(-1,3), axis='both')
axc.grid(ls="--")
ax2 = plt.subplot2grid((1,6),(0,5))
ax2.ticklabel_format(style='sci', scilimits=(-1,3), axis='y')
ax2.grid(ls="--")
ax2.set_ylabel("Hz")
plt.tight_layout()

# Custom plotting on the left side
axc.plot(tecs_timeus,
         tecs_sp,
         label="TECS.sp",
         markersize = 2,
         linestyle="-",
         linewidth=0.6,
         color='r')
axc.plot(gps0_timeus,
         gps0_spd,
         label="GPS0.Spd",
         marker = '',
         markersize = 2,
         linestyle="-",
         linewidth=0.6,
         color='b')
axc.legend()

# Recording frequency plotting on the right side
label_f = ["TECS","GPS0","ARSP0"]
plot_frequency(label_f, ax2)

plt.show()
)";

    static inline QString example_02_1_of_3=R"(#!/usr/bin/python3
# -*- coding:utf-8 -*-
import numpy as np
from matplotlib import pyplot as plt
import sys
import os
from utilities.LogDBParser import LogDBParser
import numpy as np
from utilities.MathCommon import rad2deg,constrain_float,MIN,MAX,LowPassFilter
import platform
import matplotlib

# Check if running on Linux
if platform.system() == 'Linux':
    try:
        matplotlib.use('TkAgg')
    except ImportError:
        print("sudo apt-get install python3-tk")
        # Fallback to default backend
        pass

# Handle command line arguments
if len(sys.argv) > 1:
    db_name = sys.argv[1]
else:
    db_name = "00000023.db"

# Cross-platform path handling
db_path = os.path.join('..', db_name)

# Check if file exists
if not os.path.exists(db_path):
    print(f"Error: Database file not found: {db_path}")
    sys.exit(1)

log = LogDBParser(db_path)

GRAVITY_MSS   = 9.80665

try:
    data          = log.getData("TECS","TimeUS","th","hin","sp","spdem","h","hdem","dh","dhdem","f","dhdem","dsp","ph","pmin","pmax","dspdem")
    tecs_timeus   = data[0]
    tecs_th       = data[1]
    tecs_hin      = data[2]
    tecs_sp       = data[3]
    tecs_spdem    = data[4]
    tecs_h        = data[5]
    tecs_hdem     = data[6]
    tecs_dh       = data[7]
    tecs_dhdem    = data[8]
    tecs_f        = data[9]
    tecs_dhdem    = data[10]
    tecs_dsp      = data[11]
    tecs_ph       = data[12]
    tecs_pmin     = data[13]
    tecs_pmax     = data[14]
    tecs_dspdem   = data[15]

    data          = log.getData("TEC2","TimeUS","PEW","EBD","EBE","EBDD","EBDE","EBDDT","Imin","Imax","I","KI","pmin","pmax")
    tec2_timeus   = data[0]
    tec2_pew      = data[1]
    tec2_ebd      = data[2]
    tec2_ebe      = data[3]
    tec2_ebdd     = data[4]
    tec2_ebde     = data[5]
    tec2_ebddt    = data[6]
    tec2_imin     = data[7]
    tec2_imax     = data[8]
    tec2_i        = data[9]
    tec2_ki       = data[10]
    tec2_pmin     = data[11]
    tec2_pmax     = data[12]

    data          = log.getData("TEC3","TimeUS","KED","PED","KEDD","PEDD","TEE","TEDE","FFT","Imin","Imax","I","Emin","Emax")
    tec3_timeus   = data[0]
    tec3_ked      = data[1]
    tec3_ped      = data[2]
    tec3_kedd     = data[3]
    tec3_pedd     = data[4]
    tec3_tee      = data[5]
    tec3_tede     = data[6]
    tec3_fft      = data[7]
    tec3_imin     = data[8]
    tec3_imax     = data[9]
    tec3_i        = data[10]
    tec3_emin     = data[11]
    tec3_emax     = data[12]

    data          = log.getData("TEC6","TimeUS","te","ted","e2t","fft","dt","max","min","c2z","itma","itmi")
    tec6_timeus   = data[0]
    tec6_te       = data[1]
    tec6_ted      = data[2]
    tec6_e2t      = data[3]
    tec6_fft      = data[4]
    tec6_dt       = data[5]
    tec6_max      = data[6]
    tec6_min      = data[7]
    tec6_c2z      = data[8]
    tec6_itma     = data[9]
    tec6_itmi     = data[10]

    data          = log.getData("TEC7","TimeUS","ped","ked","pedd","kedd","pee","kee","pedt","kedt")
    tec7_timeus   = data[0]
    tec7_ped      = data[1]
    tec7_ked      = data[2]
    tec7_pedd     = data[3]
    tec7_kedd     = data[4]
    tec7_pee      = data[5]
    tec7_kee      = data[6]
    tec7_pedt     = data[7]
    tec7_kedt     = data[8]

    data          = log.getData("TEC8","TimeUS","sp","spd","pmax","pmin","punc","dt","pd","pri","pp")
    tec8_timeus   = data[0]
    tec8_sp       = data[1]
    tec8_spd      = data[2]
    tec8_pmax     = data[3]
    tec8_pmin     = data[4]
    tec8_punc     = data[5]
    tec8_dt       = data[6]
    tec8_pd       = data[7]
    tec8_pri      = data[8]
    tec8_pp       = data[9]

    data         = log.getData("ATT","TimeUS","DesRoll","Roll","DesPitch","Pitch","DesYaw","Yaw","ErrRP","ErrYaw","AEKF")
    att_timeus   = data[0]
    att_desroll  = data[1]
    att_roll     = data[2]
    att_despitch = data[3]
    att_pitch    = data[4]
    att_desyaw   = data[5]
    att_yaw      = data[6]
    att_errrp    = data[7]
    att_erryaw   = data[8]
    att_aekf     = data[9]

    data         = log.getData("RCOU","TimeUS","C1","C2","C3","C4","C5","C6","C7","C8","C9","C10","C11","C12","C13","C14")
    rcou_timeus  = data[0]
    rcou_c1      = data[1]
    rcou_c2      = data[2]
    rcou_c3      = data[3]
    rcou_c4      = data[4]
    rcou_c5      = data[5]
    rcou_c6      = data[6]
    rcou_c7      = data[7]
    rcou_c8      = data[8]
    rcou_c9      = data[9]
    rcou_c10     = data[10]
    rcou_c11     = data[11]
    rcou_c12     = data[12]
    rcou_c13     = data[13]
    rcou_c14     = data[14]

    data         = log.getData("RCO2","TimeUS","C15","C16","C17","C18")
    rco2_timeus  = data[0]
    rco2_c15     = data[1]
    rco2_c16     = data[2]
    rco2_c17     = data[3]
    rco2_c18     = data[4]

    data          = log.getData("PARM","TimeUS","Name","Value")
    parm_timeus   = data[0]
    parm_name     = data[1]
    parm_value    = data[2]
except Exception as e:
    print(f"Error: Problem reading data - {e}")
    sys.exit(1)

parms = {}
# 建立参数字典，字典形举例如下：
# {'Q_M_THST_HOVER'：[{3035145: 0.408207}, {51787417: 0.408207}, {271065946: 0.421976}]}
for i in range(len(parm_timeus)):
    if parms.get(parm_name[i]) == None:
        parms[parm_name[i]]=[{parm_timeus[i]:parm_value[i]}]
    else:
        parms.get(parm_name[i]).append({parm_timeus[i]:parm_value[i]})

# 查找一个参数
# 例：get_parm('Q_M_THST_HOVER')
def get_parm(name):
    if parms.get(name) is not None:
        t = [] # 时间 TimeUS
        v = [] # 值
        for i in range(len(parms.get(name))):
            element = parms.get(name)[i]
            t.append(int(list(element.keys())[0]))
            v.append(float(list(element.values())[0]))
        return (t,v)

class PARM:
    def __init__(self,name):
        try:
            self.t,self.v = get_parm(name)
        except:
            print("%s Error"%(name))

SERVO3_MIN             = PARM('SERVO3_MIN')
SERVO3_MAX             = PARM('SERVO3_MAX')
THR_MIN                = PARM('THR_MIN')
THR_MAX                = PARM('THR_MAX')
TECS_APPR_SMAX         = PARM('TECS_APPR_SMAX')
TECS_CLMB_MAX          = PARM('TECS_CLMB_MAX')
TECS_FLARE_HGT         = PARM('TECS_FLARE_HGT')
TECS_HDEM_TCONST       = PARM('TECS_HDEM_TCONST')
TECS_HGT_OMEGA         = PARM('TECS_HGT_OMEGA')
TECS_INTEG_GAIN        = PARM('TECS_INTEG_GAIN')
TECS_LAND_ARSPD        = PARM('TECS_LAND_ARSPD')
TECS_LAND_DAMP         = PARM('TECS_LAND_DAMP')
TECS_DSP_TCONST        = PARM('TECS_DSP_TCONST')
TECS_LAND_IGAIN        = PARM('TECS_LAND_IGAIN')
TECS_LAND_PCONST       = PARM('TECS_LAND_PCONST')
TECS_LAND_PDAMP        = PARM('TECS_LAND_PDAMP')
TECS_LAND_PMAX         = PARM('TECS_LAND_PMAX')
TECS_LAND_SINK         = PARM('TECS_LAND_SINK')
TECS_LAND_SPDWGT       = PARM('TECS_LAND_SPDWGT')
TECS_LAND_SRC          = PARM('TECS_LAND_SRC')
TECS_LAND_TDAMP        = PARM('TECS_LAND_TDAMP')
TECS_LAND_THR          = PARM('TECS_LAND_THR')
TECS_OPTIONS           = PARM('TECS_OPTIONS')
TECS_PITCH_MAX         = PARM('TECS_PITCH_MAX')
TECS_PITCH_MIN         = PARM('TECS_PITCH_MIN')
TECS_PTCH_DAMP         = PARM('TECS_PTCH_DAMP')
TECS_PTCH_FF_K         = PARM('TECS_PTCH_FF_K')
TECS_PTCH_FF_V0        = PARM('TECS_PTCH_FF_V0')
TECS_RLL2THR           = PARM('TECS_RLL2THR')
TECS_SINK_MAX          = PARM('TECS_SINK_MAX')
TECS_SINK_MIN          = PARM('TECS_SINK_MIN')
TECS_SPDWEIGHT         = PARM('TECS_SPDWEIGHT')
TECS_SPD_OMEGA         = PARM('TECS_SPD_OMEGA')
TECS_SYNAIRSPEED       = PARM('TECS_SYNAIRSPEED')
TECS_THR_DAMP          = PARM('TECS_THR_DAMP')
TECS_TIME_CONST        = PARM('TECS_TIME_CONST')
TECS_TKOFF_IGAIN       = PARM('TECS_TKOFF_IGAIN')
TECS_VERT_ACC          = PARM('TECS_VERT_ACC')
TECS_SPE_TCONST        = PARM('TECS_SPE_TCONST')
TECS_SKE_TCONST        = PARM('TECS_SKE_TCONST')

T   = TECS_TIME_CONST.v[-1]
try:
    KET = TECS_SKE_TCONST.v[-1]
except:
    KET = 10

Wk = constrain_float(TECS_SPDWEIGHT.v[-1], 0, 2)
Wp = 2.0 - Wk;
Wp = MIN(Wp, 1.0);
Wk = MIN(Wk, 1.0);

# FSM 用于以“背景色”或“色块标识”区分状态机
data          = log.getData("FSM","LineNo","TimeUS","id","fsm")
fsm_line     = data[0]
fsm_timeus   = data[1]
fsm_id       = data[2]
fsm_fsm      = data[3]

class FSM:
    def __init__(self):
        self.TimeUS = []
        self.id     = []
        self.fsm    = []
# 分拣各个实例的数据
FSM0 = FSM()
FSM1 = FSM()
for i in range(len(fsm_timeus)):
    if fsm_id[i] == 0:
        FSM0.TimeUS.append(fsm_timeus[i])
        FSM0.fsm.append(fsm_fsm[i])
    if fsm_id[i] == 1:
        FSM1.TimeUS.append(fsm_timeus[i])
        FSM1.fsm.append(fsm_fsm[i])

# 类 FLIGHT_STAGE_MARK 用于以“背景色”区分“飞行模式状态机”
import matplotlib.patches as patches
class FLIGHT_STAGE_MARK():
    def __init__(self,ax,FSM_obj,ref_timeus):
        self.ax = ax
        self.FSM_obj = FSM_obj
        self.ref_timeus = ref_timeus
    def draw_rectangle(self):
        xlim = self.ax.get_xlim()
        ylim = self.ax.get_ylim()
        pa = []
        l  = []
        h  = []
        c  = []
        last_state = 255
        start_us   = 0

        _H = 1
        h_offset = (ylim[1]-ylim[0]) * (_H-1)/ _H
        H        = (ylim[1]-ylim[0])         / _H

        fsm_color = {1:'r',     # FLIGHT_TAKEOFF
                     2:'cyan',  # FLIGHT_VTOL
                     3:'g',     # FLIGHT_NORMAL
                     4:'b',     # FLIGHT_LAND
                     7:'y'}     # FLIGHT_ABORT_LAND

        start_us = 0
        for i in range(len(self.FSM_obj.TimeUS)):
            if i == 0:
                start_us  = self.FSM_obj.TimeUS[i]
            if self.FSM_obj.fsm[i] != last_state:
                last_state = self.FSM_obj.fsm[i]
                pa.append((self.FSM_obj.TimeUS[i],ylim[0]+h_offset))
                if len(l)>=1:
                    l[len(l)-1] = self.FSM_obj.TimeUS[i] - start_us
                L = self.FSM_obj.TimeUS[i] - start_us
                start_us  = self.FSM_obj.TimeUS[i]
                l.append(L)
                h.append(H)
                c.append(self.FSM_obj.fsm[i])
            if i == len(self.FSM_obj.TimeUS) - 1:
                l[len(l)-1] = self.ref_timeus[len(self.ref_timeus)-1] - start_us

        for i in range(len(l)):
            self.back_rectangle = patches.Rectangle(
                                        pa[i],
                                        l[i],
                                        h[i],
                                        edgecolor = 'none',
                                        facecolor = fsm_color[c[i]],
                                        alpha = 0.05,
                                        fill=True,
                                        zorder=0)
            self.ax.add_patch(self.back_rectangle)

# 类 SLOPE_STAGE_MARK 用于以“背景色”区分“飞行模式状态机”
class SLOPE_STAGE_MARK():
    def __init__(self,ax,FSM_obj,ref_timeus):
        self.ax = ax
        self.FSM_obj = FSM_obj
        self.ref_timeus = ref_timeus
    def draw_rectangle(self):
        xlim = self.ax.get_xlim()
        ylim = self.ax.get_ylim()
        pa = []
        l  = []
        h  = []
        for i in range(len(self.FSM_obj.TimeUS)):
            pa.append((self.FSM_obj.TimeUS[i],ylim[0]))
            if i != len(self.FSM_obj.TimeUS)-1:
                l.append(self.FSM_obj.TimeUS[i+1] - self.FSM_obj.TimeUS[i])
            else:
                l.append(self.ref_timeus[len(self.ref_timeus)-1] - self.FSM_obj.TimeUS[i])
            h.append(ylim[1]-ylim[0])

        fsm_color = {0:'r',  # SLOPE_STAGE_NORMAL
                     1:'g',  # SLOPE_STAGE_APPROACH
                     2:'b',  # SLOPE_STAGE_PREFLARE
                     3:'y'}  # SLOPE_STAGE_FINAL
        for i in range(len(l)):
            self.back_rectangle = patches.Rectangle(
                                        pa[i],
                                        l[i],
                                        h[i]/2,
                                        edgecolor = 'none',
                                        facecolor = fsm_color[self.FSM_obj.fsm[i]],
                                        alpha = 0.15,
                                        fill=True,
                                        zorder=0)
            self.ax.add_patch(self.back_rectangle)

# 依上面取得的数据通过 matplotlib 绘图
import matplotlib.pyplot as plt
import matplotlib.gridspec as gridspec

# 绘图布局
fig  = plt.figure(figsize=(16, 9), dpi=1920/16)
gs   = gridspec.GridSpec(nrows=2, ncols=3, left=0.03, right=0.97, wspace=0.12)
# 左上图
ax1  = fig.add_subplot(gs[0,0])
ax1.ticklabel_format(style='sci', scilimits=(-1,2), axis='both')
ax1.grid(ls="--",lw=0.3)
# 中上图
ax2  = fig.add_subplot(gs[0,1])
ax2.ticklabel_format(style='sci', scilimits=(-1,2), axis='both')
ax2.grid(ls="--",lw=0.3)
# 右上图
ax3  = fig.add_subplot(gs[0,2])
ax3.ticklabel_format(style='sci', scilimits=(-1,2), axis='both')
ax3.grid(ls="--",lw=0.3)
# 左下图
ax4  = fig.add_subplot(gs[1,0])
ax4.ticklabel_format(style='sci', scilimits=(-1,2), axis='both')
ax4.grid(ls="--",lw=0.3)
# 中下图
ax5  = fig.add_subplot(gs[1,1])
ax5.ticklabel_format(style='sci', scilimits=(-1,2), axis='both')
ax5.grid(ls="--",lw=0.3)
# 右下图
ax6  = fig.add_subplot(gs[1,2])
ax6.ticklabel_format(style='sci', scilimits=(-1,2), axis='both')
ax6.grid(ls="--",lw=0.3)

ax1.plot(rcou_timeus,
         (np.array(rcou_c3)-np.array(SERVO3_MIN.v[-1]))/(np.array(SERVO3_MAX.v[-1])-np.array(SERVO3_MIN.v[-1])),
         label="RCOU.C3[0~1]",
         marker = '',
         markersize = 2,
         linestyle="--",
         linewidth=0.6,
         color='b')
ax1.plot(tecs_timeus,
         tecs_th,
         label="TECS.th",
         marker = '',
         markersize = 2,
         linestyle=":",
         linewidth=0.6,
         color='r')
ax1.legend()

ax2.plot(tec3_timeus,
         tec3_fft,
         label="TEC3.FFT",
         marker = '',
         markersize = 2,
         linestyle="-",
         linewidth=0.6,
         color='r')
ax2.plot(tec3_timeus,
         tec3_i,
         label="TEC3.I",
         marker = '',
         markersize = 2,
         linestyle="-",
         linewidth=0.6,
         color='b')

len_common = MIN(len(tecs_timeus),len(tec3_timeus))
len_common = MIN(len_common,len(tec6_timeus))
import copy
tecs_timeus_cut = copy.deepcopy(tecs_timeus[:len_common])
tec3_timeus_cut = copy.deepcopy(tec3_timeus[:len_common])
tec3_tee_cut    = copy.deepcopy(tec3_tee[:len_common])
tecs_th_cut     = copy.deepcopy(tecs_th[:len_common])
tec3_i_cut      = copy.deepcopy(tec3_i[:len_common])
tec3_fft_cut    = copy.deepcopy(tec3_fft[:len_common])
tec3_fft_cut    = copy.deepcopy(tec3_fft[:len_common])
tec6_e2t_cut    = copy.deepcopy(tec6_e2t[:len_common])

Th          = np.array(tecs_th_cut)-np.array(tec3_i_cut)-np.array(tec3_fft_cut)
Th1         = np.array(Th)/np.array(tec6_e2t_cut) - np.array(tec3_tee_cut)/np.array(KET)
ax2.plot(tecs_timeus_cut,
         Th,
         label="(STEerr/%d+dotSTEerr*damp)*K_e2t"%(KET),
         marker = '',
         markersize = 2,
         linestyle="-",
         linewidth=0.6,
         color='g')
ax2.plot(tec3_timeus,
         np.array(tec3_tee)*np.array(tec6_e2t)/KET,
         label="TEC3.TEE*K_e2t/%d"%(KET),
         marker = '',
         markersize = 2,
         linestyle="--",
         linewidth=0.6,
         color='c')
ax2t=ax2.twinx()
ax2t.plot(tec6_timeus,
         tec6_e2t,
         label="TEC6.e2t",
         marker = '',
         markersize = 2,
         linestyle=":",
         linewidth=0.6,
         color='c')
ax2.legend(loc="upper left")
ax2t.legend(loc="upper right")
ax2t.set_yticks([])
ax2.set_title("055,057,066,068,071")

ax3.plot(tec3_timeus,
         tec3_tede,
         label="TEC3.TEDE",
         marker = '',
         markersize = 2,
         linestyle="-",
         linewidth=0.6,
         color='b')
ax3.plot(tec3_timeus,
         np.array(tec3_tede)*TECS_THR_DAMP.v[-1],
         label="TEC3.TEDE*TECS_THR_DAMP",
         marker = '',
         markersize = 2,
         linestyle="-",
         linewidth=0.6,
         color='r')
ax3.plot(tec3_timeus_cut,
         Th1,
         label="dotSTEerr*damp",
         marker = '',
         markersize = 2,
         linestyle="--",
         linewidth=0.6,
         color='c')
ax3.legend()
ax3.set_title("058,068")
)";
    static inline QString example_02_2_of_3=R"(
ax4.plot(tec3_timeus,
         tec3_pedd,
         label="TEC3.PEDD",
         marker = '',
         markersize = 2,
         linestyle=":",
         linewidth=0.6,
         color='g')
ax4.plot(tec3_timeus,
         tec3_kedd,
         label="TEC3.KEDD",
         marker = '',
         markersize = 2,
         linestyle=":",
         linewidth=0.6,
         color='b')
dotSTEdem = np.array(tec3_pedd)+np.array(tec3_kedd)
ax4.plot(tec3_timeus,
         dotSTEdem,
         label="dotSTEdem",
         marker = '',
         markersize = 2,
         linestyle="-",
         linewidth=0.6,
         color='r')
ax4t=ax4.twinx()
ax4t.plot(tec3_timeus,
         tec3_fft,
         label="TEC3.FFT",
         marker = '',
         markersize = 2,
         linestyle="--",
         linewidth=0.6,
         color='c')
ax4.legend(loc="upper left")
ax4t.legend(loc="upper right")
ax4t.set_yticks([])
ax4.set_title("017,055,057,066")

ax5.plot(tec3_timeus,
         np.array(tec3_kedd),
         label="TEC3.KEDD",
         marker = '',
         markersize = 2,
         linestyle="-",
         linewidth=1,
         color='r')

len_common = MIN(len(tecs_timeus),len(tec3_timeus))
import copy
tecs_timeus_cut = copy.deepcopy(tecs_timeus[:len_common])
tec3_timeus_cut = copy.deepcopy(tec3_timeus[:len_common])
tecs_spdem_cut  = copy.deepcopy(tecs_spdem[:len_common])
tecs_dspdem_cut = copy.deepcopy(tecs_dspdem[:len_common])
tecs_sp_cut     = copy.deepcopy(tecs_sp[:len_common])
tec3_ked_cut    = copy.deepcopy(tec3_ked[:len_common])
tec3_kedd_cut   = copy.deepcopy(tec3_kedd[:len_common])
ax5.plot(tec3_timeus_cut,
         np.array(tecs_spdem_cut)*np.array(tecs_dspdem_cut)-np.array(tec3_ked_cut),
         label="TECS.spdem*TECS.dspdem-TEC3.KED",
         marker = '',
         markersize = 2,
         linestyle="-",
         linewidth=0.35,
         color='b')
ax5.plot(tecs_timeus,
         np.array(tecs_spdem)*np.array(tecs_dspdem)-np.array(tecs_sp)*np.array(tecs_dsp),
         label="TECS.spdem*TECS.dspdem-TECS.sp*TECS.dsp",
         marker = '',
         markersize = 2,
         linestyle=":",
         linewidth=0.6,
         color='c')
ax5t=ax5.twinx()
ax5t.plot(tec3_timeus_cut,
         (np.array(tecs_spdem_cut)*np.array(tecs_dspdem_cut)-np.array(tec3_kedd_cut))/np.array(tecs_sp_cut),
         label="vel_dot_lpf",
         marker = '',
         markersize = 2,
         linestyle="--",
         linewidth=0.6,
         color='b')
ax5t.plot(tecs_timeus,
         tecs_dsp,
         label="TECS.dsp",
         marker = '',
         markersize = 2,
         linestyle="--",
         linewidth=0.6,
         color='orange')
ax5.legend(loc="upper left")
ax5t.legend(loc="lower right")
ax5t.set_yticks([])
ax5.set_title("017,024,025")

ax6.plot(tec3_timeus,
         tec3_pedd,
         label="TEC3.PEDD",
         marker = '',
         markersize = 2,
         linestyle="-",
         linewidth=0.6,
         color='r')
ax6.plot(tecs_timeus,
         np.array(tecs_dhdem)*GRAVITY_MSS,
         label="TECS.dhdem*G",
         marker = '',
         markersize = 2,
         linestyle="-",
         linewidth=0.6,
         color='orange')
ax6.plot(tec3_timeus,
         tec3_ped,
         label="TEC3.PED",
         marker = '',
         markersize = 2,
         linestyle=":",
         linewidth=0.6,
         color='b')
ax6t = ax6.twinx()
ax6t.plot(tecs_timeus,
         tecs_hdem,
         label="TECS.hdem",
         marker = '',
         markersize = 2,
         linestyle="--",
         linewidth=0.6,
         color='c')
ax6t.plot(tecs_timeus,
         tecs_h,
         label="TECS.h",
         marker = '',
         markersize = 2,
         linestyle="--",
         linewidth=0.6,
         color='g')
ax6.legend(loc="upper left")
ax6t.legend(loc="upper right")
ax6.set_title("022,055")
)";
    static inline QString example_02_3_of_3=R"(
def annotate_parm(axd, name):
    print(name)
    print('=====================')
    print("idx\ttimeus\t\tvalue")
    print('---------------------')
    try:
        t,v = get_parm(name)
    except:
        print("%s Error"%(name))
        return
    tl = [0]
    cnt = 0
    for i in range(len(t)):
        xmin,xmax = axd.get_xlim()
        ymin,ymax = axd.get_ylim()
        if t[i] - tl[-1] > 2000:
            tl.append(t[i])
            cnt += 1
            print("%d\t%d\t%.3f"%(cnt,t[i],v[i]))
            s = "_".join(name.split('_')[-2:])
            c='b'
            off_y     = 0
            offy_step = (ymax-ymin)/30
            offx_step = (xmax-xmin)/250

            if s == 'TECS_RLL2THR':
                c='grey'
                off_y += offy_step * 4
            elif s == 'THR_MAX':
                c='orange'
                off_y += offy_step * 3
            elif s == 'THR_DAMP':
                c='sienna'
                off_y += offy_step * 2
            elif s == 'SKE_TCONST':
                c='magenta'
                off_y += offy_step * 1
            elif s == 'FF':
                c='blueviolet'
                off_y += offy_step * 0
            else:
                if s == 'ITH_GAIN':
                    off_y += offy_step * 5
            axd.plot([t[i],t[i]],[ymin,ymax],
                     marker = '',
                     markersize = 2,
                     linestyle="--",
                     linewidth=0.8,
                     color=c)
            axd.set_ylim(ymin,ymax)

            axd.text(t[i]+offx_step,ymin+off_y, r'%s=%.3f'%(s,v[i]), color=c, fontsize=8)
    print()

annotate_parm(ax1, 'TRIM_THROTTLE')
annotate_parm(ax2, 'TECS_RLL2THR')
annotate_parm(ax2, 'TECS_ITH_GAIN')
annotate_parm(ax2, 'THR_MAX')
annotate_parm(ax2, 'THR_MIN')
annotate_parm(ax2, 'TECS_SKE_TCONST')
annotate_parm(ax2, 'TECS_THR_DAMP')
annotate_parm(ax3, 'TECS_THR_DAMP')
annotate_parm(ax4, 'TECS_SPE_TCONST')
annotate_parm(ax5, 'TECS_DSP_TCONST')

flight_stage = FLIGHT_STAGE_MARK(ax1,FSM1, tecs_timeus)
flight_stage.draw_rectangle()
slope_stage = SLOPE_STAGE_MARK(ax1,FSM0, tecs_timeus)
slope_stage.draw_rectangle()

flight_stage = FLIGHT_STAGE_MARK(ax2,FSM1, tecs_timeus)
flight_stage.draw_rectangle()
slope_stage = SLOPE_STAGE_MARK(ax2,FSM0, tecs_timeus)
slope_stage.draw_rectangle()

flight_stage = FLIGHT_STAGE_MARK(ax3,FSM1, tecs_timeus)
flight_stage.draw_rectangle()
slope_stage = SLOPE_STAGE_MARK(ax3,FSM0, tecs_timeus)
slope_stage.draw_rectangle()

flight_stage = FLIGHT_STAGE_MARK(ax4,FSM1, tecs_timeus)
flight_stage.draw_rectangle()
slope_stage = SLOPE_STAGE_MARK(ax4,FSM0, tecs_timeus)
slope_stage.draw_rectangle()

flight_stage = FLIGHT_STAGE_MARK(ax5,FSM1, tecs_timeus)
flight_stage.draw_rectangle()
slope_stage = SLOPE_STAGE_MARK(ax5,FSM0, tecs_timeus)
slope_stage.draw_rectangle()

flight_stage = FLIGHT_STAGE_MARK(ax6,FSM1, tecs_timeus)
flight_stage.draw_rectangle()
slope_stage = SLOPE_STAGE_MARK(ax6,FSM0, tecs_timeus)
slope_stage.draw_rectangle()

# 图形交互
PT_X    = tec8_timeus
axd     = ax1

from utilities.utilities import find_closest_idx,find_closest_value

def on_press(event):
    global PT_X
    global axd, ax1, ax2, ax3, ax4, ax5, ax6

    if event.button==3: #鼠标右键点击
        idx = find_closest_idx(PT_X,event.xdata)
        xmin,xmax = axd.get_xlim()
        ymin,ymax = axd.get_ylim()
        axd.tick_params(axis='x',colors='red')
        axd.tick_params(axis='y',colors='red')
        axd.set_title('TimeUS: %d\n(Zoom this one.)'%(PT_X[idx]))
        axd.set_xlim(xmin,xmax)
        axd.set_ylim(ymin,ymax)

        ax = [ax1,ax2,ax3,ax4,ax5,ax6]
        for i in range(0,6):
            ymin,ymax = ax[i].get_ylim()
            ax[i].plot([PT_X[idx],PT_X[idx]],[ymin,ymax],
                       linestyle="--",
                       linewidth=0.5,
                       color="r")
            ax[i].set_ylim(ymin,ymax)
        plt.draw()

def button_release(event):
    global axd,ax1,ax2,ax3,ax4,ax5,ax6

    ax = [ax1,ax2,ax3,ax4,ax5,ax6]
    if event.button == 1:
        xmin, xmax = axd.get_xlim()
        for i in range(0,6):
            if axd != ax[i]:
                ax[i].set_xlim(xmin,xmax)

fig.canvas.mpl_connect('button_press_event', on_press)
fig.canvas.mpl_connect('button_release_event', button_release)

plt.show()
)";

    static inline QString lib_01=R"(#!/usr/bin/python3
# -*- coding:utf-8 -*-
import sqlite3

class LogDBParser:
    def __init__(self, db):
        self.conn = sqlite3.connect(db)

    def check_unit(self, table):
        res = None
        try:
            SQLITE_CMD = 'SELECT id,units,multipliers FROM maintable WHERE name=\\"'
            SQLITE_CMD += table
            SQLITE_CMD += '\\"'
            with self.conn:
                cur = self.conn.cursor()
                data = cur.execute(SQLITE_CMD)
                rows = cur.fetchall()
                for row in rows:
                    res = {'id':row[0], 'units':row[1], 'multipliers':row[2]}
                if len(res['units']) == 0 and len(res['multipliers']) == 0:
                    res = None
        except TypeError:
            pass
        return res

    def getData(self, table, *args):
        SQLITE_CMD = 'SELECT '
        arg_num = 0

        for n in args:
            arg_num += 1
            SQLITE_CMD += n
            SQLITE_CMD += ','
        SQLITE_CMD = SQLITE_CMD[:-1]
        SQLITE_CMD += ' FROM '
        SQLITE_CMD += table

        with self.conn:
            cur = self.conn.cursor()
            SQLITE_CMD = SQLITE_CMD.replace('Limit', '[Limit]')
            data = cur.execute(SQLITE_CMD)

        res = [[],[],[],[],[],
               [],[],[],[],[],
               [],[],[],[],[],
               [],[],[],[],[]]
        for row in data:
            if arg_num > 0:
                res[0].append(row[0])
            if arg_num > 1:
                res[1].append(row[1])
            if arg_num > 2:
                res[2].append(row[2])
            if arg_num > 3:
                res[3].append(row[3])
            if arg_num > 4:
                res[4].append(row[4])
            if arg_num > 5:
                res[5].append(row[5])
            if arg_num > 6:
                res[6].append(row[6])
            if arg_num > 7:
                res[7].append(row[7])
            if arg_num > 8:
                res[8].append(row[8])
            if arg_num > 9:
                res[9].append(row[9])
            if arg_num > 10:
                res[10].append(row[10])
            if arg_num > 11:
                res[11].append(row[11])
            if arg_num > 12:
                res[12].append(row[12])
            if arg_num > 13:
                res[13].append(row[13])
            if arg_num > 14:
                res[14].append(row[14])
            if arg_num > 15:
                res[15].append(row[15])
            if arg_num > 16:
                res[16].append(row[16])
            if arg_num > 17:
                res[17].append(row[17])
            if arg_num > 18:
                res[18].append(row[18])
            if arg_num > 19:
                res[19].append(row[19])
        return res

    def getDataASC(self, table, order, *args):
        SQLITE_CMD = 'SELECT '
        arg_num = 0

        for n in args:
            arg_num += 1
            SQLITE_CMD += n
            SQLITE_CMD += ','
        SQLITE_CMD = SQLITE_CMD[:-1]
        SQLITE_CMD += ' FROM '
        SQLITE_CMD += table
        SQLITE_CMD += ' ORDER BY '
        SQLITE_CMD += order

        with self.conn:
            cur = self.conn.cursor()
            data = cur.execute(SQLITE_CMD)

        res = [[],[],[],[],[],
               [],[],[],[],[],
               [],[],[],[],[],
               [],[],[],[],[]]
        for row in data:
            if arg_num > 0:
                res[0].append(row[0])
            if arg_num > 1:
                res[1].append(row[1])
            if arg_num > 2:
                res[2].append(row[2])
            if arg_num > 3:
                res[3].append(row[3])
            if arg_num > 4:
                res[4].append(row[4])
            if arg_num > 5:
                res[5].append(row[5])
            if arg_num > 6:
                res[6].append(row[6])
            if arg_num > 7:
                res[7].append(row[7])
            if arg_num > 8:
                res[8].append(row[8])
            if arg_num > 9:
                res[9].append(row[9])
            if arg_num > 10:
                res[10].append(row[10])
            if arg_num > 11:
                res[11].append(row[11])
            if arg_num > 12:
                res[12].append(row[12])
            if arg_num > 13:
                res[13].append(row[13])
            if arg_num > 14:
                res[14].append(row[14])
            if arg_num > 15:
                res[15].append(row[15])
            if arg_num > 16:
                res[16].append(row[16])
            if arg_num > 17:
                res[17].append(row[17])
            if arg_num > 18:
                res[18].append(row[18])
            if arg_num > 19:
                res[19].append(row[19])
        return res
)";

    static inline QString lib_02=R"(#!/usr/bin/python3
# -*- coding:utf-8 -*-
import math
FLT_ESPILON = 1.1920928955078125e-7

def safe_sqrt(v):
    return math.sqrt(v)

def rad2deg(rad):
    import numpy as np
    rad_array = np.array(rad)
    deg_array = rad_array*180/np.pi
    return deg_array

def constrain_float(v,min,max):
    if v < min:
        v = min
    if v > max:
        v = max
    return v

def is_negative(v):
    if v <= -1.0 * FLT_ESPILON:
        return True
    else:
        return False

def is_positive(v):
    if v >= FLT_ESPILON:
        return True
    else:
        return False

def is_zero(v):
    if abs(v) < FLT_ESPILON:
        return True
    else:
        return False

def sq(v):
    return v**2

def MIN(a,b):
    return a if a<b else b

def MAX(a,b):
    return a if a>b else b

def inv_sqrt_controller(output, p, D_max):
    if (is_positive(D_max) and is_zero(p)):
        return (output * output) / (2.0 * D_max);

    if ((is_negative(D_max) or is_zero(D_max)) and not is_zero(p)):
        return output / p;

    if ((is_negative(D_max) or is_zero(D_max)) and is_zero(p)):
        return 0.0;

    # calculate the velocity at which we switch from calculating the stopping point using a linear function to a sqrt function.
    linear_velocity = D_max / p;

    if (abs(output) < linear_velocity):
        # if our current velocity is below the cross-over point we use a linear function
        return output / p;

    linear_dist = D_max / sq(p);
    stopping_dist = (linear_dist * 0.5) + sq(output) / (2.0 * D_max);
    return stopping_dist if is_positive(output) else -stopping_dist

def sqrt_controller(error, p, second_ord_lim, dt):
    correction_rate = 0.0;
    if (is_negative(second_ord_lim) or is_zero(second_ord_lim)):
        # second order limit is zero or negative.
        correction_rate = error * p;
    elif (is_zero(p)):
        # P term is zero but we have a second order limit.
        if (is_positive(error)):
            correction_rate = safe_sqrt(2.0 * second_ord_lim * (error));
        elif (is_negative(error)):
            correction_rate = -safe_sqrt(2.0 * second_ord_lim * (-error));
        else:
            correction_rate = 0.0;
    else:
        # Both the P and second order limit have been defined.
        linear_dist = second_ord_lim / sq(p);
        if (error > linear_dist):
            correction_rate = safe_sqrt(2.0 * second_ord_lim * (error - (linear_dist / 2.0)));
        elif (error < -linear_dist):
            correction_rate = -safe_sqrt(2.0 * second_ord_lim * (-error - (linear_dist / 2.0)));
        else:
            correction_rate = error * p;

    if (not is_zero(dt)):
        # this ensures we do not get small oscillations by over shooting the error correction in the last time step.
        return constrain_float(correction_rate, -abs(error) / dt, abs(error) / dt);
    else:
        return correction_rate;


def LowPassFilter(sample, cutoff_freq, dt):
    if (cutoff_freq <= 0.0 or dt <= 0.0):
        return sample

    import numpy as np
    rc = 1/(2*np.pi*cutoff_freq);
    alpha = constrain_float(dt/(dt+rc), 0.0, 1.0);
    output = []
    output.append(sample[0]);
    for i in range(1,len(sample)):
        output.append(output[i-1] + (sample[i] - output[i-1]) * alpha)
    return alpha,output
)";

    static inline QString lib_03=R"(#!/usr/bin/python3
# -*- coding:utf-8 -*-
def get_bit_val(byte, index):
    if byte & (1 << index):
        return 1
    else:
        return 0

def filter_parm(start_us,end_us,parm_us):
    if parm_us >= start_us and parm_us <= end_us:
        return True
    else:
        return False

def get_same_elem(list1, list2):
    set1 = set(list1)
    set2 = set(list2)
    iset = set1.intersection(set2)
    return iset

def get_v_by_us(list_us, us, list_v):
    idx = list_us.index(us)
    return list_v[idx]

def linear_interpolation1(tb,tt,vt):
    tb_tt_same = list(get_same_elem(tb,tt))
    v_pick = {}
    for us in tb:
        if us in tb_tt_same:
            v_pick[us] = get_v_by_us(tt, us, vt)
        else:
            import copy
            temp_us = copy.deepcopy(tt)
            temp_us.append(us)
            temp_us.sort()
            vt_idx_prev = temp_us.index(us)-1 if temp_us.index(us)-1 > 0 else 0
            if vt_idx_prev < len(tt)-1:
                pct = (us - tt[vt_idx_prev]) / (tt[vt_idx_prev+1] - tt[vt_idx_prev])
                v_pick[us] = vt[vt_idx_prev] + (vt[vt_idx_prev+1] - vt[vt_idx_prev]) * pct
            else:
                v_pick[us] = vt[vt_idx_prev]
    return v_pick

# 找最近值的索引
def find_closest_idx(lst, value):
    import numpy as np
    array = np.asarray(lst)
    idx = (np.abs(array - value)).argmin()
    return idx

# 找最近的值
def find_closest_value(lst, value):
    import numpy as np
    array = np.asarray(lst)
    idx = (np.abs(array - value)).argmin()
    return array[idx]

# 计算时间差
def calt_delta_t(ms1, ms2):
    delta_str = ""
    delta_m   = 0
    delta_us = ms2-ms1
    delta_s  = delta_us * 1e-6
    delta_str = "%.2f s"%(delta_s)
    if delta_s > 60:
        delta_m = delta_s // 60
        delta_s = delta_s % 60
        delta_str = "%d min %.2f s"%(delta_m,delta_s)
    if delta_m > 60:
        delta_h = delta_m // 60
        delta_m = delta_m % 60
        delta_str = "%d h %d min %.2f s"%(delta_h,delta_m,delta_s)
    return delta_str
)";
};

#endif // DIALOGPYTHON_H
