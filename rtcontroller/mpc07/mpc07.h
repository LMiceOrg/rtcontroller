// mpc07.h: interface for the CMPC07Controller class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOADDLL_H__B8267873_59D6_11D5_B5A0_B42C742B6543__INCLUDED_)
#define AFX_LOADDLL_H__B8267873_59D6_11D5_B5A0_B42C742B6543__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//////////////////////////////////////////////////////////////////////
//DLL函数指针类型定义
//控制卡和轴设置函数
typedef int (CALLBACK* LPFNDLL_auto_set)(void);
typedef int (CALLBACK* LPFNDLL_init_board)(void);
typedef int (CALLBACK* LPFNDLL_get_max_axe)(void);
typedef int (CALLBACK* LPFNDLL_get_board_num)(void);
typedef int (CALLBACK* LPFNDLL_get_axe)(int board_no);
typedef int (CALLBACK* LPFNDLL_set_outmode)(int ch,int mode,int logic);
typedef int (CALLBACK* LPFNDLL_set_maxspeed)(int ch , double speed);
typedef int (CALLBACK* LPFNDLL_set_conspeed)(int ch , double conspeed);
typedef double (CALLBACK* LPFNDLL_get_conspeed)(int ch);
typedef int (CALLBACK* LPFNDLL_set_profile)(int ch , double vl , double vh , double ad);
typedef int (CALLBACK* LPFNDLL_get_profile)(int ch , double *vl , double *vh , double *ad);
typedef int (CALLBACK* LPFNDLL_set_vector_conspeed)(double conspeed);
typedef int (CALLBACK* LPFNDLL_set_vector_profile)(double vec_vl , double vec_vh ,double vec_ad);
typedef double (CALLBACK* LPFNDLL_get_vector_conspeed)();
typedef int (CALLBACK* LPFNDLL_get_vector_profile)(double *vec_vl , double *vec_vh ,double *vec_ad);
typedef double (CALLBACK* LPFNDLL_get_rate)(int ch);

//运动指令函数
typedef int (CALLBACK* LPFNDLL_con_pmove)(int ch,long step);
typedef int (CALLBACK* LPFNDLL_fast_pmove)(int ch,long step);
typedef int (CALLBACK* LPFNDLL_con_pmove2)(int ch1,long step1,int ch2,long step2);
typedef int (CALLBACK* LPFNDLL_fast_pmove2)(int ch1,long step1,int ch2,long step2);
typedef int (CALLBACK* LPFNDLL_con_pmove3)(int ch1,long step1,int ch2,long step2,int ch3,long step3);
typedef int (CALLBACK* LPFNDLL_fast_pmove3)(int ch1,long step1,int ch2,long step2,int ch3,long step3);
typedef int (CALLBACK* LPFNDLL_con_vmove)(int ch,int dir);
typedef int (CALLBACK* LPFNDLL_fast_vmove)(int ch,int dir);
typedef int (CALLBACK* LPFNDLL_con_vmove2)(int ch1,int dir1,int ch2,int dir2);
typedef int (CALLBACK* LPFNDLL_fast_vmove2)(int ch1,int dir1,int ch2,int dir2);
typedef int (CALLBACK* LPFNDLL_con_vmove3)(int ch1,int dir1,int ch2,int dir2,int ch3,int dir3);
typedef int (CALLBACK* LPFNDLL_fast_vmove3)(int ch1,int dir1,int ch2,int dir2,int ch3,int dir3);
typedef int (CALLBACK* LPFNDLL_con_hmove)(int ch,int dir);
typedef int (CALLBACK* LPFNDLL_fast_hmove)(int ch,int dir);
typedef int (CALLBACK* LPFNDLL_con_hmove2)(int ch1,int dir1,int ch2,int dir2);
typedef int (CALLBACK* LPFNDLL_fast_hmove2)(int ch1,int dir1,int ch2,int dir2);
typedef int (CALLBACK* LPFNDLL_con_hmove3)(int ch1,int dir1,int ch2,int dir2,int ch3,int dir3);
typedef int (CALLBACK* LPFNDLL_fast_hmove3)(int ch1,int dir1,int ch2,int dir2,int ch3,int dir3);
typedef int (CALLBACK* LPFNDLL_con_line2)(int CHX,long pos1,int CHY, long pos2);
typedef int (CALLBACK* LPFNDLL_con_line3)(int ch1,long pos1,int ch2,long pos2,int ch3,long pos3);
typedef int (CALLBACK* LPFNDLL_fast_line2)(int CHX,long pos1,int CHY, long pos2);
typedef int (CALLBACK* LPFNDLL_fast_line3)(int ch1,long pos1,int ch2,long pos2,int ch3,long pos3);

//制动函数
typedef void (CALLBACK* LPFNDLL_sudden_stop)(int ch);
typedef void (CALLBACK* LPFNDLL_sudden_stop2)(int ch1,int ch2);
typedef void (CALLBACK* LPFNDLL_sudden_stop3)(int ch1,int ch2,int ch3);
typedef void (CALLBACK* LPFNDLL_decel_stop)(int ch);
typedef void (CALLBACK* LPFNDLL_decel_stop2)(int ch1,int ch2);
typedef void (CALLBACK* LPFNDLL_decel_stop3)(int ch1,int ch2,int ch3);

