/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include "usbd_cdc_if.h"

#define CMDLINE_PARAMETER_COUNT_ERROR      "!PARAMTER COUNT ERROR, REQUIED %d, CURRENT IS %d."
#define CMDLINE_UNKNOWN_COMMAND_ERROR      "!UNKNOWN COMMAND ERROR, COMMAND WAS '%s'."
#define CMDLINE_UNKNOWN_PARAMETER_ERROR    "!UNKNOWN PARAMETER ERROR '%s'"
#define CMDLINE_PARAM_TOO_HIGH_ERROR       "!PARAMETER TOO HIGH ERROR, MAX IS %d."
#define CMDLINE_PARAM_TOO_LOW_ERROR        "!PARAMETER TOO LOW ERROR, MIN IS %d."
#define CMDLINE_SIGNAL_NOT_FOUND           "!CMDLINE_SIGNAL_NOT_FOUND"
#define CMDLINE_CANNOT_BROADCAST_ERROR     "!THIS QUERY CANNOT BE A BROADCAST, COMMAND WAS '%s'."
#define CMDLINE_COMPLETE                   "COMPLETE"
#define CMDLINE_RESPONSE_OK                "OK"

#define IsBroadcast(__node__)               (strcmp("XX", (__node__) ) == 0)
#define GetNodeAddres(__node__)            (strtol(__node__, NULL, 16))

/* Cmd Line -------------------------------------------------------------------*/
char Temp [APP_TX_DATA_SIZE];
void USBD_CDC_DataRecivedCallback(char *rx)
{
  CmdLineMainMenu(rx, Temp);
  int respLen=strlen(Temp);

  memcpy(Temp + respLen, (uint8_t[]) { '\n', 0x00 }, 2 );

  CDC_Transmit_FS((uint8_t*)Temp, strlen(Temp));
}



/**
  * @brief Command Execute
  */
