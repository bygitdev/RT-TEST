#ifndef _INTERFACE_H_
#define _INTERFACE_H_

#include "Thread.h"

#ifdef __cplusplus
extern "C"
{
#endif

	class DLL_TYPE CPiperInterface
	{
	public:
		virtual BOOL	__stdcall	Init(BOOL RorW, _char* cMyName, _char* cMyMap) = 0;
		virtual void	__stdcall	Close(void) = 0;
	};

	class DLL_TYPE CTpBaseInterface
	{
	public:	/// tp base
		virtual BOOL	__stdcall Init(_char* cMyMap) = 0;
		virtual VOID	__stdcall Close(void) = 0;

		virtual VOID	__stdcall logProcess(_char* deviceId, _char* eventId, _char* status, _char* materialId, _char* lotId, _char* recipeId, _char* key1 = NULL, _char* value1 = NULL, _char* key2 = NULL, _char* value2 = NULL, _char* key3 = NULL, _char* value3 = NULL, _char* key4 = NULL, _char* value4 = NULL, _char* key5 = NULL, _char* value5 = NULL, _char* key6 = NULL, _char* value6 = NULL) = 0;
		virtual VOID	__stdcall logTransfer(_char* deviceId, _char* eventId, _char* status, _char* materialId, _char* materialType, _char* fromDevice, _char* toDevice, _char* key1 = NULL, _char* value1 = NULL, _char* key2 = NULL, _char* value2 = NULL, _char* key3 = NULL, _char* value3 = NULL, _char* key4 = NULL, _char* value4 = NULL, _char* key5 = NULL, _char* value5 = NULL, _char* key6 = NULL, _char* value6 = NULL) = 0;
		virtual VOID	__stdcall logFunction(_char* deviceId, _char* eventId, _char* status, _char* materialId, _char* materialType, _char* key1 = NULL, _char* value1 = NULL, _char* key2 = NULL, _char* value2 = NULL, _char* key3 = NULL, _char* value3 = NULL, _char* key4 = NULL, _char* value4 = NULL, _char* key5 = NULL, _char* value5 = NULL, _char* key6 = NULL, _char* value6 = NULL) = 0;
		virtual VOID	__stdcall logLot(_char* deviceId, _char* eventId, _char* lotId, _char* recipeId, _char* carrierId, _char* key1 = NULL, _char* value1 = NULL, _char* key2 = NULL, _char* value2 = NULL, _char* key3 = NULL, _char* value3 = NULL, _char* key4 = NULL, _char* value4 = NULL, _char* key5 = NULL, _char* value5 = NULL, _char* key6 = NULL, _char* value6 = NULL) = 0;
		virtual VOID	__stdcall logConfigure(_char* deviceId, _char* cfgId, _char* key1 = NULL, _char* value1 = NULL, _char* key2 = NULL, _char* value2 = NULL, _char* key3 = NULL, _char* value3 = NULL, _char* key4 = NULL, _char* value4 = NULL, _char* key5 = NULL, _char* value5 = NULL, _char* key6 = NULL, _char* value6 = NULL) = 0;
		virtual VOID	__stdcall logAlarm(_char* deviceId, _char* eventId, _char* alarmCode, _char* status, _char* key1 = NULL, _char* value1 = NULL, _char* key2 = NULL, _char* value2 = NULL, _char* key3 = NULL, _char* value3 = NULL, _char* key4 = NULL, _char* value4 = NULL, _char* key5 = NULL, _char* value5 = NULL, _char* key6 = NULL, _char* value6 = NULL) = 0;
	};

#ifdef __cplusplus
}	/// extern "c"
#endif

#endif	/// _INTERFACE_H_
