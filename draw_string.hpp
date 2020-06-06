#pragma once

#include "purry.hpp"
#include <cstdint>
#include <string_view>

constexpr inline auto draw_string = [](auto o, auto pith) {
  static const char *fontData =
      "?Q`0001oOch0o01o@F40o0<AGD4090LAGD<090@A7ch0?00O7Q`0600>00000000"
      "O000000nOT0063Qo4d8>?7a14Gno94AA4gno94AaOT0>o3`oO400o7QN00000400"
      "Of80001oOg<7O7moBGT7O7lABET024@aBEd714AiOdl717a_=TH013Q>00000000"
      "720D000V?V5oB3Q_HdUoE7a9@DdDE4A9@DmoE4A;Hg]oM4Aj8S4D84@`00000000"
      "OaPT1000Oa`^13P1@AI[?g`1@A=[OdAoHgljA4Ao?WlBA7l1710007l100000000"
      "ObM6000oOfMV?3QoBDD`O7a0BDDH@5A0BDD<@5A0BGeVO5ao@CQR?5Po00000000"
      "Oc``000?Ogij70PO2D]??0Ph2DUM@7i`2DTg@7lh2GUj?0TO0C1870T?00000000"
      "70<4001o?P<7?1QoHg43O;`h@GT0@:@LB@d0>:@hN@L0@?aoN@<0O7ao0000?000"
      "OcH0001SOglLA7mg24TnK7ln24US>0PL24U140PnOgl0>7QgOcH0K71S0000A000"
      "00H00000@Dm1S007@DUSg00?OdTnH7YhOfTL<7Yh@Cl0700?@Ah0300700000000"
      "<008001QL00ZA41a@6HnI<1i@FHLM81M@@0LG81?O`0nC?Y7?`0ZA7Y300080000"
      "O`082000Oh0827mo6>Hn?Wmo?6HnMb11MP08@C11H`08@FP0@@0004@000000000"
      "00P00001Oab00003OcKP0006@6=PMgl<@440MglH@000000`@000001P00000000"
      "Ob@8@@00Ob@8@Ga13R@8Mga172@8?PAo3R@827QoOb@820@0O`0007`0000007P0"
      "O`000P08Od400g`<3V=P0G`673IP0`@3>1`00P@6O`P00g`<O`000GP800000000"
      "?P9PL020O`<`N3R0@E4HC7b0@ET<ATB0@@l6C4B0O`H3N7b0?P01L3R000000020";
  int px = 0, py = 0;
  uint8_t fontSprite[128 * 48] = {};
  for (int b = 0; b < 1024; b += 4) {
    uint32_t sym1 = (uint8_t)fontData[b + 0] - 48;
    uint32_t sym2 = (uint8_t)fontData[b + 1] - 48;
    uint32_t sym3 = (uint8_t)fontData[b + 2] - 48;
    uint32_t sym4 = (uint8_t)fontData[b + 3] - 48;
    uint32_t r = sym1 << 18 | sym2 << 12 | sym3 << 6 | sym4;
    for (int i = 0; i < 24; i++) {
      fontSprite[px + py * 128] = r & (1 << i) ? 255 : 0;
      if (++py == 48) {
        px++;
        py = 0;
      }
    }
  }
  o([&](auto o) {
    pith(o::rays{o, [&](int32_t x, int32_t y, const char *sText, uint32_t col = -1,
                     uint32_t scale = 1) {
                int32_t sx = 0;
                int32_t sy = 0;
                for (auto c : std::string_view(sText)) {
                  if (c == '\n') {
                    sx = 0;
                    sy += 8 * scale;
                  } else {
                    int32_t ox = (c - 32) % 16;
                    int32_t oy = (c - 32) / 16;
                    if (scale > 1) {
                      for (uint32_t i = 0; i < 8; i++)
                        for (uint32_t j = 0; j < 8; j++)
                          if (fontSprite[i + ox * 8 + (j + oy * 8) * 128])
                            for (uint32_t is = 0; is < scale; is++)
                              for (uint32_t js = 0; js < scale; js++)
                                o(x + sx + (i * scale) + is,
                                  y + sy + (j * scale) + js, col);
                    } else {
                      for (uint32_t i = 0; i < 8; i++)
                        for (uint32_t j = 0; j < 8; j++)
                          if (fontSprite[i + ox * 8 + (j + oy * 8) * 128])
                            o(x + sx + i, y + sy + j, col);
                    }
                    sx += 8 * scale;
                  }
                }
              }});
  });
};
