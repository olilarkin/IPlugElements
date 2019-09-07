#include "IPlugElements.h"
#include "IPlug_include_in_plug_src.h"

IPlugElements::IPlugElements(const InstanceInfo& info)
: Plugin(info, MakeConfig(kNumParams, kNumPrograms))
{
  GetParam(kGain)->InitDouble("Gain", 0., 0., 100.0, 0.01, "%");
}

void IPlugElements::ProcessBlock(sample** inputs, sample** outputs, int nFrames)
{
  const double gain = GetParam(kGain)->Value() / 100.;
  const int nChans = NOutChansConnected();
  
  for (int s = 0; s < nFrames; s++) {
    for (int c = 0; c < nChans; c++) {
      outputs[c][s] = inputs[c][s] * gain;
    }
  }
}

//GUI

// Main window background color
auto constexpr bkd_color = rgba(35, 35, 37, 255);
auto background = box(bkd_color);

using slider_ptr = std::shared_ptr<basic_slider_base>;
slider_ptr hsliders[3];
slider_ptr vsliders[3];

using dial_ptr = std::shared_ptr<dial_base>;
dial_ptr dials[3];

template <bool is_vertical>
auto make_markers()
{
  auto track = basic_track<5, is_vertical>();
  return slider_labels<10>(
                           slider_marks<40>(track),         // Track with marks
                           0.8,                             // Label font size (relative size)
                           "0", "1", "2", "3", "4",         // Labels
                           "5", "6", "7", "8", "9", "10"
                           );
}

auto make_hslider(int index)
{
  hsliders[index] = share(slider(
                                 basic_thumb<25>(),
                                 make_markers<false>(),
                                 (index + 1) * 0.25
                                 ));
  return align_middle(xside_margin({ 20, 20 }, hold(hsliders[index])));
}

auto make_hsliders()
{
  return hmin_size(300,
                   vtile(
                         make_hslider(0),
                         make_hslider(1),
                         make_hslider(2)
                         )
                   );
}

auto make_vslider(int index)
{
  vsliders[index] = share(slider(
                                 basic_thumb<25>(),
                                 make_markers<true>(),
                                 (index + 1) * 0.25
                                 ));
  return align_center(yside_margin({ 20, 20 }, hold(vsliders[index])));
}

auto make_vsliders()
{
  return hmin_size(300,
                   htile(
                         make_vslider(0),
                         make_vslider(1),
                         make_vslider(2)
                         )
                   );
}

auto make_dial(int index)
{
  dials[index] = share(
                       dial(
                            radial_marks<20>(basic_knob<50>()),
                            (index + 1) * 0.25
                            )
                       );
  
  auto markers = radial_labels<15>(
                                   hold(dials[index]),
                                   0.7,                                   // Label font size (relative size)
                                   "0", "1", "2", "3", "4",               // Labels
                                   "5", "6", "7", "8", "9", "10"
                                   );
  
  return align_center_middle(markers);
}

auto make_dials()
{
  return xside_margin(20,
                      vtile(
                            make_dial(0),
                            make_dial(1),
                            make_dial(2)
                            )
                      );
}

auto make_controls()
{
  return
  margin({ 20, 10, 20, 10 },
         vmin_size(400,
                   htile(
                         margin({ 20, 20, 20, 20 }, pane("Vertical Sliders", make_vsliders(), 0.8f)),
                         margin({ 20, 20, 20, 20 }, pane("Horizontal Sliders", make_hsliders(), 0.8f)),
                         hstretch(0.5, margin({ 20, 20, 20, 20 }, pane("Knobs", make_dials(), 0.8f)))
                         )
                   )
         );
}

void* IPlugElements::OpenWindow(void* pParent)
{
  mView = std::make_unique<view>(static_cast<cycfi::elements::host_view_handle>(pParent));
  
  mView->content({
                  share(make_controls()),
                  share(background)
                }
                );
  
  vsliders[0]->on_change =
    [&](double val)
    {
      SendParameterValueFromUI(kGain, val);
      hsliders[0]->slider_base::value(val);
      dials[0]->dial_base::value(val);
      mView->refresh(*hsliders[0]);
      mView->refresh(*dials[0]);
    };
  
  return mView->host();
}

void IPlugElements::CloseWindow()
{
  mView = nullptr;
}
