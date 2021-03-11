#include "EventFilter/CSCRawToDigi/interface/CSCGEMData.h"
#include "DataFormats/GEMDigi/interface/GEMPadDigiCluster.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include <string>
#include <cstdio>
#include <strings.h> // for bzero
#include <cstring>

bool CSCGEMData::debug = false;

CSCGEMData::CSCGEMData(int ntbins)
  : ntbins_(ntbins), size_( 0 )
{
  theData[0] = 0x6C04;
  ntbins_ = ntbins;
  gems_enabled_ = 2;
  size_ = 2 + ntbins_*gems_enabled_*8;
  theData[size_-1] = 0x6D04;
}

CSCGEMData::CSCGEMData(const unsigned short * buf, int length)
  : size_(length)
{
  size_ = length;
  ntbins_ = (size_-2)/(8*2);
  gems_enabled_ = 2;
  memcpy(theData, buf, size_*2);
}

void CSCGEMData::Print() const
{
  LogTrace ("CSCGEMData|CSCRawToDigi") << "CSCGEMData.Print";
  for(int line = 0; line < ((size_)); ++line)
    {
      LogTrace("CSCGEMData|CSCRawToDigi") <<std::hex << theData[line];
    }
}

int CSCGEMData::getPartitionNumber(int addr, int npads) const
{
  return addr / (npads -1);
}

int CSCGEMData::getPartitionStripNumber(int address, int nPads, int etaPart) const
{
  return address - (nPads * etaPart);
}


std::vector<GEMPadDigiCluster> CSCGEMData::digis(int gem_chamber) const
{
  std::vector<GEMPadDigiCluster> result;
  result.clear();
  int nPads = 192; // From geometry
  int maxAddr = 1536;
  int nGEMs = 2;
  int maxClusters = 8;
  // std::cout << std::hex << "markers " << theData[0] << ": " << theData[size_-1] << std::dec << " size: " << size_ << std::endl;
  for (int i=0; i<ntbins_; i++)
    {
      for (int gem=0; gem<nGEMs; gem++)
        {
          if (gem==gem_chamber) // Return only digis for specified GEM chamber
            {
              for (int TMBCluster=0; TMBCluster<maxClusters; TMBCluster++)
                {
                  int dataAddr = 1 + (i*nGEMs+gem)*maxClusters + TMBCluster;
                  int address = theData[dataAddr] & 0x7ff;
                  // std::cout << dataAddr << ": " << address <<std::endl;
                  int nExtraPads = (theData[dataAddr] >>11)&0x7;
                  if (address<maxAddr)
                    {
                      int  etaPart   = getPartitionNumber(address,nPads);
                      int  padInPart = getPartitionStripNumber(address,nPads,etaPart);
                      std::vector<short unsigned int> pads;
                      for(int iP = 0; iP <= nExtraPads; ++iP)
                        pads.push_back(padInPart + iP );
                      GEMPadDigiCluster cluster ( pads, i);
                      result.push_back(cluster);
                    }
                }
            }
        }
    }
  return result;
}

