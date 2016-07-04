#include "Brush.h"
using namespace Tools;

Brush::Brush()
{
}

Brush::~Brush()
{
}

void Brush::process(Canvas &canvasMain, Canvas &canvasTemp,
    const MouseState &mouse, const PaintParameters &param)
{
    if (!m_leftClicked && mouse.isLeftPressed())
    {
        if (mouse.isOnCanvas())
        {
            m_lineStart = mouse.getPos();
            m_lineStart -= canvasMain.getPos();
            m_lineEnd = m_lineStart;
            m_startOnCanvas = true;
        }
        else
        {
            m_startOnCanvas = false;
        }
    }
    else if (m_leftClicked && mouse.isLeftPressed() && m_startOnCanvas)
    {
        m_lineStart = m_lineEnd;
        m_lineEnd = mouse.getPos();
        m_lineEnd -= canvasMain.getPos();
        Line::drawLine(*canvasMain.ptr(), m_lineStart, m_lineEnd, param);
    }
    m_leftClicked = mouse.isLeftPressed();
}
