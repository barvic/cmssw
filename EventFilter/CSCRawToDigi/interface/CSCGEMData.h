#ifndef EventFilter_CSCRawToDigi_CSCGEMData_h
#define EventFilter_CSCRawToDigi_CSCGEMData_h

#include <vector>

class GEMPadDigiCluster;


class CSCGEMData
{
public:
  /// default constructor
  CSCGEMData(int ntbins=12);
  // length is in 16-bit words
  CSCGEMData(const unsigned short *c04buf , int length);

  // std::vector<int> BXN() const;
  std::vector<GEMPadDigiCluster> digis(int gem_chamber) const;
  int sizeInWords()
  {
    return size_;
  }
  int numGEMs()
  {
    return 2; // !!! TODO actual number of GEM chambers in readout
  }
  int nTbins()
  {
    return ntbins_;
  }
  void Print() const;
  bool check() const
  {
    return ((theData[0]==0x6C04) && (theData[size_-1] == 0x6D04));
  }

  static void setDebug(bool debugValue)
  {
    debug = debugValue;
  }

private:

  int getPartitionNumber(int address, int nPads) const;
  int getPartitionStripNumber(int address, int nPads, int etaPart) const;

  static bool debug;
  int ntbins_;
  int gems_enabled_;
  int size_;
  unsigned short theData[8*2*32+2];
};

#endif

