/* Automatically generated nanopb constant definitions */
/* Generated by nanopb-0.3.9.1 at Fri Nov 01 12:07:24 2019. */

#include "faccfg.pb.h"

/* @@protoc_insertion_point(includes) */
#if PB_PROTO_HEADER_VERSION != 30
#error Regenerate this file with the current version of nanopb generator.
#endif



const pb_field_t fac_Alarm_fields[4] = {
    PB_FIELD(  1, UENUM   , REQUIRED, STATIC  , FIRST, fac_Alarm, type, type, 0),
    PB_FIELD(  2, FLOAT   , OPTIONAL, STATIC  , OTHER, fac_Alarm, alarmHigh, type, 0),
    PB_FIELD(  3, FLOAT   , OPTIONAL, STATIC  , OTHER, fac_Alarm, alarmLow, alarmHigh, 0),
    PB_LAST_FIELD
};

const pb_field_t fac_Slot_fields[5] = {
    PB_FIELD(  1, UINT32  , OPTIONAL, STATIC  , FIRST, fac_Slot, index, index, 0),
    PB_FIELD(  2, UENUM   , OPTIONAL, STATIC  , OTHER, fac_Slot, type, index, 0),
    PB_FIELD(  3, BOOL    , OPTIONAL, STATIC  , OTHER, fac_Slot, active, type, 0),
    PB_FIELD(  4, BYTES   , OPTIONAL, STATIC  , OTHER, fac_Slot, frame, active, 0),
    PB_LAST_FIELD
};

const pb_field_t fac_Config_fields[29] = {
    PB_FIELD(100, UENUM   , REQUIRED, STATIC  , FIRST, fac_Config, appDeviceType, appDeviceType, 0),
    PB_FIELD(101, BYTES   , REQUIRED, STATIC  , OTHER, fac_Config, appSn, appDeviceType, 0),
    PB_FIELD(102, BYTES   , REQUIRED, STATIC  , OTHER, fac_Config, appToken, appSn, 0),
    PB_FIELD(103, BYTES   , REQUIRED, STATIC  , OTHER, fac_Config, appHardwareversion, appToken, 0),
    PB_FIELD(104, BYTES   , REQUIRED, STATIC  , OTHER, fac_Config, appSecureKey, appHardwareversion, 0),
    PB_FIELD(105, BYTES   , REQUIRED, STATIC  , OTHER, fac_Config, appPassword, appSecureKey, 0),
    PB_FIELD(106, UINT32  , REQUIRED, STATIC  , OTHER, fac_Config, appInterval, appPassword, 0),
    PB_FIELD(107, UINT32  , OPTIONAL, STATIC  , OTHER, fac_Config, appBleOnTime, appInterval, 0),
    PB_FIELD(108, UINT32  , OPTIONAL, STATIC  , OTHER, fac_Config, appBleOffTime, appBleOnTime, 0),
    PB_FIELD(109, SINT32  , REQUIRED, STATIC  , OTHER, fac_Config, appBleTxPower, appBleOffTime, 0),
    PB_FIELD(110, FLOAT   , REQUIRED, STATIC  , OTHER, fac_Config, appBleInterval, appBleTxPower, 0),
    PB_FIELD(111, MESSAGE , REPEATED, STATIC  , OTHER, fac_Config, appSlots, appBleInterval, &fac_Slot_fields),
    PB_FIELD(112, MESSAGE , REPEATED, STATIC  , OTHER, fac_Config, appAlarms, appSlots, &fac_Alarm_fields),
    PB_FIELD(114, UINT32  , OPTIONAL, STATIC  , OTHER, fac_Config, appAlarmReportIntervalAcked, appAlarms, 0),
    PB_FIELD(115, UINT32  , OPTIONAL, STATIC  , OTHER, fac_Config, appAlarmReportIntervalUnacked, appAlarmReportIntervalAcked, 0),
    PB_FIELD(204, BYTES   , REQUIRED, STATIC  , OTHER, fac_Config, appUUID, appAlarmReportIntervalUnacked, 0),
    PB_FIELD(207, SINT32  , REQUIRED, STATIC  , OTHER, fac_Config, appRSSI, appUUID, 0),
    PB_FIELD(208, UINT32  , OPTIONAL, STATIC  , OTHER, fac_Config, appLowBatteryBeep, appRSSI, 0),
    PB_FIELD(209, UINT32  , OPTIONAL, STATIC  , OTHER, fac_Config, appLedStatus, appLowBatteryBeep, 0),
    PB_FIELD(210, UINT32  , OPTIONAL, STATIC  , OTHER, fac_Config, appAlarmShieldSwitch, appLedStatus, 0),
    PB_FIELD(211, UINT32  , OPTIONAL, STATIC  , OTHER, fac_Config, appAlarmShieldTime, appAlarmShieldSwitch, 0),
    PB_FIELD(212, UINT32  , OPTIONAL, STATIC  , OTHER, fac_Config, appInsulateSwitch, appAlarmShieldTime, 0),
    PB_FIELD(213, UINT32  , OPTIONAL, STATIC  , OTHER, fac_Config, appDemoMode, appInsulateSwitch, 0),
    PB_FIELD(214, UINT32  , OPTIONAL, STATIC  , OTHER, fac_Config, appCdsSwitch, appDemoMode, 0),
    PB_FIELD(215, UINT32  , OPTIONAL, STATIC  , OTHER, fac_Config, appWarningSwtich, appCdsSwitch, 0),
    PB_FIELD(216, UINT32  , OPTIONAL, STATIC  , OTHER, fac_Config, appHumanDetectionSwitch, appWarningSwtich, 0),
    PB_FIELD(217, UINT32  , OPTIONAL, STATIC  , OTHER, fac_Config, appHumanDetectionSync, appHumanDetectionSwitch, 0),
    PB_FIELD(218, UINT32  , OPTIONAL, STATIC  , OTHER, fac_Config, appNightLight, appHumanDetectionSync, 0),
    PB_LAST_FIELD
};




/* Check that field information fits in pb_field_t */
#if !defined(PB_FIELD_32BIT)
/* If you get an error here, it means that you need to define PB_FIELD_32BIT
 * compile-time option. You can do that in pb.h or on compiler command line.
 * 
 * The reason you need to do this is that some of your messages contain tag
 * numbers or field sizes that are larger than what can fit in 8 or 16 bit
 * field descriptors.
 */
PB_STATIC_ASSERT((pb_membersize(fac_Config, appSlots[0]) < 65536 && pb_membersize(fac_Config, appAlarms[0]) < 65536), YOU_MUST_DEFINE_PB_FIELD_32BIT_FOR_MESSAGES_fac_Alarm_fac_Slot_fac_Config)
#endif

#if !defined(PB_FIELD_16BIT) && !defined(PB_FIELD_32BIT)
/* If you get an error here, it means that you need to define PB_FIELD_16BIT
 * compile-time option. You can do that in pb.h or on compiler command line.
 * 
 * The reason you need to do this is that some of your messages contain tag
 * numbers or field sizes that are larger than what can fit in the default
 * 8 bit descriptors.
 */
PB_STATIC_ASSERT((pb_membersize(fac_Config, appSlots[0]) < 256 && pb_membersize(fac_Config, appAlarms[0]) < 256), YOU_MUST_DEFINE_PB_FIELD_16BIT_FOR_MESSAGES_fac_Alarm_fac_Slot_fac_Config)
#endif


/* @@protoc_insertion_point(eof) */
