#include "Panel.h"
#include "Mouse.h"

Color::Color()
{
    r = 1.f; g = 1.f; b = 1.f; 
}

//ColorName{WHITE,BLACK,RED,GREEN,BLUE,DARK_GRAY,GRAY,LIGHT_GRAY};
Color::Color(ColorName color)
{
    r = 1.f; g = 1.f; b = 1.f; 
    switch (color){
    case WHITE:
        r = 1.f; g = 1.f; b = 1.f; break;
    case BLACK:
        r = 0.f; g = 0.f; b = 0.f; break;
    case RED:
        r = 1.f; g = 0.f; b = 0.f; break;
    case BLUE:
        r = 0.f; g = 0.f; b = 1.f; break;
    case GREEN:
        r = 0.f; g = 1.f; b = 0.f; break;
    case DARK_GRAY:
        r = 0.1f; g = 0.1f; b = 0.1f; break;
    case GRAY:
        r = 0.5f; g = 0.5f; b = 0.5f; break;
    case LIGHT_GRAY:
        r = 0.75f; g = 0.75f; b = 0.75f; break;
    }
}

Panel::Panel()
{
    m_name = "None";
}

Panel::Panel(const RectInt rectInt, const SizeDefinitionMethod sizeDefinition,
    const std::string name, const Color color, Panel* parent)
{
    m_name = name;
    m_backgroundColor = color;
    m_sizeDefinition = sizeDefinition;
    if (parent != NULL)
    {
        m_parent = parent;
    }
    if (sizeDefinition == DEF_ABS)
    {
        m_rectInt_abs = rectInt;
        m_rectFloat_abs = RectIntAbsToRectFloatAbs();
    }
    else
    {
        //not fully supported yet. need converting function
        m_rectInt_rel = rectInt;
    }
}

Panel::Panel(const RectFloat rectFloat, const SizeDefinitionMethod sizeDefinition,
    const std::string name, const Color color, Panel* parent)
{
    m_name = name;
    m_backgroundColor = color;
    m_sizeDefinition = sizeDefinition;
    if (parent != NULL)
    {
        m_parent = parent;
    }
    if (sizeDefinition == DEF_ABS)
    {
        m_rectFloat_abs = rectFloat;
    }
    else
    {
        m_rectFloat_rel = rectFloat;
        m_rectFloat_abs = RectFloatRelToRectFloatAbs();
    }
}

std::string Panel::GetName()
{
    return m_name;
}

void Panel::SetName(const std::string name)
{
    m_name = name;
}

int Panel::GetWidth()
{
    return m_rectInt_abs.m_w;
}

int Panel::GetHeight()
{
    return m_rectInt_abs.m_h;
}

void Panel::SetSize_Relative(const RectFloat rectFloatRel)
{
    m_rectFloat_rel = rectFloatRel;
    if (m_sizeDefinition == DEF_ABS)
        m_rectFloat_abs = RectFloatRelToRectFloatAbs();
}

void Panel::SetSize_Absolute(const RectFloat rectFloatAbs)
{
    m_rectFloat_abs = rectFloatAbs;
    if (m_sizeDefinition == DEF_REL)
        m_rectFloat_rel = RectFloatAbsToRectFloatRel();
}

void Panel::SetSize_Absolute(const RectInt rectIntAbs)
{
    m_rectInt_abs = rectIntAbs;
    if (m_sizeDefinition == DEF_ABS)
        m_rectFloat_abs = RectIntAbsToRectFloatAbs();
}

RectFloat Panel::GetRectFloatAbs()
{
    return m_rectFloat_abs;
}

RectInt Panel::GetRectIntAbs()
{
    return m_rectInt_abs;
}

void Panel::CreateGraphicsManager()
{
    m_graphicsManager = new GraphicsManager(this);
}

Panel* Panel::CreateSubPanel(const RectFloat rectFloat, const SizeDefinitionMethod sizeDefinition,
    const std::string name, const Color color)
{
    Panel* subPanel = new Panel(rectFloat, sizeDefinition, name, color, this);
    m_subPanels.push_back(subPanel);
    return subPanel;
}