//位置和状态设置函数
typedef long (CALLBACK* LPFNDLL_set_abs_pos)(int ch,long pos);
typedef int (CALLBACK* LPFNDLL_reset_pos)(int ch);
typedef int (CALLBACK* LPFNDLL_reset_cmd_counter)();
typedef int (CALLBACK* LPFNDLL_set_dir)(int ch,int dir);
typedef int (CALLBACK* LPFNDLL_enable_sd)(int ch,int flag);
typedef int (CALLBACK* LPFNDLL_set_sd_logic)(int ch,int flag);
typedef int (CALLBACK* LPFNDLL_set_el_logic)(int ch,int flag);
typedef int (CALLBACK* LPFNDLL_set_org_logic)(int ch,int flag);
typedef int (CALLBACK* LPFNDLL_set_alm_logic)(int ch,int flag);
typedef int (CALLBACK* LPFNDLL_get_abs_pos)(int ch,long *pos);
typedef int (CALLBACK* LPFNDLL_get_rel_pos)(int ch,long *pos);
typedef long (CALLBACK* LPFNDLL_get_cur_dir)(int ch);

//状态查询函数
typedef int (CALLBACK* LPFNDLL_check_status)(int ch);
typedef int (CALLBACK* LPFNDLL_check_done)(int ch);
typedef int (CALLBACK* LPFNDLL_check_limit)(int ch);
typedef int (CALLBACK* LPFNDLL_check_home)(int ch);
typedef int (CALLBACK* LPFNDLL_check_SD)(int ch);
typedef int (CALLBACK* LPFNDLL_check_alarm)(int ch);
typedef int (CALLBACK* LPFNDLL_check_IC)(int cardno);
typedef int (CALLBACK* LPFNDLL_get_cmd_counter)();

//I/O口操作函数
typedef int (CALLBACK* LPFNDLL_checkin_byte)(int cardno);
typedef int (CALLBACK* LPFNDLL_checkin_bit)(int cardno,int bitno);
typedef int (CALLBACK* LPFNDLL_outport_bit)(int cardno,int bitno,int status);
typedef int (CALLBACK* LPFNDLL_outport_byte)(int cardno,int bytedata);

//其它函数
typedef int (CALLBACK* LPFNDLL_set_backlash)(int axis,int blash);
typedef int (CALLBACK* LPFNDLL_start_backlash)(int axis);
typedef int (CALLBACK* LPFNDLL_end_backlash)(int axis);
typedef int (CALLBACK* LPFNDLL_change_speed)(int ch,double speed);
typedef int (CALLBACK* LPFNDLL_Outport)(int portid,unsigned char byte);
typedef int (CALLBACK* LPFNDLL_Inport)(int portid);
typedef int (CALLBACK* LPFNDLL_get_lib_ver)(long* major,long *minor1,long *minor2);
typedef int (CALLBACK* LPFNDLL_get_sys_ver)(long* major,long *minor1,long *minor2);
typedef int (CALLBACK* LPFNDLL_get_card_ver)(long cardno,long *type,long* major,long *minor1,long *minor2);

typedef int (CALLBACK* LPFNDLL_get_last_err)();
typedef int (CALLBACK* LPFNDLL_get_err)(int index,int *data);
typedef int (CALLBACK* LPFNDLL_reset_err)();

enum mpc_state {
    MPC_UNKNOWN = 0,    //没有加载动态库
    MPC_LOADED,         //已经加载完成动态库.没有初始化
    MPC_INITED,         //已经初始化，没有工作或者退出工作线程
    MPC_WORKING         //正在工作,创建工作线程

};

enum mpc_error {
    MPCE_SUCCESS = 0,   //正常
    MPCE_ERROR,         //错误
    MPCE_ENOCARD,       //卡错误
    MPCE_EMULTICARD,    //多张卡
    MPCE_EINIT,         //初始化错误

    //initboard
    MPCE_ELOADLIBRARY,  //载入动态库错误
    MPCE_EPROCADDRESS,  //函数指针错误

    //working
    MPCE_EWORKING
};

//mpc.dll函数封装类
class CMPC07Controller
{
public:
    CMPC07Controller();
    ~CMPC07Controller();
    mpc_state state() const;
    mpc_error canWork();
    mpc_error stop();
    //正常工作时只需调用此函数
    mpc_error work(double x, double y);

private:
    mpc_state m_state;  //控制器状态
	HINSTANCE hDLL;
    int axe_size;
    int card_size;

    double max_speed;
    double con_speed;
    double fast_low;
    double fast_high;
    double fast_acc;

    //轴当前状态
    long xpos;
    long ypos;
    //运动目标
    long xpuls;
    long ypuls;
    //运动误差判断标准
    double delta;
    //运动时间判断
    double beginTime;
    double duration;


