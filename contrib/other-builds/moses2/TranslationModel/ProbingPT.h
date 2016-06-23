/*
 * ProbingPT.h
 *
 *  Created on: 3 Nov 2015
 *      Author: hieu
 */

#pragma once

#include <boost/iostreams/device/mapped_file.hpp>
#include <boost/thread/tss.hpp>
#include <boost/bimap.hpp>
#include <deque>
#include "PhraseTable.h"
#include "../Vector.h"
#include "../Phrase.h"
#include "../SCFG/ActiveChart.h"

namespace Moses2
{
class AlignmentInfo;
class QueryEngine;
class target_text;
class MemPool;
class System;
class RecycleData;

namespace SCFG
{
class TargetPhraseImpl;
}

class ProbingPT: public Moses2::PhraseTable
{
  //////////////////////////////////////
    class ActiveChartEntryProbing : public SCFG::ActiveChartEntry
    {
      typedef SCFG::ActiveChartEntry Parent;
    public:

      ActiveChartEntryProbing(MemPool &pool)
      :Parent(pool)
      ,m_key(0)
      {}

      ActiveChartEntryProbing(
          MemPool &pool,
          const ActiveChartEntry &prevEntry)
      :Parent(prevEntry)
      ,m_key(0)
      {}

      uint64_t GetKey() const
      { return m_key; }

      std::pair<bool, uint64_t> GetKey(const SCFG::Word &nextWord, const ProbingPT &pt) const;

      virtual void AddSymbolBindElement(
          const Range &range,
          const SCFG::Word &word,
          const Moses2::Hypotheses *hypos,
          const Moses2::PhraseTable &pt);

    protected:
      uint64_t m_key;
    };
    //////////////////////////////////////

public:
  ProbingPT(size_t startInd, const std::string &line);
  virtual ~ProbingPT();
  void Load(System &system);

  void Lookup(const Manager &mgr, InputPathsBase &inputPaths) const;

  void InitActiveChart(
      MemPool &pool,
      const SCFG::Manager &mgr,
      SCFG::InputPath &path) const;

  virtual void Lookup(MemPool &pool,
      const SCFG::Manager &mgr,
      size_t maxChartSpan,
      const SCFG::Stacks &stacks,
      SCFG::InputPath &path) const;

  void LookupUnary(MemPool &pool,
      const SCFG::Manager &mgr,
      const SCFG::Stacks &stacks,
      SCFG::InputPath &path) const;

  uint64_t GetUnk() const
  { return m_unkId; }

protected:
  std::vector<uint64_t> m_sourceVocab; // factor id -> pt id
  std::vector< std::pair<bool, const Factor*> > m_targetVocab; // pt id -> factor*
  std::vector<const AlignmentInfo*> m_aligns;

  uint64_t m_unkId;
  QueryEngine *m_engine;

  boost::iostreams::mapped_file_source file;
  const char *data;

  void CreateAlignmentMap(System &system, const std::string path);

  TargetPhrases *Lookup(const Manager &mgr, MemPool &pool,
      InputPath &inputPath) const;
  TargetPhrases *CreateTargetPhrase(MemPool &pool, const System &system,
      const Phrase<Moses2::Word> &sourcePhrase, uint64_t key) const;
  TargetPhrase<Moses2::Word> *CreateTargetPhrase(MemPool &pool, const System &system,
      const char *&offset) const;

  inline const std::pair<bool, const Factor*> *GetTargetFactor(uint32_t probingId) const
  {
    if (probingId >= m_targetVocab.size()) {
      return NULL;
    }
    return &m_targetVocab[probingId];
  }

  std::pair<bool, uint64_t> GetKey(const Phrase<Moses2::Word> &sourcePhrase) const;

  void GetSourceProbingIds(const Phrase<Moses2::Word> &sourcePhrase, bool &ok,
      uint64_t probingSource[]) const;

  uint64_t GetSourceProbingId(const Word &word) const;

  // caching
  typedef boost::unordered_map<uint64_t, TargetPhrases*> Cache;
  Cache m_cache;

  void CreateCache(System &system);

  void ReformatWord(System &system, std::string &wordStr, bool &isNT);

  // scfg
  void LookupNT(
      MemPool &pool,
      const SCFG::Manager &mgr,
      const Moses2::Range &subPhraseRange,
      const SCFG::InputPath &prevPath,
      const SCFG::Stacks &stacks,
      SCFG::InputPath &outPath) const;

  void LookupGivenWord(
      MemPool &pool,
      const SCFG::Manager &mgr,
      const SCFG::InputPath &prevPath,
      const SCFG::Word &wordSought,
      const Moses2::Hypotheses *hypos,
      const Moses2::Range &subPhraseRange,
      SCFG::InputPath &outPath) const;

  void LookupGivenNode(
      MemPool &pool,
      const SCFG::Manager &mgr,
      const ActiveChartEntryProbing &prevEntry,
      const SCFG::Word &wordSought,
      const Moses2::Hypotheses *hypos,
      const Moses2::Range &subPhraseRange,
      SCFG::InputPath &outPath) const;

  SCFG::TargetPhraseImpl *CreateTargetPhraseSCFG(
      MemPool &pool,
      const System &system,
      const char *&offset) const;

};

}