Panel* Panel::CreateSubPanel(const RectInt rectInt, const SizeDefinitionMethod sizeDefinition,
    const std::string name, const Color color)
{
    Panel* subPanel = new Panel(rectInt, sizeDefinition, name, color, this);
    m_subPanels.push_back(subPanel);
    return subPanel;
}

Panel* Panel::GetRootPanel()
{
    Panel* rootPanel(this);
    bool isBasePanel(this->m_parent == NULL);
    while (rootPanel->m_parent != NULL)
    {
        rootPanel = rootPanel->m_parent;
    }
    return rootPanel;
}

RectFloat Panel::RectIntAbsToRectFloatAbs()
{
//	Panel* rootPanel(this);
    bool isBasePanel(this->m_parent == NULL);
//	while (rootPanel->m_parent != NULL)
//	{
//		rootPanel = rootPanel->m_parent;
//	}
    Panel* rootPanel = GetRootPanel();
    int windowWidth = rootPanel->m_rectInt_abs.m_w;
    int windowHeight = rootPanel->m_rectInt_abs.m_h;
    RectFloat rectFloat;
    if (isBasePanel)
    {
        rectFloat.m_x = -1.f;
        rectFloat.m_y = -1.f;
    }
    else
    {
        rectFloat.m_x = static_cast<float> (m_rectInt_abs.m_x) / rootPanel->m_rectInt_abs.m_w*2.f - 1.f;
        rectFloat.m_y = static_cast<float> (m_rectInt_abs.m_y) / rootPanel->m_rectInt_abs.m_h*2.f - 1.f;
    }
    rectFloat.m_w = static_cast<float> (m_rectInt_abs.m_w) / rootPanel->m_rectInt_abs.m_w*2.f;
    rectFloat.m_h = static_cast<float> (m_rectInt_abs.m_h) / rootPanel->m_rectInt_abs.m_h*2.f;
    return rectFloat;
}

RectFloat Panel::RectFloatRelToRectFloatAbs()
{
    RectFloat rectFloat;
    if (m_parent != NULL)
    {
        rectFloat = m_parent->m_rectFloat_abs*m_rectFloat_rel;
    }
    else
    {
        rectFloat = m_rectFloat_rel;
    }
    return rectFloat;
}

RectFloat Panel::RectFloatAbsToRectFloatRel()
{
    RectFloat rectFloat;
    if (m_parent != NULL)
    {
        rectFloat = m_rectFloat_abs/m_parent->m_rectFloat_abs;
    }
    else
    {
        rectFloat = m_rectFloat_abs;
    }
    return rectFloat;
}

void Panel::Update()
{
    if (m_sizeDefinition == DEF_ABS)
    {
        m_rectFloat_abs = RectIntAbsToRectFloatAbs();
    }
    else{
        m_rectFloat_abs = RectFloatRelToRectFloatAbs();
    }
}

void Panel::UpdateAll()
{
    Update();
    for (std::vector<Panel*>::iterator it = m_subPanels.begin(); it != m_subPanels.end(); ++it)
    {
        (*it)->UpdateAll();
    }
    for (std::vector<Button*>::iterator it = m_buttons.begin(); it != m_buttons.end(); ++it)
    {
        (*it)->UpdateAll();
    }
    for (std::vector<Slider*>::iterator it = m_sliders.begin(); it != m_sliders.end(); ++it)
    {
        (*it)->UpdateAll();
    }
}

