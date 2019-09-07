#pragma once

#include "IPlug_include_in_plug_hdr.h"
#include "elements.hpp"

const int kNumPrograms = 1;

enum EParams
{
  kGain = 0,
  kNumParams
};

using namespace iplug;
using namespace cycfi::elements;

class IPlugElements : public Plugin
{
public:
  IPlugElements(const InstanceInfo& info);
  
  //IEditorDelegate
  void* OpenWindow(void* pParent) override;
  void CloseWindow() override;
  
  void ProcessBlock(sample** inputs, sample** outputs, int nFrames) override;
  
private:
  std::unique_ptr<view> mView;
};