void CmdLineMainMenu(char *request, char* response)
{
   char cmd[APP_TX_DATA_SIZE/4];
   sscanf(request, "%s", cmd);
   DeviceDbgLog("%s", request);

  if(!strcmp(cmd, "*OPC?")) 
  {
    strcpy(response, "*OPC");
  }
  else if(!strcmp(cmd, "*RDY?"))
  {
    strcpy(response, "*RDY");
  }
  else if(!strcmp(cmd, "*WHOIS?"))
  {
    strcpy(response, DEVICE_NAME);
  }
  else if(!strcmp(cmd, "*VER?"))
  {
    strcpy(response, DEVICE_FW);
  }
  else if(!strcmp(cmd, "*UID?"))
  {
    //sprintf(response, "SN:%4X%4X%4X",HAL_GetUIDw0(),HAL_GetUIDw1(),HAL_GetUIDw2());
  }
  else if(strchr(request, '#'))
  { 
     char node[4];
     char param[32];
     float flt;
     uint8_t u8;
     int i32;

     sscanf(request,"#%s %s %s", node, cmd, param);
     if(!strcmp(cmd, "MEAS:VOLT?"))
     {
        if(IsBroadcast(node))
          sprintf(response, CMDLINE_CANNOT_BROADCAST_ERROR, cmd);
        else
          sprintf(response, "%f", Device.Vcells[GetNodeAddres(node)].CVmeas); 
     }
     else if(!strcmp(cmd, "MEAS:CURR?"))
     {
        if(IsBroadcast(node))
          sprintf(response, CMDLINE_CANNOT_BROADCAST_ERROR, cmd);
        else
          sprintf(response, "%f",Device.Vcells[GetNodeAddres(node)].CCmeas);
     }
     else if (!strcmp(cmd, "CURR:RNG?"))
     {
        if(IsBroadcast(node))
          sprintf(response, CMDLINE_CANNOT_BROADCAST_ERROR, cmd);
        else
          sprintf(response, "%d",Device.Vcells[GetNodeAddres(node)].Range);
     }
     else if (!strcmp(cmd, "READ:UPTIME?"))
     {
       if(IsBroadcast(node))
         sprintf(response, CMDLINE_CANNOT_BROADCAST_ERROR, cmd);
       else
         sprintf(response, "%ld",Device.Vcells[GetNodeAddres(node)].UpTimeSec);
     }
     else if(!strcmp(cmd, "READ:C-OFFSET?"))
     { 
        if(IsBroadcast(node))
          sprintf(response, CMDLINE_CANNOT_BROADCAST_ERROR, cmd);
        else
          sprintf(response, "%f",Device.Vcells[GetNodeAddres(node)].CCoffset);
     }
     else if(!strcmp(cmd, "SET:VOLT"))
     {
        flt = atof(param);
        if(CanNetSendSignal(&Device.CanNet,GetNodeAddres(node), IsBroadcast(node), MSG_PC_CV, &flt)== CANNET_OK)
          sprintf(response, CMDLINE_RESPONSE_OK);
        else
          sprintf(response, CMDLINE_SIGNAL_NOT_FOUND);
      }
      else if (!strcmp(cmd, "SET:CURR"))
      {
        flt = atof(param);
        if(CanNetSendSignal(&Device.CanNet,GetNodeAddres(node), IsBroadcast(node),MSG_PC_CC, &flt)== CANNET_OK)
          sprintf(response, CMDLINE_RESPONSE_OK);
        else
          sprintf(response, CMDLINE_SIGNAL_NOT_FOUND);
      }
      else if (!strcmp(cmd, "SET:RNG"))
      {
        i32=atoi(param);
        if(CanNetSendSignal(&Device.CanNet,GetNodeAddres(node), IsBroadcast(node), MSG_PC_RANGE, &i32)== CANNET_OK)
          sprintf(response, CMDLINE_RESPONSE_OK);
         else
          sprintf(response, CMDLINE_SIGNAL_NOT_FOUND);
      }
      else if (!strcmp(cmd, "SET:OE"))
      {
        i32=atoi(param);
        if(CanNetSendSignal(&Device.CanNet,GetNodeAddres(node), IsBroadcast(node), MSG_PC_OE, &i32)== CANNET_OK)
          sprintf(response, CMDLINE_RESPONSE_OK);
         else
          sprintf(response, CMDLINE_SIGNAL_NOT_FOUND);
      }
      else if (!strcmp(cmd, "SET:UP-SPEED"))
      {
        flt = atof(param);
        if(CanNetSendSignal(&Device.CanNet,GetNodeAddres(node), IsBroadcast(node), MSG_PC_UP_SPEED, &flt)== CANNET_OK)
          sprintf(response, CMDLINE_RESPONSE_OK);
        else
          sprintf(response, CMDLINE_SIGNAL_NOT_FOUND);
      }
      else if (!strcmp(cmd, "SET:DOWN-SPEED"))
      {
        flt = atof(param);
        if(CanNetSendSignal(&Device.CanNet,GetNodeAddres(node), IsBroadcast(node), MSG_PC_DOWN_SPEED, &flt)== CANNET_OK)
          sprintf(response, CMDLINE_RESPONSE_OK);
        else
          sprintf(response, CMDLINE_SIGNAL_NOT_FOUND);
      }
      else if (!strcmp(cmd, "SET:RSENSE"))
      {
        i32 = atoi(param);
        if(CanNetSendSignal(&Device.CanNet,GetNodeAddres(node), IsBroadcast(node), MSG_PC_RSENSE, &i32)== CANNET_OK)
          sprintf(response, CMDLINE_RESPONSE_OK);
         else
          sprintf(response, CMDLINE_SIGNAL_NOT_FOUND);
      }
      else if (!strcmp(cmd, "SET:GUARD"))
      {
        i32 = atoi(param);
        if(CanNetSendSignal(&Device.CanNet, GetNodeAddres(node), IsBroadcast(node), MSG_PC_GD, &i32)== CANNET_OK)
          sprintf(response, CMDLINE_RESPONSE_OK);
         else
          sprintf(response, CMDLINE_SIGNAL_NOT_FOUND);
      }
      else if (!strcmp(cmd, "TRIG:VOLT"))
      {
        u8 = 0;
        if(CanNetSendSignal(&Device.CanNet, GetNodeAddres(node), IsBroadcast(node), MSG_PC_TRIG, &u8)== CANNET_OK)
          sprintf(response, CMDLINE_RESPONSE_OK);
         else
          sprintf(response, CMDLINE_SIGNAL_NOT_FOUND);
      }
      else if (!strcmp(cmd, "TRIG:CURR"))
      {
        u8 = 1;
        if(CanNetSendSignal(&Device.CanNet, GetNodeAddres(node), IsBroadcast(node), MSG_PC_TRIG, &u8)== CANNET_OK)
          sprintf(response, CMDLINE_RESPONSE_OK);
         else
          sprintf(response, CMDLINE_SIGNAL_NOT_FOUND);
      }
      else if (!strcmp(cmd, "TRIG:C-OFFSET"))
      {
        u8 = 2;
        if(CanNetSendSignal(&Device.CanNet, GetNodeAddres(node), IsBroadcast(node), MSG_PC_TRIG, &u8)== CANNET_OK)
          sprintf(response, CMDLINE_RESPONSE_OK);
         else
          sprintf(response, CMDLINE_SIGNAL_NOT_FOUND);
      }
      else if (!strcmp(cmd, "TRIG:CURR_R79"))
      {
        u8 = 3;
        if(CanNetSendSignal(&Device.CanNet,GetNodeAddres(node), IsBroadcast(node), MSG_PC_TRIG, &u8)== CANNET_OK)
          sprintf(response, CMDLINE_RESPONSE_OK);
         else
          sprintf(response, CMDLINE_SIGNAL_NOT_FOUND);
      }
      else if (!strcmp(cmd, "TRIG:CURR_R80"))
      {
        u8 = 4;
        if(CanNetSendSignal(&Device.CanNet,GetNodeAddres(node), IsBroadcast(node), MSG_PC_TRIG, &u8)== CANNET_OK)
          sprintf(response, CMDLINE_RESPONSE_OK);
         else
          sprintf(response, CMDLINE_SIGNAL_NOT_FOUND);
      }
      else if (!strcmp(cmd, "SRV:RUN"))
      {
        if(isdigit(param[0]))
        {
          i32=atoi(param);
        }
        else
        {
          if(strcmp("RESET", param) == 0)
            i32 = SRV_RESTART; 
          else if(strcmp("STA-UPDATE", param) == 0)
            i32= SRV_UPDATE_STATUS;
          else if(strcmp("CLB-SAVE", param) == 0)
            i32 = SRV_CALIB_SAVE;
          else if(strcmp("CLB-OFF", param) == 0)
            i32 = SRV_CALIB_OFF;
          else if(strcmp("CLB-DEFAULT", param) == 0)
            i32 = SRV_CALIB_DEFAULT;
        }
        if(CanNetSendSignal(&Device.CanNet,GetNodeAddres(node), IsBroadcast(node), MSG_PC_SERVICES, &i32)== CANNET_OK)
          sprintf(response, CMDLINE_RESPONSE_OK);
         else
          sprintf(response, CMDLINE_SIGNAL_NOT_FOUND);
      }
      else
      {
        sprintf(response, CMDLINE_UNKNOWN_COMMAND_ERROR, cmd);
        cmd[0] = 0;;
      }
    }
}


/************************ (C) COPYRIGHT Konvol�ci� **********END OF FILE****/
