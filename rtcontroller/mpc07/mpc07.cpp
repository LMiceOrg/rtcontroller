// mpc07.cpp: implementation of the CLoadDll class.
//
//////////////////////////////////////////////////////////////////////

#include "mpc07.h"

#if defined(__WIN32)
#include <WinBase.h>
#pragma comment(lib, "Kernel32.lib")
#else
#include <unistd.h>
#include <sys/time.h>
#endif

#include <math.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMPC07Controller::CMPC07Controller()
{
    m_state = MPC_UNKNOWN;
    hDLL = NULL;
    axe_size = 0;
    card_size = 0;

    max_speed = 10000; //pps
    con_speed = 1000;
    fast_low = 100;
    fast_high = 6000;
    fast_acc = 3000;
    delta = 500;

    beginTime = 0;  //sec
    duration = 10;

	LoadDllFun();			//加载DLL
}

CMPC07Controller::~CMPC07Controller()
{
	if (hDLL != NULL)
	{
		FreeLibrary(hDLL);	//释放DLL
    }
}

mpc_state CMPC07Controller::state() const
{
    return m_state;
}

#define MPC_FAILED(x) ((x) != MPCE_SUCCESS)

mpc_error CMPC07Controller::canWork()
{
    int ret = MPCE_ERROR;
    switch(m_state) {
    case MPC_UNKNOWN:
        ret = LoadDllFun();
        if (MPC_FAILED(ret)) {
            break;
        }
        m_state = MPC_LOADED;
    case MPC_LOADED:
        ret = InitBoard();
        if(MPC_FAILED(ret)) {
            break;
        }
        m_state = MPC_INITED;
    case MPC_INITED:
        ret = MPCE_SUCCESS;
        break;
    case MPC_WORKING:
        ret = CheckWork();
        if(MPC_FAILED(ret)) {
            break;
        }
        m_state = MPC_INITED;
        break;
    }

    return ret;
}

mpc_error CMPC07Controller::stop()
{
    sudden_stop2(1,2);
    return MPCE_SUCCESS;
}

mpc_error CMPC07Controller::work(double x, double y)
{
    mpc_error ret = canWork();
    if( ret ==MPCE_SUCCESS) {

        StartWork();
        m_state = MPC_WORKING;

        if(x != 0 && y == 0) {          // 1. only X-axe
            fast_pmove(1, (long)x);
        } else if(x == 0 && y != 0) {   // 2. only Y-axe
            fast_pmove(2, (long)y);
        } else if(x != 0 && y != 0) {   // 3. both of X,Y axes
            fast_pmove2(1, (long)x, 2, (long)y);
        } else {
            m_state = MPC_INITED;
        }
    }
    return ret;
}

