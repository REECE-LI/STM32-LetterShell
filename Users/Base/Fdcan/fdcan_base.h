//
// Created by X on 2023/4/11.
//

#ifndef FDCAN_FDCAN_BASE_H_
#define FDCAN_FDCAN_BASE_H_

#include <cstdint>
#include "fdcan.h"

#define CAN_CONTROL_ID_BASE	0xfff

class FDCANBase {
 public:
  explicit FDCANBase(FDCAN_HandleTypeDef*_fdcan): fdcan(_fdcan) {};

  void Init();		// filter init

  void Send(uint8_t* _data, uint16_t _id);

  // virtual void Receive();

 protected:
  FDCAN_HandleTypeDef* fdcan;
  uint8_t             txData[8];
  uint8_t *            rxData;
  FDCAN_TxHeaderTypeDef txHeader;
  FDCAN_RxHeaderTypeDef rxHeader;

};






#endif //FDCAN_FDCAN_BASE_H_
