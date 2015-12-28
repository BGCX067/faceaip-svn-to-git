/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * EZSignals.h - _explain_
 *
 * Copyright (C) 2011 joy.woo@hotmail.com, All Rights Reserved.
 *
 * $Id: EZSignals.h 5884 2012-05-16 09:09:51Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2012-05-16 09:09:51   Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifndef __EZTSIGNALS_H__
#define __EZTSIGNALS_H__

#if defined(WIN32)
	#pragma warning (disable : 4786)
#endif
#include "EZGuard.h"
#include <deque>

//TSignal1
#define SIGNAL_NUMBER 1
#define SIGNAL_CLASS_TYPES class T1
#define SIGNAL_TYPES T1
#define SIGNAL_TYPE_ARGS T1 a1 
#define SIGNAL_ARGS a1 
#include "EZSignal.h"
#undef  SIGNAL_NUMBER
#undef  SIGNAL_CLASS_TYPES
#undef	SIGNAL_TYPES
#undef	SIGNAL_TYPE_ARGS
#undef	SIGNAL_ARGS

//TSignal2
#define SIGNAL_NUMBER 2
#define SIGNAL_CLASS_TYPES class T1, class T2
#define SIGNAL_TYPES T1, T2
#define SIGNAL_TYPE_ARGS T1 a1, T2 a2
#define SIGNAL_ARGS a1, a2
#include "EZSignal.h"
#undef  SIGNAL_NUMBER
#undef  SIGNAL_CLASS_TYPES
#undef	SIGNAL_TYPES
#undef	SIGNAL_TYPE_ARGS
#undef	SIGNAL_ARGS

//TSignal3
#define SIGNAL_NUMBER 3
#define SIGNAL_CLASS_TYPES class T1, class T2, class T3
#define SIGNAL_TYPES T1, T2, T3
#define SIGNAL_TYPE_ARGS T1 a1, T2 a2, T3 a3
#define SIGNAL_ARGS a1, a2, a3
#include "EZSignal.h"
#undef  SIGNAL_NUMBER
#undef  SIGNAL_CLASS_TYPES
#undef	SIGNAL_TYPES
#undef	SIGNAL_TYPE_ARGS
#undef	SIGNAL_ARGS

//TSignal4
#define SIGNAL_NUMBER 4
#define SIGNAL_CLASS_TYPES class T1, class T2, class T3, class T4
#define SIGNAL_TYPES T1, T2, T3, T4
#define SIGNAL_TYPE_ARGS T1 a1, T2 a2, T3 a3, T4 a4
#define SIGNAL_ARGS a1, a2, a3, a4
#include "EZSignal.h"
#undef  SIGNAL_NUMBER
#undef  SIGNAL_CLASS_TYPES
#undef	SIGNAL_TYPES
#undef	SIGNAL_TYPE_ARGS
#undef	SIGNAL_ARGS

//TSignal5
#define SIGNAL_NUMBER 5
#define SIGNAL_CLASS_TYPES class T1, class T2, class T3, class T4, class T5
#define SIGNAL_TYPES T1, T2, T3, T4, T5
#define SIGNAL_TYPE_ARGS T1 a1, T2 a2, T3 a3, T4 a4, T5 a5 
#define SIGNAL_ARGS a1, a2, a3, a4, a5
#include "EZSignal.h"
#undef  SIGNAL_NUMBER
#undef  SIGNAL_CLASS_TYPES
#undef	SIGNAL_TYPES
#undef	SIGNAL_TYPE_ARGS
#undef	SIGNAL_ARGS

//TSignal6
#define SIGNAL_NUMBER 6
#define SIGNAL_CLASS_TYPES class T1, class T2, class T3, class T4, class T5, class T6
#define SIGNAL_TYPES T1, T2, T3, T4, T5, T6
#define SIGNAL_TYPE_ARGS T1 a1, T2 a2, T3 a3, T4 a4, T5 a5, T6 a6
#define SIGNAL_ARGS a1, a2, a3, a4, a5, a6
#include "EZSignal.h"
#undef  SIGNAL_NUMBER
#undef  SIGNAL_CLASS_TYPES
#undef	SIGNAL_TYPES
#undef	SIGNAL_TYPE_ARGS
#undef	SIGNAL_ARGS

#endif //__TSIGNALS_H__
