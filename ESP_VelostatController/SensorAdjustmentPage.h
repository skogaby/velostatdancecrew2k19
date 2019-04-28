#ifndef __SENSOR_ADJUSTMENT_PAGE_H__
#define __SENSOR_ADJUSTMENT_PAGE_H__

// uncomment this for 5-panel (PIU) mode
// #define __5_PANEL__ 

#include <EmbAJAX.h>

// display for the current polling rates
EmbAJAXMutableSpan p1pollrate("p1pollrate");
EmbAJAXMutableSpan p2pollrate("p2pollrate");

// displays for the current input values
EmbAJAXMutableSpan p1a1display("p1a1display");
EmbAJAXMutableSpan p1a2display("p1a2display");
EmbAJAXMutableSpan p1a3display("p1a3display");
EmbAJAXMutableSpan p1a4display("p1a4display");
EmbAJAXMutableSpan p1a5display("p1a5display");
EmbAJAXMutableSpan p2a1display("p2a1display");
EmbAJAXMutableSpan p2a2display("p2a2display");
EmbAJAXMutableSpan p2a3display("p2a3display");
EmbAJAXMutableSpan p2a4display("p2a4display");
EmbAJAXMutableSpan p2a5display("p2a5display");

// sliders to adjust the threshold values
EmbAJAXSlider p1a1slider("p1a1slider", 500, 1000, 500);
EmbAJAXSlider p1a2slider("p1a2slider", 500, 1000, 500);
EmbAJAXSlider p1a3slider("p1a3slider", 500, 1000, 500);
EmbAJAXSlider p1a4slider("p1a4slider", 500, 1000, 500);
EmbAJAXSlider p1a5slider("p1a5slider", 500, 1000, 500);
EmbAJAXSlider p2a1slider("p2a1slider", 500, 1000, 500);
EmbAJAXSlider p2a2slider("p2a2slider", 500, 1000, 500);
EmbAJAXSlider p2a3slider("p2a3slider", 500, 1000, 500);
EmbAJAXSlider p2a4slider("p2a4slider", 500, 1000, 500);
EmbAJAXSlider p2a5slider("p2a5slider", 500, 1000, 500);

#ifdef __5_PANEL__
  #define PAGE_TITLE "Pump It Up - Pad Configuration"
  #define ARROW_1 "Up-Left"
  #define ARROW_2 "Up-Right"
  #define ARROW_3 "Down-Left"
  #define ARROW_4 "Down-Right"
  #define ARROW_5 "Center"
#else
  #define PAGE_TITLE "Dance Dance Revolution - Pad Configuration"
  #define ARROW_1 "Up"
  #define ARROW_2 "Right"
  #define ARROW_3 "Down"
  #define ARROW_4 "Left"
#endif

// construct the page itself
MAKE_EmbAJAXPage(page, PAGE_TITLE, "",
    new EmbAJAXStatic(""
        "<!DOCTYPE html>"
        "<html>"
        "<head>"
            "<meta name=\"viewport\" content\"width=device-width\">"
            "<style>"
                "html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}"
                "table, th, td { border: 1px solid black; }"
                "th, td { padding: 5px }"
                "table { margin-left: auto; margin-right: auto }"
            "</style>"
        "</head>"
        "<body>"
           "<h1>"
           PAGE_TITLE
           "</h1>"
            "<h2>Player 1 polling rate: "),
    &p1pollrate,
    new EmbAJAXStatic(""
        "<b> Hz</b></h2>"
        "<h2>Player 2 polling rate: "),
    &p2pollrate,
    new EmbAJAXStatic(""
        "<b> Hz</b></h2>"
        "<h2>Current input values:</h2>"
        "<table>"
            "<tbody>"
                "<tr><td><b>"
                ARROW_1              
                ": </b>"),
    &p1a1display,
    new EmbAJAXStatic("</td><td><b>"
    ARROW_1
    ": </b>"),
    &p2a1display,
    new EmbAJAXStatic("</td></tr><tr><td><b>"
    ARROW_2
    ": </b>"),
    &p1a2display,
    new EmbAJAXStatic("</td><td><b>"
    ARROW_2
    ": </b>"),
    &p2a2display,
    new EmbAJAXStatic("</td></tr><tr><td><b>"
    ARROW_3
    ": </b>"),
    &p1a3display,
    new EmbAJAXStatic("</td><td><b>"
    ARROW_3
    ": </b>"),
    &p2a3display,
    new EmbAJAXStatic("</td></tr><tr><td><b>"
    ARROW_4
    ": </b>"),
    &p1a4display,
    new EmbAJAXStatic("</td><td><b>"
    ARROW_4
    ": </b>"),
    &p2a4display,
    new EmbAJAXStatic("</td></tr>"),
#ifdef __5_PANEL__
    new EmbAJAXStatic("<tr><td><b>"
    ARROW_5
    ": </b>"),
    &p1a5display,
    new EmbAJAXStatic("</td><td><b>"
    ARROW_5
    ": </b>"),
    &p2a5display,
    new EmbAJAXStatic("</td></tr>"),
#endif
    new EmbAJAXStatic("</tbody>"
        "</table>"
        "<h2>Adjust sensitivities:</h2>"
        "<table>"
            "<tbody>"
                "<tr><td><b>"
                ARROW_1
                ": </b>"),
    &p1a1slider,
    new EmbAJAXStatic("</td><td><b>"
    ARROW_1
    ": </b>"),
    &p2a1slider,
    new EmbAJAXStatic("</td></tr><tr><td><b>"
    ARROW_2
    ": </b>"),
    &p1a2slider,
    new EmbAJAXStatic("</td><td><b>"
    ARROW_2
    ": </b>"),
    &p2a2slider,
    new EmbAJAXStatic("</td></tr><tr><td><b>"
    ARROW_3
    ": </b>"),
    &p1a3slider,
    new EmbAJAXStatic("</td><td><b>"
    ARROW_3
    ": </b>"),
    &p2a3slider,
    new EmbAJAXStatic("</td></tr><tr><td><b>"
    ARROW_4
    ": </b>"),
    &p1a4slider,
    new EmbAJAXStatic("</td><td><b>"
    ARROW_4
    ": </b>"),
    &p2a4slider,
    new EmbAJAXStatic("</td></tr>"),
#ifdef __5_PANEL__
    new EmbAJAXStatic("<tr><td><b>"
    ARROW_5
    ": </b>"),
    &p1a5slider,
    new EmbAJAXStatic("</td><td><b>"
    ARROW_5
    ": </b>"),
    &p2a5slider,
    new EmbAJAXStatic("</td></tr>"),
#endif
    new EmbAJAXStatic("</tbody>"
        "</table>"
        "</body>"
        "</html>")
)

#endif