void Panel::Draw()
{
    glColor3f(m_backgroundColor.r, m_backgroundColor.g, m_backgroundColor.b);
    glBegin(GL_QUADS);
    glVertex2f(m_rectFloat_abs.m_x, m_rectFloat_abs.m_y + m_rectFloat_abs.m_h);
    glVertex2f(m_rectFloat_abs.m_x, m_rectFloat_abs.m_y);
    glVertex2f(m_rectFloat_abs.m_x + m_rectFloat_abs.m_w, m_rectFloat_abs.m_y);
    glVertex2f(m_rectFloat_abs.m_x + m_rectFloat_abs.m_w, m_rectFloat_abs.m_y + m_rectFloat_abs.m_h);
    glEnd();

    Panel* rootPanel = GetRootPanel();
    glColor3f(m_foregroundColor.r, m_foregroundColor.g, m_foregroundColor.b);
    int stringWidth = 0;
    for (char& c:m_displayText)
    {
        stringWidth += (glutBitmapWidth(GLUT_BITMAP_HELVETICA_10, c));
    }
    float stringWidthf = static_cast<float>(stringWidth) / rootPanel->m_rectInt_abs.m_w*2.f;
    float stringHeightf = static_cast<float>(glutBitmapWidth(GLUT_BITMAP_HELVETICA_10, 'A')) /
        rootPanel->m_rectInt_abs.m_h*2.f;
    glRasterPos2f(m_rectFloat_abs.m_x + m_rectFloat_abs.m_w*0.5f-stringWidthf*0.5f, 
                    m_rectFloat_abs.m_y + m_rectFloat_abs.m_h*0.5f-stringHeightf*0.5f);
    for (char& c:m_displayText)
    {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, c);
    }
}

void Panel::DrawAll()
{
    if (m_draw == true) Draw();
    for (std::vector<Panel*>::iterator it = m_subPanels.begin(); it != m_subPanels.end(); ++it)
    {
        (*it)->DrawAll();
    }
    for (std::vector<Button*>::iterator it = m_buttons.begin(); it != m_buttons.end(); ++it)
    {
        (*it)->DrawAll();
    }
    for (std::vector<Slider*>::iterator it = m_sliders.begin(); it != m_sliders.end(); ++it)
    {
        (*it)->DrawAll();
    }
}


Panel* Panel::GetPanel(const std::string name)
{
    for (std::vector<Panel*>::iterator it = m_subPanels.begin(); it != m_subPanels.end(); ++it)
    {
        if ((*it)->m_name == name)
        {
            return *it;
        }
        Panel* panelSearchResult = (*it)->GetPanel(name);
        if (panelSearchResult != NULL)
        {
            return panelSearchResult;
        }
    }
    return NULL;
}

Button* Panel::GetButton(const std::string name)
{
    for (std::vector<Panel*>::iterator it = m_subPanels.begin(); it != m_subPanels.end(); ++it)
    {
        Button* buttonSearchResult = (*it)->GetButton(name);
        if (buttonSearchResult != NULL)
        {
            return buttonSearchResult;
        }
    }
    for (std::vector<Button*>::iterator it = m_buttons.begin(); it != m_buttons.end(); ++it)
    {
        if ((*it)->m_name == name)
        {
            return *it;
        }
    }
    return NULL;
}

Slider* Panel::GetSlider(const std::string name)
{
    for (std::vector<Panel*>::iterator it = m_subPanels.begin(); it != m_subPanels.end(); ++it)
    {
        Slider* sliderSearchResult = (*it)->GetSlider(name);
        if (sliderSearchResult != NULL)
        {
            return sliderSearchResult;
        }
    }
    for (std::vector<Slider*>::iterator it = m_sliders.begin(); it != m_sliders.end(); ++it)
    {
        if ((*it)->m_name == name)
        {
            return *it;
        }
    }
    return NULL;
}

Button* Panel::CreateButton(const RectFloat rectFloat, const SizeDefinitionMethod sizeDefinition,
    const std::string name, const Color color)
{
    Button* button = new Button(rectFloat, sizeDefinition, name, color, this);
    m_buttons.push_back(button);
    return button;
}

Slider* Panel::CreateSlider(const RectFloat rectFloat, const SizeDefinitionMethod sizeDefinition,
    const std::string name, const Color color)
{
    Slider* slider = new Slider(rectFloat, sizeDefinition, name, color, this);
    m_sliders.push_back(slider);
    return slider;
}

void Panel::Drag(const int x, const int y, const float dx, const float dy, const int button)
{
    if (m_graphicsManager != NULL)
    {
        m_graphicsManager->Drag(x,y,dx,dy,button);
    }
}

