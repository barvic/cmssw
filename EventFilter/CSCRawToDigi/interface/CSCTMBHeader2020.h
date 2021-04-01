#ifndef EventFilter_CSCRawToDigi_CSCTMBHeader2020_h
#define EventFilter_CSCRawToDigi_CSCTMBHeader2020_h
#include "EventFilter/CSCRawToDigi/interface/CSCVTMBHeaderFormat.h"
#include "DataFormats/MuonDetId/interface/CSCDetId.h"

struct CSCTMBHeader2020 : public CSCVTMBHeaderFormat {
  enum { NWORDS = 43 };
  CSCTMBHeader2020();
  CSCTMBHeader2020(const unsigned short* buf);
  void setEventInformation(const CSCDMBHeader& dmbHeader);

  uint16_t BXNCount() const override { return bits.bxnCount; }
  uint16_t ALCTMatchTime() const override { return bits.matchWin; }
  uint16_t CLCTOnly() const override { return bits.clctOnly; }
  uint16_t ALCTOnly() const override { return bits.alctOnly; }
  uint16_t TMBMatch() const override { return bits.tmbMatch; }
  uint16_t Bxn0Diff() const override { return 0; }
  uint16_t Bxn1Diff() const override { return 0; }
  uint16_t L1ANumber() const override { return bits.l1aNumber; }
  uint16_t NTBins() const override { return bits.nTBins; }
  uint16_t NCFEBs() const override { return bits.nCFEBs; }
  void setNCFEBs(uint16_t ncfebs) override { bits.nCFEBs = ncfebs & 0x7F; }
  uint16_t firmwareRevision() const override { return bits.firmRevCode; }
  uint16_t syncError() const override { return bits.syncError; }
  uint16_t syncErrorCLCT() const override { return bits.clct_sync_err; }
  uint16_t syncErrorMPC0() const override { return 0; }
  uint16_t syncErrorMPC1() const override { return 0; }

  // == Run 3 CSC-GEM Trigger Format
  virtual uint16_t CLCT0_ComparatorCode() const { return bits.clct0_comparator_code; }
  virtual uint16_t CLCT1_ComparatorCode() const { return bits.clct1_comparator_code; }
  virtual uint16_t CLCT0_xky() const { return bits.clct0_xky; }
  virtual uint16_t CLCT1_xky() const { return bits.clct1_xky; }
  virtual uint16_t HMT_nhits() const {
    return ((bits.hmt_nhits_bit0 & 0x1) + ((bits.hmt_nhits_bit1 & 0x1) << 1) +
            ((bits.hmt_nhits_bits_high & 0x1F) << 2));
  }
  virtual uint16_t GEM_enabled_fibers() const { return (bits.gem_enabled_fibers & 0xF); }
  virtual uint16_t GEM_fifo_tbins() const { return bits.fifo_tbins_gem; }
  virtual uint16_t GEM_fifo_pretrig() const { return bits.fifo_pretrig_gem; }
  virtual uint16_t GEM_zero_suppress() const { return bits.gem_zero_suppression; }
  virtual uint16_t GEM_sync_dataword() const {
    return ((bits.copad_match & 0xFF) + (bits.gemA_vpf << 8) + (bits.gemB_vpf << 9) + (bits.gemA_over_flow << 10) +
            (bits.gemB_over_flow << 11) + (bits.gemA_sync << 12) + (bits.gemB_sync << 13) + (bits.gems_sync << 14));
  }
  virtual uint16_t GEM_timing_dataword() const {
    return ((bits.gem_delay & 0xFF) + ((bits.gem_clct_win & 0xF) << 8) + ((bits.alct_gem_win & 0x7) << 12));
  }

  virtual uint16_t Run3_CLCT_patternID() const {
    return (bits.MPC_Muon_clct_pattern_low | (bits.MPC_Muon_clct_pattern_bit5 << 4));
  }
  // ==

  ///returns CLCT digis
  virtual std::vector<CSCCLCTDigi> CLCTDigis(uint32_t idlayer);
  ///returns CorrelatedLCT digis
  virtual std::vector<CSCCorrelatedLCTDigi> CorrelatedLCTDigis(uint32_t idlayer) const;

  /// in 16-bit words.  Add olne because we include beginning(b0c) and
  /// end (e0c) flags
  unsigned short int sizeInWords() const { return NWORDS; }