	int LoadDllFun();
    int InitBoard();
    int StartWork();
    int CheckWork();
    void AfterAWhile();
    double TimeNow();
public:
	///////////////////////////////////
	//函数指针定义
	//控制卡和轴设置函数
    //////////////////////////////////
	LPFNDLL_auto_set auto_set;
	LPFNDLL_init_board init_board;
	LPFNDLL_get_max_axe get_max_axe;
	LPFNDLL_get_board_num get_board_num;
	LPFNDLL_get_axe get_axe;
	LPFNDLL_set_outmode set_outmode;
	LPFNDLL_set_maxspeed set_maxspeed;
	LPFNDLL_set_conspeed set_conspeed;
	LPFNDLL_get_conspeed get_conspeed;
	LPFNDLL_set_profile set_profile;
	LPFNDLL_get_profile get_profile;
	LPFNDLL_set_vector_conspeed set_vector_conspeed;
	LPFNDLL_set_vector_profile set_vector_profile;
	LPFNDLL_get_vector_conspeed get_vector_conspeed;
	LPFNDLL_get_vector_profile get_vector_profile;
	LPFNDLL_get_rate get_rate;

	//运动函数
	LPFNDLL_con_pmove con_pmove;
	LPFNDLL_fast_pmove fast_pmove;
	LPFNDLL_con_pmove2 con_pmove2;
	LPFNDLL_fast_pmove2 fast_pmove2;
	LPFNDLL_con_pmove3 con_pmove3;
	LPFNDLL_fast_pmove3 fast_pmove3;
	LPFNDLL_con_vmove con_vmove;
	LPFNDLL_fast_vmove fast_vmove;
	LPFNDLL_con_vmove2 con_vmove2;
	LPFNDLL_fast_vmove2 fast_vmove2;
	LPFNDLL_con_vmove3 con_vmove3;
	LPFNDLL_fast_vmove3 fast_vmove3;
	LPFNDLL_con_hmove con_hmove;
	LPFNDLL_fast_hmove fast_hmove;
	LPFNDLL_con_hmove2 con_hmove2;
	LPFNDLL_fast_hmove2 fast_hmove2;
	LPFNDLL_con_hmove3 con_hmove3;
	LPFNDLL_fast_hmove3 fast_hmove3;
	LPFNDLL_con_line2 con_line2;
	LPFNDLL_con_line3 con_line3;
	LPFNDLL_fast_line2 fast_line2;
	LPFNDLL_fast_line3 fast_line3;

	//制动函数
	LPFNDLL_sudden_stop sudden_stop;
	LPFNDLL_sudden_stop2 sudden_stop2;
	LPFNDLL_sudden_stop3 sudden_stop3;
	LPFNDLL_decel_stop decel_stop;
	LPFNDLL_decel_stop2 decel_stop2;
	LPFNDLL_decel_stop3 decel_stop3;
	
	//位置和状态设置函数
	LPFNDLL_set_abs_pos set_abs_pos;
	LPFNDLL_reset_pos reset_pos;
	LPFNDLL_reset_cmd_counter reset_cmd_counter;
	LPFNDLL_set_dir set_dir;
	LPFNDLL_enable_sd enable_sd;
	LPFNDLL_set_sd_logic set_sd_logic;
	LPFNDLL_set_el_logic set_el_logic;
	LPFNDLL_set_org_logic set_org_logic;
	LPFNDLL_set_alm_logic set_alm_logic;
	LPFNDLL_get_abs_pos get_abs_pos;
	LPFNDLL_get_rel_pos get_rel_pos;
	LPFNDLL_get_cur_dir get_cur_dir;

	//状态查询函数
	LPFNDLL_check_status check_status;
	LPFNDLL_check_done check_done;
	LPFNDLL_check_limit check_limit;
	LPFNDLL_check_home check_home;
	LPFNDLL_check_SD check_SD;
	LPFNDLL_check_alarm check_alarm;
	LPFNDLL_check_IC check_IC;
	LPFNDLL_get_cmd_counter get_cmd_counter;

	//I/O口操作函数
	LPFNDLL_checkin_byte checkin_byte;
	LPFNDLL_checkin_bit checkin_bit;
	LPFNDLL_outport_bit outport_bit;
	LPFNDLL_outport_byte outport_byte;

	//其它函数
	LPFNDLL_set_backlash set_backlash;
	LPFNDLL_start_backlash start_backlash;
	LPFNDLL_end_backlash end_backlash;
	LPFNDLL_change_speed change_speed;
	LPFNDLL_Outport Outport;
	LPFNDLL_Inport Inport;
	LPFNDLL_get_lib_ver get_lib_ver;
	LPFNDLL_get_sys_ver get_sys_ver;
	LPFNDLL_get_card_ver get_card_ver;

	//错误代码操作函数
	LPFNDLL_get_last_err get_last_err;
	LPFNDLL_get_err get_err;
	LPFNDLL_reset_err reset_err;
};

#endif // !defined(AFX_LOADDLL_H__B8267873_59D6_11D5_B5A0_B42C742B6543__INCLUDED_)
