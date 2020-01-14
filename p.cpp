#include "parsec.hpp"
#include "xml.hpp"
#include <iostream>

using namespace parsec;
using namespace parsec::xml;

int main() {
  //
  const char *xmltext =
      R"xml(<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<sst xmlns="http://schemas.openxmlformats.org/spreadsheetml/2006/main" count="145" uniqueCount="67"><si><t>Azkot Hotel</t></si><si><t>Assam hotel</t></si><si><t>Astoria Tbilisi</t></si><si><t>Vardzia Resort</t></si><si><t>Silk road Hotel</t></si><si><t>laertoni</t></si><si><t>raodenoba</t></si><si><t>Mr Paul Croft</t></si><si><t>mail</t></si><si><t>paulcroft@madasafish.com</t></si><si><t>agent</t></si><si><t>tbilisi</t></si><si><t>bako</t></si><si><t>sheki</t></si><si><t>vardzia</t></si><si><t>erevan</t></si><si><t>Azerbaijan, Georgia and Armenia 12 days tour</t></si><si><t>Start Date</t></si><si><t>End Date</t></si><si><t>Total Travelers</t></si><si><t>Quantity</t></si><si><t>bbb</t></si><si><t>dddd</t></si><si><t>Date</t></si><si><t>Location</t></si><si><t>axalcixe</t></si><si><t>erevani</t></si><si><t>departure</t></si><si><t>Hotels</t></si><si><t>In</t></si><si><t>Out</t></si><si><t>Twin</t></si><si><t>Double</t></si><si><t>Single</t></si><si><t>Dextra</t></si><si><t>aaaa</t></si><si><t>Baku</t></si><si><t>Sheki</t></si><si><t>Tbilisi</t></si><si><t>Vardzia</t></si><si><t>Yerevan</t></si><si><t>Name</t></si><si><t>sheki driver and guide</t></si><si><t>vardzia driver and guide</t></si><si><t>Contact</t></si><si><t>Country</t></si><si><t>Azerbaijan</t></si><si><t>Georgia</t></si><si><t>Armenia</t></si><si><t>Confirmation mail</t></si><si><t>DateStr</t></si><si><t>Day</t></si><si><t>Triple</t></si><si><t>Driver/Name/Contact</t></si><si><t>Extra</t></si><si><t>Dates &amp; Time</t></si><si><t>Flight #</t></si><si><t>From</t></si><si><t>To</t></si><si><t>Driver</t></si><si><t>Hotel</t></si><si><t>Gude</t></si><si><t>Restaurants</t></si><si><t>Entrance Fee</t></si><si><t>N</t></si><si><t>ExHotels</t></si><si><t>ExTransfer</t></si></sst>)xml";
  (till{"-->"}, "---->")(::rays{
      [](error_ray *, int err) { std::cout << "error: " << err << '\n'; },
      [&](size_t len) {
        std::cout << "len: " << len << '|' << xmltext[len] << '|'
                  << xmltext + len;
      }});
  return 9;
}