  virtual unsigned short int NHeaderFrames() const { return bits.nHeaderFrames; }
  /// returns the first data word
  virtual unsigned short* data() { return (unsigned short*)(&bits); }
  virtual bool check() const { return bits.e0bline == 0x6e0b; }

  /// Needed before data packing
  //void setChamberId(const CSCDetId & detId) {theChamberId = detId;}

  /// for data packing
  virtual void addCLCT0(const CSCCLCTDigi& digi);
  virtual void addCLCT1(const CSCCLCTDigi& digi);
  virtual void addALCT0(const CSCALCTDigi& digi);
  virtual void addALCT1(const CSCALCTDigi& digi);
  virtual void addCorrelatedLCT0(const CSCCorrelatedLCTDigi& digi);
  virtual void addCorrelatedLCT1(const CSCCorrelatedLCTDigi& digi);

  void swapCLCTs(CSCCLCTDigi& digi1, CSCCLCTDigi& digi2);

  virtual void print(std::ostream& os) const;

  struct {
    // 0
    unsigned b0cline : 16;
    unsigned bxnCount : 12, dduCode1 : 3, flag1 : 1;
    unsigned l1aNumber : 12, dduCode2 : 3, flag2 : 1;
    unsigned readoutCounter : 12, dduCode3 : 3, flag3 : 1;
    // 4
    unsigned boardID : 5, cscID : 4, runID : 4, stackOvf : 1, syncError : 1, flag4 : 1;
    unsigned nHeaderFrames : 6, fifoMode : 3, r_type : 2, l1atype : 2, hasBuf : 1, bufFull : 1, flag5 : 1;
    unsigned bd_status : 15, flag6 : 1;
    unsigned firmRevCode : 15, flag7 : 1;
    // 8
    unsigned bxnPreTrigger : 12, tmb_clct0_discard : 1, tmb_clct1_discard : 1, clock_lock_lost : 1, flag8 : 1;
    unsigned preTrigCounter : 15, flag9 : 1;
    unsigned clct0_comparator_code : 12, clct0_xky : 2, hmt_nhits_bit0 : 1, flag10 : 1;

