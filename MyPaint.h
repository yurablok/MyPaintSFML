#pragma once

#ifdef _MSC_VER
#   // Используемые библиотеки:
#   // - SFML 2.3.2
#   // - SFGUI 0.3.0
#   pragma comment(lib, "opengl32.lib")
#   pragma comment(lib, "winmm.lib")
#   pragma comment(lib, "freetype.lib")
#   pragma comment(lib, "jpeg.lib")
#   ifdef _DEBUG
#       pragma comment(lib, "sfml-main-d.lib")
#       pragma comment(lib, "sfml-window-s-d.lib")
#       pragma comment(lib, "sfml-graphics-s-d.lib")
#       pragma comment(lib, "sfml-system-s-d.lib")
#       pragma comment(lib, "sfgui-s-d.lib")
#   else
#       pragma comment(lib, "sfml-main.lib")
#       pragma comment(lib, "sfml-window-s.lib")
#       pragma comment(lib, "sfml-graphics-s.lib")
#       pragma comment(lib, "sfml-system-s.lib")
#       pragma comment(lib, "sfgui-s.lib")
#   endif // _DEBUG
#   
#   // Оконное (не консоль + окно) приложение
#   pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#   //pragma comment(linker, "/SUBSYSTEM:console")
#endif // _MSC_VER

#include <string>
#include <memory>
#include <unordered_map>

#define SFML_STATIC
#define SFGUI_STATIC
#include <SFGUI/SFGUI.hpp>
#include <SFGUI/Widgets.hpp>
#include <SFML/Graphics.hpp>

#include "Canvas.h"
#include "Line.h"
#include "Rectangle.h"
#include "Ellipse.h"
#include "Brush.h"

namespace Tools
{
    namespace Names
    {
        const std::string Line("Line");
        const std::string Rectangle("Rectangle");
        const std::string Ellipse("Ellipse");
        const std::string Brush("Brush");
    };
};

// Включает функции загрузки и сохранения
#define LOAD_SAVE_DIALOG

class MyPaint
{
    const sf::Color colorCanvasBG { 200, 212, 226 };
    const sf::Vector2i windowSize { 816, 616 };
    const sf::Vector2i imageMaxSize { 800, 600 };
    const int fpsLimit = 60;
    const sf::Vector2i penWidthMinMax { 1, 400 };
    const sf::Vector2i pickerSize { 104, 104 };

public:
    MyPaint();
    ~MyPaint();

    void show();

private:
    // Регистрация инструментов
    void setupTools();
    // Настройка интерфейса
    // Должно быть вызвано не раньше setupTools!
    void setupUI();
    // Инициализация цветов палитры (заливка)
    // Должно быть вызвано не раньше setupUI!
    void fillPalette();
    // Добавление (регистрация) инструмента
    template <typename T>
    void registerTool(const std::string &name);
    // Обработка выбранного инструмента
    void updateTool();

    void onClickButtonLine();
    void onClickButtonRectangle();
    void onClickButtonEllipse();
    void onClickButtonBrush();
    void onAdjustmentChange();
    void onWidthValueChanged();
    void onRValueChanged();
    void onGValueChanged();
    void onBValueChanged();
    void onAValueChanged();
    void onPickerClicked();
#ifdef LOAD_SAVE_DIALOG
    void onLoadClicked();
    void onSaveAsClicked();
#endif

    sf::Vector2i canvasSize = imageMaxSize;
    sf::Vector2i canvasPos  { 8, 8 };

    std::unique_ptr<sf::RenderWindow> m_window;
    std::unique_ptr<sfg::SFGUI>       m_sfgui;
    std::unordered_map<std::string,
        std::unique_ptr<Tool>>        m_tools;
    Tool                             *m_currentTool = nullptr;

    std::unordered_map<std::string,
        std::shared_ptr<sfg::Window>> m_guiWindows;

    std::shared_ptr<sfg::Box>         m_guiToolsBox;
    std::unordered_map<std::string,
        std::shared_ptr<sfg::Button>> m_guiToolsButtons;
    std::shared_ptr<sfg::Label>       m_guiToolsLabel;
    std::shared_ptr<sfg::Button>      m_guiToolsLoad;
    std::shared_ptr<sfg::Button>      m_guiToolsSaveAs;

    std::shared_ptr<sfg::Box>         m_guiParamBox1;
    std::shared_ptr<sfg::Box>         m_guiParamBox21;
    std::shared_ptr<sfg::Label>       m_guiParamLabelWidth;
    std::shared_ptr<sfg::SpinButton>  m_guiParamEntryWidth;
    std::shared_ptr<sfg::Adjustment>  m_guiParamAdjustmentWidth;
    std::shared_ptr<sfg::Scale>       m_guiParamScaleWidth;
    std::shared_ptr<sfg::Box>         m_guiParamBox22;
    std::shared_ptr<sfg::Canvas>      m_guiParamCanvasClrResult;
    std::shared_ptr<sfg::Canvas>      m_guiParamCanvasClrPicker;
    std::shared_ptr<sfg::Box>         m_guiParamBox3;
    std::shared_ptr<sfg::Box>         m_guiParamBox41;
    std::shared_ptr<sfg::Label>       m_guiParamLabelR;
    std::shared_ptr<sfg::SpinButton>  m_guiParamEntryR;
    std::shared_ptr<sfg::Box>         m_guiParamBox42;
    std::shared_ptr<sfg::Label>       m_guiParamLabelG;
    std::shared_ptr<sfg::SpinButton>  m_guiParamEntryG;
    std::shared_ptr<sfg::Box>         m_guiParamBox43;
    std::shared_ptr<sfg::Label>       m_guiParamLabelB;
    std::shared_ptr<sfg::SpinButton>  m_guiParamEntryB;
    std::shared_ptr<sfg::Box>         m_guiParamBox44;
    std::shared_ptr<sfg::Label>       m_guiParamLabelA;
    std::shared_ptr<sfg::SpinButton>  m_guiParamEntryA;

    std::unique_ptr<Canvas>           m_canvasMain;
    std::unique_ptr<Canvas>           m_canvasTemp;

    PaintParameters                   m_paintParam;
};
