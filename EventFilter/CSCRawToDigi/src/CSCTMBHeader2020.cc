#include "EventFilter/CSCRawToDigi/interface/CSCTMBHeader2020.h"
#include "EventFilter/CSCRawToDigi/interface/CSCDMBHeader.h"
#include "FWCore/Utilities/interface/Exception.h"

const std::vector<std::pair<unsigned, unsigned> >
    run3_pattern_lookup_tbl = {{0, 0}, {0, 1}, {0, 2}, {0, 3}, {0, 4}, /* Valid LCT0, invalid LCT1 combination. Check LCT1 vpf */
                               {0, 0}, {0, 1}, {0, 2}, {0, 3}, {0, 4}, {1, 0}, {1, 1}, {1, 2}, {1, 3},
                               {1, 4}, {2, 0}, {2, 1}, {2, 2}, {2, 3}, {2, 4}, {3, 0}, {3, 1}, {3, 2},
                               {3, 3}, {3, 4}, {4, 0}, {4, 1}, {4, 2}, {4, 3}, {4, 4}}; /* pattern IDs 30,31 are reserved */

const unsigned run2_pattern_lookup_tbl[2][16] = {{10, 10, 10, 8, 8, 8, 6, 6, 6, 4, 4, 4, 2, 2, 2, 2},
                                                 {10, 10, 0, 9, 9, 9, 7, 7, 7, 5, 5, 5, 3, 3, 3, 3}};

CSCTMBHeader2020::CSCTMBHeader2020() {
  bzero(data(), sizeInWords() * 2);
  bits.nHeaderFrames = 42;
  bits.e0bline = 0x6E0B;
  bits.b0cline = 0xDB0C;
  bits.firmRevCode = 0x1000;
  bits.nTBins = 12;
  bits.nCFEBs = 7;
}

CSCTMBHeader2020::CSCTMBHeader2020(const unsigned short* buf) { memcpy(data(), buf, sizeInWords() * 2); }

void CSCTMBHeader2020::setEventInformation(const CSCDMBHeader& dmbHeader) {
  bits.cscID = dmbHeader.dmbID();
  bits.l1aNumber = dmbHeader.l1a();
  bits.bxnCount = dmbHeader.bxn();
}

///returns CLCT digis
std::vector<CSCCLCTDigi> CSCTMBHeader2020::CLCTDigis(uint32_t idlayer) {
  std::vector<CSCCLCTDigi> result;
  int halfstrip = bits.clct0_key_low + (bits.clct0_key_high << 7);
  int strip = halfstrip % 32;
  int xky = bits.clct0_xky & 0x3;  // CLCT0 1/4 and 1/8 strip bits
  int cfeb = halfstrip / 32;
  int pattern = bits.clct0_shape;
  int bend = pattern & 0x1;
  int quart = (xky >> 1) & 0x1;
  int eight = xky & 0x1;
  //offlineStripNumbering(strip, cfeb, pattern, bend);
  CSCCLCTDigi digi0(bits.clct0_valid,
                    bits.clct0_quality,
                    pattern,
                    1,
                    bend,
                    strip + (quart << 5) + (eight << 6),
                    cfeb,
                    bits.clct_bxn,
                    1,
                    bits.bxnPreTrigger,
                    bits.clct0_comparator_code,
                    CSCCLCTDigi::Version::Run3);
  //digi0.setFullBX(bits.bxnPreTrigger);

  halfstrip = bits.clct1_key_low + (bits.clct1_key_high << 7);
  strip = halfstrip % 32;
  xky = bits.clct1_xky & 0x3;  // CLCT1 1/4 and 1/8 strip bits
  cfeb = halfstrip / 32;
  pattern = bits.clct1_shape;
  bend = pattern & 0x1;
  quart = (xky >> 1) & 0x1;
  eight = xky & 0x1;

  //offlineStripNumbering(strip, cfeb, pattern, bend);
  CSCCLCTDigi digi1(bits.clct1_valid,
                    bits.clct1_quality,
                    pattern,
                    1,
                    bend,
                    strip + (quart << 5) + (eight << 6),
                    cfeb,
                    bits.clct_bxn,
                    2,
                    bits.bxnPreTrigger,
                    bits.clct1_comparator_code,
                    CSCCLCTDigi::Version::Run3);
  //digi1.setFullBX(bits.bxnPreTrigger);

  result.push_back(digi0);
  result.push_back(digi1);
  return result;
}

