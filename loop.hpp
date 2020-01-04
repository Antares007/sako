#pragma once

#include "pith.hpp"

constexpr inline auto loopB = [](auto pith) {
  return [](auto o, auto f) {
    bool active = true;
    while (active) {
      f(rays{o, [&](bool b) { active = b; }});
    }
  } ^ pith;
};
