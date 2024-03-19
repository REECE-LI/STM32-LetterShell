//
// Created by X on 2023/4/11.
//
#include "fdcan_base.h"
#include <cstring>

void FDCANBase::Init() {
  // filter init
  FDCAN_FilterTypeDef  can_filter;

  HAL_FDCAN_ActivateNotification(fdcan, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0);


  can_filter.IdType = FDCAN_STANDARD_ID;
  can_filter.FilterIndex = 0;
  can_filter.FilterType = FDCAN_FILTER_MASK;
  can_filter.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
  can_filter.FilterID1 = 0x000;                               // 双电子掩码 全通过
  can_filter.FilterID2 = 0x7ff;
  if(HAL_FDCAN_ConfigFilter(&hfdcan1,&can_filter)!=HAL_OK)
  {
	Error_Handler();
  }

  // tx init
  txHeader.Identifier = CAN_CONTROL_ID_BASE;
  txHeader.IdType = FDCAN_STANDARD_ID;
  txHeader.TxFrameType = FDCAN_DATA_FRAME;
  txHeader.DataLength = FDCAN_DLC_BYTES_8;
  txHeader.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
  txHeader.BitRateSwitch = FDCAN_BRS_OFF;
  txHeader.FDFormat = FDCAN_CLASSIC_CAN;
  txHeader.TxEventFifoControl = FDCAN_NO_TX_EVENTS;

  // rx init
  // to do list
  HAL_FDCAN_Start(fdcan);
}

// can send
void FDCANBase::Send(uint8_t* _data, uint16_t _id) {
  txHeader.Identifier = _id;
  memcpy(txData, _data, 8);
  while(HAL_FDCAN_AddMessageToTxFifoQ(fdcan, &txHeader, txData) != HAL_OK);
    HAL_Delay(1);
  // while ();
}

//// can receive
//void FDCANBase::Receive() {
//  HAL_FDCAN_GetRxMessage(fdcan, FDCAN_RX_FIFO0, &rxHeader, rxData);
//}