///returns CorrelatedLCT digis
std::vector<CSCCorrelatedLCTDigi> CSCTMBHeader2020::CorrelatedLCTDigis(uint32_t idlayer) const {
  std::vector<CSCCorrelatedLCTDigi> result;
  /// for the zeroth MPC word:
  int strip = bits.MPC_Muon0_clct_key_halfstrip;  //this goes from 0-223
  strip = (strip & 0xFF) | (bits.MPC_Muon0_clct_QuarterStrip << 8) | (bits.MPC_Muon0_clct_EighthStrip << 9);
  int slope = (bits.MPC_Muon0_clct_bend_low & 0x7) | (bits.MPC_Muon0_clct_bend_bit4 << 3);
  int hmt = bits.MPC_Muon_HMT_bit0 | (bits.MPC_Muon_HMT_high << 1);  // HighMultiplicityTrigger
  int clct_pattern_id = bits.MPC_Muon_clct_pattern_low | (bits.MPC_Muon_clct_pattern_bit5 << 4);

  std::pair<unsigned, unsigned> run3_pattern_pair = run3_pattern_lookup_tbl[clct_pattern_id % 30];
  int run2_pattern = run2_pattern_lookup_tbl[bits.MPC_Muon0_clct_LR][slope];
  int pattern = (run2_pattern & 0xf) + ((run3_pattern_pair.second & 0x7) << 4) + ((slope & 0xF) << 7);

  //offlineHalfStripNumbering(strip);
  CSCCorrelatedLCTDigi digi(1,
                            bits.MPC_Muon0_lct_vpf,
                            bits.MPC_Muon0_lct_quality,
                            bits.MPC_Muon0_alct_key_wire,
                            strip,
                            pattern,
                            bits.MPC_Muon0_clct_LR,
                            bits.MPC_Muon_alct_bxn,
                            0,
                            bits.MPC_Muon0_clct_bx0,
                            0,
                            0,
                            hmt,
                            CSCCorrelatedLCTDigi::Version::Run3);
  // bits.MPC_Muon0_clct_bx0, 0, 0, hmt, CSCCorrelatedLCTDigi::Version::Run3,clct_pattern_id);
  result.push_back(digi);
  /// for the first MPC word:
  strip = bits.MPC_Muon1_clct_key_halfstrip;  //this goes from 0-223
  strip = (strip & 0xFF) | (bits.MPC_Muon1_clct_QuarterStrip << 8) | (bits.MPC_Muon1_clct_EighthStrip << 9);
  slope = (bits.MPC_Muon1_clct_bend_low & 0x7) | (bits.MPC_Muon1_clct_bend_bit4 << 3);
  run2_pattern = run2_pattern_lookup_tbl[bits.MPC_Muon1_clct_LR][slope];
  pattern = (run2_pattern & 0xf) + ((run3_pattern_pair.first & 0x7) << 4) + ((slope & 0xF) << 7);

  //offlineHalfStripNumbering(strip);
  digi = CSCCorrelatedLCTDigi(2,
                              bits.MPC_Muon1_lct_vpf,
                              bits.MPC_Muon1_lct_quality,
                              bits.MPC_Muon1_alct_key_wire,
                              strip,
                              pattern,
                              bits.MPC_Muon1_clct_LR,
                              bits.MPC_Muon_alct_bxn,
                              0,
                              bits.MPC_Muon1_clct_bx0,
                              0,
                              0,
                              hmt,
                              CSCCorrelatedLCTDigi::Version::Run3);
  // bits.MPC_Muon1_clct_bx0, 0, 0, hmt, CSCCorrelatedLCTDigi::Version::Run3, clct_pattern_id);
  result.push_back(digi);

  return result;
}

