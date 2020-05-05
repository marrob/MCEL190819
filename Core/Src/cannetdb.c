/* Includes ------------------------------------------------------------------*/
#include "cannetdb.h"
#include "main.h"

VcellDev_Type VcellDevTemp;

static void SIG_MCEL_V_MEAS_Changed (uint8_t node);
static void SIG_MCEL_C_MEAS_Changed (uint8_t node);
static void SIG_MCEL_C_RANGE_Changed (uint8_t node);
static void SIG_MCEL_C_OFS_Changed (uint8_t node);
static void SIG_MCEL_TR_TEMP_Changed (uint8_t node);
static void SIG_MCEL_UC_TEMP_Changed (uint8_t node);
static void SIG_MCEL_CC_STATUS_Changed (uint8_t node);
static void SIG_MCEL_CV_STATUS_Changed (uint8_t node);
static void SIG_MCEL_OE_STATUS_Changed (uint8_t node);
static void SIG_MCEL_UPTIME_Changed (uint8_t node);
static void SIG_MCEL_VERSION_Changed (uint8_t node);

CanNetMessageItem_Type Messages[] = 
{
  /*Name                   Id,               NodeTypeI,     IsPeriod, PeriodTime,  pInComeNotify,               pValue,                      Type                  */
  /*01234567890123456                                                                                                                                              */
  { "MSG_MCEL_V_MEAS",     MSG_MCEL_V_MEAS,   NODE_MCEL,    0,        0,           &SIG_MCEL_V_MEAS_Changed,    &VcellDevTemp.CVmeas,           CANNET_FLOAT        },
  { "MSG_MCEL_C_MEAS",     MSG_MCEL_C_MEAS,   NODE_MCEL,    0,        0,           &SIG_MCEL_C_MEAS_Changed,    &VcellDevTemp.CCmeas,           CANNET_FLOAT        },
  { "MSG_MCEL_RANGE",      MSG_MCEL_RANGE,    NODE_MCEL,    0,        0,           &SIG_MCEL_C_RANGE_Changed,   &VcellDevTemp.Range,            CANNET_U8           },
  { "MSG_MCEL_OFFSET",     MSG_MCEL_OFFSET,   NODE_MCEL,    0,        0,           &SIG_MCEL_C_OFS_Changed,     &VcellDevTemp.CCoffset,         CANNET_FLOAT        },
  { "MSG_MCEL_TEMP_TR",    MSG_MCEL_TEMP_TR,  NODE_MCEL,    0,        0,           &SIG_MCEL_TR_TEMP_Changed,   &VcellDevTemp.TransistorTemp,   CANNET_FLOAT        },
  { "MSG_MCEL_TEMP_UC",    MSG_MCEL_TEMP_UC,  NODE_MCEL,    0,        0,           &SIG_MCEL_UC_TEMP_Changed,   &VcellDevTemp.ControllerTemp,   CANNET_FLOAT,       },
  { "MSG_MCEL_STA_CC",     MSG_MCEL_STA_CC,   NODE_MCEL,    0,        0,           &SIG_MCEL_CC_STATUS_Changed, &VcellDevTemp.CC,               CANNET_BOOL,        },
  { "MSG_MCEL_STA_CV",     MSG_MCEL_STA_CV,   NODE_MCEL,    0,        0,           &SIG_MCEL_CV_STATUS_Changed, &VcellDevTemp.CV,               CANNET_BOOL,        },
  { "MSG_MCEL_STA_OE",     MSG_MCEL_STA_OE,   NODE_MCEL,    0,        0,           &SIG_MCEL_OE_STATUS_Changed, &VcellDevTemp.OE,               CANNET_BOOL,        },
  { "MSG_MCEL_VER",        MSG_MCEL_VER,      NODE_MCEL,    0,        0,           &SIG_MCEL_VERSION_Changed,   &VcellDevTemp.Version,          CANNET_U64,         },
  { "MSG_MCEL_LIVE",       MSG_MCEL_LIVE,     NODE_MCEL,    0,        0,           &SIG_MCEL_UPTIME_Changed,    &VcellDevTemp.UpTimeSec,        CANNET_U32,         },

  { "MSG_PC_CV",           MSG_PC_CV,         NODE_PC,      0,        0,           NULL,                        NULL,                           CANNET_FLOAT        },
  { "MSG_PC_CC",           MSG_PC_CC,         NODE_PC,      0,        0,           NULL,                        NULL,                           CANNET_FLOAT        },
  { "MSG_PC_RANGE",        MSG_PC_RANGE,      NODE_PC,      0,        0,           NULL,                        NULL,                           CANNET_U8,          },
  { "MSG_PC_OE",           MSG_PC_OE,         NODE_PC,      0,        0,           NULL,                        NULL,                           CANNET_U8,          },
  { "MSG_PC_RSENSE",       MSG_PC_RSENSE,     NODE_PC,      0,        0,           NULL,                        NULL,                           CANNET_BOOL,        },
  { "MSG_PC_GD",           MSG_PC_GD,         NODE_PC,      0,        0,           NULL,                        NULL,                           CANNET_BOOL,        },
  { "MSG_PC_TRIG",         MSG_PC_TRIG,       NODE_PC,      0,        0,           NULL,                        NULL,                           CANNET_U8,          },
  { "MSG_PC_UP_SPEED",     MSG_PC_UP_SPEED,   NODE_PC,      0,        0,           NULL,                        NULL,                           CANNET_FLOAT,       },
  { "MSG_PC_DOWN_SPEED",   MSG_PC_DOWN_SPEED, NODE_PC,      0,        0,           NULL,                        NULL,                           CANNET_FLOAT,       },
  { "MSG_PC_SERVICES",     MSG_PC_SERVICES,   NODE_PC,      0,        0,           NULL,                        NULL,                           CANNET_U32,         },
  { "MSG_PC_CLB_SET01",    MSG_PC_CLB_SET01,  NODE_PC,      0,        0,           NULL,                        NULL,                           CANNET_FLOAT,       },
  { "MSG_PC_CLB_SET02",    MSG_PC_CLB_SET02,  NODE_PC,      0,        0,           NULL,                        NULL,                           CANNET_FLOAT,       },
  { "MSG_PC_CLB_SET03",    MSG_PC_CLB_SET03,  NODE_PC,      0,        0,           NULL,                        NULL,                           CANNET_FLOAT,       },
  { "MSG_PC_CLB_SET04",    MSG_PC_CLB_SET04,  NODE_PC,      0,        0,           NULL,                        NULL,                           CANNET_FLOAT,       },
  { "MSG_PC_CLB_SET05",    MSG_PC_CLB_SET05,  NODE_PC,      0,        0,           NULL,                        NULL,                           CANNET_FLOAT,       },
  { "MSG_PC_CLB_SET06",    MSG_PC_CLB_SET06,  NODE_PC,      0,        0,           NULL,                        NULL,                           CANNET_FLOAT,       },
  { "MSG_PC_CLB_SET07",    MSG_PC_CLB_SET07,  NODE_PC,      0,        0,           NULL,                        NULL,                           CANNET_FLOAT,       },
  { "MSG_PC_CLB_SET08",    MSG_PC_CLB_SET08,  NODE_PC,      0,        0,           NULL,                        NULL,                           CANNET_FLOAT,       },
  { "MSG_PC_CLB_SET09",    MSG_PC_CLB_SET09,  NODE_PC,      0,        0,           NULL,                        NULL,                           CANNET_FLOAT,       },
  { "MSG_PC_CLB_SET10",    MSG_PC_CLB_SET10,  NODE_PC,      0,        0,           NULL,                        NULL,                           CANNET_FLOAT,       },
  { "MSG_PC_CLB_SET11",    MSG_PC_CLB_SET11,  NODE_PC,      0,        0,           NULL,                        NULL,                           CANNET_FLOAT,       },
  { "MSG_PC_CLB_SET12",    MSG_PC_CLB_SET12,  NODE_PC,      0,        0,           NULL,                        NULL,                           CANNET_FLOAT,       },

};

