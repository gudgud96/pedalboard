/*
 * pedalboard
 * Copyright 2021 Spotify AB
 *
 * Licensed under the GNU Public License, Version 3.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    https://www.gnu.org/licenses/gpl-3.0.html
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include "../RubberbandPlugin.h"

namespace Pedalboard {
/*
Modifies pitch of an audio without affecting duration
*/
class PitchShift : public RubberbandPlugin

{
private:
  double _semitones = 0.0;

  // Allow pitch shifting by up to 6 octaves up or down:
  static constexpr int MIN_SEMITONES = -6 * 12;
  static constexpr int MAX_SEMITONES = 6 * 12;

  double getScaleFactor() { return pow(2, (getSemitones() / 12)); }

public:
  void setSemitones(double semitones) {
    if (semitones < MIN_SEMITONES || semitones > MAX_SEMITONES) {
      throw std::range_error("Semitones of pitch must be a value between " +
                             std::to_string(MIN_SEMITONES) + "st and " +
                             std::to_string(MAX_SEMITONES) + "st.");
    }

    _semitones = semitones;

    if (rbPtr)
      rbPtr->setPitchScale(getScaleFactor());
  }

  double getSemitones() { return _semitones; }

  void prepare(const juce::dsp::ProcessSpec &spec) override final {
    RubberbandPlugin::prepare(spec);
    rbPtr->setPitchScale(getScaleFactor());
  }
};

inline void init_pitch_shift(py::module &m) {
  py::class_<PitchShift, Plugin>(
      m, "PitchShift", "Shift pitch without affecting audio duration.")
      .def(py::init([](double scale) {
             auto plugin = new PitchShift();
             plugin->setSemitones(scale);
             return plugin;
           }),
           py::arg("semitones") = 0.0)
      .def_property("semitones", &PitchShift::getSemitones,
                    &PitchShift::setSemitones);
}
}; // namespace Pedalboard
