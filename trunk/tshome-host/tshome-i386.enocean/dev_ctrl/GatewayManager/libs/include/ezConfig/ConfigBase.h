#ifndef __CONFIG_BASE__
#define __CONFIG_BASE__

#include <string.h>
#include "EZThread.h"
#include "EZTimer.h"
#include "EZSignals.h"
#include <json.h>

typedef struct tagENUM_MAP
{
	const char* name;
	int value;
}
ENUM_MAP;

class CConfigExchange;

class CConfigKey
{
	friend class CConfigExchange;
public:
	CConfigKey(int index)
	{
		m_kind = kindIndex;
		m_value.index = index;
	};

	CConfigKey(const char* name)
	{
		m_kind = kindName;
		m_value.name = name;
	};

private:
	enum Kind
	{
		kindNone = 0,
		kindIndex,
		kindName
	}m_kind;

	union
	{
		int index;
		const char *name;
	}m_value;
};

typedef Json::Value CConfigTable;
#ifdef WIN32
typedef Json::StyledWriter CConfigWriter;
#else
typedef Json::FastWriter CConfigWriter;
#endif
typedef Json::Reader CConfigReader;

typedef TSignal2<CConfigExchange &, int &> TCONFIG_SIGNAL;
typedef TCONFIG_SIGNAL::SigProc TCONFIG_PROC;

//配置应用结果，按位表示，由配置的观察者设置，配置的提交者检查
enum CONFIG_APPLY_RET
{
	CONFIG_APPLY_OK = 0,					// 成功
	CONFIG_APPLY_RESTART = 0x00000001,		// 需要重启应用程序
	CONFIG_APPLY_REBOOT = 0x00000002,		// 需要重启系统
	CONFIG_APPLY_FILE_ERROR = 0x00000004,	// 写文件出错
	CONFIG_APPLY_CAPS_ERROR = 0x00000008,	// 特性不支持
	CONFIG_APPLY_GATEWAY_ERROR = 0x00000010,//网关设置失败,仍然归类为成功
	CONFIG_APPLY_VALIT_ERROR = 0x00000020,	/*!< 验证失败 */
	CONFIG_APPLY_NOT_EXSIST = 0x00000040,	/*!< 配置不存在 */
	CONFIG_APPLY_NOT_SAVE = 0x00000080,		// 不要保存到文件
	CONFIG_APPLY_DELAY_SAVE = 0x00000100,	//延时保存..把位占掉取值为0x180的原因是在配置保存的时候需要互斥

	CONFIG_APPLY_USER_DEFINE = 0x0FF00000,	//用户自定义—— 在处理函数中发现ret传入了这个值，则执行自定义处理
};

class CConfigExchange
{
public:
	typedef enum tagCONFIG_STATE
	{
		CS_SAVING = 0,
		CS_LOADING,
		CS_VALIDATING,
		CS_DEFAUTING,
	} CONFIG_STATE;

public:
	CConfigExchange(int n, int& used, int id, TCONFIG_SIGNAL& sig);

	//状态与有效性
	void setValidity(int bValid);
	int getValidity();
	CONFIG_STATE getState() const;
	void setState(CONFIG_STATE state);

	//整型数据交换
	void exchange(CConfigTable& table, CConfigKey key, int& value, int scope, int dflt, ENUM_MAP* map);
	void exchange(CConfigTable& table, CConfigKey key, int& value, int min, int max, int dflt, ENUM_MAP* map);
	void exchange(CConfigTable& table, CConfigKey key, int& value, int min, int max, int dflt);

	//字符串数据交换
	void exchange(CConfigTable& table, CConfigKey key, std::string& value, const std::string& scope, const std::string& dflt);
	void exchange(CConfigTable& table, CConfigKey key, std::string& value, const std::string& dflt);
	void exchange(CConfigTable& table, CConfigKey key, char* value, const char* scope, const char* dflt);
	void exchange(CConfigTable& table, CConfigKey key, char* value, const char* dflt);

	//布尔型数据交换
	void exchange(CConfigTable& table, CConfigKey key, int& value, int dflt);

	//16进制数据交换
	void exchange(CConfigTable& table, CConfigKey key, unsigned int& value, unsigned int dflt);

public:
	//取默认值
	void recall(int index = -1);

	//检查合法性
	int validate(int index = -1);

	//更新到最新配置
	void update(int index = -1);

	//提交配置，含合法性检查。返回值为配置应用的结果，为CONFIG_APPLY_RET按位组合。
	int commit(const char* user = NULL, int index = -1, int retInitial = CONFIG_APPLY_DELAY_SAVE);

