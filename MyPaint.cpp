#include "MyPaint.h"
#include "MouseUtils.h"

#ifdef LOAD_SAVE_DIALOG
#   ifdef _WIN32
#      include <Windows.h>
#      include <Commdlg.h>
#   endif
#endif

MyPaint::MyPaint()
{
    sf::ContextSettings cs;
    cs.antialiasingLevel = 4;
    // Создание главного окна
    m_window = std::make_unique<sf::RenderWindow>(
        sf::VideoMode(windowSize.x, windowSize.y),
        "MyPaint", sf::Style::Close, cs);
    m_window->setFramerateLimit(fpsLimit);
    // Создание SFGUI (требуется перед любой работой с SFGUI)
    m_sfgui = std::make_unique<sfg::SFGUI>();

    setupTools();
    setupUI();
    fillPalette();

    m_canvasMain = std::make_unique<Canvas>();
    m_canvasTemp = std::make_unique<Canvas>();
    m_canvasMain->setSize(canvasSize);
    m_canvasMain->setPos(canvasPos);
    m_canvasMain->clear(sf::Color::White);
    m_canvasTemp->setSize(canvasSize);
    m_canvasTemp->setPos(canvasPos);
    m_canvasTemp->clear();
}

MyPaint::~MyPaint()
{
}

void MyPaint::show()
{
    while (m_window->isOpen())
    {
        sf::Event ev;

        while (m_window->pollEvent(ev))
        {
            for (auto &window : m_guiWindows)
                window.second->HandleEvent(ev);
            switch (ev.type)
            {
            case sf::Event::Closed:
                m_window->close();
                break;
            case sf::Event::KeyPressed:
                switch (ev.key.code)
                {
                case sf::Keyboard::Escape:
                    m_currentTool = nullptr;
                    m_guiToolsLabel->SetText("None");
                    break;
                default:
                    break;
                }
                break;
            default:
                break;
            }
        }

        // Обновление GUI
        for (auto &window : m_guiWindows)
            window.second->Update(0.f);
        // Очистка главного окна
        m_window->clear(colorCanvasBG);
        // Обработка инстурмента
        updateTool();
        // Отрисовка полотен
        m_canvasMain->draw(*m_window);
        m_canvasTemp->draw(*m_window);
        // Сброс OpenGL состояний (требуется перед отрисовкой SFGUI)
        m_window->resetGLStates();
        // Отрисовка GUI
        m_sfgui->Display(*m_window);
        // Обновление главного окна
        m_window->display();
    }
}

template<typename T>
inline void MyPaint::registerTool(const std::string &name)
{
    m_tools[name] = std::make_unique<T>();
}

void MyPaint::setupTools()
{
    registerTool<Tools::Line>(
          Tools::Names::Line);
    registerTool<Tools::Rectangle>(
          Tools::Names::Rectangle);
    registerTool<Tools::Ellipse>(
          Tools::Names::Ellipse);
    registerTool<Tools::Brush>(
          Tools::Names::Brush);
}