void CSCTMBHeader2020::addALCT0(const CSCALCTDigi& digi) {
  throw cms::Exception("In CSC TMBHeaderFormat 2007, ALCTs belong in  ALCT header");
}

void CSCTMBHeader2020::addALCT1(const CSCALCTDigi& digi) {
  throw cms::Exception("In CSC TMBHeaderFormat 2007, ALCTs belong in  ALCT header");
}

void CSCTMBHeader2020::addCLCT0(const CSCCLCTDigi& digi) {
  int halfStrip = digi.getKeyStrip();
  int pattern = digi.getPattern();
  //hardwareStripNumbering(strip, cfeb, pattern, bend);
  bits.clct0_valid = digi.isValid();
  bits.clct0_quality = digi.getQuality();
  bits.clct0_shape = pattern;
  // first 7 bits of halfstrip
  bits.clct0_key_low = halfStrip & (0x7F);
  // most-significant (8th) bit
  bits.clct0_key_high = (halfStrip >> 7) & (0x1);
  bits.clct_bxn = digi.getBX();
  bits.bxnPreTrigger = digi.getFullBX();
  bits.clct0_comparator_code = digi.getCompCode();
  bits.clct0_xky = (digi.getEightStrip() & 0x1) + ((digi.getQuartStrip() & 0x1) << 1);
}

void CSCTMBHeader2020::addCLCT1(const CSCCLCTDigi& digi) {
  int halfStrip = digi.getKeyStrip();
  int pattern = digi.getPattern();
  //hardwareStripNumbering(strip, cfeb, pattern, bend);
  bits.clct1_valid = digi.isValid();
  bits.clct1_quality = digi.getQuality();
  bits.clct1_shape = pattern;
  // first 7 bits of halfstrip
  bits.clct1_key_low = halfStrip & (0x7F);
  // most-significant (8th) bit
  bits.clct1_key_high = (halfStrip >> 7) & (0x1);
  // There is just one BX field common for CLCT0 and CLCT1 (since both
  // are latched at the same BX); set it in addCLCT0().
  //bits.clct_bxn = digi.getBX();
  bits.bxnPreTrigger = digi.getFullBX();
  bits.clct1_comparator_code = digi.getCompCode();
  bits.clct1_xky = (digi.getEightStrip() & 0x1) + ((digi.getQuartStrip() & 0x1) << 1);
}

void CSCTMBHeader2020::addCorrelatedLCT0(const CSCCorrelatedLCTDigi& digi) {
  // int halfStrip = digi.getStrip();
  // hardwareHalfStripNumbering(halfStrip);

  bits.MPC_Muon0_lct_vpf = digi.isValid();
  bits.MPC_Muon0_alct_key_wire = digi.getKeyWG();
  bits.MPC_Muon0_clct_key_halfstrip = digi.getStrip(2) & 0x7F;
  bits.MPC_Muon0_clct_QuarterStrip = digi.getQuartStrip() & 0x1;
  bits.MPC_Muon0_clct_EighthStrip = digi.getEightStrip() & 0x1;
  bits.MPC_Muon0_lct_quality = digi.getQuality() & 0x7;

  // TODO: review and change
  // To restore 5-bits Run3 CLCT Pattern ID first assume and set pattern ID = LCT0 Run3 pattern
  uint16_t run3_pattern = digi.getRun3Pattern();
  bits.MPC_Muon_clct_pattern_low = run3_pattern & 0xF;
  bits.MPC_Muon_clct_pattern_bit5 = (run3_pattern >> 4) & 0x1;
  /*
  // Alternative method. Requires modification of CSCDigi/CSCCorrelatedDigi constructor
  bits.MPC_Muon_clct_pattern_low = digi.getRun3PatternID()&0xF;
  bits.MPC_Muon_clct_pattern_bit5 = (digi.getRun3PatternID()>>4)&0x1;
  */
  bits.MPC_Muon0_clct_bend_low = digi.getSlope() & 0x7;
  bits.MPC_Muon0_clct_bend_bit4 = (digi.getSlope() >> 3) & 0x1;
  bits.MPC_Muon0_clct_LR = digi.getBend() & 0x1;
  bits.MPC_Muon_HMT_bit0 = digi.getHMT() & 0x1;
  bits.MPC_Muon_HMT_high = (digi.getHMT() >> 1) & 0x7;
  bits.MPC_Muon_alct_bxn = digi.getBX();
  bits.MPC_Muon0_clct_bx0 = digi.getBX0();
}

