// File: pimPerfEnergyLUT.cpp
// PIMeval Simulator - Performance Energy Models for LUT-based PIM

// Need to make changes accordingly 


#include "pimPerfEnergyLUT.h"
#include "pimCmd.h"
#include <iostream>

//! @brief  Perf energy model of LUT for func1
pimeval::perfEnergy
pimPerfEnergyLUT::getPerfEnergyForFunc1(PimCmdEnum cmdType, const pimObjInfo& obj) const
{
  double msRuntime = 0.0;
  double mjEnergy = 0.0;
  unsigned numPass = obj.getMaxNumRegionsPerCore();
  unsigned bitsPerElement = obj.getBitsPerElement();
  unsigned numCores = obj.getNumCoresUsed();

  unsigned maxElementsPerRegion = obj.getMaxElementsPerRegion();
  double numberOfLUTOperationsPerElement = ((double)bitsPerElement / m_lutBitWidth);

  switch (cmdType)
  {
    case PimCmdEnum::POPCOUNT:
    {
      msRuntime = m_tR + m_tW + (maxElementsPerRegion * m_lutReadLatency * numberOfLUTOperationsPerElement * numPass);
      mjEnergy = (m_eAP + (maxElementsPerRegion * m_lutEnergyPerAccess * numberOfLUTOperationsPerElement)) * numCores * numPass;
      mjEnergy += m_pBChip * m_numChipsPerRank * m_numRanks * msRuntime;
      break;
    }
    case PimCmdEnum::ADD_SCALAR:
    case PimCmdEnum::SUB_SCALAR:
    case PimCmdEnum::MUL_SCALAR:
    case PimCmdEnum::DIV_SCALAR:
    {
      msRuntime = m_tR + m_tW + (maxElementsPerRegion * m_lutReadLatency * numberOfLUTOperationsPerElement * numPass);
      mjEnergy = numPass * numCores * (m_eAP + ((maxElementsPerRegion) * m_lutEnergyPerAccess * numberOfLUTOperationsPerElement));
      mjEnergy += m_pBChip * m_numChipsPerRank * m_numRanks * msRuntime;
      break;
    }
    default:
      std::cout << "PIM-Warning: Perf energy model not available for PIM command " << pimCmd::getName(cmdType, "") << std::endl;
      break;
  }

  return pimeval::perfEnergy(msRuntime, mjEnergy);
}

//! @brief  Perf energy model of LUT for func2
pimeval::perfEnergy
pimPerfEnergyLUT::getPerfEnergyForFunc2(PimCmdEnum cmdType, const pimObjInfo& obj) const
{
  double msRuntime = 0.0;
  double mjEnergy = 0.0;
  unsigned numPass = obj.getMaxNumRegionsPerCore();
  unsigned bitsPerElement = obj.getBitsPerElement();
  unsigned numCoresUsed = obj.getNumCoresUsed();

  unsigned maxElementsPerRegion = obj.getMaxElementsPerRegion();
  double numberOfLUTOperationsPerElement = ((double)bitsPerElement / m_lutBitWidth);

  switch (cmdType)
  {
    case PimCmdEnum::ADD:
    case PimCmdEnum::SUB:
    case PimCmdEnum::MUL:
    case PimCmdEnum::DIV:
    {
      msRuntime = 2 * m_tR + m_tW + (maxElementsPerRegion * m_lutReadLatency * numberOfLUTOperationsPerElement);
      msRuntime *= numPass;
      mjEnergy = numCoresUsed * numPass * ((m_eAP + ((maxElementsPerRegion) * m_lutEnergyPerAccess * numberOfLUTOperationsPerElement)));
      mjEnergy += m_pBChip * m_numChipsPerRank * m_numRanks * msRuntime;
      break;
    }
    case PimCmdEnum::SCALED_ADD:
    {
      msRuntime = m_tR + m_tW + (maxElementsPerRegion * m_lutReadLatency * numberOfLUTOperationsPerElement * 2) * numPass;
      mjEnergy = numCoresUsed * numPass * ((m_eAP + ((maxElementsPerRegion) * m_lutEnergyPerAccess * numberOfLUTOperationsPerElement)));
      mjEnergy += m_pBChip * m_numChipsPerRank * m_numRanks * msRuntime;
      break;
    }
    default:
      std::cout << "PIM-Warning: Perf energy model not available for PIM command " << pimCmd::getName(cmdType, "") << std::endl;
      break;
  }

  return pimeval::perfEnergy(msRuntime, mjEnergy);
}