void MyPaint::setupUI()
{
    ///////////////////////
    //  Окно инструментов
    ///////////////////////

    m_guiWindows["Tools"] = std::move(sfg::Window::Create());
    m_guiWindows["Tools"]->SetTitle("Tools");
    m_guiWindows["Tools"]->SetPosition(sf::Vector2f(4, 4));
    m_guiWindows["Tools"]->SetStyle(
        m_guiWindows["Tools"]->GetStyle() ^ sfg::Window::RESIZE);

    m_guiToolsBox = sfg::Box::Create(
        sfg::Box::Orientation::VERTICAL);
    m_guiToolsBox->SetSpacing(3.f);
    for (auto &tool : m_tools)
        m_guiToolsButtons[tool.first] = std::move(
            sfg::Button::Create(tool.first));

    m_guiToolsButtons[Tools::Names::Line]
        ->GetSignal(sfg::Widget::OnLeftClick)
        .Connect(std::bind(&MyPaint::onClickButtonLine, this));

    m_guiToolsButtons[Tools::Names::Rectangle]
        ->GetSignal(sfg::Widget::OnLeftClick)
        .Connect(std::bind(&MyPaint::onClickButtonRectangle, this));

    m_guiToolsButtons[Tools::Names::Ellipse]
        ->GetSignal(sfg::Widget::OnLeftClick)
        .Connect(std::bind(&MyPaint::onClickButtonEllipse, this));

    m_guiToolsButtons[Tools::Names::Brush]
        ->GetSignal(sfg::Widget::OnLeftClick)
        .Connect(std::bind(&MyPaint::onClickButtonBrush, this));

    for (auto &button : m_guiToolsButtons)
        m_guiToolsBox->Pack(button.second);
    m_guiToolsLabel = sfg::Label::Create("None");
    m_guiToolsBox->Pack(m_guiToolsLabel);

#ifdef LOAD_SAVE_DIALOG
    m_guiToolsLoad = sfg::Button::Create("Load");
    m_guiToolsLoad->GetSignal(sfg::Button::OnLeftClick)
        .Connect(std::bind(&MyPaint::onLoadClicked, this));
    m_guiToolsSaveAs = sfg::Button::Create("Save As");
    m_guiToolsSaveAs->GetSignal(sfg::Button::OnLeftClick)
        .Connect(std::bind(&MyPaint::onSaveAsClicked, this));
    m_guiToolsBox->Pack(sfg::Separator::Create(
        sfg::Separator::Orientation::HORIZONTAL));
    m_guiToolsBox->Pack(m_guiToolsLoad);
    m_guiToolsBox->Pack(m_guiToolsSaveAs);
#endif LOAD_SAVE_DIALOG

    m_guiWindows["Tools"]->Add(m_guiToolsBox);

    ///////////////////////////
    //  Окно параметров кисти
    ///////////////////////////

    m_guiWindows["Param"] = std::move(sfg::Window::Create());
    m_guiWindows["Param"]->SetTitle("Pen Parameters");
    m_guiWindows["Param"]->SetPosition(sf::Vector2f(100, 4));
    m_guiWindows["Param"]->SetStyle(
        m_guiWindows["Param"]->GetStyle() ^ sfg::Window::RESIZE);

    m_guiParamLabelWidth = sfg::Label::Create("Width");

    m_guiParamAdjustmentWidth = sfg::Adjustment::Create(
        penWidthMinMax.x, penWidthMinMax.x, penWidthMinMax.y);
    m_guiParamAdjustmentWidth->GetSignal(sfg::Adjustment::OnChange)
        .Connect(std::bind(&MyPaint::onAdjustmentChange, this));

    m_guiParamEntryWidth = sfg::SpinButton::Create(m_guiParamAdjustmentWidth);
    m_guiParamEntryWidth->SetRequisition(sf::Vector2f(50.f, 0.f));
    m_guiParamEntryWidth->GetSignal(sfg::SpinButton::OnValueChanged)
        .Connect(std::bind(&MyPaint::onWidthValueChanged, this));    
    
    m_guiParamScaleWidth = sfg::Scale::Create(sfg::Range::Orientation::HORIZONTAL);
    m_guiParamScaleWidth->SetAdjustment(m_guiParamAdjustmentWidth);
    m_guiParamScaleWidth->SetRequisition(sf::Vector2f(100.f, 0.f));
    
    m_guiParamBox21 = sfg::Box::Create(sfg::Box::Orientation::HORIZONTAL);
    m_guiParamBox21->Pack(m_guiParamLabelWidth);
    m_guiParamBox21->Pack(m_guiParamEntryWidth);
    m_guiParamBox21->Pack(m_guiParamScaleWidth);
    sfg::Separator::Ptr tempSeparator = sfg::Separator::Create();
    tempSeparator->SetRequisition(sf::Vector2f(200, 0));
    m_guiParamBox21->Pack(tempSeparator);

    m_guiParamLabelR = sfg::Label::Create("R");
    m_guiParamLabelG = sfg::Label::Create("G");
    m_guiParamLabelB = sfg::Label::Create("B");
    m_guiParamLabelA = sfg::Label::Create("A");
    m_guiParamEntryR = sfg::SpinButton::Create(0.f, 255.f, 1.f);
    m_guiParamEntryG = sfg::SpinButton::Create(0.f, 255.f, 1.f);
    m_guiParamEntryB = sfg::SpinButton::Create(0.f, 255.f, 1.f);
    m_guiParamEntryA = sfg::SpinButton::Create(0.f, 255.f, 1.f);
    m_guiParamEntryR->SetValue(0.f);
    m_guiParamEntryG->SetValue(0.f);
    m_guiParamEntryB->SetValue(0.f);
    m_guiParamEntryA->SetValue(255.f);
    m_guiParamEntryR->SetRequisition(sf::Vector2f(50.f, 0.f));
    m_guiParamEntryG->SetRequisition(sf::Vector2f(50.f, 0.f));
    m_guiParamEntryB->SetRequisition(sf::Vector2f(50.f, 0.f));
    m_guiParamEntryA->SetRequisition(sf::Vector2f(50.f, 0.f));
    m_guiParamEntryR->GetSignal(sfg::SpinButton::OnValueChanged)
        .Connect(std::bind(&MyPaint::onRValueChanged, this));
    m_guiParamEntryG->GetSignal(sfg::SpinButton::OnValueChanged)
        .Connect(std::bind(&MyPaint::onGValueChanged, this));
    m_guiParamEntryB->GetSignal(sfg::SpinButton::OnValueChanged)
        .Connect(std::bind(&MyPaint::onBValueChanged, this));
    m_guiParamEntryA->GetSignal(sfg::SpinButton::OnValueChanged)
        .Connect(std::bind(&MyPaint::onAValueChanged, this));

    m_guiParamBox41 = sfg::Box::Create(sfg::Box::Orientation::HORIZONTAL);
    m_guiParamBox42 = sfg::Box::Create(sfg::Box::Orientation::HORIZONTAL);
    m_guiParamBox43 = sfg::Box::Create(sfg::Box::Orientation::HORIZONTAL);
    m_guiParamBox44 = sfg::Box::Create(sfg::Box::Orientation::HORIZONTAL);
    m_guiParamBox41->Pack(m_guiParamLabelR);
    m_guiParamBox41->Pack(m_guiParamEntryR);
    m_guiParamBox42->Pack(m_guiParamLabelG);
    m_guiParamBox42->Pack(m_guiParamEntryG);
    m_guiParamBox43->Pack(m_guiParamLabelB);
    m_guiParamBox43->Pack(m_guiParamEntryB);
    m_guiParamBox44->Pack(m_guiParamLabelA);
    m_guiParamBox44->Pack(m_guiParamEntryA);
    m_guiParamBox3 = sfg::Box::Create(sfg::Box::Orientation::VERTICAL);
    m_guiParamBox3->Pack(m_guiParamBox41);
    m_guiParamBox3->Pack(m_guiParamBox42);
    m_guiParamBox3->Pack(m_guiParamBox43);
    m_guiParamBox3->Pack(m_guiParamBox44);

    m_guiParamCanvasClrResult = sfg::Canvas::Create();
    m_guiParamCanvasClrPicker = sfg::Canvas::Create();
    m_guiParamCanvasClrResult->SetRequisition(
        sf::Vector2f(20.f, pickerSize.y));
    m_guiParamCanvasClrPicker->SetRequisition(
        sf::Vector2f(pickerSize.x * 3, pickerSize.y));
    m_guiParamCanvasClrResult->Clear(sf::Color::Black);
    m_guiParamCanvasClrPicker->GetSignal(sfg::Canvas::OnLeftClick)
        .Connect(std::bind(&MyPaint::onPickerClicked, this));
    
    m_guiParamBox22 = sfg::Box::Create(sfg::Box::Orientation::HORIZONTAL);
    m_guiParamBox22->SetSpacing(4);
    m_guiParamBox22->Pack(m_guiParamBox3);
    m_guiParamBox22->Pack(m_guiParamCanvasClrResult);
    m_guiParamBox22->Pack(m_guiParamCanvasClrPicker);

    m_guiParamBox1 = sfg::Box::Create(sfg::Box::Orientation::VERTICAL);
    m_guiParamBox1->SetSpacing(4);
    m_guiParamBox1->Pack(m_guiParamBox21);
    m_guiParamBox1->Pack(m_guiParamBox22);

    m_guiWindows["Param"]->Add(m_guiParamBox1);
}