void CSCTMBHeader2020::addCorrelatedLCT1(const CSCCorrelatedLCTDigi& digi) {
  // int halfStrip = digi.getStrip();
  // hardwareHalfStripNumbering(halfStrip);

  bits.MPC_Muon1_lct_vpf = digi.isValid();
  bits.MPC_Muon1_alct_key_wire = digi.getKeyWG();
  bits.MPC_Muon1_clct_key_halfstrip = digi.getStrip(2) & 0x7F;
  bits.MPC_Muon1_clct_QuarterStrip = digi.getQuartStrip() & 0x1;
  bits.MPC_Muon1_clct_EighthStrip = digi.getEightStrip() & 0x1;
  bits.MPC_Muon1_lct_quality = digi.getQuality() & 0x7;

  // To restore 5-bits Run3 CLCT Pattern ID assume that LCT0 pattern ID is already processed
  // and combine LCT1 Run3 pattern to set final 5-bit pattern ID
  if (digi.isValid()) {
    uint16_t clct_pattern_id = bits.MPC_Muon_clct_pattern_low | (bits.MPC_Muon_clct_pattern_bit5 << 4);
    uint16_t run3_pattern = digi.getRun3Pattern();
    clct_pattern_id = (clct_pattern_id + (run3_pattern + 1) * 5) % 30;
    bits.MPC_Muon_clct_pattern_low = clct_pattern_id & 0xF;
    bits.MPC_Muon_clct_pattern_bit5 = (clct_pattern_id >> 4) & 0x1;
  }
  /*
  // Alternative method. Requires modification of CSCDigi/CSCCorrelatedDigi constructor
  bits.MPC_Muon_clct_pattern_low = digi.getRun3PatternID()&0xF;
  bits.MPC_Muon_clct_pattern_bit5 = (digi.getRun3PatternID()>>4)&0x1;
  */
  bits.MPC_Muon1_clct_bend_low = digi.getSlope() & 0x7;
  bits.MPC_Muon1_clct_bend_bit4 = (digi.getSlope() >> 3) & 0x1;
  bits.MPC_Muon1_clct_LR = digi.getBend() & 0x1;
  bits.MPC_Muon_HMT_bit0 = digi.getHMT() & 0x1;
  bits.MPC_Muon_HMT_high = (digi.getHMT() >> 1) & 0x7;
  bits.MPC_Muon_alct_bxn = digi.getBX();
  bits.MPC_Muon1_clct_bx0 = digi.getBX0();
}