//! @brief  Perf energy model of LUT for reduction sum
pimeval::perfEnergy
pimPerfEnergyLUT::getPerfEnergyForRedSum(PimCmdEnum cmdType, const pimObjInfo& obj, unsigned numPass) const
{
  double msRuntime = 0.0;
  double mjEnergy = 0.0;
  unsigned bitsPerElement = obj.getBitsPerElement();
  unsigned maxElementsPerRegion = obj.getMaxElementsPerRegion();
  unsigned numCore = obj.getNumCoresUsed();

  // read a row to LUT, then reduce in parallel
  double numberOfOperationPerElement = ((double)bitsPerElement / m_lutBitWidth);
  msRuntime = m_tR + (maxElementsPerRegion * m_lutReadLatency * numberOfOperationPerElement * numPass);
  mjEnergy = numPass * numCore * (m_eAP + ((maxElementsPerRegion) * m_lutEnergyPerAccess * numberOfOperationPerElement));

  // Reduction for all regions
  double aggregateMs = static_cast<double>(numCore) / 3200000;
  msRuntime += aggregateMs;
  mjEnergy += aggregateMs * 200; // Assumed TDP value for CPU or similar system

  mjEnergy += m_pBChip * m_numChipsPerRank * m_numRanks * msRuntime;

  return pimeval::perfEnergy(msRuntime, mjEnergy);
}

//! @brief  Perf energy model of LUT for broadcast
pimeval::perfEnergy
pimPerfEnergyLUT::getPerfEnergyForBroadcast(PimCmdEnum cmdType, const pimObjInfo& obj) const
{
  double msRuntime = 0.0;
  double mjEnergy = 0.0;
  unsigned numPass = obj.getMaxNumRegionsPerCore();
  unsigned bitsPerElement = obj.getBitsPerElement();
  unsigned maxElementsPerRegion = obj.getMaxElementsPerRegion();
  unsigned numCore = obj.getNumCoresUsed();

  // Write an element using LUT
  double numberOfOperationPerElement = ((double)bitsPerElement / m_lutBitWidth);
  msRuntime = m_tW + m_lutReadLatency * maxElementsPerRegion * numberOfOperationPerElement;
  msRuntime *= numPass;
  mjEnergy = numPass * numCore * (m_eAP + ((maxElementsPerRegion) * m_lutEnergyPerAccess * numberOfOperationPerElement));
  mjEnergy += m_pBChip * m_numChipsPerRank * m_numRanks * msRuntime;

  return pimeval::perfEnergy(msRuntime, mjEnergy);
}

//! @brief  Perf energy model of LUT for rotate
pimeval::perfEnergy
pimPerfEnergyLUT::getPerfEnergyForRotate(PimCmdEnum cmdType, const pimObjInfo& obj) const
{
  double msRuntime = 0.0;
  double mjEnergy = 0.0;
  unsigned numPass = obj.getMaxNumRegionsPerCore();
  unsigned bitsPerElement = obj.getBitsPerElement();
  unsigned numRegions = obj.getRegions().size();

  // Handling boundary elements (copying between device and host)
  pimeval::perfEnergy perfEnergyBT = getPerfEnergyForBytesTransfer(PimCmdEnum::COPY_D2H, numRegions * bitsPerElement / 8);

  // Rotate within subarray (using LUT)
  msRuntime = (m_tR + (bitsPerElement + 2) * m_tL + m_tW); // For one pass
  msRuntime *= numPass;
  mjEnergy = (m_eAP + (bitsPerElement + 2) * m_eL) * numPass;
  msRuntime += 2 * perfEnergyBT.m_msRuntime;
  mjEnergy += 2 * perfEnergyBT.m_mjEnergy;

  return pimeval::perfEnergy(msRuntime, mjEnergy);
}