int CMPC07Controller::LoadDllFun()
{
    int ret = MPCE_SUCCESS;
    hDLL = LoadLibraryA("MPC07");	//加载mpc07.dll
	if (hDLL != NULL)
	{
		///////////////////////////////////////////////////////////
		//以下取得各个DLL函数指针
		auto_set = (LPFNDLL_auto_set)GetProcAddress(hDLL,"auto_set");
		init_board = (LPFNDLL_init_board)GetProcAddress(hDLL,"init_board");
		get_max_axe = (LPFNDLL_get_max_axe)GetProcAddress(hDLL,"get_max_axe");
		get_board_num = (LPFNDLL_get_board_num)GetProcAddress(hDLL,"get_board_num");
		get_axe = (LPFNDLL_get_axe)GetProcAddress(hDLL,"get_axe");
		set_outmode = (LPFNDLL_set_outmode)GetProcAddress(hDLL,"set_outmode");
		set_maxspeed = (LPFNDLL_set_maxspeed)GetProcAddress(hDLL,"set_maxspeed");
		set_conspeed = (LPFNDLL_set_conspeed)GetProcAddress(hDLL,"set_conspeed");
		get_conspeed = (LPFNDLL_get_conspeed)GetProcAddress(hDLL,"get_conspeed");
		set_profile = (LPFNDLL_set_profile)GetProcAddress(hDLL,"set_profile");
		get_profile = (LPFNDLL_get_profile)GetProcAddress(hDLL,"get_profile");
		set_vector_conspeed = (LPFNDLL_set_vector_conspeed)GetProcAddress(hDLL,"set_vector_conspeed");
		set_vector_profile = (LPFNDLL_set_vector_profile)GetProcAddress(hDLL,"set_vector_profile");
		get_vector_conspeed = (LPFNDLL_get_vector_conspeed)GetProcAddress(hDLL,"get_vector_conspeed");
		get_vector_profile = (LPFNDLL_get_vector_profile)GetProcAddress(hDLL,"get_vector_profile");
		get_rate = (LPFNDLL_get_rate)GetProcAddress(hDLL,"get_rate");

		con_pmove = (LPFNDLL_con_pmove)GetProcAddress(hDLL,"con_pmove");
		fast_pmove = (LPFNDLL_fast_pmove)GetProcAddress(hDLL,"fast_pmove");
		con_pmove2 = (LPFNDLL_con_pmove2)GetProcAddress(hDLL,"con_pmove2");
		fast_pmove2 = (LPFNDLL_fast_pmove2)GetProcAddress(hDLL,"fast_pmove2");
		con_pmove3 = (LPFNDLL_con_pmove3)GetProcAddress(hDLL,"con_pmove3");
		fast_pmove3 = (LPFNDLL_fast_pmove3)GetProcAddress(hDLL,"fast_pmove3");
		con_vmove = (LPFNDLL_con_vmove)GetProcAddress(hDLL,"con_vmove");
		fast_vmove = (LPFNDLL_fast_vmove)GetProcAddress(hDLL,"fast_vmove");
		con_vmove2 = (LPFNDLL_con_vmove2)GetProcAddress(hDLL,"con_vmove2");
		fast_vmove2 = (LPFNDLL_fast_vmove2)GetProcAddress(hDLL,"fast_vmove2");
		con_vmove3 = (LPFNDLL_con_vmove3)GetProcAddress(hDLL,"con_vmove3");
		fast_vmove3 = (LPFNDLL_fast_vmove3)GetProcAddress(hDLL,"fast_vmove3");
		con_hmove = (LPFNDLL_con_hmove)GetProcAddress(hDLL,"con_hmove");
		fast_hmove = (LPFNDLL_fast_hmove)GetProcAddress(hDLL,"fast_hmove");
		con_hmove2 = (LPFNDLL_con_hmove2)GetProcAddress(hDLL,"con_hmove2");
		fast_hmove2 = (LPFNDLL_fast_hmove2)GetProcAddress(hDLL,"fast_hmove2");
		con_hmove3 = (LPFNDLL_con_hmove3)GetProcAddress(hDLL,"con_hmove3");
		fast_hmove3 = (LPFNDLL_fast_hmove3)GetProcAddress(hDLL,"fast_hmove3");
		con_line2 = (LPFNDLL_con_line2)GetProcAddress(hDLL,"con_line2");
		con_line3 = (LPFNDLL_con_line3)GetProcAddress(hDLL,"con_line3");
		fast_line2 = (LPFNDLL_fast_line2)GetProcAddress(hDLL,"fast_line2");
		fast_line3 = (LPFNDLL_fast_line3)GetProcAddress(hDLL,"fast_line3");

		//制动函数
		sudden_stop = (LPFNDLL_sudden_stop)GetProcAddress(hDLL,"sudden_stop");
		sudden_stop2 = (LPFNDLL_sudden_stop2)GetProcAddress(hDLL,"sudden_stop2");
		sudden_stop3 = (LPFNDLL_sudden_stop3)GetProcAddress(hDLL,"sudden_stop3");
		decel_stop = (LPFNDLL_decel_stop)GetProcAddress(hDLL,"decel_stop");
		decel_stop2 = (LPFNDLL_decel_stop2)GetProcAddress(hDLL,"decel_stop2");
		decel_stop3 = (LPFNDLL_decel_stop3)GetProcAddress(hDLL,"decel_stop3");

		//位置和状态设置函数
		set_abs_pos = (LPFNDLL_set_abs_pos)GetProcAddress(hDLL,"set_abs_pos");
		reset_pos = (LPFNDLL_reset_pos)GetProcAddress(hDLL,"reset_pos");
		reset_cmd_counter = (LPFNDLL_reset_cmd_counter)GetProcAddress(hDLL,"reset_cmd_counter");
		set_dir = (LPFNDLL_set_dir)GetProcAddress(hDLL,"set_dir");
		enable_sd = (LPFNDLL_enable_sd)GetProcAddress(hDLL,"enable_sd");
		set_sd_logic = (LPFNDLL_set_sd_logic)GetProcAddress(hDLL,"set_sd_logic");
		set_el_logic = (LPFNDLL_set_el_logic)GetProcAddress(hDLL,"set_el_logic");
		set_org_logic = (LPFNDLL_set_org_logic)GetProcAddress(hDLL,"set_org_logic");
		set_alm_logic = (LPFNDLL_set_alm_logic)GetProcAddress(hDLL,"set_alm_logic");
		get_abs_pos = (LPFNDLL_get_abs_pos)GetProcAddress(hDLL,"get_abs_pos");
		get_rel_pos = (LPFNDLL_get_rel_pos)GetProcAddress(hDLL,"get_rel_pos");
		get_cur_dir = (LPFNDLL_get_cur_dir)GetProcAddress(hDLL,"get_cur_dir");

		//状态查询函数
		check_status = (LPFNDLL_check_status)GetProcAddress(hDLL,"check_status");
		check_done = (LPFNDLL_check_done)GetProcAddress(hDLL,"check_done");
		check_limit = (LPFNDLL_check_limit)GetProcAddress(hDLL,"check_limit");
		check_home = (LPFNDLL_check_home)GetProcAddress(hDLL,"check_home");
		check_SD = (LPFNDLL_check_SD)GetProcAddress(hDLL,"check_SD");
		check_alarm = (LPFNDLL_check_alarm)GetProcAddress(hDLL,"check_alarm");
		check_IC = (LPFNDLL_check_IC)GetProcAddress(hDLL,"check_IC");
		get_cmd_counter = (LPFNDLL_get_cmd_counter)GetProcAddress(hDLL,"get_cmd_counter");

		checkin_byte = (LPFNDLL_checkin_byte)GetProcAddress(hDLL,"checkin_byte");
		checkin_bit = (LPFNDLL_checkin_bit)GetProcAddress(hDLL,"checkin_bit");
		outport_bit = (LPFNDLL_outport_bit)GetProcAddress(hDLL,"outport_bit");
		outport_byte = (LPFNDLL_outport_byte)GetProcAddress(hDLL,"outport_byte");

		//其它函数
		set_backlash = (LPFNDLL_set_backlash)GetProcAddress(hDLL,"set_backlash");
		start_backlash = (LPFNDLL_start_backlash)GetProcAddress(hDLL,"start_backlash");
		end_backlash = (LPFNDLL_end_backlash)GetProcAddress(hDLL,"end_backlash");
		change_speed = (LPFNDLL_change_speed)GetProcAddress(hDLL,"change_speed");
		Outport = (LPFNDLL_Outport)GetProcAddress(hDLL,"Outport");
		Inport = (LPFNDLL_Inport)GetProcAddress(hDLL,"Inport");
		get_lib_ver = (LPFNDLL_get_lib_ver)GetProcAddress(hDLL,"get_lib_ver");
		get_sys_ver = (LPFNDLL_get_sys_ver)GetProcAddress(hDLL,"get_sys_ver");
		get_card_ver = (LPFNDLL_get_card_ver)GetProcAddress(hDLL,"get_card_ver");

		//错误代码操作函数
		get_last_err = (LPFNDLL_get_last_err)GetProcAddress(hDLL,"get_last_err");
		get_err = (LPFNDLL_get_err)GetProcAddress(hDLL,"get_err");
		reset_err = (LPFNDLL_reset_err)GetProcAddress(hDLL,"reset_err");

		////////////////////////////////////////////////////////////////////
		//若无法取得函数指针
        if (!auto_set||!init_board) {
			// handle the error
			FreeLibrary(hDLL);
            ret = MPCE_EPROCADDRESS;
		}
    } else {
        ret = MPCE_ELOADLIBRARY;
    }

    return ret;

}