void CSCTMBHeader2020::print(std::ostream& os) const {
  os << "...............TMB Header.................."
     << "\n";
  os << std::hex << "BOC LINE " << bits.b0cline << " EOB " << bits.e0bline << "\n";
  os << std::dec << "fifoMode = " << bits.fifoMode << ", nTBins = " << bits.nTBins << "\n";
  //  os << "dumpCFEBs = " << dumpCFEBs << ", nHeaderFrames = "
  //     << nHeaderFrames << "\n";
  os << "boardID = " << bits.boardID << ", cscID = " << bits.cscID << "\n";
  os << "l1aNumber = " << bits.l1aNumber << ", bxnCount = " << bits.bxnCount << "\n";
  //  os << "preTrigTBins = " << preTrigTBins << ", nCFEBs = "<< nCFEBs<< " ";
  os << "trigSourceVect = " << bits.trigSourceVect
     << ", activeCFEBs = " << (bits.activeCFEBs | (bits.activeCFEBs_2 << 5))
     << ", readCFEBs = " << (bits.readCFEBs | (bits.readCFEBs_2 << 5)) << "\n";
  os << "bxnPreTrigger = " << bits.bxnPreTrigger << "\n";
  os << "tmbMatch = " << bits.tmbMatch << " alctOnly = " << bits.alctOnly << " clctOnly = " << bits.clctOnly << "\n";
  //     << " alctMatchTime = " << alctMatchTime << " ";
  //  os << "hs_thresh = " << hs_thresh << ", ds_thresh = " << ds_thresh
  //     << " ";
  //
  os << "CLCT Words:\n"
     << " bits.clct0_valid = " << bits.clct0_valid << " bits.clct0_shape = " << bits.clct0_shape
     << " bits.clct0_quality = " << bits.clct0_quality
     << " halfstrip = " << (bits.clct0_key_low + (bits.clct0_key_high << 7)) << "\n";
  os << " bits.clct0_xky = " << bits.clct0_xky << " bits.clct0_comparator_code = " << bits.clct0_comparator_code
     << "\n";

  os << " bits.clct1_valid = " << bits.clct1_valid << " bits.clct1_shape = " << bits.clct1_shape
     << " bits.clct1_quality = " << bits.clct1_quality
     << " halfstrip = " << (bits.clct1_key_low + (bits.clct1_key_high << 7)) << "\n";
  os << " bits.clct1_xky = " << bits.clct1_xky << " bits.clct1_comparator_code = " << bits.clct1_comparator_code
     << "\n";

  os << "MPC Words:\n"
     << " LCT0 valid = " << bits.MPC_Muon0_lct_vpf << " key WG = " << bits.MPC_Muon0_alct_key_wire
     << " key halfstrip = " << bits.MPC_Muon0_clct_key_halfstrip
     << " 1/4strip flag = " << bits.MPC_Muon0_clct_QuarterStrip
     << " 1/8strip flag = " << bits.MPC_Muon0_clct_EighthStrip << "\n"
     << " quality = " << bits.MPC_Muon0_lct_quality
     << " slope/bend = " << ((bits.MPC_Muon0_clct_bend_low & 0x7) | (bits.MPC_Muon0_clct_bend_bit4 << 3))
     << " L/R bend = " << bits.MPC_Muon0_clct_LR << "\n";

  os << " LCT1 valid = " << bits.MPC_Muon1_lct_vpf << " key WG = " << bits.MPC_Muon1_alct_key_wire
     << " key halfstrip = " << bits.MPC_Muon1_clct_key_halfstrip
     << " 1/4strip flag = " << bits.MPC_Muon1_clct_QuarterStrip
     << " 1/8strip flag = " << bits.MPC_Muon1_clct_EighthStrip << "\n"
     << " quality = " << bits.MPC_Muon1_lct_quality
     << " slope/bend = " << ((bits.MPC_Muon1_clct_bend_low & 0x7) | (bits.MPC_Muon1_clct_bend_bit4 << 3))
     << " L/R bend = " << bits.MPC_Muon1_clct_LR << "\n";

  os << " clct_5bit_pattern_id = " << (bits.MPC_Muon_clct_pattern_low | (bits.MPC_Muon_clct_pattern_bit5 << 4))
     << " HMT = " << (bits.MPC_Muon_HMT_bit0 | (bits.MPC_Muon_HMT_high << 1)) << "\n";

  // os << "..................CLCT....................." << "\n";
  os << "GEM Data:\n"
     << " gem_enabled_fibers = " << GEM_enabled_fibers() << " gem_fifo_tbins = " << GEM_fifo_tbins()
     << " gem_fifo_pretrig = " << GEM_fifo_pretrig() << " gem_zero_suppress = " << GEM_zero_suppress()
     << " gem_csc_bend_enable = " << bits.gem_csc_bend_enable << " gem_sync_dataword = 0x" << std::hex
     << GEM_sync_dataword() << " gem_timing_dataword = 0x" << std::hex << GEM_timing_dataword() << std::dec << "\n";
}