void MyPaint::fillPalette()
{
    sf::Image img;
    sf::Texture tex;
    sf::Sprite spr;
    img.create(pickerSize.x, pickerSize.y);
    m_guiParamCanvasClrPicker->Bind();

    auto toClr = [this](int index)
    {
        return static_cast<int>(255.f
            * static_cast<float>(index)
            / static_cast<float>(pickerSize.x));
    };

    // yxx red-cyan
    for (int y = 0; y < pickerSize.y; ++y)
    {
        for (int x = 0; x < pickerSize.x; ++x)
        {
            img.setPixel(x, y, sf::Color(toClr(y), toClr(x), toClr(x)));
        }
    }
    tex.loadFromImage(img);
    spr.setTexture(tex);
    m_guiParamCanvasClrPicker->Draw(spr);

    // xyx green-magenta
    for (int y = 0; y < pickerSize.y; ++y)
    {
        for (int x = 0; x < pickerSize.x; ++x)
        {
            img.setPixel(x, y, sf::Color(toClr(x), toClr(y), toClr(x)));
        }
    }
    tex.loadFromImage(img);
    spr.setTexture(tex);
    spr.setPosition(pickerSize.x, 0);
    m_guiParamCanvasClrPicker->Draw(spr);

    // xxy blue-yellow
    for (int y = 0; y < pickerSize.y; ++y)
    {
        for (int x = 0; x < pickerSize.x; ++x)
        {
            img.setPixel(x, y, sf::Color(toClr(x), toClr(x), toClr(y)));
        }
    }
    tex.loadFromImage(img);
    spr.setTexture(tex);
    spr.setPosition(pickerSize.x << 1, 0);
    m_guiParamCanvasClrPicker->Draw(spr);

    m_guiParamCanvasClrPicker->Display();
    m_guiParamCanvasClrPicker->Unbind();
}