void Panel::Wheel(const int button, const int dir, const int x, const int y)
{
    if (m_graphicsManager != NULL)
    {
        m_graphicsManager->Wheel(button,dir,x,y);
    }
}

void Panel::ClickDown(Mouse mouse)
{
    if (m_graphicsManager != NULL)
    {
        m_graphicsManager->ClickDown(mouse);
    }
}


Button::Button(const RectFloat rectFloat, const SizeDefinitionMethod sizeDefinition, 
    const std::string name, const Color color, Panel* parent) 
    : Panel(rectFloat, sizeDefinition, name, color, parent)
{
    m_displayText = this->GetName();
}

Button::Button(const RectInt rectInt, const SizeDefinitionMethod sizeDefinition, 
    const std::string name, const Color color, Panel* parent) 
    : Panel(rectInt, sizeDefinition, name, color, parent)
{
    m_displayText = this->GetName();
}

void Button::ClickDown(Mouse mouse)
{
    if (m_callBack != NULL)
    {
        m_callBack();
    }
}

SliderBar::SliderBar()
{
}

SliderBar::SliderBar(const RectFloat rectFloat, const SizeDefinitionMethod sizeDefinition,
    const std::string name, const Color color, Slider* parent) 
    : Panel(rectFloat, sizeDefinition, name, color, parent)
{
    m_backgroundColor = Color::GRAY;
}

void SliderBar::Draw()
{
    RectFloat rect = this->GetRectFloatAbs();
    glColor3f(m_backgroundColor.r, m_backgroundColor.g, m_backgroundColor.b);
    glBegin(GL_QUADS);
    glVertex2f(rect.m_x, rect.m_y + rect.m_h);
    glVertex2f(rect.m_x, rect.m_y);
    glVertex2f(rect.m_x + rect.m_w, rect.m_y);
    glVertex2f(rect.m_x + rect.m_w, rect.m_y + rect.m_h);
    glEnd();

    glColor3f(m_foregroundColor.r, m_foregroundColor.g, m_foregroundColor.b);
    float outlineWidth = 0.003f;
    glBegin(GL_QUADS);
    glVertex2f(rect.m_x + outlineWidth, rect.m_y + rect.m_h - outlineWidth*2.f);
    glVertex2f(rect.m_x + outlineWidth, rect.m_y + outlineWidth*2.f);
    glVertex2f(rect.m_x + rect.m_w - outlineWidth, rect.m_y + outlineWidth*2.f);
    glVertex2f(rect.m_x + rect.m_w - outlineWidth, rect.m_y + rect.m_h - outlineWidth*2.f);
    glEnd();
}

void SliderBar::UpdateValue()
{
    RectFloat rect = this->GetRectFloatAbs();
    if (m_orientation == VERTICAL)
    {
        m_value = m_parent->m_minValue + (m_parent->m_maxValue - m_parent->m_minValue)*
            (rect.GetCentroidY() - (m_parent->GetRectFloatAbs().m_y+rect.m_h*0.5f)) /
            (m_parent->GetRectFloatAbs().m_h-rect.m_h);
    }
    else
    {
        m_value = m_parent->m_minValue + (m_parent->m_maxValue - m_parent->m_minValue)*
            (rect.GetCentroidX() - (m_parent->GetRectFloatAbs().m_x+rect.m_w*0.5f)) /
            (m_parent->GetRectFloatAbs().m_w-rect.m_w);
    }
}

float SliderBar::GetValue()
{
    UpdateValue();
    return m_value;
}

void SliderBar::Drag(int x, int y, float dx, float dy, int button)
{
    RectFloat rect = this->GetRectFloatAbs();
    //dx and dy are coming in as float abs coordinates
    if (m_orientation == VERTICAL)
    {
        rect.m_y = max(m_parent->GetRectFloatAbs().m_y, 
            min(m_parent->GetRectFloatAbs().m_y + m_parent->GetRectFloatAbs().m_h - rect.m_h, rect.m_y + dy));
    }
    else
    { 
        rect.m_x = max(m_parent->GetRectFloatAbs().m_x,
            min(m_parent->GetRectFloatAbs().m_x + m_parent->GetRectFloatAbs().m_w - rect.m_w, rect.m_x + dx));
    }
    SetSize_Absolute(rect);
    UpdateValue();
}

