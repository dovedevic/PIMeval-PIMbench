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
  double m_lutReadLatency = 0.000005;  // To change
  double m_lutWriteLatency = 0.000005; // To change
  double m_lutEnergyPerAccess = 0.00000001; // To change
  unsigned m_lutBitWidth = 8;   // Width of LUT entries (in bits)
};

#endif // LAVA_PIM_PERF_ENERGY_LUT_H
