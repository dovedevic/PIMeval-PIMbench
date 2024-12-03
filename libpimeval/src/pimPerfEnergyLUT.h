// File: pimPerfEnergyLUT.h
// PIMeval Simulator - Performance Energy Models

// Need to make changes accordingly 


#ifndef LAVA_PIM_PERF_ENERGY_LUT_H
#define LAVA_PIM_PERF_ENERGY_LUT_H

#include "libpimeval.h"                // for PimDeviceEnum, PimDataType
#include "pimParamsDram.h"             // for pimParamsDram
#include "pimCmd.h"                    // for PimCmdEnum
#include "pimResMgr.h"                 // for pimObjInfo
#include "pimPerfEnergyBase.h"         // for pimPerfEnergyBase


//! @class  pimPerfEnergyLUT
//! @brief  PIM performance energy model for LUT-based architecture
class pimPerfEnergyLUT : public pimPerfEnergyBase
{
public:
  pimPerfEnergyLUT(const pimPerfEnergyModelParams& params) : pimPerfEnergyBase(params) {}
  virtual ~pimPerfEnergyLUT() {}

  // Override the energy calculation methods for specific LUT operations
  virtual pimeval::perfEnergy getPerfEnergyForFunc1(PimCmdEnum cmdType, const pimObjInfo& obj) const override;
  virtual pimeval::perfEnergy getPerfEnergyForFunc2(PimCmdEnum cmdType, const pimObjInfo& obj) const override;
  virtual pimeval::perfEnergy getPerfEnergyForRedSum(PimCmdEnum cmdType, const pimObjInfo& obj, unsigned numPass) const override;
  virtual pimeval::perfEnergy getPerfEnergyForBroadcast(PimCmdEnum cmdType, const pimObjInfo& obj) const override;
  virtual pimeval::perfEnergy getPerfEnergyForRotate(PimCmdEnum cmdType, const pimObjInfo& obj) const override;

protected:
  // LUT-specific parameters for energy calculations
  double m_lutReadLatency = 0.000000002;  // SRAM based scrathcpad memory, To crosscheck
  double m_lutWriteLatency = 0.000000002; // SRAM based scrathcpad memory, To crosscheck
  // unsigned m_lutBitWidth = 8;   // Width of LUT entries (in bits)
  unsigned m_lutBitWidth = 4;   // Width of LUT entries (in bits)
  double m_lutEnergyPerAccess = 0.00000001; // To change

  double m_fulcrumAluLatency = 0.00000609; // 6.09ns
  unsigned m_fulcrumAluBitWidth = 32;
  // Following values are taken from fulcrum paper.
  double m_fulcrumALUArithmeticEnergy = 0.0000000004992329586; // mJ
  double m_fulcrumALULogicalEnergy = 0.0000000001467846411; // mJ
  double m_fulcrumShiftEnergy = 0.0000000075; // mJ
};

#endif // LAVA_PIM_PERF_ENERGY_LUT_H