int CMPC07Controller::InitBoard()
{
    int ret = MPCE_SUCCESS;

    axe_size=auto_set();			//自动设置
    if(axe_size > 0) {
        card_size = init_board();   //初始化
        if(card_size <= 0) {
            ret = MPCE_EINIT;       //初始化错误
        }
    } else {
        if(axe_size == -1)
            ret = MPCE_ENOCARD;     //检测不到卡
        else if(axe_size == -10)
            ret = MPCE_EMULTICARD;  //使用多卡搭配不当
        else
            ret = MPCE_ERROR;
    }

    int chi;
    for(chi=0; chi< axe_size; ++chi) {
        set_outmode(chi, 1, 0); //脉冲输出模式设置(1 为脉冲/方向方式,0 为双脉冲方式);
        //设置每个轴的最大速度
        set_maxspeed(chi, max_speed); //pps
        //设定常速运动
        set_conspeed(chi, con_speed);
        //设定快速运动
        set_profile(chi, fast_low, fast_high, fast_acc);

    }

    return ret;
}

int CMPC07Controller::StartWork()
{
    int ret = MPCE_SUCCESS;
    xpos = 0;
    ypos = 0;
    sudden_stop2(1,2);
    reset_pos(1);
    reset_pos(2);

    beginTime = TimeNow();

    return ret;
}

int CMPC07Controller::CheckWork()
{
    int ret = MPCE_SUCCESS;
    double now = TimeNow();
    get_rel_pos(1, &xpos);
    get_rel_pos(2, &ypos);
    if((fabs(xpos - xpuls) <delta &&
            fasb(ypos - ypuls) < delta) ||
        ( now - beginTime < duration ) ) {
        //Ok
        AfterAWhile();
        sudden_stop2(1,2);
    } else {
        ret = MPCE_EWORKING;
    }
    return ret;
}

void CMPC07Controller::AfterAWhile()
{
#if defined(__WIN32)
    Sleep(100); //100-ms
#else
    usleep(100*1000);//100-ms
#endif
}

double CMPC07Controller::TimeNow()
{
    double t;
#if defined(__WIN32)
    DWORD dw = GetTickCount();
    t = dw;
    t = t / 1000;
#else
    struct timeval tp;

    gettimeofday(&tp, NULL);
    t = tp.tv_sec + (double)tp.tv_usec / 1000000.;
#endif
    return t;
}
