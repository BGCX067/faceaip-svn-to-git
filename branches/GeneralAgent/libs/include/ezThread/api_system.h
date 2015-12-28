/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * api_system.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: api_system.h 5884 2012-05-16 09:16:13Z WuJunjie $
 *
 *  Explain:
 *     -系统时间设置，关机与重启，系统资源状况，升级等API-
 *
 *  Update:
 *     2012-05-16 09:09:51   Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifndef __SYSTEMM_API_H__
#define __SYSTEMM_API_H__

#include "typedef_ezthread.h"

#ifdef __cplusplus
extern "C"
{
#endif

	/// \defgroup SystemAPI API System
	/// 系统时间设置，关机与重启，系统资源状况，升级等API。
	/// @{

	/// 系统时间结构
	typedef struct SYSTEM_TIME
	{
		int  year;///< 年。
		int  month;///< 月，January = 1, February = 2, and so on.
		int  day;///< 日。
		int  wday;///< 星期，Sunday = 0, Monday = 1, and so on
		int  hour;///< 时。
		int  minute;///< 分。
		int  second;///< 秒。
		int  isdst;///< 夏令时标识。
	}
	SYSTEM_TIME;

	/// 系统特性结构
	typedef struct SYSTEM_CAPS
	{
		/// 总共内存大小，以Kilo-Bytes为单位。
		unsigned int MemoryTotal;

		/// 剩余内存大小，以Kilo-Bytes为单位。将关系到应用程序的内存使用策略。
		unsigned int MemoryLeft;
	}
	SYSTEM_CAPS;

	/// 衍生版本性能
	/*
	typedef struct SYSTEM_CAPS_EX
	{
		unsigned int ForNRT;			///< 是否为非实时
		unsigned char ForATM;			///< 是否为ATM机
		unsigned char HasAudioBoard;		///< 带音频板
		unsigned char HasLoopBoard;		///< 带环通板
		unsigned char HasMatrixBoard;	///< 带矩阵板
	}SYSTEM_CAPS_EX;
	*/
	typedef struct tagSYSTEM_CAPS_EX
	{
		unsigned int ForNRT;   //是否为非实时, 按位表示各个通道是否设为非实时.如0xffff表示前16路都是非实时.
		unsigned char ForATM;   //是否为ATM机, 1表示不带液晶ATM,2表示带液晶ATM
		unsigned char HasAudioBoard;  //带音频板
		unsigned char HasLoopBoard;  //带环通板
		unsigned char HasMatrixBoard; //带矩阵板
		unsigned char HasPtzBoard;           //带云台板
		unsigned char HasWlanBoard;        //带无线板
		unsigned char reserved[2];
	}
	SYSTEM_CAPS_EX;

	/// 系统升级状态
	enum system_upgrade_state
	{
	    SYSTEM_UPGRADE_OVER = 0,
	    SYSTEM_UPGRADE_INIT = 1,
	    SYSTEM_UPGRADE_RUNNING = 2
	};

	/* 一次分配缓存数量 */
	#define  PRE_MALLOC_IMG_BUF_SIZE  5*1024*1024

	/// 得到当前系统时间。这个函数可能调用的很频繁, 所有调用时应该是从内存中取得
	/// 时间,而不是直接从硬件, 在定时器里更新内存中的值.
	///
	/// \param [out] pTime 指向系统时间结构SYSTEM_TIME的指针。
	/// \retval 0  成功
	/// \retval <0  失败
	int SystemGetCurrentTime(SYSTEM_TIME * pTime);


	/// 设置当前系统时间。
	///
	/// \param [in] pTime 指向系统时间结构SYSTEM_TIME的指针。
	/// \retval 0  成功
	/// \retval <0  失败
	int SystemSetCurrentTime(SYSTEM_TIME * pTime);
	//int SystemGetTimeSecond(int *pTime);


	/// 得到系统的毫秒计数。两次调用之间的间隔时间就是得到的计数的差值。
	///
	/// \param 无
	/// \return 当前计数。
	/// \note 一般情况下，49.7天后得到计数会溢出，使用到的地方要做溢出后处理！
	unsigned int SystemGetMSCount(void);


	/// 得到系统的微秒计数, 此函数主要用于模块性能的测试。溢出的更快。
	///
	/// \param 无
	/// \return 当前计数。
	unsigned int SystemGetUSCount(void);


	/// 得到CPU占用率。
	///
	/// \param 无
	/// \return CPU占用百分比。
	int SystemGetCPUUsage(void);


	/// 得到系统特性。
	///
	/// \param [out] pCaps 指向系统特性结构SYSTEM_CAPS的指针。
	/// \retval 0  获取成功。
	/// \retval <0  获取失败。
	int SystemGetCaps(SYSTEM_CAPS * pCaps);



	///
	int SystemGetCapsEx(SYSTEM_CAPS_EX * pCaps);


	/// 挂起当前线程，时间到后再唤醒。
	///
	/// \param [in] dwMilliSeconds 挂起的时间。
	void SystemSleep(unsigned int dwMilliSeconds);


	///
	void SystemUSleep(unsigned int dwMicroSeconds);


	/// 让系统蜂鸣器发出单音调的声音，是同步模式，时间到才返回。
	///
	/// \param [in] dwFrequence 音调频率。
	/// \param [in] dwDuration 持续的时间。
	void SystemBeep(unsigned int dwFrequence, unsigned int dwDuration );


	/// 系统复位。
	void SystemReboot(void);


	/// 系统关闭。
	void SystemShutdown(void);

	enum EnUpgrade_VerifyMode
	{
	    EnUpgrade_VerifyMode_None = 0 ,  /* 不做校验 */
	    EnUpgrade_VerifyMode_Length ,  /* 只做文件长度校验 */
	    EnUpgrade_VerifyMode_CrcOnly ,  /* 只做Crc校验 */
	    EnUpgrade_VerifyMode_Full=50,	 /* 完全校验 */
	};

	/// 收到升级数据后，让系统解压。是同步调用模式，直到解压完成才返回。
	/// \param [in] pData 数据包在内存中的指针。
	/// \param [in] dwLength 数据长度。
	/// \param [in] pProgress 当前进度百分比，函数执行过程中定时更新，调用处使用
	/// \param [in] iVerifyMode 校验方式， 具体数值参考：EnUpgrade_VerifyMode
	/// \retval 0  成功
	/// \retval <0  失败
	/// \retval -1  --  data check error
	/// \retval -2  --  version error
	/// \retval -3  --  date error
	/// \retval -4  --  vendor error
	/// \retval -10  --  write flash error
	/// \retval -11  --  fail when update(other error)
	/// \note 不同的系统数据打包的格式不同，对应的完成数据打包和解压的的程序也不同。
	int SystemUpgrade(unsigned char* pData, unsigned int dwLength, unsigned int *pProgress, unsigned char iVerifyMode);

	//pData 数据格式
	typedef struct burning_fheader_t
	{
		unsigned char ver;		/*   */
		unsigned char pkt_type;	/* packet type */
		unsigned char parts;	/*  升级包包含几个部分，*/
		unsigned char Rev2;	/* */
		unsigned int crc;		/* 针对全部数据的crc校验码 */
		unsigned int head_len;	/* header length */
		unsigned int total_len;	/* packet length */
	}
	BURNING_FHEADER_T;
	// 分部分头
	typedef struct burning_part_header_t
	{
		char partname[32];		/*   */
		unsigned int offset_start;	/* 部分从缓存初始位置开始到本部分起始的偏移 */
		unsigned int offset_end;	/* 部分从缓存初始位置开始到本部分末尾的偏移 */
	}
	BURNING_PART_HEADER_T;

	/// 系统日志初始化。系统日志用来记录应用程序运行时所有的标准输出。
	///
	/// \retval 0  成功
	/// \retval <0  失败
	int SystemLogInit(void);


	/// 记录系统日志， 同时将内容写到到标准输出缓冲。
	///
	/// \retval 0  成功
	/// \retval <0  失败
	int SystemLogWrite(char * format, ...);


	#define SERIAL_NUMBER_STRING_LEN 32

	/// 获取系统序列号
	/// \param [out] pData 产品序列号字符串
	/// \param [in] len 缓存长度
	/// \retval 0  成功
	/// \retval <0  失败
	int SystemGetSerialNumber(char *pData, int len);

	typedef struct __nvr_product_info_t
	{
	    unsigned int ProductCode; // 产品代号
	    unsigned int VendorCode;  // 客户代号
	}NVR_PRODUCT_INFO_T, *pNVR_PRODUCT_INFO_T;

    #define PROD_D8004	21111 // 8004， 2.2版本主板
    #define PROD_D8004B	21112 // 8004， 3.0版本主板
    #define PROD_D8004_E	21113 // 8004-e， 2.2版本主板
    #define PROD_D8004B_E	21114 // 8004-e， 3.0版本主板

    #define PROD_D8104	21113
    #define PROD_D8008_H	21211
    #define PROD_D8008	21212
    #define PROD_D8016	21311
    #define PROD_D8404	22111

    #define PROD_D8408	22112
    #define PROD_D8416	22113

    #define PROD_D8808	23111
    #define PROD_D8816	23112
    #define PROD_D9004	26111
	/// 获取产品型号
	/// \param [out] pProductInfo 指向产品型号NVR_PRODUCT_INFO_T的指针。
	///
	/// \retval 0  成功
	/// \retval <0  失败
	int SystemGetProductInfo(NVR_PRODUCT_INFO_T *pProductInfo);

	/// 是否合法授权。
	///
	/// \retval 1  合法
	/// \retval 0  非法
	int SystemValidate(void);

	/// @} end of group

#ifdef __cplusplus
}
#endif

#endif //__SYSTEMM_API_H__

