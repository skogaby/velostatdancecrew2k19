#ifndef __SENSOR_ADJUSTMENT_PAGE_H__
#define __SENSOR_ADJUSTMENT_PAGE_H__

#include <EmbAJAX.h>

// display for the current polling rate
EmbAJAXMutableSpan pollrate("pollrate");

// displays for the current input values
EmbAJAXMutableSpan p1a1display("p1a1display");
EmbAJAXMutableSpan p1a2display("p1a2display");
EmbAJAXMutableSpan p1a3display("p1a3display");
EmbAJAXMutableSpan p1a4display("p1a4display");
EmbAJAXMutableSpan p1a5display("p1a5display");

// sliders to adjust the threshold values
EmbAJAXSlider p1a1slider("p1a1slider", 50, 1000, 50);
EmbAJAXSlider p1a2slider("p1a2slider", 50, 1000, 50);
EmbAJAXSlider p1a3slider("p1a3slider", 50, 1000, 50);
EmbAJAXSlider p1a4slider("p1a4slider", 50, 1000, 50);
EmbAJAXSlider p1a5slider("p1a5slider", 50, 1000, 50);

// construct the page itself
MAKE_EmbAJAXPage(page, "Dance Pad Configuration", "",
    new EmbAJAXStatic(""
        "<!DOCTYPE html>"
        "<html>"
        "<head>"
            "<meta name=\"viewport\" content\"width=device-width,initial-scale=\">"
            "<style>"
                "html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}"
                "table, th, td { border: 1px solid black; }"
                "th, td { padding: 5px }"
                "table { margin-left: auto; margin-right: auto }"
            "</style>"
        "</head>"
        "<body>"
           "<h1>Wireless Dance Pad Configuration</h1>"
            "<h2>Current polling rate: "),
    &pollrate,
    new EmbAJAXStatic(""
        "<b> Hz</b></h2>"
        "<h2>Current input values:</h2>"
        "<table>"
            "<tbody>"
                "<tr><td><p><strong>Arrow 1: </strong>"),
    &p1a1display,
    new EmbAJAXStatic("</p></td></tr><tr><td><p><strong>Arrow 2: </strong>"),
    &p1a2display,
    new EmbAJAXStatic("</p></td></tr><tr><td><p><strong>Arrow 3: </strong>"),
    &p1a3display,
    new EmbAJAXStatic("</p></td></tr><tr><td><p><strong>Arrow 4: </strong>"),
    &p1a4display,
    new EmbAJAXStatic("</p></td></tr><tr><td><p><strong>Arrow 5: </strong>"),
    &p1a5display,
    new EmbAJAXStatic("</p></td></tr>"
            "</tbody>"
        "</table>"
        "<h2>Adjust sensitivities:</h2>"
        "<table>"
            "<tbody>"
                "<tr><td><p><strong>Arrow 1: </strong>"),
    &p1a1slider,
    new EmbAJAXStatic("</p></td></tr><tr><td><p><strong>Arrow 2: </strong>"),
    &p1a2slider,
    new EmbAJAXStatic("</p></td></tr><tr><td><p><strong>Arrow 3: </strong>"),
    &p1a3slider,
    new EmbAJAXStatic("</p></td></tr><tr><td><p><strong>Arrow 4: </strong>"),
    &p1a4slider,
    new EmbAJAXStatic("</p></td></tr><tr><td><p><strong>Arrow 5: </strong>"),
    &p1a5slider,
    new EmbAJAXStatic("</p></td></tr>"
                "</tbody>"
            "</table>"
        "</body>"
        "</html>")
)

#endif