	// 设置配置的字符串表
	void setTable(CConfigTable& table);

	// 获取配置的字符串表
	void getTable(CConfigTable& table);

	// 注册观察函数
	int attach(CEZObject * pObj, TCONFIG_PROC pProc);

	// 销毁观察函数
	int detach(CEZObject * pObj, TCONFIG_PROC pProc);

	// 得到正在调用commit的用户名
	const char* getUser();

	// 得到配置制标识符
	int getID();

	// 设置配置结构数组中已经使用的项数
	void setUsed(int used);

	// 得到配置结构数组中已经使用的项数
	int getUsed();

	// 得到配置结构数组长度
	int getNumber();

private:// 下面的函数由类模板实现
	virtual void exchangeTable(CConfigTable& table, int index) = 0;
	virtual void copy(int index, int save) = 0;
	virtual int equal(int index) = 0;

private:
	const char* getValue(CConfigTable& table, CConfigKey key, const char* dflt);
	int getValue(CConfigTable& table, CConfigKey key, int dflt);
	CConfigTable& getValue(CConfigTable& table, CConfigKey key);

private:
	static	CEZMutex m_mutex;
	static const char* m_strUser;
	CONFIG_STATE m_iState;
	int m_bValid;
	int m_eleNumber;
	int m_eleUsed;			/*! 已经使用的元素个数，初始化时等于数组的长度，配置读取时根据配置字符串文件中的实际
							结构数量来设置这个值，读取默认时可以显示的调用setUsed来设置默认项的个数。其他地方使
							用配置也可以调用getUsed和setUsed接口。比如要得到配置中用户的个数，直接使用getUsed
							接口即可，保存用户配置时可以先调用setUsed操作把实际的用户数设置下去，再commit。*/
	int& m_eleUsedLatest;	//最新配置已经使用的元素个数
	int m_id;
	TCONFIG_SIGNAL& m_sigRef;
};

//T 配置结构类型
template <class T> void exchangeTable(CConfigExchange& xchg, CConfigTable& table, T& config, int index, int app);

//T 配置结构类型
//number 结构数目
//observers 观察者最大数目
//appInstance 应用实例，如果前面3个参数一样，要通过这个参数来区分不同的应用，否则使用相同结构的类会相互干扰
//As 配置比较时的类型，如果是void, 表示使用内存比较函数来比较
template <class T, int number, int observers, int appInstance = 0, class As = void> class TConfig : public CConfigExchange
{
public:
	TConfig():CConfigExchange(number, m_used, (int)m_latest, m_sigConfig)
	{
	}
	virtual ~TConfig()
	{
	}

	// 得到最新配置的引用
	inline static const T& getLatest(int index = 0)
	{
		return m_latest[index];
	}

	// 得到当前配置的引用
	inline T& getConfig(int index = 0)
	{
		return m_config[index];
	}

	// 得到当前配置的引用，通过下标运算符
	inline T& operator[](int index)
	{
		return m_config[index];
	}

	// 静态注册观察函数
	static inline int attach(CEZObject * pObj, TCONFIG_PROC pProc)
	{
		return m_sigConfig.Attach(pObj, pProc);
	}

	// 静态注销观察函数
	static inline int detach(CEZObject * pObj, TCONFIG_PROC pProc)
	{
		return m_sigConfig.Detach(pObj, pProc);
	}

	// 配置更新接口, 各配置实现
	void exchangeTable(CConfigTable& table, int index)
	{
		::exchangeTable(*(CConfigExchange*)this, table, getConfig(index), index, appInstance);
	}

	void copy(int index, int save)
	{
		if(save)
		{
			m_latest[index] = m_config[index];
		}
		else
		{
			m_config[index] = m_latest[index];
		}
	}

	inline int equal(const T* x, const T* y)
	{
		return (*x == *y);
	}

	inline int equal(const void * x, const void * y)
	{
		return (memcmp(x, y, sizeof(T)) == 0);
	}

	int equal(int index)
	{
		return equal((As*)&m_latest[index], (As*)&m_config[index]);
	}

private:
	static TCONFIG_SIGNAL m_sigConfig;
	static T m_latest[number];
	T m_config[number];
	static int m_used;
};

template<class T, int number, int observers, int appInstance, class As>
T TConfig<T, number, observers, appInstance, As>::m_latest[];

template<class T, int number, int observers, int appInstance, class As>
TCONFIG_SIGNAL TConfig<T, number, observers, appInstance, As>::m_sigConfig(observers);

template<class T, int number, int observers, int appInstance, class As>
int TConfig<T, number, observers, appInstance, As>::m_used = number;

#endif //__CONFIG_BASE__