Slider::Slider(const RectFloat rectFloat, const SizeDefinitionMethod sizeDefinition,
    const std::string name, const Color color, Panel* parent) 
    : Panel(rectFloat, sizeDefinition, name, color, parent)
{
}

Slider::Slider(const RectInt rectInt, const SizeDefinitionMethod sizeDefinition,
    const std::string name, const Color color, Panel* parent) 
    : Panel(rectInt, sizeDefinition, name, color, parent)
{
}

void Slider::CreateSliderBar(const RectFloat rectFloat,
    const SizeDefinitionMethod sizeDefinition, const std::string name, const Color color)
{
    SliderBar* slider = new SliderBar(rectFloat, sizeDefinition, name, color, this);
    if (m_sliderBar1 == NULL)
    {
        m_sliderBar1 = slider;
    }
    else if (m_sliderBar2 == NULL)
    {
        m_sliderBar2 = slider;
    }
}

void Slider::UpdateAll()
{
    Update();
    if (m_sliderBar1 != NULL)
    {
        m_sliderBar1->Update();
    }
    if (m_sliderBar2 != NULL)
    {
        m_sliderBar2->Update();
    }
}

void Slider::Draw()
{
    Color minColor, maxColor;
    RectFloat rect = this->GetRectFloatAbs();
    if (m_sliderBar2 == NULL)
    {
        if (m_sliderBar1->m_orientation == SliderBar::VERTICAL)
        {
            minColor = Color::BLUE;
            maxColor = Color::WHITE;
            glBegin(GL_QUADS);
            glColor3f(maxColor.r, maxColor.g, maxColor.b);
            glVertex2f(rect.m_x, rect.m_y + rect.m_h);
            glColor3f(minColor.r, minColor.g, minColor.b);
            glVertex2f(rect.m_x, rect.m_y);
            glVertex2f(rect.m_x + rect.m_w, rect.m_y);
            glColor3f(maxColor.r, maxColor.g, maxColor.b);
            glVertex2f(rect.m_x + rect.m_w, rect.m_y + rect.m_h);
            glEnd();
        }
        else
        {
            minColor = Color::BLUE;
            maxColor = Color::WHITE;
            glBegin(GL_QUADS);
            glColor3f(minColor.r, minColor.g, minColor.b);
            glVertex2f(rect.m_x, rect.m_y + rect.m_h);
            glVertex2f(rect.m_x, rect.m_y);
            glColor3f(maxColor.r, maxColor.g, maxColor.b);
            glVertex2f(rect.m_x + rect.m_w, rect.m_y);
            glVertex2f(rect.m_x + rect.m_w, rect.m_y + rect.m_h);
            glEnd();

        }
    }
    else{ //this doesn't work for horizontal sliders yet
        minColor = Color::BLUE;
        maxColor = Color::WHITE;
        Color lowerColor, higherColor;
        SliderBar* lowerSliderBar;
        SliderBar* higherSliderBar;
        if (m_sliderBar1->GetValue() < m_sliderBar2->GetValue())
        {
            lowerColor = m_sliderBar1->m_foregroundColor;
            higherColor = m_sliderBar2->m_foregroundColor;
            lowerSliderBar = m_sliderBar1;
            higherSliderBar = m_sliderBar2;
        }
        else
        {
            lowerColor = m_sliderBar2->m_foregroundColor;
            higherColor = m_sliderBar1->m_foregroundColor;
            lowerSliderBar = m_sliderBar2;
            higherSliderBar = m_sliderBar1;
        }
        if (m_sliderBar1->m_orientation == SliderBar::VERTICAL)
        {
            glBegin(GL_QUADS);
            glColor3f(lowerColor.r, lowerColor.g, lowerColor.b);
            glVertex2f(rect.m_x, lowerSliderBar->GetRectFloatAbs().GetCentroidY());
            glColor3f(lowerColor.r, lowerColor.g, lowerColor.b);
            glVertex2f(rect.m_x, rect.m_y);
            glVertex2f(rect.m_x + rect.m_w, rect.m_y);
            glColor3f(lowerColor.r, lowerColor.g, lowerColor.b);
            glVertex2f(rect.m_x + rect.m_w, lowerSliderBar->GetRectFloatAbs().GetCentroidY());
            glEnd();

            glBegin(GL_QUADS);
            glColor3f(higherColor.r, higherColor.g, higherColor.b);
            glVertex2f(rect.m_x, higherSliderBar->GetRectFloatAbs().GetCentroidY());
            glColor3f(lowerColor.r, lowerColor.g, lowerColor.b);
            glVertex2f(rect.m_x, lowerSliderBar->GetRectFloatAbs().GetCentroidY());
            glVertex2f(rect.m_x + rect.m_w, lowerSliderBar->GetRectFloatAbs().GetCentroidY());
            glColor3f(higherColor.r, higherColor.g, higherColor.b);
            glVertex2f(rect.m_x + rect.m_w, higherSliderBar->GetRectFloatAbs().GetCentroidY());
            glEnd();


            glBegin(GL_QUADS);
            glColor3f(higherColor.r, higherColor.g, higherColor.b);
            glVertex2f(rect.m_x, rect.m_y+rect.m_h);
            glColor3f(higherColor.r, higherColor.g, higherColor.b);
            glVertex2f(rect.m_x, higherSliderBar->GetRectFloatAbs().GetCentroidY());
            glVertex2f(rect.m_x + rect.m_w, higherSliderBar->GetRectFloatAbs().GetCentroidY());
            glColor3f(higherColor.r, higherColor.g, higherColor.b);
            glVertex2f(rect.m_x + rect.m_w, rect.m_y+rect.m_h);
            glEnd();
        }
        else
        {
            glBegin(GL_QUADS);
            glColor3f(lowerColor.r, lowerColor.g, lowerColor.b);
            glVertex2f(rect.m_x, rect.m_y+rect.m_h);
            glVertex2f(rect.m_x, rect.m_y);
            glVertex2f(lowerSliderBar->GetRectFloatAbs().GetCentroidX(), rect.m_y);
            glVertex2f(lowerSliderBar->GetRectFloatAbs().GetCentroidX(), rect.m_y+rect.m_h);
            glEnd();

            glBegin(GL_QUADS);
            glColor3f(lowerColor.r, lowerColor.g, lowerColor.b);
            glVertex2f(lowerSliderBar->GetRectFloatAbs().GetCentroidX(), rect.m_y+rect.m_h);
            glVertex2f(lowerSliderBar->GetRectFloatAbs().GetCentroidX(), rect.m_y);
            glColor3f(higherColor.r, higherColor.g, higherColor.b);
            glVertex2f(higherSliderBar->GetRectFloatAbs().GetCentroidX(), rect.m_y);
            glVertex2f(higherSliderBar->GetRectFloatAbs().GetCentroidX(), rect.m_y+rect.m_h);
            glEnd();

            glBegin(GL_QUADS);
            glColor3f(higherColor.r, higherColor.g, higherColor.b);
            glVertex2f(higherSliderBar->GetRectFloatAbs().GetCentroidX(), rect.m_y+rect.m_h);
            glVertex2f(higherSliderBar->GetRectFloatAbs().GetCentroidX(), rect.m_y);
            glVertex2f(rect.m_x+rect.m_w, rect.m_y);
            glVertex2f(rect.m_x+rect.m_w, rect.m_y+rect.m_h);
            glEnd();
        }
    }
}