void MyPaint::updateTool()
{
    if (!m_window->hasFocus())
        return;
    if (m_currentTool == nullptr)
        return;
	MouseState mouse;
	mouse.update(*m_window);
    if (isMouseInRect(mouse, sf::IntRect(canvasPos, canvasSize)))
        mouse.setOnCanvas(true);
    for (auto &window : m_guiWindows)
    {
        if (isMouseInRect(mouse, window.second->GetAllocation()))
        {
            mouse.setOnCanvas(false);
            break;
        }
    }
    m_currentTool->process(*m_canvasMain, *m_canvasTemp, mouse, m_paintParam);
}

void MyPaint::onClickButtonLine()
{
    m_guiToolsLabel->SetText("Line");
    m_currentTool = m_tools["Line"].get();
}

void MyPaint::onClickButtonRectangle()
{
    m_guiToolsLabel->SetText("Rectangle");
    m_currentTool = m_tools["Rectangle"].get();
}

void MyPaint::onClickButtonEllipse()
{
    m_guiToolsLabel->SetText("Ellipse");
    m_currentTool = m_tools["Ellipse"].get();
}

void MyPaint::onClickButtonBrush()
{
    m_guiToolsLabel->SetText("Brush");
    m_currentTool = m_tools["Brush"].get();
}

void MyPaint::onAdjustmentChange()
{
    m_paintParam.width = m_guiParamAdjustmentWidth->GetValue();
    m_guiParamEntryWidth->SetText(std::to_string(m_paintParam.width));
}

void MyPaint::onWidthValueChanged()
{
    if (m_guiParamEntryWidth->GetText().toAnsiString().empty())
        m_paintParam.width = 1;
    else
        m_paintParam.width = std::stoi(
            m_guiParamEntryWidth->GetText().toAnsiString());
    if (m_paintParam.width < penWidthMinMax.x)
        m_paintParam.width = penWidthMinMax.x;
    else if (m_paintParam.width > penWidthMinMax.y)
        m_paintParam.width = penWidthMinMax.y;
    m_guiParamAdjustmentWidth->SetValue(m_paintParam.width);
}

void MyPaint::onRValueChanged()
{
    m_paintParam.color = sf::Color(
        m_guiParamEntryR->GetValue(),
        m_paintParam.color.g,
        m_paintParam.color.b,
        m_paintParam.color.a);
    m_guiParamCanvasClrResult->Clear(m_paintParam.color);
}

void MyPaint::onGValueChanged()
{
    m_paintParam.color = sf::Color(
        m_paintParam.color.r,
        m_guiParamEntryG->GetValue(),
        m_paintParam.color.b,
        m_paintParam.color.a);
    m_guiParamCanvasClrResult->Clear(m_paintParam.color);
}

void MyPaint::onBValueChanged()
{
    m_paintParam.color = sf::Color(
        m_paintParam.color.r,
        m_paintParam.color.g,
        m_guiParamEntryB->GetValue(),
        m_paintParam.color.a);
    m_guiParamCanvasClrResult->Clear(m_paintParam.color);
}

void MyPaint::onAValueChanged()
{
    m_paintParam.color = sf::Color(
        m_paintParam.color.r,
        m_paintParam.color.g,
        m_paintParam.color.b,
        m_guiParamEntryA->GetValue());
    m_guiParamCanvasClrResult->Clear(m_paintParam.color);
}

