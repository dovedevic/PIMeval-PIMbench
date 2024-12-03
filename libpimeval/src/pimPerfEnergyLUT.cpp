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


   // Calculate the number of LUT operations per element
  unsigned numberOfLUTOperationsPerElement = ceil((unsigned)bitsPerElement / m_lutBitWidth);


  std::cout<< "numPass: " << numPass << std::endl;
  std::cout<< "bitsPerElement: " << bitsPerElement << std::endl;
  std::cout<< "numCores: " << numCores << std::endl;

  std::cout << "m_lutBitWidth: " << m_lutBitWidth << std::endl;
  std::cout << "m_fulcrumAluBitWidth: " << m_fulcrumAluBitWidth << std::endl;

  std::cout << "numberOfLUTOperationsPerElement: " << numberOfLUTOperationsPerElement << std::endl;


  switch (cmdType)
  {
    case PimCmdEnum::POPCOUNT:
    {
      double numberOfALUOperationPerElement = ((double)bitsPerElement / m_fulcrumAluBitWidth);
      numberOfALUOperationPerElement *= 12; // 4 shifts, 4 ands, 3 add/sub, 1 mul
      msRuntime = m_tR + m_tW + (maxElementsPerRegion * m_fulcrumAluLatency * numberOfALUOperationPerElement * numPass);
      double energyArithmetic = ((maxElementsPerRegion - 1) * 2 *  m_fulcrumShiftEnergy) + ((maxElementsPerRegion) * m_fulcrumALUArithmeticEnergy * 4);
      double energyLogical = ((maxElementsPerRegion - 1) * 2 *  m_fulcrumShiftEnergy) + ((maxElementsPerRegion) * m_fulcrumALULogicalEnergy * 8);
      mjEnergy = ((energyArithmetic + energyLogical) + m_eAP) * numCores * numPass;
      mjEnergy += m_pBChip * m_numChipsPerRank * m_numRanks * msRuntime;
      break;
      break;
    }
    case PimCmdEnum::ADD_SCALAR:
    {
      if(bitsPerElement <= 4){
        msRuntime = m_tR + m_tW + (maxElementsPerRegion * ((numberOfLUTOperationsPerElement * m_lutReadLatency)) * numPass); // Assuming LUT read latency represents LUT operation time
      }
      else{
        // Calculate the number of ALU operations needed for carry handling
        unsigned numChunks = ceil((unsigned)bitsPerElement / m_lutBitWidth);
        unsigned numberOfALUOperationPerElement = numChunks - 1;
        msRuntime = m_tR + m_tW + (maxElementsPerRegion * ((numberOfLUTOperationsPerElement * m_lutReadLatency) + (numberOfALUOperationPerElement * m_fulcrumAluLatency)) * numPass); // Assuming LUT read latency represents LUT operation time
      } 
      break;     
    }
    
    case PimCmdEnum::MUL_SCALAR:
    {
      if(bitsPerElement <= 4){
        msRuntime = m_tR + m_tW + (maxElementsPerRegion * ((numberOfLUTOperationsPerElement * m_lutReadLatency)) * numPass); // Assuming LUT read latency represents LUT operation time
      }
      else{
        unsigned numChunks = ceil((unsigned)bitsPerElement / m_lutBitWidth);
        unsigned totalShifts = (numChunks * numChunks) - numChunks;  // Except the first in each row
        unsigned totalAdditions = (numChunks * numChunks) - 1;  // Adding all the products into one result
        unsigned numberOfALUOperationPerElement = totalShifts + totalAdditions;
        msRuntime = m_tR + m_tW + (maxElementsPerRegion * ((numberOfLUTOperationsPerElement * m_lutReadLatency) + (numberOfALUOperationPerElement * m_fulcrumAluLatency)) * numPass); // Assuming LUT read latency represents LUT operation time
      }
      break;
    }
    case PimCmdEnum::SUB_SCALAR:
    case PimCmdEnum::DIV_SCALAR:
    case PimCmdEnum::ABS:
    {
      double numberOfALUOperationPerElement = ((double)bitsPerElement / m_fulcrumAluBitWidth);
      msRuntime = m_tR + m_tW + (maxElementsPerRegion * m_fulcrumAluLatency * numberOfALUOperationPerElement * numPass);
      mjEnergy = numPass * numCores * ((m_eAP * 2) + ((maxElementsPerRegion - 1) * 2 *  m_fulcrumShiftEnergy) + ((maxElementsPerRegion) * m_fulcrumALUArithmeticEnergy * numberOfALUOperationPerElement));
      mjEnergy += m_pBChip * m_numChipsPerRank * m_numRanks * msRuntime;
      break;
    }
    // case PimCmdEnum::ABS:
    // {
    //   break;
    // }
    case PimCmdEnum::AND_SCALAR:
    case PimCmdEnum::OR_SCALAR:
    case PimCmdEnum::XOR_SCALAR:
    case PimCmdEnum::XNOR_SCALAR:
    case PimCmdEnum::GT_SCALAR:
    case PimCmdEnum::LT_SCALAR:
    case PimCmdEnum::EQ_SCALAR:
    case PimCmdEnum::MIN_SCALAR:
    case PimCmdEnum::MAX_SCALAR:
    case PimCmdEnum::SHIFT_BITS_L:
    case PimCmdEnum::SHIFT_BITS_R:
    {
      if(bitsPerElement <= 4){
        msRuntime = m_tR + m_tW + (maxElementsPerRegion * ((numberOfLUTOperationsPerElement * m_lutReadLatency)) * numPass);
      }
      else{
        unsigned numChunks = ceil((unsigned)bitsPerElement / m_lutBitWidth);
        unsigned totalShifts = (numChunks * numChunks) - 1;
        unsigned totalOrs = (numChunks * numChunks) - 1;
        unsigned numberOfALUOperationPerElement = totalShifts + totalOrs;
        msRuntime = m_tR + m_tW + (maxElementsPerRegion * ((numberOfLUTOperationsPerElement * m_lutReadLatency) + (numberOfALUOperationPerElement * m_fulcrumAluLatency)) * numPass); // Assuming LUT read latency represents LUT operation time
      }
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

  std::cout<< "numPass: " << numPass << std::endl;
  std::cout<< "bitsPerElement: " << bitsPerElement << std::endl;
  std::cout<< "numCoresUsed: " << numCoresUsed << std::endl;

  std::cout << "m_lutBitWidth: " << m_lutBitWidth << std::endl;
  std::cout << "m_fulcrumAluBitWidth: " << m_fulcrumAluBitWidth << std::endl;

  unsigned maxElementsPerRegion = obj.getMaxElementsPerRegion();
  double numberOfLUTOperationsPerElement = ((double)bitsPerElement / m_lutBitWidth);
  std::cout << "numberOfLUTOperationsPerElement: " << numberOfLUTOperationsPerElement << std::endl;
  double numberOfALUOperationPerElement = ((double)bitsPerElement / m_fulcrumAluBitWidth);
  std::cout << "numberOfALUOperationPerElement: " << numberOfALUOperationPerElement << std::endl;

  switch (cmdType)
  {
    case PimCmdEnum::ADD:
    {
      if(bitsPerElement <= 4){
        msRuntime = 2 * m_tR + m_tW + maxElementsPerRegion * numberOfLUTOperationsPerElement * m_lutReadLatency; // Assuming LUT read latency represents LUT operation time
        msRuntime *= numPass;
      }
      else{
        // Calculate the number of ALU operations needed for carry handling
        unsigned numChunks = ceil((unsigned)bitsPerElement / m_lutBitWidth);
        unsigned numberOfALUOperationPerElement = numChunks - 1;
        msRuntime = 2 * m_tR + m_tW + maxElementsPerRegion * ((numberOfLUTOperationsPerElement * m_lutReadLatency) + (numberOfALUOperationPerElement * m_fulcrumAluLatency)); // Assuming LUT read latency represents LUT operation time
        msRuntime *= numPass;
        // std::cout<< "msRuntime: " << msRuntime << std::endl;
      } 
      break;     
    }
    case PimCmdEnum::MUL:
    {
      if(bitsPerElement <= 4){
        msRuntime = 2 * m_tR + m_tW + maxElementsPerRegion * numberOfLUTOperationsPerElement * m_lutReadLatency; // Assuming LUT read latency represents LUT operation time
        msRuntime *= numPass;
      }
      else{
        unsigned numChunks = ceil((unsigned)bitsPerElement / m_lutBitWidth);
        unsigned totalShifts = (numChunks * numChunks) - numChunks;  // Except the first in each row
        unsigned totalAdditions = (numChunks * numChunks) - 1;  // Adding all the products into one result
        unsigned numberOfALUOperationPerElement = totalShifts + totalAdditions;
        msRuntime = 2 * m_tR + m_tW + maxElementsPerRegion * ((numberOfLUTOperationsPerElement * m_lutReadLatency) + (numberOfALUOperationPerElement * m_fulcrumAluLatency)); // Assuming LUT read latency represents LUT operation time
        msRuntime *= numPass;
      }
      break;
    }
    case PimCmdEnum::SUB:
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
      // msRuntime = m_tR + m_tW + (maxElementsPerRegion * m_lutReadLatency * numberOfLUTOperationsPerElement * 2) * numPass;
      msRuntime = m_tR + m_tW + (maxElementsPerRegion * m_lutReadLatency * numberOfLUTOperationsPerElement) + (maxElementsPerRegion * m_fulcrumAluLatency * numberOfALUOperationPerElement) * numPass;
      mjEnergy = numCoresUsed * numPass * ((m_eAP + ((maxElementsPerRegion) * m_lutEnergyPerAccess * numberOfLUTOperationsPerElement)));
      mjEnergy += m_pBChip * m_numChipsPerRank * m_numRanks * msRuntime;
      break;
    }
    case PimCmdEnum::AND:
    case PimCmdEnum::OR:
    case PimCmdEnum::XOR:
    case PimCmdEnum::XNOR:
    case PimCmdEnum::GT:
    case PimCmdEnum::LT:
    case PimCmdEnum::EQ:
    case PimCmdEnum::MIN:
    case PimCmdEnum::MAX:
    {
      if(bitsPerElement <= 4){
        msRuntime = 2 * m_tR + m_tW + maxElementsPerRegion * numberOfLUTOperationsPerElement * m_lutReadLatency; // Assuming LUT read latency represents LUT operation time
        msRuntime *= numPass;
      }
      else{
        unsigned numChunks = ceil((unsigned)bitsPerElement / m_lutBitWidth);
        unsigned totalShifts = (numChunks * numChunks) - 1;
        unsigned totalOrs = (numChunks * numChunks) - 1;
        unsigned numberOfALUOperationPerElement = totalShifts + totalOrs;
        msRuntime = 2 * m_tR + m_tW + maxElementsPerRegion * ((numberOfLUTOperationsPerElement * m_lutReadLatency) + (numberOfALUOperationPerElement * m_fulcrumAluLatency)); // Assuming LUT read latency represents LUT operation time
        msRuntime *= numPass;
      }
      break;
    }
    default:
      std::cout << "PIM-Warning: Perf energy model not available for PIM command " << pimCmd::getName(cmdType, "") << std::endl;
      break;
  }

  return pimeval::perfEnergy(msRuntime, mjEnergy);
}

// *************************************In th following code, NO modification has been done on the Fulcrum's Code*************************************


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
