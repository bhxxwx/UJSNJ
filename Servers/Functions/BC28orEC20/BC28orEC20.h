/*
 * BC28orEC20.h
 *
 *  Created on: 2020年6月2日
 *      Author: wxujs
 */

#ifndef FUNCTIONS_BC28OREC20_BC28OREC20_H_
#define FUNCTIONS_BC28OREC20_BC28OREC20_H_
#include "UserConfig.h"
#include "DATAstruct/DATAstruct.h"
void ClearCmdBuffer();

void IOT_Reset();
void FullSystemReset();
void IOT_init();

void SendToCloud();
void pack_to_aliyun();
bool iscontants(char *str, char *cmd);
bool check_receives(uint8_t cmd_number, char *cmd);


#endif /* FUNCTIONS_BC28OREC20_BC28OREC20_H_ */