void Slider::DrawAll()
{
    if (m_draw == true)
    {
        Draw();
        if (m_sliderBar1 != NULL)
        {
            m_sliderBar1->Draw();
        }
        if (m_sliderBar2 != NULL)
        {
            m_sliderBar2->Draw();
        }
    }
}

void Slider::Hide()
{
    m_draw = false;
    if (m_sliderBar1 != NULL)
    {
        m_sliderBar1->m_draw = false;
    }
    if (m_sliderBar2 != NULL)
    {
        m_sliderBar2->m_draw = false;
    }
}

void Slider::Show()
{
    m_draw = true;
    if (m_sliderBar1 != NULL)
    {
        m_sliderBar1->m_draw = true;
    }
    if (m_sliderBar2 != NULL)
    {
        m_sliderBar2->m_draw = true;
    }
}

ButtonGroup::ButtonGroup()
{
}

ButtonGroup::ButtonGroup(std::vector<Button*> &buttons)
{
    m_buttons = buttons;
}

void ButtonGroup::ExclusiveEnable(Button* button)
{
    for (std::vector<Button*>::iterator it = m_buttons.begin(); it != m_buttons.end(); ++it)
    {
        Slider* sliderForButton(NULL);
        if ((*it)->GetRootPanel()->GetSlider((*it)->GetName()) != NULL){
            sliderForButton = (*it)->GetRootPanel()->GetSlider((*it)->GetName());
        }
        if (*it == button)
        {
            (*it)->m_highlighted = true;
            (*it)->m_backgroundColor = Color::LIGHT_GRAY;
            if (sliderForButton != NULL)
            {
                sliderForButton->Show();
            }
        }
        else
        {
            (*it)->m_highlighted = false;
            (*it)->m_backgroundColor = Color::GRAY;
            if (sliderForButton != NULL)
            {
                sliderForButton->Hide();
            }
        }
    }
}

