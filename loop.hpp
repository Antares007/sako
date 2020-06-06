#pragma once

#include "purry.hpp"

constexpr inline auto loopB = [](auto o, auto pith) {
  bool active = true;
  while (active) {
    pith(o::rays{o, [&](bool b) { active = b; }});
  }
};