uint8_t CanNetMessagesCount(void)
{
  return sizeof(Messages)/sizeof(CanNetMessageItem_Type);
}

static void SIG_MCEL_V_MEAS_Changed (uint8_t node) 
{
  Device.Vcells[node].CVmeas = VcellDevTemp.CVmeas; 
}

static void SIG_MCEL_C_MEAS_Changed (uint8_t node) 
{
  Device.Vcells[node].CCmeas = VcellDevTemp.CCmeas; 
}

static void SIG_MCEL_C_RANGE_Changed (uint8_t node)
{
  Device.Vcells[node].Range = VcellDevTemp.CCmeas; 
}

static void SIG_MCEL_OE_STATUS_Changed (uint8_t node)
{
  Device.Vcells[node].OE = VcellDevTemp.OE; 
}

static void SIG_MCEL_CV_STATUS_Changed (uint8_t node)
{
  Device.Vcells[node].CV = VcellDevTemp.CV; 
}

static void SIG_MCEL_CC_STATUS_Changed (uint8_t node)
{
  Device.Vcells[node].CC = VcellDevTemp.CC; 
}

static void SIG_MCEL_UC_TEMP_Changed (uint8_t node)
{
  Device.Vcells[node].ControllerTemp = VcellDevTemp.ControllerTemp; 
}

static void SIG_MCEL_TR_TEMP_Changed (uint8_t node)
{
  Device.Vcells[node].TransistorTemp = VcellDevTemp.TransistorTemp; 
}

static void SIG_MCEL_C_OFS_Changed (uint8_t node)
{
  Device.Vcells[node].CCoffset = VcellDevTemp.CCoffset;
}

static void SIG_MCEL_UPTIME_Changed (uint8_t node)
{
  Device.Vcells[node].UpTimeSec = VcellDevTemp.UpTimeSec;
}

static void SIG_MCEL_VERSION_Changed (uint8_t node)
{
  Device.Vcells[node].Version = VcellDevTemp.Version;
}




/******************* (C) COPYRIGHT Konvolucio Design *****END OF FILE******/