Button* ButtonGroup::GetCurrentEnabledButton()
{
    for (std::vector<Button*>::iterator it = m_buttons.begin(); it != m_buttons.end(); ++it)
    {
        if ((*it)->m_highlighted == true)
        {
            return *it;
        }
    }
    return NULL;
}



bool IsPointInRect(float x, float y, RectFloat rect, float tol = 0.0f)
{
    if (x+tol > rect.m_x && x-tol<rect.m_x + rect.m_w 
        && y+tol>rect.m_y && y-tol < rect.m_y + rect.m_h)
        return true;
    return false;
}


Panel* GetPanelThatPointIsIn(Panel* parentPanel, float x, float y)
{
    Panel* panelThatPointIsIn = NULL; 
    if (IsPointInRect(x, y, parentPanel->GetRectFloatAbs()))
    {
        panelThatPointIsIn = parentPanel;
        Panel* temp;
        for (std::vector<Panel*>::iterator it = parentPanel->m_subPanels.begin(); it != parentPanel->m_subPanels.end(); ++it)
        {
            temp = GetPanelThatPointIsIn(*it, x, y);
            if (temp != NULL)// && temp->m_draw == true)   //OK to ignore the m_draw for now. For sliders, it affects the contour sliders. Need to differentiate between draw=false and inactive objects
            {
                panelThatPointIsIn = temp;
            }
        }
        for (std::vector<Button*>::iterator it = parentPanel->m_buttons.begin(); it != parentPanel->m_buttons.end(); ++it)
        {
            temp = GetPanelThatPointIsIn(*it, x, y);
            if (temp != NULL && temp->m_draw == true)
            {
                panelThatPointIsIn = temp;
            }
        }
        for (std::vector<Slider*>::iterator it = parentPanel->m_sliders.begin(); it != parentPanel->m_sliders.end(); ++it)
        {
            temp = GetPanelThatPointIsIn(*it, x, y);
            if (temp != NULL && temp->m_draw == true)
            {
                panelThatPointIsIn = temp;
                if ((*it)->m_sliderBar1 != NULL && temp->m_draw == true)
                {
                    if (IsPointInRect(x, y, (*it)->m_sliderBar1->GetRectFloatAbs()))
                    {
                        panelThatPointIsIn = (*it)->m_sliderBar1;
                    }
                }
                if ((*it)->m_sliderBar2 != NULL && temp->m_draw == true)
                {
                    if (IsPointInRect(x, y, (*it)->m_sliderBar2->GetRectFloatAbs()))
                    {
                        panelThatPointIsIn = (*it)->m_sliderBar2;
                    }
                }
            }
        }
    }
    return panelThatPointIsIn;
}