void MyPaint::onPickerClicked()
{
    sf::Vector2f pickerPos = m_guiParamCanvasClrPicker
        ->GetAbsolutePosition();
    sf::Vector2i mousePos = sf::Mouse::getPosition(*m_window);
    mousePos.x = mousePos.x - pickerPos.x;
    mousePos.y = mousePos.y - pickerPos.y;
    
    auto toClr = [this](int index)
    {
        return static_cast<int>(255.f
            * static_cast<float>(index)
            / static_cast<float>(pickerSize.x));
    };
    // yxx red-cyan
    if (mousePos.x < pickerSize.x)
    {
        m_paintParam.color.r = toClr(mousePos.y);
        m_paintParam.color.g = toClr(mousePos.x);
        m_paintParam.color.b = toClr(mousePos.x);
    }
    // xyx green-magenta
    else if (mousePos.x < pickerSize.x << 1)
    {
        mousePos.x -= pickerSize.x;
        m_paintParam.color.r = toClr(mousePos.x);
        m_paintParam.color.g = toClr(mousePos.y);
        m_paintParam.color.b = toClr(mousePos.x);
    }
    // xxy blue-yellow
    else
    {
        mousePos.x -= pickerSize.x << 1;
        m_paintParam.color.r = toClr(mousePos.x);
        m_paintParam.color.g = toClr(mousePos.x);
        m_paintParam.color.b = toClr(mousePos.y);
    }
    m_guiParamEntryR->SetValue(m_paintParam.color.r);
    m_guiParamEntryG->SetValue(m_paintParam.color.g);
    m_guiParamEntryB->SetValue(m_paintParam.color.b);
    m_guiParamCanvasClrResult->Clear(m_paintParam.color);
}

#ifdef LOAD_SAVE_DIALOG
void MyPaint::onLoadClicked()
{
#ifdef _WIN32
    OPENFILENAMEA ofn;      // common dialog box structure
    char szFile[260];       // buffer for file name

    // Initialize OPENFILENAME
    memset(&ofn, 0, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = nullptr;
    ofn.lpstrFile = szFile;
    // Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
    // use the contents of szFile to initialize itself.
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = "Image (bmp, png, tga, "
        "jpg, gif, psd, hdr, pic)\0*.bmp;*.png;"
        "*.tga;*.jpg;*.gif;*.psd;*.hdr;*.pic\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    // Display the Open dialog box
    if (GetOpenFileNameA(&ofn) == true)
    {
        sf::Image img;
        if (!img.loadFromFile(ofn.lpstrFile))
        {
            MessageBoxA(nullptr, "Error in load image", "Error", MB_ICONERROR);
            return;
        }
        if (img.getSize().x > imageMaxSize.x
            || img.getSize().y > imageMaxSize.y)
        {
            MessageBoxA(nullptr, std::string(
                "Image size must be less than "
                + std::to_string(imageMaxSize.x) + "x"
                + std::to_string(imageMaxSize.y)).c_str(),
                "Error", MB_ICONERROR);
            return;
        }
        img.flipVertically();
        sf::Texture tex;
        tex.loadFromImage(img);
        sf::Sprite spr(tex);
        canvasSize.x = img.getSize().x;
        canvasSize.y = img.getSize().y;
        canvasPos.x = (m_window->getSize().x >> 1) - (canvasSize.x >> 1);
        canvasPos.y = (m_window->getSize().y >> 1) - (canvasSize.y >> 1);

        m_canvasMain->setSize(canvasSize);
        m_canvasTemp->setSize(canvasSize);
        m_canvasMain->setPos(canvasPos);
        m_canvasTemp->setPos(canvasPos);
        m_canvasMain->ptr()->draw(spr);
    }
#else
#   error No implementation of Open File dialog 
#endif
}
#endif // LOAD_SAVE_DIALOG

#ifdef LOAD_SAVE_DIALOG
void MyPaint::onSaveAsClicked()
{
#ifdef _WIN32
    OPENFILENAMEA ofn;      // common dialog box structure
    char szFile[260];       // buffer for file name

    // Initialize OPENFILENAME
    memset(&ofn, 0, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = nullptr;
    ofn.lpstrFile = szFile;
    // Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
    // use the contents of szFile to initialize itself.
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = "Image (bmp, png, tga, jpg)\0"
        "*.bmp;*.png;*.tga;*.jpg\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST;// | OFN_FILEMUSTEXIST;

    // Display the Save dialog box
    if (GetSaveFileNameA(&ofn) == true)
    {
        sf::Image img = m_canvasMain->ptr()->getTexture().copyToImage();
        if (!img.saveToFile(ofn.lpstrFile))
        {
            MessageBoxA(nullptr, "Wrong image format", "Error", MB_ICONERROR);
        }
    }
#else
#   error No implementation of Save File dialog 
#endif
}
#endif // LOAD_SAVE_DIALOG
