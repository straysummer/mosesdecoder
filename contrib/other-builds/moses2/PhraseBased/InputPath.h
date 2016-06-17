/*
 * InputPath.h
 *
 *  Created on: 23 Oct 2015
 *      Author: hieu
 */

#pragma once

#include <iostream>
#include <vector>
#include "../InputPathBase.h"

namespace Moses2
{
class TargetPhrases;

class InputPath: public InputPathBase
{
public:
  const TargetPhrases** targetPhrases;
  SubPhrase<Moses2::Word> subPhrase;

  InputPath(MemPool &pool, const SubPhrase<Moses2::Word> &subPhrase, const Range &range,
      size_t numPt, const InputPath *prefixPath);
  virtual ~InputPath();

  void AddTargetPhrases(const PhraseTable &pt, const TargetPhrases *tps);
  const TargetPhrases *GetTargetPhrases(const PhraseTable &pt) const;

  inline bool IsUsed() const
  {
    return m_isUsed;
  }

  std::ostream &Debug(std::ostream &out, const System &system) const;

protected:
  bool m_isUsed;
};

}

