#pragma once
#include "Canvas.h"
#include "MouseUtils.h"
#include "PaintParameters.h"

// Базовый класс для всех инструментов
class Tool
{
public:
    Tool() {};
    virtual ~Tool() {};

    virtual void process(Canvas &canvasMain, Canvas &canvasTemp,
        const MouseState &mouse, const PaintParameters &param) = 0;
};
