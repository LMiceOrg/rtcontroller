// testdemo.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "LoadDll.h"
#include<iostream>
#include<string.h>
#include <string>
#include <iostream>
using std::string;
using std::cout;




          CLoadDll  m_cDll;
          string	m_sXAxes;
	string	m_sXAccel;
	string	m_sXHighSpeed;
	string	m_sXLowSpeed;
	string	m_sXStep;
	string	m_sYAccel;
	string	m_sYAxes;
	string	m_sYHighSpeed;
	string	m_sYLowSpeed;
	string	m_sYStep;
	string	m_sArcR;
	string	m_sArcVAccel;
	string	m_sArcVHighSpeed;
	string	m_sArcVLowSpeed;
	string	m_sArcXAxes;
	string	m_sArcXCen;
	string	m_sArcYAxes;
	string	m_sArcYCen;
	string	m_sLAccel;
	string	m_sLHighSpeed;
	string	m_sLLowSpeed;
	string	m_sLXAxes;
	string	m_sLXStep;
	string	m_sLYAxes;
	string	m_sLYStep;

int InitBoard()
{
	int Rtn;
	//////////////////
	
	//初始化板卡
	std::cout<<m_cDll.auto_set<<"\n";
	Rtn=m_cDll.auto_set();			//自动设置
	if(Rtn<=0 )
	{
		if(Rtn==-1)

		//::AfxMessageBox("检测不到卡!",MB_OK);
		{std::cout<<"no card"<<std::endl;}
		if(Rtn==-10)
		//::AfxMessageBox("使用多卡搭配不当!",MB_OK);
		{std::cout<<"multiple card"<<std::endl;}
	}
	std::cout<<"aix size:"<<Rtn;
	//glTotalAxes = Rtn;

	//MessageBox("OK");
	Rtn=m_cDll.init_board();		//初始化
	if(Rtn<=0)
	{
		//MessageBox("初始化错误!");
		std::cout<<"bad init"<<std::endl;
	}
//	glTotalCards = Rtn;
	std::cout<<"card size:"<<Rtn;
	return 0;
	
}
void InitParam()
{


	//////////////////////////////////////////////
	//直线运动
	m_sXAxes="1";				//X轴参数
	m_sXLowSpeed="2000";
	m_sXHighSpeed="10000";
	m_sXAccel="500";
	m_sXStep="400000";

	m_sYAxes="2";				//Y轴参数
	m_sYLowSpeed="1";
	m_sYHighSpeed="50";
	m_sYAccel="20";
	m_sYStep="50";

	//////////////////////////////////////////////
	//圆弧插补
	m_sArcXAxes="1";
	m_sArcYAxes="2";
	m_sArcVLowSpeed="2";
	m_sArcVHighSpeed="40";
	m_sArcVAccel="2";
	m_sArcXCen="20";
	m_sArcYCen="0";
	m_sArcR="36";

	//////////////////////////////////////////////
	//直线插补
	m_sLXAxes="1";				//X轴轴号
	m_sLXStep="40";			//X轴运动距离
	m_sLYAxes="2";				//Y轴轴号
	m_sLYStep="40";			//Y轴运动距离
	m_sLLowSpeed="1";
	m_sLHighSpeed="5";
	m_sLAccel="2";

	
}
void move()
{
	int ret;

	int ChX;
	int ChY;
	int StepX;
	int StepY;
	int MaxSpeed;
	int LowSpeed;
	int Accl;

                    ChX=atoi(m_sXAxes.c_str());	
		ChY=atoi(m_sYAxes.c_str());
		StepX=atoi(m_sXStep.c_str());	
		StepY=atoi(m_sYStep.c_str());
		MaxSpeed = atoi(m_sXHighSpeed.c_str());
		LowSpeed = atoi(m_sXLowSpeed.c_str());
		Accl = atoi(m_sXAccel.c_str());
		
		m_cDll.reset_pos(0);

		ret = m_cDll.set_maxspeed(ChX, MaxSpeed);
		std::cout<<	"m_cDll.set_maxspeed("<<ChX<<", "<<MaxSpeed<<")="<<ret<<"\n";

		//m_cDll.set_maxspeed(ChY, HighSpeed);
		//std::cout<<	"m_cDll.set_maxspeed("<<ChX<<", "<<MaxSpeed<<")="<<ret<<"\n";

		ret = m_cDll.set_conspeed(ChX, LowSpeed);
		std::cout<<	"m_cDll.set_conspeed("<<ChX<<", "<<LowSpeed<<")="<<ret<<"\n";


		//m_cDll.set_conspeed(ChY,atoi(m_sYLowSpeed.c_str()));

		ret = m_cDll.set_profile(ChX,
			LowSpeed,
			MaxSpeed,
			Accl);
		std::cout<<	"m_cDll.set_profile("<<ChX<<", "<<Accl<<")="<<ret<<"\n";

		//m_cDll.set_profile(ChY,atoi(m_sYLowSpeed.c_str()),
		//	atoi(m_sYHighSpeed.c_str()),
		//	atoi(m_sYAccel.c_str()));

                   ret = m_cDll.con_pmove(ChX,StepX);
	         std::cout<<"m_cDll.con_pmove="<<ret<<"\n";


}


int _tmain(int argc, _TCHAR* argv[])
{

////////////////////////
	std::cout<<"begin"<<std::endl;
	//初始化设备
	InitBoard();
	std::cout<<"finish initboard"<<std::endl;
	
	
	
	////////////////////////
	//初始化运动参数设置
	InitParam();
          std::cout<<"finish InitParam"<<std::endl;
          std::cout<<"begin move"<<std::endl;

	move();

	getchar();

         
	return 0;
}