    unsigned clctCounterLow : 15, flag11 : 1;
    // 12
    unsigned copad_match : 8, gemA_vpf : 1, gemB_vpf : 1, gemA_over_flow : 1, gemB_over_flow : 1, gemA_sync : 1,
        gemB_sync : 1, gems_sync : 1, flag12 : 1;
    unsigned trigCounter : 15, flag13 : 1;
    unsigned clct1_comparator_code : 12, clct1_xky : 2, hmt_nhits_bit1 : 1, flag14 : 1;
    unsigned alctCounterLow : 15, flag15 : 1;
    // 16
    unsigned gem_delay : 8, gem_clct_win : 4, alct_gem_win : 3, flag16 : 1;
    unsigned uptimeCounterLow : 15, flag17 : 1;
    unsigned uptimeCounterHigh : 15, flag18 : 1;
    unsigned nCFEBs : 3, nTBins : 5, fifoPretrig : 5, scopeExists : 1, vmeExists : 1, flag19 : 1;
    // 20
    unsigned hitThresh : 3, pidThresh : 4, nphThresh : 3, pid_thresh_postdrift : 4, staggerCSC : 1, flag20 : 1;
    unsigned triadPersist : 4, dmbThresh : 3, alct_delay : 4, clct_width : 4, flag21 : 1;
    unsigned trigSourceVect : 9, r_nlayers_hit_vec : 6, flag22 : 1;
    unsigned activeCFEBs : 5, readCFEBs : 5, pop_l1a_match_win : 4, aff_source : 1, flag23 : 1;
    // 24
    unsigned tmbMatch : 1, alctOnly : 1, clctOnly : 1, matchWin : 4, noALCT : 1, oneALCT : 1, oneCLCT : 1, twoALCT : 1,
        twoCLCT : 1, dupeALCT : 1, dupeCLCT : 1, lctRankErr : 1, flag24 : 1;
    unsigned clct0_valid : 1, clct0_quality : 3, clct0_shape : 4, clct0_key_low : 7, flag25 : 1;
    unsigned clct1_valid : 1, clct1_quality : 3, clct1_shape : 4, clct1_key_low : 7, flag26 : 1;
    unsigned clct0_key_high : 1, clct1_key_high : 1, clct_bxn : 2, clct_sync_err : 1, clct0Invalid : 1,
        clct1Invalid : 1, clct1Busy : 1, parity_err_cfeb_ram : 5, parity_err_rpc : 1, parity_err_summary : 1,
        flag27 : 1;
    // 28
    unsigned alct0Valid : 1, alct0Quality : 2, alct0Amu : 1, alct0Key : 7, alct_pretrig_win : 4, flag28 : 1;
    unsigned alct1Valid : 1, alct1Quality : 2, alct1Amu : 1, alct1Key : 7, drift_delay : 2, bcb_read_enable : 1,
        hs_layer_trig : 1, flag29 : 1;
    unsigned hmt_nhits_bits_high : 5, alct_ecc_err : 2, cfeb_badbits_found : 5, cfeb_badbits_blocked : 1, alctCfg : 1,
        bx0_match : 1, flag30 : 1;
    unsigned MPC_Muon0_alct_key_wire : 7, MPC_Muon_clct_pattern_low : 4, MPC_Muon0_lct_quality : 3,
        MPC_Muon0_clct_QuarterStrip : 1, flag31 : 1;
    // 32
    unsigned MPC_Muon0_clct_key_halfstrip : 8, MPC_Muon0_clct_LR : 1, MPC_Muon0_clct_EighthStrip : 1,
        MPC_Muon_alct_bxn : 1, MPC_Muon0_clct_bx0 : 1, MPC_Muon0_clct_bend_low : 3, flag32 : 1;
    unsigned MPC_Muon1_alct_key_wire : 7, MPC_Muon_clct_pattern_bit5 : 1, MPC_Muon_HMT_high : 3,
        MPC_Muon1_lct_quality : 3, MPC_Muon1_clct_QuarterStrip : 1, flag33 : 1;
    unsigned MPC_Muon1_clct_key_halfstrip : 8, MPC_Muon1_clct_LR : 1, MPC_Muon1_clct_EighthStrip : 1,
        MPC_Muon_HMT_bit0 : 1, MPC_Muon1_clct_bx0 : 1, MPC_Muon1_clct_bend_low : 3, flag34 : 1;
    unsigned MPC_Muon0_lct_vpf : 1, MPC_Muon0_clct_bend_bit4 : 1, MPC_Muon1_lct_vpf : 1, MPC_Muon1_clct_bend_bit4 : 1,
        MPCDelay : 4, MPCAccept : 2, CFEBsEnabled : 5, flag35 : 1;
    // 36
    unsigned gem_enabled_fibers : 4, gem_zero_suppression : 1, fifo_tbins_gem : 5, fifo_pretrig_gem : 5, flag36 : 1;

    unsigned r_wr_buf_adr : 11, r_wr_buf_ready : 1, wr_buf_ready : 1, buf_q_full : 1, buf_q_empty : 1, flag37 : 1;
    unsigned r_buf_fence_dist : 11, buf_q_ovf_err : 1, buf_q_udf_err : 1, buf_q_adr_err : 1, buf_stalled : 1,
        flag38 : 1;
    unsigned buf_fence_cnt : 12, reverse_hs_csc : 1, reverse_hs_me1a : 1, reverse_hs_me1b : 1, flag39 : 1;
    // 40
    unsigned activeCFEBs_2 : 2, readCFEBs_2 : 2, cfeb_badbits_found_2 : 2, parity_err_cfeb_ram_2 : 2,
        CFEBsEnabled_2 : 2, buf_fence_cnt_is_peak : 1, gem_csc_bend_enable : 1, trig_source_vec : 2, tmb_trig_pulse : 1,
        flag40 : 1;
    unsigned tmb_allow_alct : 1, tmb_allow_clct : 1, tmb_allow_match : 1, tmb_allow_alct_ro : 1, tmb_allow_clct_ro : 1,
        tmb_allow_match_ro : 1, tmb_alct_only_ro : 1, tmb_clct_only_ro : 1, tmb_match_ro : 1, tmb_trig_keep : 1,
        tmb_non_trig_keep : 1, lyr_thresh_pretrig : 3, layer_trig_en : 1, flag41 : 1;
    unsigned e0bline : 16;
  } bits;
};

#endif
