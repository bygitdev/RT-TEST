#ifndef _TPBASE_H_
#define _TPBASE_H_

#include "Interface.h"
#include "Que.h"

#ifdef __cplusplus
extern "C"
{
#endif

	class CTpBasePimpl;
	class DLL_TYPE CTpBase : public CTpBaseInterface
	{
	public:
		CTpBase();
		virtual ~CTpBase(){};

	public:	/// tp base
		virtual BOOL	__stdcall Init(_char* cMyMap);
		virtual VOID	__stdcall Close(void);

		virtual VOID	__stdcall logProcess(_char* deviceId, _char* eventId, _char* status, _char* materialId, _char* lotId, _char* recipeId, _char* key1 = NULL, _char* value1 = NULL, _char* key2 = NULL, _char* value2 = NULL, _char* key3 = NULL, _char* value3 = NULL, _char* key4 = NULL, _char* value4 = NULL, _char* key5 = NULL, _char* value5 = NULL, _char* key6 = NULL, _char* value6 = NULL);
		virtual VOID	__stdcall logTransfer(_char* deviceId, _char* eventId, _char* status, _char* materialId, _char* materialType, _char* fromDevice, _char* toDevice, _char* key1 = NULL, _char* value1 = NULL, _char* key2 = NULL, _char* value2 = NULL, _char* key3 = NULL, _char* value3 = NULL, _char* key4 = NULL, _char* value4 = NULL, _char* key5 = NULL, _char* value5 = NULL, _char* key6 = NULL, _char* value6 = NULL);
		virtual VOID	__stdcall logFunction(_char* deviceId, _char* eventId, _char* status, _char* materialId, _char* materialType, _char* key1 = NULL, _char* value1 = NULL, _char* key2 = NULL, _char* value2 = NULL, _char* key3 = NULL, _char* value3 = NULL, _char* key4 = NULL, _char* value4 = NULL, _char* key5 = NULL, _char* value5 = NULL, _char* key6 = NULL, _char* value6 = NULL);
		virtual VOID	__stdcall logLot(_char* deviceId, _char* eventId, _char* lotId, _char* recipeId, _char* carrierId, _char* key1 = NULL, _char* value1 = NULL, _char* key2 = NULL, _char* value2 = NULL, _char* key3 = NULL, _char* value3 = NULL, _char* key4 = NULL, _char* value4 = NULL, _char* key5 = NULL, _char* value5 = NULL, _char* key6 = NULL, _char* value6 = NULL);
		virtual VOID	__stdcall logConfigure(_char* deviceId, _char* cfgId, _char* key1 = NULL, _char* value1 = NULL, _char* key2 = NULL, _char* value2 = NULL, _char* key3 = NULL, _char* value3 = NULL, _char* key4 = NULL, _char* value4 = NULL, _char* key5 = NULL, _char* value5 = NULL, _char* key6 = NULL, _char* value6 = NULL);
		virtual VOID	__stdcall logAlarm(_char* deviceId, _char* eventId, _char* alarmCode, _char* status, _char* key1 = NULL, _char* value1 = NULL, _char* key2 = NULL, _char* value2 = NULL, _char* key3 = NULL, _char* value3 = NULL, _char* key4 = NULL, _char* value4 = NULL, _char* key5 = NULL, _char* value5 = NULL, _char* key6 = NULL, _char* value6 = NULL);

	private:
		CTpBasePimpl* _pPimpl;
	};

	DLL_TYPE CTpBase*	__stdcall	CreateTpBase(void);
	DLL_TYPE void		__stdcall	DistoryTpBase(CTpBase* pBase);

#ifdef __cplusplus
}	/// extern "c"
#endif

#endif	/// _TPBASE_H_
